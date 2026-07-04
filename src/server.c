#include <unistd.h>
#include <err.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <stdio.h>
#include <errno.h>

void process_request(char *request, char *method, char *path){
  sscanf(request, "%s %s", method, path);
}

void response(int cfd, int status_code, char *status_text, char *content_type, char *body){
  char response[4096];
  int body_len = strlen(body);
  int response_len = snprintf(
  response, 
  sizeof(response),
    "HTTP/1.1 %d %s\r\n"
    "Content-Type: %s\r\n"
    "Content-Length: %d\r\n"
    "Connection: close\r\n"
    "\r\n"
    "%s",
  status_code,
  status_text,
  content_type,
  body_len,
  body

                              );
  write(cfd, response, response_len);
}

int main(void)
{
  int sfd;
  struct sockaddr_in my_addr;
  socklen_t my_addr_len;

  //create generic socket
  if((sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    err(EXIT_FAILURE, "socket");

  int opt = 1;
  if(setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))){
    err(EXIT_FAILURE, "Adress reuse");
  }

  //setting memory to the my_addr
  memset(&my_addr, 0, sizeof(my_addr));

  //config socket addeess
  my_addr.sin_family = AF_INET;
  my_addr.sin_port = htons(8080);
  my_addr.sin_addr.s_addr = INADDR_ANY;

  //assign address to the file discriper created by the socket
  if(bind(sfd, (struct sockaddr*)&my_addr, sizeof(my_addr)))
    err(EXIT_FAILURE, "bind");

  //listen for connections
  if(listen(sfd, SOMAXCONN))
    err(EXIT_FAILURE, "listen");


  //listen continously
  while(true){
    int cfd;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len;

    //setting memory all bytes to zero
    memset(&client_addr, 0, sizeof(client_addr));

    //accept the client request
    cfd = accept(sfd, (struct sockaddr*)&client_addr, &client_addr_len);
    if(cfd == -1)
      err(EXIT_FAILURE, "accept");
    char buffer[4096] = {0};
    read(cfd, buffer, sizeof(buffer));
    char method[16] = {0};
    char path[256] = {0};
    process_request(buffer, method, path);

    //response 
    if(strcmp(method, "GET") == 0){
      if(strcmp(path, "/") == 0){
        response(cfd, 200, "OK", "text/html",
                 "<html><body><h1>Hello from my C server!</h1></body></html>");
      }else if(strcmp(path, "/about") == 0){
        response(cfd, 200, "ok", "text/html",
                 "<html><body><h1>About Page</h1><p>Built with raw C sockets.</p></body></html>");
      }else {
        response(cfd, 404, "Not Found", "text/html",
                 "<html><body><h1>404 - Not Found</h1></body></html>");
      }
    }else {
      response(cfd, 405, "Method Not Allowed", "text/plain",
               "Method not allowed");
    }
    close(cfd);
  }
  close(sfd);
  exit(0);
}
