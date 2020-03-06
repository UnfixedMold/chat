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