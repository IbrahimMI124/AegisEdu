
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include "Utilities.h"
#define PORT 8080
#define BUFFER_SIZE 1024
int client_socket;

void handle_sigint();
