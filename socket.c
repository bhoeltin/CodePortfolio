/* socket.c: Simple Socket Functions */

#include "spidey.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

/**
 * Allocate socket, bind it, and listen to specified port.
 **/
int
socket_listen(const char *port)
{
    /* Lookup server address information */
    struct addrinfo hints = {
        .ai_family = AF_UNSPEC, // use either IPv4 or IPv6
        .ai_socktype = SOCK_STREAM, // use TCP
        .ai_flags = AI_PASSIVE, // use all interfaces to listen
    };
    struct addrinfo *results;
    int status;
    if ((status = getaddrinfo(NULL, port, &hints, &results)) != 0) {
        fprintf(stderr, "getaddrinfo failed: %s\n", gai_strerror(status));
        return -1;
    }
    
    /* For each server entry, allocate socket and try to connect */
    int server_fd = -1;
    for (struct addrinfo *p = results; p != NULL && server_fd < 0; p = p->ai_next) {	
        /* Allocate socket */
        if((server_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) {
            fprintf(stderr, "socket failed: %s\n", strerror(errno));
            continue;
        }
	/* Bind socket */
        if (bind(server_fd, p->ai_addr, p->ai_addrlen) < 0) {
            fprintf(stderr, "bind failed: %s\n", strerror(errno));
            close(server_fd);
            server_fd = -1;
            continue;
        }

    	/* Listen to socket */
        if (listen(server_fd, SOMAXCONN) < 0) {
            fprintf(stderr, "listen failed: %s\n", strerror(errno));
            close(server_fd);
            server_fd = -1;
            continue;
        }
    }   

    freeaddrinfo(results);
    return server_fd;
}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
