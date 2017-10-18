#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include "http.h"

#define BUF_SIZE 1024


Buffer *new_buffer() {

	Buffer *buff = malloc(sizeof(Buffer));
	
	(buff->data) = malloc(sizeof(char));
	(buff->length) = 0;
	
	return buff;

}


void append_buffer(Buffer *buffer, char *data, size_t length) {

	/* 
	 * 1. realloc so when more memory re-allocated to the buffer the 
	 * original data won't get lost
	 * 
	 * 2. memset will copy binary data like '\0'
	*/
	buffer -> data = realloc(buffer->data, buffer->length + length);
	memcpy((buffer->data) + (buffer->length), data, length);
	buffer->length += length;

}


int setup_and_connect(char *host, int port) {

	//assert(0 && "not implemented yet!");
    char service_or_port[20];
    struct addrinfo hints; // server's addrinfo hints
    struct addrinfo *servinfo = NULL; // server's addrinfo
    
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    sprintf(service_or_port, "%d", port);
    
    if (getaddrinfo(host, service_or_port, &hints, &servinfo) != 0) {
		perror("ERROR getting address info from host");
		exit(1);
	}
	
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	if (sockfd == -1) {
		perror("ERROR creating socket");
		exit(1);
	}
	
	if (connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
		perror("ERROR connecting server");
		exit(1);
	}
	
	freeaddrinfo(servinfo);
	return sockfd;
}


Buffer* http_response(int sockfd) {

	Buffer *response = new_buffer();
	
	int response_bytes = 0;
	
	do {
		char data[BUF_SIZE] = {'\0'};
		response_bytes = recv(sockfd, data, BUF_SIZE-1, 0);
		
		if (response_bytes > 0) {
			append_buffer(response, data, (size_t) response_bytes);
		} else if (response_bytes == -1) {
			perror("ERROR receiving failed");
			buffer_free(response);
			close(sockfd);
			exit(1);
		}
	} while (response_bytes > 0);
	
	return response;
}


Buffer* http_query(char *host, char *page, int port) {

	int sockfd = setup_and_connect(host, port);
	
	char http_get_request[BUF_SIZE] = {'\0'};
	
	// formatting the request string and write it to http_get_request
	char *get_quest;
	if (page[0] == '/') {
		get_quest = "GET %s HTTP/1.0\r\n"
						"Host: %s\r\n"
						"User-Agent: getter\r\n\r\n";
	} else {
		get_quest = "GET /%s HTTP/1.0\r\n"
						"Host: %s\r\n"
						"User-Agent: getter\r\n\r\n";
	}
	
	int request_bytes = snprintf(http_get_request, BUF_SIZE, 
							get_quest, page, host);
	
	if (request_bytes >= BUF_SIZE) {
		perror("ERROR sending the request; too long");
		close(sockfd);
		exit(1);
	}
	
	int send_bytes = send(sockfd, http_get_request, BUF_SIZE, 0);
	
	if (send_bytes == -1) {
		perror("ERROR request-sending failed");
		close(sockfd);
		exit(1);
	}
	
	Buffer *response = http_response(sockfd);
	
	close(sockfd);
	return response;

}


// split http content from the response string
char* http_get_content(Buffer *response) {

    char* header_end = strstr(response->data, "\r\n\r\n");

    if (header_end) {
        return header_end + 4;
    }
    else {
        return response->data;
    }

}


Buffer *http_url(const char *url) {

    char host[BUF_SIZE];
    strncpy(host, url, BUF_SIZE);

    char *page = strstr(host, "/");
    if (page) {
        page[0] = '\0';

        ++page;
        return http_query(host, page, 80);
    }
    else {

        fprintf(stderr, "could not split url into host/page %s\n", url);
        return NULL;
    }

}
