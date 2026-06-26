#ifndef UTILITIES_H
#define UTILITIES_H


#include <semaphore.h>

// Constants
#define PORT 8080
#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024
#define MAX_COURSES 50
#define MAX_SEATS 10
#define ADMIN "admin@iiitb"
#define ADMIN_PWD "AdmiN.in"

// Structure definitions
typedef struct {
    int id;
    char name[50];
    char password[50];
    int active; // 1 for active, 0 for inactive
} Student;

typedef struct {
    int id;
    char name[50];
    char password[50];
} Faculty;

typedef struct {
    int id;
    char name[100];
    int faculty_id;
    int total_seats;
    int available_seats;
    int enrolled_students[MAX_SEATS];
    int enrollment_count;
} Course;

extern sem_t student_sem, faculty_sem, course_sem;

// Function prototypes for utility functions
int authenticate(int client_socket, int role);
void change_password(int client_socket, int id, int role);
void exit_portal(int client_socket);
void log_out(int client_socket);
void send_message_and_prompt(int socket, const char *message);
void send_message(int socket, const char *message);
int recv_message(int socket, char* buffer);
#endif // UTILS_H
