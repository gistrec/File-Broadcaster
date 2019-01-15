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
 * Run in secondary thread
 * Start while receive packet 'FINISH'
 * Get empty part's and requests it's from the server
 */
void async checkParts() {
	char* buffer = new char[100];

	std::vector<int> emptyParts = getEmptyParts();

	while (emptyParts.size() > 0) {
		for (auto index : emptyParts) {
			snprintf(buffer, 6, "RESEND");                               //
			Utils::writeBytesFromInt(buffer + 6, index, 4);              // Create request packet
			sendto(_socket, buffer, 10, 0, (sockaddr*) &server_address,  //
				   sizeof(server_address));                              //

			std::cout << "Requested " << index << " part" << std::endl;
		}
		std::this_thread::sleep_for(1s);
		emptyParts = getEmptyParts();
	}

	std::ofstream output("output.txt", std::ofstream::binary);  // 
	output.write(file, file_length);                            // Save file
	system("CertUtil -hashfile output.txt SHA256");             //
}


void run(cxxopts::ParseResult &options) {
	std::thread(checkTTL); // Create thread, to check server timeout

	char* buffer = new char[2 * mtu];

	while (int length = recvfrom(_socket, (char *)&buffer, sizeof(buffer), 0, (sockaddr*) &server_address, &server_address_length)) {
		ttl = options["ttl"].as<int>(); // Update ttl

		if (strncmp(buffer, "NEW_PACKET", 10) == 0) {
			file_length = Utils::getIntFromBytes(buffer + 10, 4);

			file = new char[file_length];
			memset(file, 0, file_length);

			std::cout << "Сейчас сервер начнет передавать файл размером" << file_length << std::endl;
			std::cout << "Всего " << int((float)file_length / (float)mtu + 0.5) << " частей" << std::endl;
		} else if (strncmp(buffer, "TRANSFER", 8) == 0) {
			int part = Utils::getIntFromBytes(buffer + 8, 4);  // Номер части файла
			int size = Utils::getIntFromBytes(buffer + 12, 4); // Размер передаваемых данных
			parts.insert(part);
			std::cout << "Сервер передал " << part << " часть с размером " << size << std::endl;

			memcpy(file + part * options["mtu"].as<int>(), buffer + 16, size);
		} else if (strncmp(buffer, "FINISH", 6) == 0) {
			std::cout << "Сервер завершил передавать файл" << std::endl;

			std::cout << "Создаем поток который будет проверять все части" << std::endl;
			std::thread(checkParts);
		}
	}
	delete[] buffer;
}

} //namespace Receiver