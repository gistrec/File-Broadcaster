#ifndef FILEBROADCASTER_CONFIG_H
#define FILEBROADCASTER_CONFIG_H

#include "Utils.hpp"

std::string fileName;
int mtu; // Max packet size to send and receive

std::atomic<int> ttl; // Max time to wait new packets
int ttl_max;

SOCKET _socket;

SOCKADDR_IN server_address    = { 0 };
SOCKADDR_IN client_address    = { 0 };
SOCKADDR_IN broadcast_address = { 0 };

addr_len server_address_length = sizeof(server_address);
addr_len client_address_length = sizeof(client_address);

char*  file = nullptr; // Pointer to file in RAM
size_t file_length;    // File szie in bytes
char*  buffer;         // Pointer to buffer

#endif //FILEBROADCASTER_CONFIG_H
