#ifndef STATUS_CODE_H
#define STATUS_CODE_H

#define internal_server_error        500
#define ok                           200
#define not_found                    404


typedef struct{
  int cfd;
  char buffer[4096];
}req_parameters;

#endif
