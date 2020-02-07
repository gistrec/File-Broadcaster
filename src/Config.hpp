#ifndef FILEBROADCASTER_CONFIG_H
#define FILEBROADCASTER_CONFIG_H

#include "Utils.hpp"


std::string fileName; // File Name to transfer or receive

int mtu;     // Max packet size to send and receive

int ttl;     // Current wait time for new packages before shutting down
int ttl_max; // Maximum wait time for new packages before shutting down

SOCKET _socket;

SOCKADDR_IN server_address    = { 0 };
SOCKADDR_IN client_address    = { 0 };
SOCKADDR_IN broadcast_address = { 0 };

addr_len server_address_length = sizeof(server_address);
addr_len client_address_length = sizeof(client_address);

size_t file_length; // File size in bytes
char*  file   = nullptr; // Pointer to file in RAM
char*  buffer = nullptr; // Pointer to buffer

#endif //FILEBROADCASTER_CONFIG_H
