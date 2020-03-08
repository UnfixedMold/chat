#include "../headers/helpers.h"

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

int send_msg(int sockfd, char *msg) {

    uint16_t msglen = strlen(msg);

    if(msglen == 0) {
        return 0;
    }

    uint16_t buflen = msglen + 2, bsent = 0;

    char *buf = malloc(buflen);

    if(buf == NULL) {
        return -1;
    }

    memcpy(buf, &buflen, 2);
    memcpy(buf + 2, msg, msglen);
    
    while(bsent < buflen) {

        int n = send(sockfd, (buf + bsent), buflen - bsent, 0);
        
        if(n == -1) {
            free(buf);
            return -1;
        }

       bsent += n;
    }

    free(buf);

    return msglen;
}

int recv_msg(int sockfd, char *msg) {

	uint16_t brecv = 0, buflen = 2, msglen;
	char buf[BUFSIZE];

	while(brecv < buflen) {
		uint16_t n = recv(sockfd, buf + brecv, BUFSIZE, 0);

		if(n<=0) {
			return n;
		}

		brecv+=n;

		if(brecv >= buflen) {
			memcpy(&buflen, buf, 2);
			msglen = buflen - 2;
		}
	}

	memcpy(msg, buf + 2, msglen);
	msg[msglen] = '\0';

	return msglen;
}
