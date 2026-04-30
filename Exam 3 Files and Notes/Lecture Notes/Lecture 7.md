# Socket Programming: Protocol
## Network Protocols
- rules for comms between two parties
  - protocols need at every layer
  - TCP and IP are protocols
- once TCP connection is established, two processes talk at application layer
  - need to determine:
    - who should talk first?
    - how much can each process talk?
    - how are messages encoded?
## Byte Stream in Sockets
- applications can see byte streams to and from sockets
  - up to them to *interpret* the bytes
- two common options
  - everything in plain text: needs conversion and parsing
    - XML | JSON
  - raw data
    - issues with endianness and passing
### Example
```
int v = 128;
// send v
```
- option 1: plain text
  - three ASCII chars with delimiter
    - '1', '2', '8', '\n'
- option 2: raw data
  - four bytes
    - 0x00, 0x00, 0x00, 0x80 (or 0x80, 0x00, 0x00, 0x00?)
- is the size of v actually 4?
## Endianness
- two ways to use bytes if a data item has more than 1 byte
  - some use big-endian, others use small-endian
- **both parties** need to be in agreement to interpret the bytes 

integer: 2150173913 **or** 0x80 29 0C D9
bytes: 128(0x80), 41(0x29), 12(0x0C), 217(0xD9)
## Change Byte Ordering Based on 16 or 32-bit Integers
- two orderings may not be the same 
  - host byte ordering: natural for platform
  - network byte ordering: *always* big-endian
- convert to network byte ordering before sending
- convert to host byte ordering after recieving    
  - htonl and ntohl for 32-bit, and htons and ntohs for 16-bit
## Sending a String
- we are fine in this course, since we are assuming *ASCII Strings*
  - each char = 1 byte
- modern applications use *Unicode* internally 
  - important for web servers, since customers use and speak many kinds of languages 
## Issues with Structure: Alignment and Padding 
- compiling code can case padding inside of structs
  - affects struct size and offset of fields 
  - important for two endpoints with diff archs
    - 32 bit vs. 64 bit *even if* both are Intel based
- how to resolve?
  - use **explicit padding**
    - do not use any language or machine dependent data or structures 
### How About Arrays?
- convert each element to bytes individually
- what about arrays of structs *that have array memebers*
  - can become very tedious
```C
// connection and greeting, 220 is a success code
// 250 is the acknowledgement code
S: 220 smpt.example.com ESMTP Postfix
C: HELO relay.example.com
S: 250 stmp.example.com, I am glad to meet you
// establishes sender and reciver of email
C: MAIL FROM:<bob@example.com>
S: 250 Ok
C: RCPT TO:<alice@example.com>
S: 250 Ok
// transfering email data from client to server
C: DATA
S: 354 End data with <CR><LF>.<CR><LF>
// client sends email payload, starting with email header
C: From: "Bob Example" <bob@example.com>
C: To: "Alice Example" <alice@example.com>
C: Date: Tue, 15 Jan 2008 16:02:43 -0500
C: Subject: Test Message
C: // divider between email headers and body
// text of email
C: Hello, Alice,
C: This is a test message.
C: Bob
C: .  // signal that contents of email is ending 
S: 250 OK: queued as 12345 // accepts message, "250 OK" means transmission successful and assigned tracking ID
C: QUIT // ends sesssion
S: 221 Bye // 221 code = closing transmission
{server closes connection}
```
## TFTP-style Applications
- trivial file transfer protocol (tftp)
  - server waits for commands and performs them
  - client sends a command to server and waits for response
- three commands are suppored
  - LS = list files in directory on server side
  - GET fn = download file named "fn" from the server
  - EXIT = end session
### Commands
- client gets a command, and server responds
  - LS = list of files
  - GET fn = file 
  - EXIT = nothing
- *how to encode the command, messages, and files*
#### Command Options: Text
- command is ASCII string ending with delimiter like '\n'
  - sender and reciver need to agree to the delimiter
```C
GET(fn)
  send _str("get hello.txt\n) // no need to send NUL
```
- pro
  - easy to read
  - flexible, compatible
- cons
  - needs parsing
  - variable length b/c checking delimiter
  - not compact for *long* commands
#### Command Options: Fixed Binary
- command is represented by a *structure*
  - all commands have same size
```C
#define LS 0
#define GET 1
#define PUT 2
#define EXIT 3

struct Command {
  int code;
  char fileName[252];
  cmd;
}

cmd.code = htonl(GET)
send_all(sid, &cmd, sizeof(cmd);)
```
- pro
  - nothing to parse
  - fixed size
- con
  - hard to read and maintain
  - fixed size
  - endianness and padding issues 
#### Command Options: Dynamic Size 
- LS and EXIT need 4 bytes online
- GET needs an additional (filename length + 4) bytes
```C
#define LS 0
#define GET 1
#define PUT 2
#define EXIT 3

int code = htonl(GET);
int fnlen = strlen(fname);
int nlen = htonl(fnlen)

send_all(fd, &code, sizeof(int));
send_all(fd, &nlen, sizeof(int));
send_all(fd, fname, flen)l
```
- pro
  - nothing to parse
  - dynamic sizing
- con
  - hard to read and maintain
  - endianness and more brittle
## Sending a Response
```C
typedef struct ResponseHeader_tag { 
  int code; // type of payload (txt message or file)
  int length; // number of bytes in txt message or file
} ResponseHeader;
```
- payload is followed by *length* bytes of message or file content 
## Recapping
```C
// DO these
scanf("%10s", &buf);
fgets(buf, BUF_SIZE, stdin);
snprintf(buf, BUF_SIZE, "%s", s);

// DONT DO these
scanf("%s", buf); // reads a string until a whitespace, but unbounded %s means it doesnt know the size of buf --> writes until an overflow
gets(buf); // officially removed from C standard: NEVER USE IT
sprint(buf, "%s", s); // writes formatted data into char string by copying s, buf if len(s) > len(buf), it will overflow 
```
## Safer String Handling
```C
fgets(bug, sizeof(buf), stdin) // replaces gets(buf)
strncpy(dst, src, n) // missing NUL, replaced strcpy(dst, src)
strncat(dst, src, n) // replaces strcat(dst, src)
strnlen(s, n) // replaces strlen(s)
scanf("/%10s", buf) // replaces scanf("%s", buf)
snprintf(buf, sizeof(buf), "%s" s) // replaces sprintf(buf, "%s", s)
```