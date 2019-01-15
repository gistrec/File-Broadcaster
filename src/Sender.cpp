#include "Utils.hpp"
#include "Config.hpp"


namespace Sender {

/**
 * As server may receive many requests for send some part
 * we need to restrict sending the same parts for some time
 * This container consist part number and time, when part was send
 */
std::map<int, int> sent_part;

void sendPart(int part_index) {
    long packet_length = file_length - part_index * mtu;  // Get part length
    if (packet_length > mtu) packet_length = mtu;         // 

    snprintf(buffer, 8, "TRANSFER");
    Utils::writeBytesFromInt(buffer + 8,  (size_t)part_index, 4);    // Write part number
    Utils::writeBytesFromInt(buffer + 12, (size_t)packet_length, 4); // Write part length
    memcpy(buffer + 16, (void *)(intptr_t)(file + part_index * mtu), packet_length); // Write part data

    // Send part to broadcast address
    sendto(_socket, buffer, packet_length + 16, 0, (struct sockaddr*) &broadcast_address, sizeof(broadcast_address));
    std::cout << "Part " << part_index << " with size " << packet_length << " was send" << std::endl;
}

void run(cxxopts::ParseResult &options) {
    std::mt19937 gen(time(0)); // Для тестов
    std::uniform_int_distribution<> uid(0, 100);  // Для тестов
    std::cout << "Sender was started" << std::endl;

    buffer = new char[2 * mtu];

    std::ifstream input("file.txt", std::ios::binary);      //
    // Thk Windows.h, where define max(). It so horrible... //
    input.ignore((std::numeric_limits<int>::max)());        //
    file_length = (int) input.gcount();                     // Get file length
    input.seekg(0, input.beg);                              // And read it to RAM
                                                            //
    file = new char[file_length + 1];                       //
    input.read(file, file_length);                          //

    std::cout << "File successfully copied to RAM" << std::endl;



    snprintf(buffer, 10, "NEW_PACKET");                            //
    Utils::writeBytesFromInt(buffer + 10, file_length, 4);         // Send information
    sendto(_socket, buffer, 14, 0, (sockaddr*) &broadcast_address, // about new file
           sizeof(broadcast_address));                             //
    std::cout << "Send information about new file with size " << (int) file_length << std::endl;

    int part_index = 0;

    while (part_index * mtu < file_length) {  //
        sent_part.insert({ part_index, 0 });  //
                                              //
        if (uid(gen) < 90) {                  // Send all parts every 1 sec
            sendPart(part_index);             //
        }                                     //
                                              //
        part_index++;                         //
        std::this_thread::sleep_for(1s);      //
    }                                         //

    snprintf(buffer, 6, "FINISH");                                // Send information
    sendto(_socket, buffer, 6, 0, (sockaddr*) &broadcast_address, // the end of transaction
           sizeof(broadcast_address));                            //
    std::cout << "Передача файла завершена" << std::endl;

    std::thread(checkTTL);

    while (ttl) {
        int result = recvfrom(_socket, (char *)buffer, 100, 0, (struct sockaddr*) &client_address, &client_address_length);
        ttl = ttl_max;

        if (strncmp(buffer, "RESEND", 6) == 0) {
            int part = Utils::getIntFromBytes(buffer + 6, 4);

            auto now      = std::chrono::system_clock::now();
            auto now_ms   = std::chrono::time_point_cast<std::chrono::seconds>(now);
            auto epoch    = now_ms.time_since_epoch();
            auto value    = std::chrono::duration_cast<std::chrono::seconds>(epoch);
            long duration = value.count();

            if (duration - sent_part[part] > 1) {
                sent_part[part] = duration;
                std::cout << "Клиент запросил " << part << " часть" << std::endl;
                sendPart(part);
            }
        } else if (strncmp(buffer, "STATUS", 6) == 0) {
            std::cout << "Клиент запросил статус" << std::endl;

            snprintf(buffer, 6, "FINISH");
            sendto(_socket, buffer, 6, 0, (struct sockaddr*) &broadcast_address, sizeof(broadcast_address));
        }
    }
}


} //namespace Sender