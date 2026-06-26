#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h> // Still needed for flock structure definition, though not fcntl locking calls

#include "Utilities.h"

// Function prototypes for faculty functions
void faculty_menu(int client_socket, int faculty_id);
void view_offering_courses(int client_socket, int faculty_id);
void add_course(int client_socket, int faculty_id);
void remove_course(int client_socket, int faculty_id);
void update_course_details(int client_socket, int faculty_id);

