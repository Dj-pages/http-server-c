#include <unistd.h>
#include <err.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>


void parse(char *request, char *method, char *path){
  sscanf(request, "%s %s", method, path);
}

//Function to response the client request 
//cfd: client file descriptor
void response(int cfd, int status_code, char *status_text, char *content_type, int fileD){
  char header_buffer[1024] = {0};

  //Calculate the size of file
  struct stat st;
  if(!(fstat(fileD, &st) == 0)){
    printf("Faild to calculate the file length");
  }
  int file_size = st.st_size;

  //generating the response
  int header_len = snprintf(
  header_buffer, 
  sizeof(header_buffer),
    "HTTP/1.1 %d %s\r\n"
    "Content-Type: %s\r\n"
    "Content-Lenght: %d\r\n"
    "Connection: close\r\n"
    "\r\n",
  status_code,
  status_text,
  content_type,
  file_size  );
  write(cfd, header_buffer, header_len);

  //buffer and stream file in chunks to client
  char file_buffer[4096] = {0};
  int bytes_read = sizeof(file_buffer);
  while((bytes_read = read(fileD, file_buffer, bytes_read)) > 0){
    write(cfd, file_buffer, bytes_read);
  }
  close(fileD);
  close(cfd);
}


//response if the error occured
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
  close(cfd);
}