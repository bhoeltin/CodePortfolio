/* forking.c: Forking HTTP Server */

#include "spidey.h"

#include <errno.h>
#include <signal.h>
#include <string.h>

#include <unistd.h>

/**
 * Fork incoming HTTP requests to handle the concurrently.
 *
 * The parent should accept a request and then fork off and let the child
 * handle the request.
 **/
void
forking_server(int sfd)
{
    struct request *request;
    http_status status;
    pid_t pid;

    /* Accept and handle HTTP request */
    while (true) {
    	/* Accept request */
        request = accept_request(sfd);
        pid = fork();

	
	/* Ignore children */
        signal(SIGCHLD, SIG_IGN);
	if (pid == 0) {
            close(sfd);
            status = handle_request(request);
            if (status != HTTP_STATUS_OK) {
                fprintf(stderr, "%s", strerror(errno));
                exit(1);
            }                  
            exit(0);
        }
        else if (pid > 0) {
            free_request(request);
        }
        else if (pid < 0) {
            fprintf(stderr, "%s", strerror(errno));
            exit(1);
        }

	
	/* Fork off child process to handle request */
        
    }

    /* Close server socket and exit*/
    close(sfd);
    exit(0);
}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
