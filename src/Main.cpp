#include "../lib/cxxopts/include/cxxopts.hpp"

#include "Receiver.cpp"
#include "Sender.cpp"
#include "Config.hpp"


int main(int argc, char* argv[]) {
	// Parse input options from CLI
	cxxopts::Options options("File-Broadcaster", "UDB Broadcast file transfer");

	options
		.positional_help("[optional args]")
		.show_positional_help();

	options.add_options(
		("p,port",    "Port",               cxxopts::value<int>()->default_value("33333"))
		("f,file",    "File name",          cxxopts::value<std::string>())
		("t,type",    "Receiver or sender", cxxopts::value<std::string>()->default_value("receiver"))
		("ttl",       "Time to live",       cxxopts::value<std::string>()->default_value("15"))
		("mtu,MTU",   "MTU packet",         cxxopts::value<int>()->default_value("1500"))
		("broadcast", "Broadcast address",  cxxopts::value<std::string>()->default_value("yes"))
	);
	auto result = options.parse(argc, argv);

	#if defined(_WIN32) || defined(_WIN64)   //
	WORD socketVer;                          // Initialize use 
	WSADATA wsaData;                         //     of the Winsock DLL 
	socketVer = MAKEWORD(2, 2);              //         by a process.
	WSAStartup(socketVer, &wsaData);         //
	#endif

	mtu = result["mtu"].as<int>();           // Init some var
	ttl = ttl_max = result["ttl"].as<int>(); // 
	
	_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);   //
	if (_socket < 0) {                                   // Create socket with
		std::cout << "Can't create socket" << std::endl; //     datagram-based protocol
		exit(_socket);                                   //
	}                                                    //

	client_address.sin_family = AF_INET;                         //
	client_address.sin_port = htons(result["port"].as<int>());   // Create local address
	client_address.sin_addr.s_addr = INADDR_ANY;                 //

	broadcast_address.sin_family = AF_INET;                       // Create server
	broadcast_address.sin_port = htons(result["port"].as<int>()); //       address

	if (result["broadcast"].as<std::string>() == "yes") {                   //
		char broadcastEnable = 1;                                           // Give access to
		if (setsockopt(_socket, SOL_SOCKET, SO_BROADCAST,                   //     broadcast address
					   &broadcastEnable, sizeof(broadcastEnable))) {        //
			std::cout << "Права на броадкаст выданы" << std::endl;          //
		} else {                                                            //
			std::cerr << "Не удалось дать права на броадкаст" << std::endl; //
			exit(1);                                                        //
		}                                                                   // If option 'broadcast' is yes
		broadcast_address.sin_addr.s_addr = htonl(INADDR_BROADCAST);        // change server address
	} else {                                                                // to broadcast
		broadcast_address.sin_addr.s_addr =                                 // Else change server address
			inet_addr(result["broadcast"].as<std::string>().c_str());       // to address in option
	}                                                                       //


	if (bind(_socket, (sockaddr *)&client_address, sizeof(client_address))) {//
		std::cout << "Сокет забинден" << std::endl;                          // 
	} else {                                                                 // Bind socket to
		std::cerr << "Не удалось забиндить сокет" << std::endl;              //     client address
		exit(1);                                                             //
	}                                                                        //

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