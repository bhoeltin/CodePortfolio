/* handler.c: HTTP Request Handlers */

#include "spidey.h"

#include <errno.h>
#include <limits.h>
#include <string.h>

#include <dirent.h>
#include <unistd.h>

/* Internal Declarations */
http_status handle_browse_request(struct request *request);
http_status handle_file_request(struct request *request);
http_status handle_cgi_request(struct request *request);
http_status handle_error(struct request *request, http_status status);

/**
 * Handle HTTP Request
 *
 * This parses a request, determines the request path, determines the request
 * type, and then dispatches to the appropriate handler type.
 *
 * On error, handle_error should be used with an appropriate HTTP status code.
 **/
http_status
handle_request(struct request *r)
{
    http_status result;

    /* Parse request */
    if (parse_request(r) < 0){
        result = handle_error(r, HTTP_STATUS_BAD_REQUEST);
        return result;
    }

    /* Determine request path */
    char* path = determine_request_path(r->uri);

    if (path == NULL) {
        result = handle_error(r, HTTP_STATUS_NOT_FOUND);
        return result;
    }
    r->path = strdup(path);
    debug("HTTP REQUEST PATH: %s", r->path);

    /* Dispatch to appropriate request handler type */
    request_type rt = determine_request_type((const char *)path);
    switch(rt){    
    case REQUEST_BROWSE:
      debug("HTTP REQUEST TYPE: BROWSE");
      result = handle_browse_request(r);
      if (result != HTTP_STATUS_OK){
	result = handle_error(r, result);
      }
      break;

    case REQUEST_CGI:
      debug("HTTP REQUEST TYPE: CGI");
      result = handle_cgi_request(r);
      if (result != HTTP_STATUS_OK){
        result = handle_error(r, result);
      }
      break;

    case REQUEST_FILE:
      debug("HTTP REQUEST TYPE: FILE");
      result = handle_file_request(r);
        if (result != HTTP_STATUS_OK){
           result = handle_error(r, result);
        }
      break;

    default:
      result = handle_error(r, HTTP_STATUS_NOT_FOUND);
      break;

    }

    free (path);
    log("HTTP REQUEST STATUS: %s", http_status_string(result));
    return result;
}

/**
 * Handle browse request
 *
 * This lists the contents of a directory in HTML.
 *
 * If the path cannot be opened or scanned as a directory, then handle error
 * with HTTP_STATUS_NOT_FOUND.
 **/
http_status
handle_browse_request(struct request *r)
{
    struct dirent **entries;
    int n;

    /* Open a directory for reading or scanning */
    r->file = fdopen(r->fd, "w+");
    if(r->file == NULL){
      http_status result = HTTP_STATUS_BAD_REQUEST;        
      return handle_error(r, result);
    }
    n = scandir(r->path, &entries, NULL, alphasort); 

    /* Write HTTP Header with OK Status and text/html Content-Type */

    fputs("HTTP/1.0 200 OK \r\nContent-Type: text/html\r\n",r->file);
    fputs("\r\n", r->file);
    fputs("<html>\r\n",r->file);
    fputs("<body>\r\n",r->file);
    fputs("<ul>\r\n", r->file);
    

    /* For each entry in directory, emit HTML list item */
    
    for (int i = 0; i < n; i++){
      char *name = entries[i]->d_name;
      if (streq(name, ".")) {
	//goto skip;
	free(entries[i]);
	continue;
      }
      if (strstr(name,".jpg") || strstr(name,".png") || strstr(name,".jpeg")){
	fprintf(r->file, "\t<img src=%s width=50 height =50/><li><a href = \"%s/%s\">%s</li>\r\n", name, streq(r->uri, "/") ? "" : r->uri, name, name);
      }

      else fprintf(r->file, "\t<li><a href = \"%s/%s\">%s</li>\r\n", streq(r->uri, "/") ? "" : r->uri, name, name);     
      
      free(entries[i]);
    }
    fputs("</ul>\r\n",r->file);
    fputs("</body>\r\n", r->file);
    fputs("</html>\r\n",r->file);
    free(entries);
    /* Flush socket, return OK */
    fflush(r->file);
    fclose(r->file);
    return HTTP_STATUS_OK;
    
}

/**
 * Handle file request
 *
 * This opens and streams the contents of the specified file to the socket.
 *
 * If the path cannot be opened for reading, then handle error with
 * HTTP_STATUS_NOT_FOUND.
 **/
http_status
handle_file_request(struct request *r)
{
    FILE *fs;
    char buffer[BUFSIZ];
    char *mimetype = NULL;
    size_t nread;

    /* Open file for reading */
    fs = fopen(r->path, "r");    

    if (fs == NULL){
      return handle_error(r,HTTP_STATUS_INTERNAL_SERVER_ERROR);
    }

    /* Determine mimetype */
    mimetype = determine_mimetype(r->path);    
    debug("mimetype: %s", mimetype);
    
    /* Write HTTP Headers with OK status and determined Content-Type */
    fprintf(r->file,"HTTP/1.0 200 OK \r\nContent-Type: %s\r\n",mimetype);
    fputs("\r\n",r->file);

    /* Read from file and write to socket in chunks */
    while ((nread = fread(buffer, sizeof(char), BUFSIZ, fs))) {
      if (fwrite(buffer, sizeof(char), nread, r->file) != nread) {
        debug("Error Writing to Socket: %s", strerror(errno));       
        goto fail;
      }
    }    
    /* Close file, flush socket, deallocate mimetype, return OK */
    fflush(r->file);
    fclose(fs);
    free(mimetype);
    return HTTP_STATUS_OK;

fail:
    if (fs) fclose(fs);
    free(mimetype);
    return HTTP_STATUS_INTERNAL_SERVER_ERROR;
}

/**
 * Handle file request
 *
 * This popens and streams the results of the specified executables to the
 * socket.
 *
 *
 * If the path cannot be popened, then handle error with
 * HTTP_STATUS_INTERNAL_SERVER_ERROR.
 **/
http_status
handle_cgi_request(struct request *r)
{
    FILE *pfs;
    char buffer[BUFSIZ];
    struct header *header;

    /* Export CGI environment variables from request:
    * http://en.wikipedia.org/wiki/Common_Gateway_Interface */

    setenv("QUERY_STRING", r->query, 1);
    setenv("DOCUMENT_ROOT",RootPath,1);
    setenv("REQUEST_URI",r->uri,1);
    setenv("REMOTE_PORT",r->port,1);
    setenv("REQUEST_METHOD",r->method,1);
    setenv("REMOTE_ADDRESS", r->host, 1);
    setenv("SCRIPT_FILENAME", r->path, 1);
    setenv("SERVER_PORT", Port, 1);

    /* Export CGI environment variables from request headers */
    for (header = r->headers; header != NULL; header = header->next) {
        if (streq("Host", header->name)) setenv("HTTP_HOST", header->value, 1);
        else if (streq("Accept", header->name)) setenv("HTTP_ACCEPT", header->value, 1);
        else if (streq("Accept-Language", header->name)) setenv("HTTP_ACCEPT_LANGUAGE", header->value, 1);
        else if (streq("Accept-Encoding", header->name)) setenv("HTTP_ACCEPT_ENCODING", header->value, 1);
        else if (streq("Connection", header->name)) setenv("HTTP_CONNECTION", header->value, 1);
        else if (streq("User-Agent", header->name)) setenv("HTTP_USER_AGENT", header->value, 1);
    }

    /* POpen CGI Script */

    pfs = popen(r->path,"r");
    if (pfs == NULL) {
        debug("Could not open path: %s", strerror(errno));
        http_status result = HTTP_STATUS_BAD_REQUEST;
        return handle_error(r, result);
    }

    /* Copy data from popen to socket */
    
    while(fgets(buffer, BUFSIZ, pfs)) {
        fputs(buffer, r->file);
    }

    /* Close popen, flush socket, return OK */
    fflush(r->file);
    pclose(pfs);
    return HTTP_STATUS_OK;
}

/**
 * Handle displaying error page
 *
 * This writes an HTTP status error code and then generates an HTML message to
 * notify the user of the error.
 **/
http_status
handle_error(struct request *r, http_status status)
{
    const char *status_string = http_status_string(status);

    /* Write HTTP Header */
    puts(r->headers->name);

    /* Write HTML Description of Error*/
    //fprintf(r->file,"HTTP/1.0 %s \r\nConnection: close\r\nContent-Type: text/html\r\n\r\n", status_string);
    fprintf(r->file,"HTTP/1.0 %s \r\n Content-Type: text/html\r\n\r\n", status_string);

    fprintf(r->file, "<h1> %s Error </h1>\r\n", status_string);                                              
    fprintf(r->file, "Better luck next time!");


    //fputs("\r\n</body>\r\n",r->file);
    //fputs("</html>\r\n",r->file);
    /* Return specified status */ 
    fflush(r->file);

    return status; 

}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
