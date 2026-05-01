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
if(get_number(buf, max) < 0) {
    die("failed to parse max value");
}

// interactive loop
do {
    // calculates next target val with binary search logic
    guess = (min + max) / 2;
    printf("My guess: %d\n", guess);

    // send the integer
    // send_int handles converting guess into a string and writing into socket FD
    if(send_int(sockfd, guess) < 0) {
        die("Failed to send guess to server");
    }

    // stop execution until server replies with grade
    if(recv_lines(sockfd, buf, LINE_SIZE) < 0) {
        die("Failed to recieve result from server");
    }

    // extract integer result from the string buffer
    // acceptable values: -1, 1, and 0
    if(sscanf(buf, "%d\n", &result) != 1) {
        die("Failed to parse the result");
    }

    // print numeric feedback
    if(result != 0) {
        printf("%d\n", result);
    } else {
        printf("0\n");
    }

    // adjust search boundaries based on the feedback
    if(result > 0) {
        min = guess + 1;
    } else if(result < 0) {
        max = guess - 1;
    } else {
        // this is the victory condition
        // server sends the "0\n" string and final message back to back
        // since recv_lines reads all things available, final message is already sitting in the buffer after the first newline

        // strchr finds memory addresws of the first newline
        char *msg_start = strchr(buf, '\n');
        if(msg_start != NUL && *(msg_start +1) != ''\0) {
            // adding one to the pointer skips the newline char, and prints everything following it
            printf("%s", msg_start + 1);
        }
    } while(result != 0);

    // close socket FD, which releases the OS resources
    close(sockfd);
    return 0;
}
```

- server.c
```C
void *thread_main(void *arg_in) {
    // unpack args passed from the main thread
    thread_arg_t *arg = arg_in;
    int sockfd = arg->sockfd // retrieves sock FD from the memory address of arg

    // need local buffer to build string payloads
    char buf[MSG_BUF_SIZE];
    int n;

    // ----------
    // GAME LOGIC
    // ----------

    // tell client highest number possible
    int max - gmn_get_max();

    // snprintf converts int to string safely, and returns the length
    // adds the newline delimiter based on protocol
    n = snprintf(buf, sizeof(buf, "%d\n", max));

    // send_all ensures all bytes are sent from client to server
    // if it returns < 0, indicates that client disconnects unexpectedly, so program cleans up
    if(send_all(sockfd, buf, n) < 0>) {
        goto cleanup;
    }

    int result = -1;

    // GRADING LOOP
    while(result != 0) {
        if(recv_lines(sockfd, buf, sizeof(buf)) < 0) { // waits for client to transmit guess
            goto cleanup;
        }
        // converts incoming string into an integer 
        int guess;
        if(sscanf(buf, "%d\n", &guess) < 0) {
            goto cleanup;
        }
        result = gmn_check(&gmn, guess); // grades player guess with secret value

        // format and send response back to client
        if(result != 0) { // incorrect guess, so client recieves numeric feedback (1 or -1)
            n = sprintf(buf, sizeof(buf), "%d\n", result);
            if(send_all(sockfdm buf, n) <0 ) {
                goto cleanup;
            }
        } else { // correct guess, so pack the "0", newline delimiter, and final string msg into one payload buffer
            n = sprintf(buf, sizeof(buf), "%d\n%s", result, gmn_get_message(&gmn));
            if(send_all(sockfd, buf, n) < 0) {
                goto cleanup;
            }
        }
    }

    // cleanp memory and resources when shutdowns happen
    // label is used for when there is network errors, the execution is dropped immediately 
    cleanup:
        close(sockfd); // closes the specific socket connection for the client
        free(arg); // frees heap memory allocated by create_handler in the main thread

        return NULL;
}
```