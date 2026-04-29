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
    for (int i = 0; i < (int)((file_length + mtu - 1) / mtu); i++) {
        if (parts.find(i) == parts.end()) result.push_back(i);
    }
    return result;
}

/**
 * Runs when "FINISH" packet is received
 * Gets empty parts and requests them from the server
 */
void checkParts() {
    char* buffer = new char[2 * mtu];

    std::vector<int> emptyParts = getEmptyParts();

    while (ttl && emptyParts.size() > 0) {
        for (auto index : emptyParts) {
            snprintf(buffer, 7, "RESEND");
            Utils::writeBytesFromNumber(buffer + 6, index, 4);
            sendto(_socket, buffer, 10, 0, (sockaddr*) &broadcast_address,
                   sizeof(broadcast_address));
            std::cout << "Request part of file with index " << index << std::endl;
            std::this_thread::sleep_for(20ms);
        }

        SOCKADDR_IN sender_address = { 0 };
        addr_len sender_address_length = sizeof(sender_address);
        auto length = recvfrom(_socket, buffer, 2 * mtu, 0,
                               (sockaddr*) &sender_address, &sender_address_length);

        if (length <= 0) {
            ttl--;
            continue;
        }

        ttl = ttl_max;

        if (strncmp(buffer, "TRANSFER", 8) == 0) {
            int part = Utils::getNumberFromBytes(buffer + 8, 4);
            int size = Utils::getNumberFromBytes(buffer + 12, 4);
            parts.insert(part);
            memcpy(file + part * mtu, buffer + 16, size);
            std::cout << "Receive " << part << " part with size " << size << std::endl;
        }

        emptyParts = getEmptyParts();
    }

    delete[] buffer;

    if (!emptyParts.empty()) {
        std::cerr << "Error: Transfer timed out, file is incomplete" << std::endl;
        delete[] file;
        file = nullptr;
        return;
    }

    std::ofstream output(fileName, std::ofstream::binary);
    output.write(file, file_length);
    std::cout << "File successfully received" << std::endl;

    delete[] file;
    file = nullptr;
    CLOSE_SOCKET(_socket);
    exit(0);
}


void run() {
    bool finish = false; // Sender finished transferring

    char* buffer = new char[2 * mtu];

    while (auto length = recvfrom(_socket, buffer, 2 * mtu, 0, (sockaddr*) &server_address, &server_address_length)) {
        // Sender is no longer available
        if (ttl <= 0) {
            delete[] buffer;
            return;
        }

        // If Sender finished transferring, check missing parts
        if (finish && file != nullptr) {
            delete[] buffer;
            checkParts();
            return;
        }

        if (length <= 0) {
            ttl--;
            continue;
        }

        ttl = ttl_max; // Update ttl

        if (strncmp(buffer, "NEW_PACKET", 10) == 0) {
            file_length = Utils::getNumberFromBytes(buffer + 10, 4); // Read section "file length"

            delete[] file;
            parts.clear();
            file = new char[file_length];
            memset(file, 0, file_length);

            std::cout << "Receive information about new file size: " << file_length << std::endl;
            std::cout << "Number of parts: " << (file_length + mtu - 1) / mtu << std::endl;
        } else if (strncmp(buffer, "TRANSFER", 8) == 0 && file != nullptr) {
            int part = Utils::getNumberFromBytes(buffer +  8, 4); // Read section "index"
            int size = Utils::getNumberFromBytes(buffer + 12, 4); // Read section "size"
            parts.insert(part);
            std::cout << "Receive " << part << " part with size " << size << std::endl;

            memcpy(file + part * mtu, buffer + 16, size);
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
