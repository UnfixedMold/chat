#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

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

#define PORT "3490"

void close_socket(int socket) {

	#ifdef _WIN32
		closesocket(socket);
	#else
		close(socket);
	#endif

}

void* get_sock_addr(struct sockaddr_storage *storage) {

	if(storage->ss_family == AF_INET) {
		return &((struct sockaddr_in*)storage)->sin_addr;
	}

	return &((struct sockaddr_in6*)storage)->sin6_addr;
}

int get_listener_socket() {

	struct addrinfo hints, *servinfo;
	int status, listener;
	
	#ifdef _WIN32
		char yes = '1';
	#else
		int yes = 1;
	#endif

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	status = getaddrinfo(NULL, PORT, &hints, &servinfo);

	if(status == -1) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
		return -1;
	}

	struct addrinfo *res = servinfo;

	for(struct addrinfo *res = servinfo; res != NULL; res=res->ai_next) {

		listener = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

		if(listener == -1) {
			perror("listener socket");
			continue;
		}

		if(setsockopt(listener,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof yes) == -1) {
			perror("setsockopt");
		}

		if(bind(listener, res->ai_addr, res->ai_addrlen) == -1) {
			perror("bind");
			close_socket(listener);
			continue;
		}

		break;
	}

	if (res == NULL) {
		freeaddrinfo(servinfo);
		return -1;
	}

  	freeaddrinfo(servinfo);

	if (listen(listener, 10) == -1) {
		perror("listen");
		return -1;
  	}

	return listener;
}

int main() {

	#ifdef _WIN32
		WSADATA data;
    	WSAStartup(MAKEWORD(2,2),&data);    
	#endif

	int listener = get_listener_socket();

	if(listener == -1) {
		fprintf(stderr, "error getting listening socket\n");
		exit(1);
	}

	printf("Listening on port %s\n", PORT);

	fd_set readfds, tempreadfds;
	struct sockaddr_storage clientsockaddr;
	int fdmax = 0;

	FD_ZERO(&readfds);
	FD_ZERO(&tempreadfds);

	FD_SET(listener, &readfds);

	fdmax = listener;

	while(1) {
		tempreadfds = readfds;

		if(select(fdmax + 1, &tempreadfds, NULL, NULL, NULL) == -1) {
			perror("select");
			exit(2);
		}

		for(int i=0; i<=fdmax; i++) {
			if(FD_ISSET(i, &tempreadfds)) {
				if(i == listener) {
					int clientsockaddrlen = sizeof clientsockaddr;
					int clientfd = accept(listener, (struct sockaddr*)&clientsockaddr, &clientsockaddrlen);

					if(clientfd == -1) {
						perror("accept");
					} else {
						FD_SET(clientfd, &readfds);

						if(clientfd > fdmax) {
							fdmax = clientfd;
						}

						#ifndef _WIN32

						char ip[INET6_ADDRSTRLEN];

						inet_ntop(clientsockaddr.ss_family, get_sock_addr(&clientsockaddr), ip, INET6_ADDRSTRLEN);

						if(ip<=0) {
							perror("inet_ntop");
						} else {
							printf("Client %s connected!", ip);
						}

						#else
							printf("Client connected!");
						#endif
					}
				}
			};
		}
	}

	#ifdef _WIN32
		WSACleanup();
	#endif

	return 0;
}

