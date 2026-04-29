#ifndef FILEBROADCASTER_CONFIG_H
#define FILEBROADCASTER_CONFIG_H

#include "Utils.hpp"


extern std::string fileName; // File Name to transfer or receive

extern int mtu;     // Max packet size to send and receive

extern int ttl;     // Current wait time for new packages before shutting down
extern int ttl_max; // Maximum wait time for new packages before shutting down

extern SOCKET _socket;

extern SOCKADDR_IN server_address;
extern SOCKADDR_IN client_address;
extern SOCKADDR_IN broadcast_address;

extern addr_len server_address_length;
extern addr_len client_address_length;

extern size_t file_length; // File size in bytes
extern char*  file;        // Pointer to file in RAM
extern char*  buffer;      // Pointer to buffer

#endif //FILEBROADCASTER_CONFIG_H
