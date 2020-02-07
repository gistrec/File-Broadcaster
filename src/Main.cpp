#include "cxxopts.hpp"

#include "Receiver.cpp"
#include "Sender.cpp"
#include "Config.hpp"


int main(int argc, char* argv[]) {
    // Parse input options from CLI
    cxxopts::Options options("File-Broadcaster", "UDB Broadcast file transfer");

    options
        .positional_help("[optional args]")
        .show_positional_help();

    options.add_options()
        ("p,port",    "Port",               cxxopts::value<int>()->default_value("33333"))
        ("f,file",    "File name",          cxxopts::value<std::string>())
        ("t,type",    "Receiver or sender", cxxopts::value<std::string>()->default_value("sender"))
        ("ttl",       "Time to live",       cxxopts::value<int>()->default_value("15"))
        ("mtu",       "MTU packet",         cxxopts::value<int>()->default_value("1500"))
        ("broadcast", "Broadcast address",  cxxopts::value<std::string>()->default_value("yes"));

    auto result = options.parse(argc, argv);

    #if defined(_WIN32) || defined(_WIN64)   //
    WORD socketVer;                          // Initialize use
    WSADATA wsaData;                         //     of the Winsock DLL
    socketVer = MAKEWORD(2, 2);              //         by a process.
    WSAStartup(socketVer, &wsaData);         //
    #endif                                   //

    mtu = result["mtu"].as<int>();                //
    ttl = ttl_max = result["ttl"].as<int>();      // Init some var
    fileName = result["file"].as<std::string>();  //


    _socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);   //
    if (_socket < 0) {                                   // Create socket with
        std::cout << "Can't create socket" << std::endl; //     datagram-based protocol
        exit(1);                                         //
    }                                                    //

    client_address.sin_family = AF_INET;                         //
    client_address.sin_port = htons(result["port"].as<int>());   // Create local address
    client_address.sin_addr.s_addr = INADDR_ANY;                 //

    memcpy(&server_address, &client_address, sizeof(server_address)); // Server address == Client address

    broadcast_address.sin_family = AF_INET;                         // Create broadcast
    broadcast_address.sin_port = htons(result["port"].as<int>());   //       address

    if (result["broadcast"].as<std::string>() == "yes") {                   //
        #if defined(_WIN32) || defined(_WIN64)                              // Give access to
        char broadcastEnable = '1';                                         //  broadcast address
        #else                                                               //
        int broadcastEnable = 1;                                            //
        #endif                                                              //
                                                                            //
        if (setsockopt(_socket, SOL_SOCKET, SO_BROADCAST,                   //
                       &broadcastEnable, sizeof(broadcastEnable)) == 0) {   //
            std::cout << "Got access to broadcast" << std::endl;            //
        } else {                                                            //
            std::cerr << "Cant't get access to broadcast" << std::endl;     //
            exit(1);                                                        //
        }                                                                   // If option 'broadcast' is yes
        broadcast_address.sin_addr.s_addr = INADDR_BROADCAST;               // change server address
    } else {                                                                // to broadcast
        broadcast_address.sin_addr.s_addr =                                 // Else change server address
            inet_addr(result["broadcast"].as<std::string>().c_str());       // to address in option
    }                                                                       //

    if (bind(_socket, (sockaddr *)&client_address, sizeof(client_address)) == 0) {//
        std::cout << "Bind socket" << std::endl;                                  //
    } else {                                                                      // Bind socket to
        std::cerr << "Can't bind socket" << std::endl;                            //     client address
        exit(1);                                                                  //
    }                                                                             //

    #if defined(_WIN32) || defined(_WIN64)                                      //
    int tv = 1 * 1000;  // user timeout in milliseconds [ms]                    //
    setsockopt(_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(tv));       // Set socket
    #else                                                                       // receive timeout
    struct timeval tv;                                                          //        to 1 sec
    tv.tv_sec = 1;                                                              //
    tv.tv_usec = 0;                                                             //
    setsockopt(_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv)); //
    #endif

    // Run receiver or sender
    if (result["type"].as<std::string>() == "receiver") {        //
        Receiver::run(result);                                   //
    } else if (result["type"].as<std::string>() == "sender") {   // Run receiver or sender
        Sender::run(result);                                     //            application
    } else {                                                     //
        std::cerr << "Type not found" << std::endl;              //
    }

    return 0;
}
