
#include "Client.h"
// Function to handle SIGINT signal (Ctrl+C)


int main(int argc, char *argv[])
{
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE] = {0};
    char input[BUFFER_SIZE] = {0};

    // Set up signal handler
    signal(SIGINT, handle_sigint);

    // Create socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Error: Socket could not be created.");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (argc > 1)
    {
        // Use provided IP address
        if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0)
        {
            perror("Invalid address");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        // Use localhost by default
        if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0)
        {
            perror("Invalid address");
            exit(EXIT_FAILURE);
        }
    }

    // Connect to server
    printf("Requesting connection with server...\n");
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Error: Request failed.");
        exit(EXIT_FAILURE);
    }

    printf("Request for connection successful.\nConnected to server.\n\n");
    int f = 0;

    // Main communication loop
    while (1)
    {
        // Clear buffer
        memset(buffer, 0, BUFFER_SIZE);
        memset(input, 0, BUFFER_SIZE);

        // Receive data from server
        f = recv_message(client_socket, buffer);
        // printf("where am i %d\n", f);
        
        if (f == 2)
        {
            printf("Exiting Portal...\n");
            break;
        }

        // Display received message
        printf("%s", buffer);

        // Get user input
        if (f == 1 && fgets(input, BUFFER_SIZE, stdin) != NULL)
        {
            // Remove newline character
            input[strcspn(input, "\n")] = 0;

            // Send input to server
            send_message(client_socket, input);

            // Check if user wants to exit
            if (strcmp(input, "5") == 0 && strstr(buffer, "MSGKL") != NULL)
            {
                // Receive goodbye message
                memset(buffer, 0, BUFFER_SIZE);
                int check = recv_message(client_socket, buffer);
                if (check < 0)
                {
                    printf("Socket Error...\n");
                }
                printf("%s", buffer);
                break;
            }
        }
    }

    // Close socket
    close(client_socket);
    return 0;
}

void handle_sigint()
{
    printf("\nDisconnecting from server...\n");
    close(client_socket);
    exit(0);
}

