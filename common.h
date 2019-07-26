#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define PORT "1234"
#define SERVER "127.0.0.1"
#define MAX_CLIENTS 2

typedef enum { MESSAGE }action;
