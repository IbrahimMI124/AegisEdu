#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <semaphore.h>
#include "Server.h"
#include <asm-generic/socket.h>

// Signal handler for graceful shutdown
void signal_handler(int sig)
{
    (void)sig; // Cast to void to suppress unused parameter warning
    printf("Closing server...\n");
    fflush(stdout); // Flush output
    sem_destroy(&student_sem);
    sem_destroy(&faculty_sem);
    sem_destroy(&course_sem);
    exit(0);
}

// Client handler function
void client_handler(void * arg)
{
    int client_socket = *((int*)arg);
    char buffer[BUFFER_SIZE];
    int role, user_id;
    while(1){
    // Send welcome message
    strcpy(buffer, "=== Academia Course Registration ===\nSelect Login Type:\n1. Admin  2. Faculty  3. Student  4. Exit\nYour Choice: ");
send_message_and_prompt(client_socket, buffer);
    // printf("%s\n handle client", buffer);
    // fflush(stdout); // Flush output
    memset(buffer, 0, BUFFER_SIZE);

    // Receive role choice
    int check = recv_message(client_socket, buffer);
    if (check < 0)
    {
        send_message(client_socket, "Socket Error\n");
    }
    role = atoi(buffer);
    if(role == 4){
        exit_portal(client_socket);        
        break;
    }

    // Authenticate user
    user_id = authenticate(client_socket, role);
    if (user_id < 0)
    {
        strcpy(buffer, "Try again...\n");
        send_message(client_socket, buffer);
        fflush(stdout); // Flush output
        continue;
    }

    // Direct to appropriate menu
    switch (role)
    {
    case 1:
        admin_menu(client_socket);
        break;
    case 2:
        faculty_menu(client_socket, user_id);
        break;
    case 3:
        student_menu(client_socket, user_id);
        break;
    default:
        strcpy(buffer, "Invalid choice. Disconnecting...\n");
        send_message(client_socket, buffer);
        fflush(stdout); // Flush output
        break;
    }
   
}

    close(client_socket);
    pthread_exit(NULL);
}




// Main function
int main()
{
    int server_fd, client_socket;
    struct sockaddr_in address;
    int options = 1;
    int addrlen = sizeof(address);
    pthread_t thread_id;

    // Initialize semaphores
    sem_init(&student_sem, 0, 1);
    sem_init(&faculty_sem, 0, 1);
    sem_init(&course_sem, 0, 1);

    // Setup signal handler
    signal(SIGINT, signal_handler);

    // Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &options, sizeof(options)))
    {
        perror("Setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // Configure address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind socket to address
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_fd, MAX_CLIENTS) < 0)
    {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Initiating server...\nServer available for connections...\n");
    fflush(stdout); // Flush output

    // Accept and handle client connections
    while (1)
    {
        if ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("Accept failed");
            continue;
        }

        printf("Client Request: %s:%d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
        fflush(stdout); // Flush output

        // Create a new thread for each client
        pthread_create(&thread_id, NULL, (void *)client_handler, (void *)(&client_socket));
        printf("Client Connected\n");
    }

    sem_destroy(&student_sem);
    sem_destroy(&faculty_sem);
    sem_destroy(&course_sem);
    close(server_fd);
    return 0;
}


