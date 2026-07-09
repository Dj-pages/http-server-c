#include <unistd.h>
#include <err.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include "response.h"
#include "status_code.h"
#include <fcntl.h>

int main(void)
{
  int sfd;
  struct sockaddr_in my_addr;
  //socklen_t my_addr_len;

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
    parse(buffer, method, path);

    //response 
    //If the method is GET
    if(strcmp(method, "GET") == 0){
      char actual_path[512];

      if(strcmp(path, "/") == 0){
        strcpy(actual_path, "src/index.html"); //path of the file
      }else{
        snprintf(actual_path, sizeof(actual_path), "src/%s", path); // Path modification as per the requested path
      }
      //check if the gives file is accessiable
      if(access(actual_path, R_OK)){
        serve_error(cfd, internal_server_error, "Something went wrong",
                    "<html><body><h1>500 - Something went wrong Please try again</h1></body></html>");
        close(cfd);
        continue;
      }
      int fileD; //file discriptor for the requested file
      if((fileD = open(actual_path, O_RDONLY)) < 0){
        serve_error(cfd, internal_server_error, "Something went wrong",
                    "<html><body><h1>500 - Something went wrong Please try again</h1></body></html>");
      }
      //check the extension of the file
      char *content_type;
      if(strstr(actual_path, ".html")) content_type = "text/html";
      else if(strstr(actual_path, ".css")) content_type = "text/css";
      else if(strstr(actual_path, ".png")) content_type = "image/png";
      response(cfd, ok, "ok", content_type, fileD);

    }else{
      //if method is not the get method then it is not allowd
      serve_error(cfd, not_found, "Method Not Allowed",
               "<html><body><h1> Method Not allowed</h1</body></html>");
    }
    close(cfd);
  }
  close(sfd);
  exit(0);
}
