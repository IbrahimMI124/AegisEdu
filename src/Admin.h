#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>
#include <semaphore.h>
#include <ctype.h> // Required for isdigit

#include "Utilities.h"

// Assuming these semaphores are defined and initialized elsewhere
extern sem_t student_sem;
extern sem_t faculty_sem;

// Function prototypes for admin functions

void add_student(int client_socket);
void view_student_details(int client_socket);
void add_faculty(int client_socket);
void view_faculty_details(int client_socket);
void activate_student(int client_socket);
void admin_menu(int client_socket);
void block_student(int client_socket);
void modify_student_details(int client_socket);
void modify_faculty_details(int client_socket);
void log_out(int client_socket); // Assuming this function exists
