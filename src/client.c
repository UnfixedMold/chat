#include <stdio.h>
#include <stdint.h>

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

#include "../headers/helpers.h"

int get_server_socket(char *hostname, char *port) {

    struct addrinfo hints, *servinfo;
    int status;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

    status = getaddrinfo(hostname, port, &hints, &servinfo);

    if(status != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
		return -1;
	}

    int sockfd= -1;
    struct addrinfo *res = servinfo;

    for(; res != NULL; res = res->ai_next) {
        if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
            perror("socket");
            continue;
        }

        if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
            close_socket(sockfd);
            perror("connect");
            continue;
        }

        break;
    }

    if (res == NULL) {
		freeaddrinfo(servinfo);
		return -1;
	}

    freeaddrinfo(servinfo);

    return sockfd;
}

int main(int argc, char *argv[]) {

    if(argc !=3 ) {
       fprintf(stderr,"err: hostname and port expected\n");
       exit(1);
    }

    char *hostname = argv[1], *port = argv[2];

    #ifdef _WIN32
		WSADATA data;
    	WSAStartup(MAKEWORD(2,2),&data);    
	#endif
    

    const int servfd = get_server_socket(hostname, port);

    if(servfd == -1) {
        printf("failed to get server socket\n");
        exit(2);
    }

    fd_set readfds, tempreadfds;
    int fdmax = servfd;

    FD_ZERO(&readfds);
    FD_ZERO(&tempreadfds);
    FD_SET(STDIN, &readfds);
    FD_SET(servfd, &readfds);

    while(1) {

        tempreadfds = readfds;

		if(select(fdmax + 1, &tempreadfds, NULL, NULL, NULL) == -1) {
			perror("select");
            close_socket(servfd);
			exit(2);
		}

        for(int i=0; i<=fdmax; i++) {
			if(FD_ISSET(i, &tempreadfds)) {

                if(i == STDIN) {
                    printf("key pressed\n");

                } else if(i == servfd) {
                    
                    char msg[MSGSIZE];

                    int msglen = recv_msg(i, msg);

					if(msglen <= 0) {

						switch(msglen) {
							case -1:
								perror("recv");
								break;
							case 0:
								printf("socket %d connection closed!\n", i);
								break;
						};
						close_socket(i);
						FD_CLR(i, &readfds);
					} else {
						printf("%d: %s\n", i, msg);
					}
                }
            }
        }
    }

    // }

    // char msg[MSGSIZE] ="hellohellohellohellohellohellohelhellohellohellohellohellohellohelhellohellohellohellohellohellohel";
    // int msglen = send_msg(servfd, msg);

    // if(msglen == -1) {
    //     perror("send");
    //     close_socket(servfd);
    // }
    
    printf("should close socket \n");
    close_socket(servfd);

	#ifdef _WIN32
		WSACleanup();
	#endif

    return 1;
}