#ifndef FILEBROADCASTER_UTILS_H
#define FILEBROADCASTER_UTILS_H

#if defined(_WIN32) || defined(_WIN64)  //
#include <winsock2.h>                   //
#include <windows.h>                    // Windows socket and threads
#include <thread>                       //
#pragma comment(lib, "Ws2_32.lib")      //
#else
#include <arpa/inet.h>                  // Linux socket
#include <pthread.h>                    //  and threads
#endif

#include <string>
#include <iostream>
#include <fstream>
#include <string.h> // memcpy
#include <map>
#include <chrono>

#include <random> // Для тестов
#include <ctime>  // Для тестов
#include <math.h>

#include <vector>
#include <set>
#include <atomic>

#include "../lib/cxxopts/include/cxxopts.hpp"
#include "Config.hpp"

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define async


using namespace std::chrono_literals;


namespace Utils {

	/**
	 * Return a coded number
	 * @param  buffer - bytes array
	 * @param  count  - bytes count
	 */
	int getIntFromBytes(char* buffer, int count) {
		int value = 0;
		for (int i = 0; i < count; i++) {
			value = value << 8;
			value = value | (buffer[i] & 0xFF);
		}
		return value;
	}

	/**
	 * Write a coded number
	 * @param buffer - ptr to write
	 * @param value  - number
	 * @param count  - count bytes
	 */
	void writeBytesFromInt(char* buffer, size_t value, int count) {
		for (int i = 0; i < count; i++) {
			buffer[count - i - 1] = (char) (value >> (i * 8));
		}
	}

	/**
	 * Run in secondary thread
	 * Decrease ttl every second
	 * If ttl < 0 program should stop 
	 */
	void async checkTTL() {
		while (ttl--) {
			std::this_thread::sleep_for(1s);
		}
		std::cout << "Process no longer be working" << std::endl;
	}
}

#endif //FILEBROADCASTER_UTILS_H
