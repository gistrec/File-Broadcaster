#ifndef FILEBROADCASTER_UTILS_H
#define FILEBROADCASTER_UTILS_H

#if defined(_WIN32) || defined(_WIN64)
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define addr_len int                    //
#include <winsock2.h>                   // Windows
#include <windows.h>                    //  socket
#pragma comment(lib, "Ws2_32.lib")      //
#else
#define SOCKET int                      //
#define SOCKADDR_IN sockaddr_in         //
#define addr_len socklen_t              // Linux socket
#include <arpa/inet.h>                  //
#endif

#include <thread>
#include <string>
#include <fstream>
#include <cstring> // memcpy
#include <chrono>

#include <map>
#include <vector>
#include <set>

#include "cxxopts.hpp"
#include "Config.hpp"


using namespace std::chrono_literals;


namespace Utils {
    /**
     * Return a coded number
     * @param  buffer - bytes array
     * @param  count  - bytes count
     */
    size_t getNumberFromBytes(char* buffer, int count) {
        size_t number = 0;
        for (int i = 0; i < count; i++) {
            number = number << 8;
            number = number | (buffer[i] & 0xFF);
        }
        return number;
    }

    /**
     * Write a coded number
     * @param buffer - ptr to write
     * @param value  - number
     * @param count  - count bytes
     */
    void writeBytesFromNumber(char* buffer, size_t number, int count) {
        for (int i = 0; i < count; i++) {
            buffer[count - i - 1] = (char) (number >> (i * 8));
        }
    }
}

#endif //FILEBROADCASTER_UTILS_H
