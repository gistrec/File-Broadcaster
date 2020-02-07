#include "Utils.hpp"
#include "Config.hpp"


namespace Sender {

/**
 * Since server may receive many requests for sending some part
 * It is necessary to limit the sending of the same parts for a while
 * This container contains part number and time when the part was sended
 */
std::map<int, int> sent_part;

void sendPart(int part_index) {
    int packet_length = file_length - part_index * mtu;  // Get packet length
    if (packet_length > mtu) packet_length = mtu;        //

    snprintf(buffer, 9, "TRANSFER");
    Utils::writeBytesFromInt(buffer +  8, (size_t)part_index,    4);                 // Write section "number"
    Utils::writeBytesFromInt(buffer + 12, (size_t)packet_length, 4);                 // Write section "length"
    memcpy(buffer + 16, (void *)(intptr_t)(file + part_index * mtu), packet_length); // Write section "data"

    // Sending part to the broadcast address
    sendto(_socket, buffer, packet_length + 16, 0, (struct sockaddr*) &broadcast_address, sizeof(broadcast_address));
    std::cout << "Part " << part_index << " with size " << packet_length << " was send" << std::endl;
}

void run(cxxopts::ParseResult &options) {
    buffer = new char[2 * mtu];

    std::ifstream input(fileName, std::ios::binary);                     //
	if (!input.is_open()) {                                              // Opening the file
		std::cout << "Error: Can't open file " << fileName << std::endl; //
		exit(-1);                                                        //
	}                                                                    //

    // Thk Windows.h, where define max(). It so horrible... //
    input.ignore((std::numeric_limits<int>::max)());        //
    file_length = (int) input.gcount();                     // Getting file length
    input.seekg(0, input.beg);                              // And writing file to RAM
                                                            //
    file = new (std::nothrow) char[file_length + 1];        //
	if (!file) {                                            //
		std::cout << "Error: Can't allocate " << file_length << " bytes" << std::endl;
		exit(-1);
	}
    input.read(file, file_length);

    std::cout << "Ok: File successfully copied to RAM" << std::endl;

    snprintf(buffer, 11, "NEW_PACKET");                            //
    Utils::writeBytesFromInt(buffer + 10, file_length, 4);         // Sending information
    sendto(_socket, buffer, 14, 0, (sockaddr*) &broadcast_address, // about size of new file
           sizeof(broadcast_address));                             //

    std::cout << "Ok: Send information about new file with size " << file_length << std::endl; 

    int part_index = 0;

    while (part_index * mtu < file_length) {  //
        sent_part.insert({ part_index, 0 });  // 
                                              // 
        sendPart(part_index);                 // Send parts 
                                              //   every 20ms
        part_index++;                         //
        std::this_thread::sleep_for(20ms);    //
    }                                         //

    snprintf(buffer, 7, "FINISH");                                // Sending file transfer
    sendto(_socket, buffer, 6, 0, (sockaddr*) &broadcast_address, // completion information
           sizeof(broadcast_address));                            //
    std::cout << "Ok: File transfer complete" << std::endl;       //
    
    long lastFinishSendTime = 0; // Last time, when sender sended file transfer completion information

    while (ttl) {
        auto result = recvfrom(_socket, (char *)buffer, 100, 0, (struct sockaddr*) &broadcast_address, &client_address_length);

        // sending file completion information every second
        if (result <= 0) {
            ttl--;
            snprintf(buffer, 7, "FINISH");
            sendto(_socket, buffer, 6, 0, (struct sockaddr*) &broadcast_address, sizeof(broadcast_address));
            continue;
        }

        if (strncmp(buffer, "RESEND", 6) == 0) {
            int part = Utils::getIntFromBytes(buffer + 6, 4);

            auto now      = std::chrono::system_clock::now();
            auto now_ms   = std::chrono::time_point_cast<std::chrono::seconds>(now);
            auto epoch    = now_ms.time_since_epoch();
            auto value    = std::chrono::duration_cast<std::chrono::seconds>(epoch);
            long duration = value.count(); // Unix time in second

            ttl = ttl_max;

            if (duration - sent_part[part] >= 1) {
                sent_part[part] = duration;
                std::cout << "Client requested part of file with index " << part << std::endl;
                sendPart(part);
            }

            // sending file completion information every second
            if (duration - lastFinishSendTime >= 1) {
                lastFinishSendTime = duration;
                snprintf(buffer, 7, "FINISH");
                sendto(_socket, buffer, 6, 0, (struct sockaddr*) &broadcast_address, sizeof(broadcast_address));
            }
        }

    }
    std::cout << "Ok: Process no longer be working" << std::endl;
}


} //namespace Sender
