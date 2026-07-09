# HTTP Server in C

A minimal HTTP server Build from raw POSIX sockets - zero external libraries

## What is this?

Learning Project to understand the how HTTP server work at socket level. No libraries, No framework just C and Linux syscalls.

## Progress

- [x] V1: Basic TCP server that accept connection and return hardcoded HTTP response.
- [x] V2: File serving, serve files from desk.
- [ ] V3: Concurrently handle multiple connection via thread pool
- [ ] V4: Reverse proxy route request to different backend servers.

## Build

```Bash
 make

```

## RUN

```Bash
 make run
```

## Test

```Bash
curl http://localhost:8080

```
You can also use the browser with local port 8080

**you can test on your `html` files also just keep the `html` files into the src directory(*folder*)**

## What I learned (V1)

- `socket()` just return the genreal purpose file descriptor.
- HTTP is just text send over the TCP connection.
- Browser send 'GET / HTTP/1.1\r\nHost: .....'
- System call `bind()` use to assing the adress to the file discripter created by the `socket()`.
- `listen()` make file discripter return by the `socket()` to available to listen the incoming request.
- `accept()` system call use to accept the incoming request, that listen by the `listen()`.


## What I learned (V2)

- Solving the **RAM Explosion Problem** by using the **4KB chunck-streaming loop** to stream. 
- `open()` evaluates paths relative to the current working directory of the exection process.
- Data cannot teleport directly form hard drive storage to the network card interface. First CPU loads data from the disk to program buffer memory and pump it out the the socket discriptor.
- Chose to use raw POSIX system calls (`open()`/`read()`) insted of the C standard library strems (`fopen`/`fread`), everything is happening in `int` file discripter keeps architecture perfectly symmetric.
- Use the `fstat` from `sys/stat.h` to get the size of file.

## Tech
- Language:- C (C99)
- OS: Linux (Arch)
- No Libraries just raw POSIX 
- Build: GCC
- Debugger: GDB