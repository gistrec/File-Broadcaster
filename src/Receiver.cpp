#include "Utils.hpp"
#include "Config.hpp"


namespace Receiver {

/**
 * List of received parts
 */
std::set<int> parts;

/**
 * Get empty parts
 */
std::vector<int> getEmptyParts() {
    std::vector<int> result;
    // For each parts
    for (int i = 0; i < int((float)file_length / (float)mtu + 0.5); i++) {
        if (parts.find(i) == parts.end()) result.push_back(i);
    }
    return result;
}

/**
 * Runs when "FINISH" packet is received
 * Gets empty parts and requests them from the server
 */
void checkParts() {
    char buffer[100];

    std::vector<int> emptyParts = getEmptyParts();

    while (ttl && emptyParts.size() > 0) {
        for (auto index : emptyParts) {
            snprintf(buffer, 7, "RESEND");                                //
            Utils::writeBytesFromInt(buffer + 6, index, 4);               // Create request packet
            sendto(_socket, buffer, 10, 0, (sockaddr*) &broadcast_address,//
                   sizeof(broadcast_address));                            //

            std::cout << "Request part of file with index " << index << std::endl;
        }
        emptyParts = getEmptyParts();
    }

    std::ofstream output(fileName, std::ofstream::binary);      //
    output.write(file, file_length);                            // Save file

    std::cout << "File successfully received" << std::endl;

    delete[] file;
    exit(0);
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
            file_length = Utils::getIntFromBytes(buffer + 10, 4); // Read section "file length"

            file = new char[file_length];
            memset(file, 0, file_length);

            std::cout << "Receive information about new file size: " << file_length << std::endl;
            std::cout << "Number of parts: " << int((float)file_length / (float)mtu + 0.5) << std::endl;
        } else if (strncmp(buffer, "TRANSFER", 8) == 0) {
            int part = Utils::getIntFromBytes(buffer +  8, 4); // Read section "index"
            int size = Utils::getIntFromBytes(buffer + 12, 4); // Read section "size"
            parts.insert(part);
            std::cout << "Receive " << part << " part with size " << size << std::endl;

            memcpy(file + part * options["mtu"].as<int>(), buffer + 16, size);
        } else if (strncmp(buffer, "FINISH", 6) == 0) {
            // If receiver didn't receive a finish message
            if (!finish) {
                std::cout << "Server finished transferring" << std::endl;
                finish = true;
            }
        }
    }
    delete[] buffer;
}

} //namespace Receiver