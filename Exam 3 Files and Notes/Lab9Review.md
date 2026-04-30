## Lab 9 Review
- client.c
```C
// stream socket client demo

inet_ntop(p->ai_family, get_in_addr((structsockaddr*)p->ai_addr), s, sizeof(s));
printf("client: connecting to %s\n", s);

freeaddrinfo(servinfo); // add done within struct

int min = 1, max, guess;
int result = 0;

// BEGINNING OF GAME LOGIC

// get max possible value from server
// recv_lines reads from socket until new-line char is reached, and then null terminates
if(recv_lines(sockfd, buf, LINE_SIZE) < 0) {
    die("Failed to recieve max val from server");
}
// convert recieved text string to native C integer

```