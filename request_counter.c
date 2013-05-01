#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <pthread.h>

int _requests = 0;

void error(const char *msg) {
	perror(msg);
	exit(1);
}

void *print_reqs_per_sec() {
	while(1) {
		int reqs = _requests;
		_requests = 0;
		printf("%i reqs / sec\n", reqs);
		fflush(stdout);
		sleep(1);
	}
}

int main(int argc, char *argv[]) {
	int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    if(argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
        
    // Start thread
    pthread_t thread;
    int t = pthread_create(&thread, NULL, print_reqs_per_sec, NULL);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) 
       error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
    	error("ERROR on binding");
    while(1) {
		listen(sockfd,5);
		clilen = sizeof(cli_addr);
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0) 
			error("ERROR on accept");
		bzero(buffer,256);
		n = read(newsockfd,buffer,255);
		if (n < 0) error("ERROR reading from socket");
		_requests++;
		char* response = "HTTP/1.1 200 OK\n\n";
		n = write(newsockfd, response, 19);
		if (n < 0) error("ERROR writing to socket");
		close(newsockfd);
    }
    
    pthread_join(thread, NULL);
    close(sockfd);
    return 0; 
}
