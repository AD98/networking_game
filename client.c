#include "common.h"
#include <pthread.h>

int main(){
  
  struct addrinfo hints, *result;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  int res = getaddrinfo(SERVER, PORT, &hints, &result);
  
  if (res){
    fprintf(stderr, "getaddrinfo failed: %s\n", gai_strerror(res));
    exit(1);
  }

  int server_sock = socket(AF_INET, SOCK_STREAM, 0);

  if (connect(server_sock, result->ai_addr, result->ai_addrlen)){
    fprintf(stderr, "Connect failed\n");
    exit(1);
  }
  
  printf("Connected\n");

  while (1){
    char buf[1024] = {0};
    read(server_sock, buf, 1024);
    char *action = strtok(buf, " ");
    printf("Action: %s\n", action);
    if (strcmp(action, "MESSAGE") == 0)
      printf("%s\n", strtok(NULL,"\n"));
    else if (strcmp(action, "READY") == 0)
      break;
  }
  
  printf("BEGIN\n");


  system("stty raw");
  while (1){
    char c = getchar();
    if ((c == '\r') || (c == '\n'))
      break;
    write(server_sock, &c, 1);
    char buf[100];
    read(server_sock, buf, 100);
    printf("%s", buf);

  }
  system("stty cooked");

  shutdown(server_sock, SHUT_WR);
  close(server_sock);
  freeaddrinfo(result);
}
