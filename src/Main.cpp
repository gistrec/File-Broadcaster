#include "cxxopts.hpp"

#include "Receiver.cpp"
#include "Sender.cpp"
#include "Config.hpp"


int main(int argc, char* argv[]) {
    // Parsing input parameters from the CLI
    cxxopts::Options options("File-Broadcaster", "UDB Broadcast file transfer");

    options
        .positional_help("[optional args]")
        .show_positional_help();

    options.add_options()
        ("f,file",    "File name",          cxxopts::value<std::string>()->default_value("file.out"))
        ("t,type",    "Receiver or sender", cxxopts::value<std::string>()->default_value("sender"))
        ("broadcast", "Broadcast address",  cxxopts::value<std::string>()->default_value("yes"))
        ("p,port",    "Port",               cxxopts::value<int>()->default_value("33333"))
        ("mtu",       "MTU packet",         cxxopts::value<int>()->default_value("1500"))
        ("ttl",       "Time to live",       cxxopts::value<int>()->default_value("15"));

    auto result = options.parse(argc, argv);

    #if defined(_WIN32) || defined(_WIN64)   //
    WORD socketVer;                          // Initializing the use
    WSADATA wsaData;                         //     of the Winsock DLL
    socketVer = MAKEWORD(2, 2);              //         by this process.
    WSAStartup(socketVer, &wsaData);         //
    #endif                                   //

    mtu = result["mtu"].as<int>();               //
    ttl = result["ttl"].as<int>();               // Initializing some variable
    ttl_max  = result["ttl"].as<int>();          //
    fileName = result["file"].as<std::string>(); //


    _socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);          //
    if (_socket < 0) {                                          // Create socket with
        std::cout << "Error: Can't create socket" << std::endl; //     datagram-based protocol
        exit(1);                                                //
    } else {                                                    //
        std::cout << "Ok: Socket created" << std::endl;         //
    }                                                           //

    client_address.sin_family = AF_INET;                         //
    client_address.sin_port = htons(result["port"].as<int>());   // Creating local address
    client_address.sin_addr.s_addr = INADDR_ANY;                 //

    memcpy(&server_address, &client_address, sizeof(server_address)); // Server address == Client address

    broadcast_address.sin_family = AF_INET;                         // Creating broadcast
    broadcast_address.sin_port = htons(result["port"].as<int>());   //       address

    if (result["broadcast"].as<std::string>() == "yes") {                      //
        #if defined(_WIN32) || defined(_WIN64)                                 // Getting access to
        char broadcastEnable = '1';                                            //  the broadcast address
        #else                                                                  //
        int broadcastEnable = 1;                                               //
        #endif                                                                 //
                                                                               //
        if (setsockopt(_socket, SOL_SOCKET, SO_BROADCAST,                      //
                       &broadcastEnable, sizeof(broadcastEnable)) != 0) {      //
            std::cout << "Ok: Got access to broadcast" << std::endl;           //
        } else {                                                               //
            std::cerr << "Error: Cant't get access to broadcast" << std::endl; //
            exit(1);                                                           //
        }                                                                      // If parameter "broadcast" is "yes", then
        broadcast_address.sin_addr.s_addr = INADDR_BROADCAST;                  //    change server address
    } else {                                                                   //    to broadcast
        broadcast_address.sin_addr.s_addr =                                    // Else change server address
            inet_addr(result["broadcast"].as<std::string>().c_str());          //    to address in parameter
    }                                                                          //

    if (bind(_socket, (sockaddr *)&client_address, sizeof(client_address)) == 0) {//
        std::cout << "Ok: Socket binded" << std::endl;                            //
    } else {                                                                      // Bind socket to
        std::cerr << "Error: Can't bind socket" << std::endl;                     //     client address
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
        std::cerr << "Error: Type not found" << std::endl;       //
    }

    return 0;
}
