# Sockets Programming: Client and Server
- TCP/IP
  - name/IP (Internet Protocol) and port number
- Socket
  - gets a file descriptor and can send and recieve 
    - place send() and recv() in loops!
## Client-Server World
- GOAL: establish a connection between a server and client 
- server
  - listens on a port for requests to "talk" from clients 
  - can talk to multiple clients at the *same time*
- client
  - *starts* communication between itself and server
  - needs to know:
    - name of server to find IP address
    - type of protocol to use (TCP or UDP)
      - "language to use"
    - port number 
### Server Side (TCP)
- after creating socket
  - bind() = reserves a port for talking
  - listen() = listens for requests to start communication
  - accept() = when requests come in
    - allocates a **seperate socket** to accept private comms with client (SAME HOST, DIFFERENT PORT)
      - original socket is available for other requests
      - new socket is bound to *ephermeral port*
    - communication continues on seperate sockets with **send()** and **recv()**
  - close() = ends conversations by closing 
### Client Side (TCP)
- after creating socket
  - connect() = client starts a conversation session
    - needs server name, type, and port
  - communication goes on with send() and recv()
  - close() = client closes a session
- NOTE: no bind() needed for the client, since OS allocates a port
## Back to Server...
```C
int bind(
  int sockfd, // socket file descriptor (DOES NOT HAVE ADDRESS)
  const struct sockaddr *addr, // hold addr info for many socket types
  socklen_t addrlen // sockaddr *addr needs seperate addrlen to indicate size
);
```
### Socket Address Types
```C
// generic socket address
struct sickaddr{
  unassigned short sa_family; // address familt, AF_xxx
  char sa_data[14]; // 14-byte protocol address, is a placeholder
}
// using Internet socket address
struct sockaddr_in sa; // socket address for IPv4
struct sockadd_in6 sa6; // socket addressfor IPv6
// struct sockaddr_in* can be cast to struct sockaddr * too
```
### addrinfo structure
```C
// recent inventor for preparing socket address
struct addrinfo{
  int ai_flags; // AI_PASSIVE, AI_CANONNAME, etc. 
  int ai_family; // AF_INET, AF_INET6, AF_UNSPEC
  int ai_socktype; // SOCK_STREAM, SOCK_DGRAM
  int ai_protocol; // "0" means any for protocol
  size_t ai_addrlen // size of address in bytes
  struct sockaddr *ai_addr; // struct sockaddr_in or sockaddr_in6
  char *ai_canonname; // the canonical hostname
  struct addrinfo *ai_next; // the next node in linked list
};
```
## getaddrinfo()
```C
// finds addrinfo for you
int getaddrinfo(
  const char *node, // address or IP
  const struct addrinfo *service, // "http" (specific name of service) or the port number
  const struct addrinfo *hints, // points to a previously filled structinfo filled with relevant information
  struct addrinfo **res
);
// output is a linked list, res, which is a list of results 
```
### Usage of getaddrinfo() for Servers
```C
struct addrinfo hints, *serverinfo; // res points to results

memset(&hints, 0, sizeof(hints)); // check if struct is empty
hints.ai_family = AF_UNSPEC; // IPv4 or IPv6 doesnt matter in this case
hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
ints.ai_flags = AI_PASSIVE; // fills in IP for the user

if(getaddrinfo(NULL, "3490", &hints, &servinfo) != 0) {
  die("error");
}
// servinfo points to linked list of at least 1 struct addrinfod
```
### Creating and Binding Socket
```C
int sockfd, yes = 1;
struct addrinfo *p;
// use socket(); setsockopt(); bind(); to try every addr in list
// continue if any fails, and check return values 

for(p = servinfo; p != NULL; p->ai_next) {
  sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protcol);
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int);)

  // bind to port we passed in getaddrinfo() --> 3490
  bind(sockfd, p->ai_addr, p->ai_addrlen);
  break; // stop attemtping since we have usable addr
}

freeaddrinfo(servinfo); // free linked list memory
```
### After bind()
```C
// listen on socket for signal to start conversation
status = listen(sockfd, 10);
checkError(status);

// client addr information, if needed
struct sockaddr_storage client_addr; 
while(1) {
  socklen_t clientSize = sizeof(client_addr);
  // new socket file descriptor is reutrned for communication
  int chatSocket = accept(
    sockfd,
    (struct sockaddr*)&client_addr,
    &clientSize;
  );
  checkError(chatSocket);
  // check demo code on displaying client addr!
  printf("We accepted a socket: %d\n", chatSocket);
  close(chatSocket);
}
```