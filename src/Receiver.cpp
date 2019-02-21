#include "Utils.hpp"
#include "Config.hpp"


namespace Receiver {

/**
 * List of received parts
 */
std::set<int> parts;

/**
 * Get empty part's
 */
std::vector<int> getEmptyParts() {
    std::vector<int> result;
    // For each parts
    for (int i = 0; i < int((float)file_length / (float)mtu + 0.5); i++) {
        if (!parts.count(i)) result.push_back(i);
    }
    return result;
}

/**
 * Start while receive packet 'FINISH'
 * Get empty part's and requests it's from the server
 */
void checkParts() {
    char* buffer = new char[100];

    std::vector<int> emptyParts = getEmptyParts();

    while (ttl && emptyParts.size() > 0) {
        for (auto index : emptyParts) {
            snprintf(buffer, 7, "RESEND");                                //
            Utils::writeBytesFromInt(buffer + 6, index, 4);               // Create request packet
            sendto(_socket, buffer, 10, 0, (sockaddr*) &broadcast_address,//
                   sizeof(broadcast_address));                            //

            std::cout << "Requested " << index << " part" << std::endl;
        }
        emptyParts = getEmptyParts();
    }

    std::ofstream output(fileName, std::ofstream::binary);      //
    output.write(file, file_length);                            // Save file
}


void run(cxxopts::ParseResult &options) {
    bool finish = false; // Sender finish transfering

    char* buffer = new char[2 * mtu];

    while (auto length = recvfrom(_socket, buffer, 2 * mtu, 0, (sockaddr*) &server_address, &server_address_length)) {
        // Sender is no longer available
        if (ttl <= 0) return;

        // If Sender finish transfering check missing parts every 1 sec
        if (finish) checkParts();

        if (length <= 0) continue; // If no more packets in buffer

        ttl = ttl_max; // Update ttl

        if (strncmp(buffer, "NEW_PACKET", 10) == 0) {
            file_length = Utils::getIntFromBytes(buffer + 10, 4);

            file = new char[file_length];
            memset(file, 0, file_length);

            std::cout << "Receive information about new file size: " << file_length << std::endl;
            std::cout << "Part count: " << int((float)file_length / (float)mtu + 0.5) << std::endl;
        } else if (strncmp(buffer, "TRANSFER", 8) == 0) {
            int part = Utils::getIntFromBytes(buffer + 8, 4);  // Part index
            int size = Utils::getIntFromBytes(buffer + 12, 4); // Part size
            parts.insert(part);
            std::cout << "Receive " << part << " part with size " << size << std::endl;

            memcpy(file + part * options["mtu"].as<int>(), buffer + 16, size);
        } else if (strncmp(buffer, "FINISH", 6) == 0) {
            std::cout << "Server finish transfering" << std::endl;
            finish = true;
        }
    }
    delete[] buffer;
}

} //namespace Receiver