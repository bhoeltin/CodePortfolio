/* request.c: HTTP Request Functions */

#include "spidey.h"

#include <errno.h>
#include <string.h>

#include <unistd.h>

int parse_request_method(struct request *r);
int parse_request_headers(struct request *r);

/**
 * Accept request from server socket.
 *
 * This function does the following:
 *
 *  1. Allocates a request struct initialized to 0.
 *  2. Initializes the headers list in the request struct.
 *  3. Accepts a client connection from the server socket.
 *  4. Looks up the client information and stores it in the request struct.
 *  5. Opens the client socket stream for the request struct.
 *  6. Returns the request struct.
 *
 * The returned request struct must be deallocated using free_request.
 **/
struct request *
accept_request(int sfd)
{
    struct request *r;
    struct sockaddr raddr;
    socklen_t rlen;

    /* Allocate request struct (zeroed) */
    r = calloc(1, sizeof(struct request));
    r->headers = NULL;
    rlen = sizeof(struct sockaddr);
    /* Accept a client */
    r->fd = accept(sfd, &raddr, &rlen);
    if (r->fd < 0) {
        fprintf(stderr, "Unable to accept: %s\n", strerror(errno));
	return NULL;
    }

    /* Lookup client information */
  
    int clientinfo;

    if ((clientinfo = getnameinfo((struct sockaddr *) &raddr, rlen, r->host, sizeof(r->host), NULL, 0, NI_NOFQDN)) != 0) {
        fprintf(stderr, "Unable to look up client: %s\n", gai_strerror(clientinfo));
        goto fail;
    }    

    /* Open socket stream */
    // TODO: save this to r->file
    r->file = fdopen(r->fd, "w+");
    if (r->file == NULL) {
        fprintf(stderr, "Unable to fdopen: %s\n", strerror(errno));
        close(r->fd);
    }
    log("Accepted request from %s:%s", r->host, r->port);
    return r;

fail:
    free_request(r);
    return NULL;
}

/**
 * Deallocate request struct.
 *
 * This function does the following:
 *
 *  1. Closes the request socket stream or file descriptor.
 *  2. Frees all allocated strings in request struct.
 *  3. Frees all of the headers (including any allocated fields).
 *  4. Frees request struct.
 **/
void
free_request(struct request *r)
{
    struct header *header;
    header = r->headers;


    if (r == NULL) {
    	return;
    }

    /* Close socket or fd */
    close(r->fd);

    /* Free allocated strings */
    free(r->method);
    free(r->uri);
    free(r->path);
    free(r->query);

    /* Free headers */
    header = r->headers;
    while (header) {
        free(header->name);
        free(header->value);
	free(header);
        header = header->next;
    }

    /* Free request */
    free(r);
}

/**
 * Parse HTTP Request.
 *
 * This function first parses the request method, any query, and then the
 * headers, returning 0 on success, and -1 on error.
 **/
int
parse_request(struct request *r)
{
    /* Parse HTTP Request Method */
  //    debug("Parsing request method...");
    int pmethod = parse_request_method(r);          

    /* Parse HTTP Requet Headers*/
    // debug("Parsing request headers...");
    int pheader = parse_request_headers(r);

    if (pmethod == 0 && pheader == 0) return 0;
    else return 1;
}

/**
 * Parse HTTP Request Method and URI
 *
 * HTTP Requests come in the form
 *
 *  <METHOD> <URI>[QUERY] HTTP/<VERSION>
 *
 * Examples:
 *
 *  GET / HTTP/1.1
 *  GET /cgi.script?q=foo HTTP/1.0
 *
 * This function extracts the method, uri, and query (if it exists).
 **/
int
parse_request_method(struct request *r)
{
    char buf[BUFSIZ];

    /* Read line from socket */
    // TODO: Use r->file
    /*FILE *fs = fdopen(r->fd, "r");*/
    if (r->file == NULL) {
        debug("Could not open file: %s", strerror(errno));
        goto fail;
    }
    if(fgets(buf, BUFSIZ, r->file) == NULL) {
        if (r->file) fclose(r->file);
        goto fail;
    }         

    /* Parse method and uri */
    char *method = strtok(skip_whitespace(buf), WHITESPACE);
    char *uri = strtok(NULL, WHITESPACE);

    if (method == NULL || uri == NULL) {
        goto fail;
    }    

    /* Parse query from uri */
    char *query = strchr(uri, '?');
    if (query != NULL) {
        *query = '\0';
        query++;
        r->query = strdup(query);
    }
    // TODO: Need to check if query is valid

    /* Record method, uri, and query in request struct */
    r->method = strdup(method); 
    r->uri = strdup(uri);

    debug("HTTP METHOD: %s", r->method);
    debug("HTTP URI:    %s", r->uri);
    debug("HTTP QUERY:  %s", r->query);

    return 0;

fail:
    return -1;
}

/**
 * Parse HTTP Request Headers
 *
 * HTTP Headers come in the form:
 *
 *  <NAME>: <VALUE>
 *
 * Example:
 *
 *  Host: localhost:8888
 *  User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:29.0) Gecko/20100101 Firefox/29.0
 *  Accept: text/html,application/xhtml+xml
 *  Accept-Language: en-US,en;q=0.5
 *  Accept-Encoding: gzip, deflate
 *  Connection: keep-alive
 *
 * This function parses the stream from the request socket using the following
 * pseudo-code:
 *
 *  while (buffer = read_from_socket() and buffer is not empty):
 *      name, value = buffer.split(':')
 *      header      = new Header(name, value)
 *      headers.append(header)
 **/
int
parse_request_headers(struct request *r)
{
    char buffer[BUFSIZ];
    //char *name;
    char *value;
    
    while(fgets(buffer, BUFSIZ, r->file) && strlen(skip_whitespace(buffer))) {
      // if (streq(buffer,"\r\n")) break;
        chomp(buffer);
        value = strchr(buffer, ':');
        if (value == NULL) continue;

	struct header *new = calloc(1, sizeof(struct header));

        *value = '\0';
        value++;
        new->value = strdup(skip_whitespace(value));
    
        new->name = strdup(skip_whitespace(buffer));
        new->next = r->headers;
        r->headers = new;
	//free(new);
  }
    
#ifndef NDEBUG
    for (struct header *header = r->headers; header != NULL; header = header->next) {
    	debug("HTTP HEADER %s = %s", header->name, header->value);
    }
#endif
    return 0;
}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
