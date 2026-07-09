#include <unistd.h>
#include <err.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <stdio.h>
#include <errno.h>

void parse(char *request, char *method, char *path){
  sscanf(request, "%s %s", method, path);
}

void response(int cfd, int status_code, char *status_text, char *content_type, FILE *file){
  char header_buffer[1024] = {0};
  fseek(file, 0, SEEK_END);
  int file_size = ftell(file);
  rewind(file);

  int header_len = snprintf(
  header_buffer, 
  sizeof(header_buffer),
    "HTTP/1.1 %d %s\r\n"
    "Content-Type: %s\r\n"
    "Content-Length: %d\r\n"
    "Connection: close\r\n"
    "\r\n",
  status_code,
  status_text,
  content_type,
  file_size

                              );
  write(cfd, response, header_len);
  char file_buffer[4096] = {0};
  int bytes_read;
  while((bytes_read = fread(file_buffer, 1, sizeof(file_buffer), file)) > 0){
    write(cfd, file_buffer, bytes_read);
  }
}

void serve_error(int cfd, int status_code, char *status_txt, char *html_msg){
  char response[1024];
  int body_len = strlen(html_msg);
  int response_len = snprintf(
    response,
    sizeof(response),
    "HTTP/1.1 %d %s\r\n"
    "Content-Type: text/html\r\n"
    "Content-Lenght: %d\r\n"
    "Connection: close\r\n"
    "\r\n"
    "%s",
    status_code, status_txt, body_len, html_msg
  );
  write(cfd, response, response_len);
}

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
    if(strcmp(method, "GET") == 0){
      char actual_path[512];

 if(strcmp(path, "/")){
        strcpy(actual_path, "index.html");
      }else{
        strcpy(actual_path, path + 1);
      }
      FILE *file = fopen(actual_path, "rb");
      if(file == NULL){
        printf("Page not found: %s\n", actual_path);
        serve_error(cfd, 404, "Not Found", 
                 "<html><body><h1>404 - Not Found</h1></body></html>");
      }else{
        char *content_type = "/text.plain";
        if(strstr(actual_path, ".html")) content_type = "text/html";
        else if(strstr(actual_path, ".css")) content_type = "text/css";
        else if(strstr(actual_path, ".image/png")) content_type = "image/png";
        response(cfd, 200, "OK", content_type, file);
        printf("Log sucessfully served");
      }

    }else{
      serve_error(cfd, 405, "Method Not Allowed",
               "<html><body><h1> Method Not allowed</h1</body></html>");
    }
    close(cfd);
  }
  close(sfd);
  exit(0);
}
