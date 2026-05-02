#ifndef FILEBROADCASTER_UTILS_H
#define FILEBROADCASTER_UTILS_H

#if defined(_WIN32) || defined(_WIN64)
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define addr_len int
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#pragma comment(lib, "Ws2_32.lib")
#define CLOSE_SOCKET(s) closesocket(s)
#define CLEANUP_NETWORK() WSACleanup()
#else
#define SOCKET int
#define SOCKADDR_IN sockaddr_in
#define addr_len socklen_t
#define INVALID_SOCKET (-1)
#include <unistd.h>
#include <arpa/inet.h>
#define CLOSE_SOCKET(s) close(s)
#define CLEANUP_NETWORK() ((void)0)
#endif

#include <cstddef>


namespace Utils {
    /**
     * Decode an unsigned integer from a big-endian byte sequence.
     */
    inline size_t getNumberFromBytes(const char* buffer, int count) {
        size_t number = 0;
        for (int i = 0; i < count; i++) {
            number = number << 8;
            number = number | (static_cast<unsigned char>(buffer[i]));
        }
        return number;
    }

    /**
     * Encode an unsigned integer into a big-endian byte sequence.
     */
    inline void writeBytesFromNumber(char* buffer, size_t number, int count) {
        for (int i = 0; i < count; i++) {
            buffer[count - i - 1] = static_cast<char>(number >> (i * 8));
        }
    }
}

#endif //FILEBROADCASTER_UTILS_H
