# HTTP Server in C

A minimal HTTP server Build from raw POSIX sockets - zero external libraries

## What is this?

Learning Project to understand the how HTTP server work at socket level. No libraries, No framework just C and Linux syscalls.

## Progress

- [x] V1: Basic TCP server that accept connection and return hardcoded HTTP response.
- [ ] V2: File serving, serve files from desk.
- [ ] V3: Concurrently handle multiple connection via thread pool
- [ ] V4: Reverse proxy route request to different backend servers.

## Build & Run

```Bash
//build
gcc server.c -o server

//run
./server

```

## Test

```Bash
curl http://localhost:8080

```
You can also use the browser with local port 8080

## What I learned (V1)

- socket() just return the file descriptor
- HTTP is just text send over the TCP connection
- Browser send 'GET / HTTP/1.1\r\nHost: .....'
- Server response with status + header + body
- Everything in Linux is file descriptor and every file descriptor is isolated from other process.

## Tech
- Language:- C (C99)
- OS: Linux (Arch)
- No Libraries just raw POSIX 
- Build: GCC
- Debugger: GDB

