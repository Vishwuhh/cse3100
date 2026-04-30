#define _GNU_SOURCE // MUST BE LINE 1

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT "3119" // the port client will be connecting to 

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

#include "socketio.c"

#define	LINE_SIZE	100

//_TMPL CUT
void die(char *s)
{
    if (errno)
        perror(s);
    else 
        fprintf(stderr, "Error: %s\n", s);
    exit(EXIT_FAILURE);
}

void print_line(int sid, char * buf, int n)
{
    if (recv_lines(sid, buf, n)) {
        fprintf(stderr, "recv_lines() returned -1.\n");
        exit(EXIT_FAILURE);
    }
    printf("%s", buf);
}

int send_int(int sid, int v)
{
    char buf[64];
    int n;

    n = snprintf(buf, sizeof(buf), "%d\n", v);
    return send_all(sid, buf, n);
}

int get_number(char *s, int *pv)
{
    if (sscanf(s, "%d\n", pv) != 1)
        return -1;
    return 0;
}
//_TMPL END

int main(int argc, char *argv[])
{
    int sockfd;
    char buf[LINE_SIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    if (argc != 2) {
        fprintf(stderr,"usage: client hostname\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                        p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            perror("client: connect");
            close(sockfd);
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure

    int min = 1, max, guess;
    int result = 0;

    char recv_buf[LINE_SIZE];

    // Get max
    if (recv_lines(sockfd, recv_buf, sizeof(recv_buf)) < 0) {
        fprintf(stderr, "Failed to receive max value.\n");
        close(sockfd);
        return 1;
    }
    printf("%s", recv_buf);
    get_number(recv_buf, &max);

    // Repeat the following until result is 0
    while (1) {
        // Calculate guess using binary search formula
        guess = min + (max - min) / 2;
        printf("My guess: %d\n", guess);
        
        // Send guess 
        if (send_int(sockfd, guess) < 0) {
            break;
        }

        // Receive result
        if (recv_lines(sockfd, recv_buf, sizeof(recv_buf)) < 0) {
            break;
        }

        // Check if the game is over.
        // S sends "0" (without newline) followed by the final message
        if (recv_buf[0] == '0') {
            // Print the received 0 on a separate line, then print the final message
            printf("0\n%s", recv_buf + 1); 
            break;
        } else {
            // Print out the 1 or -1
            printf("%s", recv_buf);
            
            get_number(recv_buf, &result);
            
            // Adjust min or max if necessary
            if (result == 1) { // 1 means guess is smaller than the target
                min = guess + 1;
            } else if (result == -1) { // -1 means guess is larger than the target
                max = guess - 1;
            }
        }
    }

    close(sockfd);
    return 0;
}