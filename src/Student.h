#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h> 

#include "Utilities.h"

// Function prototypes for student functions
void student_menu(int client_socket, int student_id);
void view_all_courses(int client_socket);
void enroll_course(int client_socket, int student_id);
void drop_course(int client_socket, int student_id);
void view_enrolled_courses(int client_socket, int student_id);

