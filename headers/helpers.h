#ifndef HELPERS_H
#define HELPERS_H

void close_socket(int socket);
void* get_sock_addr(struct sockaddr_storage *storage);

#endif