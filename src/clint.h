#include <string.h>
#include <sys/socket.h>
#include <err.h>
#include <errno.h>
#include <netinet/in.h>

#include "response.h"
#include "status_code.h"

void* handle_client(void *arg)
{

  req_parameters *req = (req_parameters*)arg; 
  
  char method[16] = {0};
  char path[256] = {0};
  char buffer[4096] = {0};
  int cfd = req -> cfd;
  strncpy(buffer, req->buffer, sizeof(buffer));

  parse(buffer, method, path);
  printf("Request: %s %s\n", method, path);

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
      return NULL;
    }
    int fileD; //file discriptor for the requested file
    if((fileD = open(actual_path, O_RDONLY)) < 0){
      serve_error(cfd, internal_server_error, "Something went wrong",
              "<html><body><h1>500 - Something went wrong Please try again</h1></body></html>");
      close(cfd);
      return NULL;
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
  return NULL;
  
}

