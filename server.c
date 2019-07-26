#include "common.h"
#include <sys/epoll.h>

#define MAX_EVENTS 2

struct epoll_event ep_ev[MAX_EVENTS];
int client_fds[MAX_CLIENTS];
char *mess;


void broadcast_to_clients(char *str, int num_clients);

int main(){

  struct addrinfo hints, *result;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
 
  int optval = 1;
  int retval = setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
  if (retval == -1) {
    perror("setsockopt");
    exit(1);
  }

  int s = getaddrinfo(NULL, PORT, &hints, &result);
  if (s != 0){
    //if call fails
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
    exit(1);
  }

  if (bind(listen_sock, result->ai_addr, result->ai_addrlen) != 0){
    perror("bind()");
    exit(1);
  }

  if (listen(listen_sock, 128) != 0){
    perror("listen()");
    exit(1);
  }

  struct sockaddr_in *result_addr = (struct sockaddr_in *) result->ai_addr;
  printf("Listening on file descriptor %d, port %d\n", listen_sock, ntohs(result_addr->sin_port));

  int epfd = epoll_create(1);
  
  struct epoll_event event;
  memset(&event, 0, sizeof(struct epoll_event));
  event.data.fd = listen_sock;
  event.events = EPOLL_IN;

  epoll_ctl(epfd, EPOLL_CTL_ADD, listen_sock, &event);

  // Waiting for connections like a passive socket
  printf("Waiting for connections...\n");
  int num_clients = 0;
  while (num_clients < MAX_CLIENTS){
    client_fds[num_clients] = accept(listen_sock, NULL, NULL);
    struct epoll_event new_client;
    memset(&new_client, 0, sizeof(struct epoll_event));
    new_client.data.fd = client_fds[num_clients];
    new_client.events = EPOLLIN;
    epoll_ctl(epfd, EPOLL_CTL_ADD, client_fds[num_clients], &new_client);

    num_clients++;

    asprintf(&mess, "MESSAGE Waiting on %d clients\n", MAX_CLIENTS - num_clients);
    broadcast_to_clients(mess, num_clients);
    free(mess);
  }
  
  printf("All connections made\n");
  broadcast_to_clients("READY\n", MAX_CLIENTS);

  
  while (1){
    int num_events = epoll_wait(epfd, &ep_evs, MAX_EVENTS, -1);
    for (int i=0; i<num_events; i++){
      int fd = ep_evs[i].data.fd;
      char c;
      int bytes = read(fd, &c, 1);
      printf("Character typed = %c\n", c);

      int player = (fd == client_fds[0]) ? 1 : 2;
      asprintf(&mess, "MESSAGE Player %d typed: %c\n", player, c);
      dprintf(client_fds[(player+1)%2], "%s", mess);
      free(mess);
    }
  }
  
  
  close(listen_sock);
  for (int i=0; i<MAX_CLIENTS; i++)
    close(client_fds[i]);
  freeaddrinfo(result);
}

void broadcast_to_clients(char* str, int num_clients){
  for (int i=0; i<num_clients; i++)
    dprintf(client_fds[i], "%s", str);
}
