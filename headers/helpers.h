#ifndef HELPERS_H
#define HELPERS_H

#include <sys/types.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#ifdef _WIN32
	#define _WIN32_WINNT 0x0A00
	#include <winsock2.h>
	#include <ws2tcpip.h>
#else
	#include <sys/socket.h>
	#include <netdb.h>
	#include <unistd.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
#endif

#define STDIN 0
#define BUFSIZE 1024
#define MSGSIZE (BUFSIZE - 2)

void close_socket(int socket);
void* get_sock_addr(struct sockaddr_storage *storage);
int send_msg(int sockfd, char *msg);
int recv_msg(int sockfd, char *msg);


#endif