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
#include <fcntl.h>
#include <pthread.h>
#include <bits/pthreadtypes.h>
#include "clint.h"

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
    
    memset(&client_addr, 0, sizeof(client_addr));
    cfd = accept(sfd, (struct sockaddr*)&client_addr, &client_addr_len);
    if(cfd < 0)
      err(EXIT_FAILURE, "accept");
    char buffer[4096] = {0};
    read(cfd, buffer, sizeof(buffer));
    
    //string values into the struct 
    req_parameters req;
    req.cfd = cfd;
    strncpy(req.buffer, buffer, sizeof(buffer));

    //Thread to handle client
    pthread_t thread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&thread, &attr, handle_client, (void*)&req);
    pthread_detach(thread);

  }
  close(sfd);
}
