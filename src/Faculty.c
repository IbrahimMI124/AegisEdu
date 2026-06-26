#include "Faculty.h"


// View offering courses function
void view_offering_courses(int client_socket, int faculty_id) {
    char buffer[BUFFER_SIZE];
    Course course;
    int fd, found = 0;

    // List courses offered by this faculty (read access, protected by semaphore)
    strcpy(buffer, "Courses Offered:\n");
    send_message(client_socket, buffer);
    fflush(stdout); // Flush output

    sem_wait(&course_sem); // Acquire semaphore for reading
    fd = open("courses.dat", O_RDONLY);
    if (fd == -1) {
        strcpy(buffer, "No courses available.\n");
        send_message(client_socket, buffer);
        fflush(stdout); // Flush output
        sem_post(&course_sem); // Release semaphore
        return;
    }

    while (read(fd, &course, sizeof(Course)) > 0) {
        if (course.faculty_id == faculty_id) {
            sprintf(buffer, "ID: %d | Name: %s | Enrolled: %d/%d\n",
                    course.id, course.name, course.enrollment_count, course.total_seats);
            send_message(client_socket, buffer);
            fflush(stdout); // Flush output
            found = 1;
        }
    }

    close(fd);
    sem_post(&course_sem); // Release semaphore after reading

    if (!found) {
        strcpy(buffer, "No courses offered yet.\n");
        send_message(client_socket, buffer);
        fflush(stdout); // Flush output
    }
}

// Add course function
void add_course(int client_socket, int faculty_id) {
    char buffer[BUFFER_SIZE];
    Course course;
    int fd, last_id = 0;

    // Get course details
    strcpy(buffer, "Enter course name: ");
    send_message_and_prompt(client_socket, buffer);
    fflush(stdout); // Flush output
    memset(buffer, 0, BUFFER_SIZE);
    {int check = recv_message(client_socket, buffer);
if(check<0){
          send_message(client_socket, "Socket Error\n");
}}
    strcpy(course.name, buffer);

    strcpy(buffer, "Enter total seats: ");
    send_message_and_prompt(client_socket, buffer);
    fflush(stdout); // Flush output
    memset(buffer, 0, BUFFER_SIZE);
    {int check = recv_message(client_socket, buffer);
if(check<0){
          send_message(client_socket, "Socket Error\n");
}}
    course.total_seats = atoi(buffer);
    course.available_seats = course.total_seats;

    // Initialize course
    course.faculty_id = faculty_id;
    course.enrollment_count = 0;
    for (int i = 0; i < MAX_SEATS; i++) {
        course.enrolled_students[i] = 0;
    }

    // Acquire semaphore for writing
    sem_wait(&course_sem);

    // Get the last course ID
    fd = open("courses.dat", O_RDONLY);
    if (fd != -1) {
        Course temp;
        while (read(fd, &temp, sizeof(Course)) > 0) {
            last_id = temp.id;
        }
        close(fd);
    }

    // Assign new ID and save course
    course.id = last_id + 1;

    // Open file in append mode
    fd = open("courses.dat", O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd == -1) {
        strcpy(buffer, "Error: Could not open course database.\n");
        send_message(client_socket, buffer);
        fflush(stdout); // Flush output
        sem_post(&course_sem); // Release semaphore
        return;
    }

    write(fd, &course, sizeof(Course));
    close(fd);
    sem_post(&course_sem); // Release semaphore

    sprintf(buffer, "Course with ID: %d added.\n", course.id);
    send_message(client_socket, buffer);
    fflush(stdout); // Flush output
}

// Remove course function
void remove_course(int client_socket, int faculty_id) {
    char buffer[BUFFER_SIZE];
    Course course;
    int course_id, fd, found = 0;

    // List courses offered by this faculty
    view_offering_courses(client_socket, faculty_id);

    // Get course ID to remove
    strcpy(buffer, "Enter course ID: ");
    send_message_and_prompt(client_socket, buffer);
    fflush(stdout); // Flush output
    memset(buffer, 0, BUFFER_SIZE);
    {int check = recv_message(client_socket, buffer);
if(check<0){
          send_message(client_socket, "Socket Error\n");
}}
    course_id = atoi(buffer);

    if (course_id == 0) {
        strcpy(buffer, "Operation canceled.\n");
        send_message(client_socket, buffer);
        fflush(stdout); // Flush output
        return;
    }

    // Acquire semaphore for writing
    sem_wait(&course_sem);

    // Create a temporary file for writing
    int temp_fd = open("temp_courses.dat", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (temp_fd == -1) {
        strcpy(buffer, "Error: Could not create temporary file.\n");
        send_message(client_socket, buffer);
        fflush(stdout); // Flush output
        sem_post(&course_sem); // Release semaphore
        return;
    }

    fd = open("courses.dat", O_RDONLY);
    if (fd == -1) {
        strcpy(buffer, "Error: Could not open course database.\n");
        send_message(client_socket, buffer);
        fflush(stdout); // Flush output
        close(temp_fd);
        unlink("temp_courses.dat"); // Clean up temp file
        sem_post(&course_sem); // Release semaphore
        return;
    }

    found = 0;
    int authorized = 0;
    int removed_successfully = 0;

    while (read(fd, &course, sizeof(Course)) > 0) {
        if (course.id == course_id) {
            found = 1;

            if (course.faculty_id == faculty_id) {
                authorized = 1;

                if (course.enrollment_count > 0) {
                    strcpy(buffer, "Error: Cannot remove course with enrolled students.\n");
                    send_message(client_socket, buffer);
                    fflush(stdout); // Flush output
                    write(temp_fd, &course, sizeof(Course)); // Keep the course in the temp file
                } else {
                    sprintf(buffer, "Course with ID: %d removed.\n", course_id);
                    send_message(client_socket, buffer);
                    fflush(stdout); // Flush output
                    removed_successfully = 1;
                    // Skip writing this course to temp file to remove it
                }
            } else {
                strcpy(buffer, "Error: You are not authorized to remove this course.\n");
                send_message(client_socket, buffer);
                fflush(stdout); // Flush output
                write(temp_fd, &course, sizeof(Course)); // Keep the course in the temp file
            }
        } else {
            write(temp_fd, &course, sizeof(Course)); // Write other courses to temp file
        }
    }

    close(fd);
    close(temp_fd);

    // Update course file
    if (found && authorized && removed_successfully) {
        if (rename("temp_courses.dat", "courses.dat") != 0) {
             strcpy(buffer, "Error: Failed to update course file after removal.\n");
             send_message(client_socket, buffer);
             fflush(stdout); // Flush output
             unlink("temp_courses.dat"); // Clean up temp file on error
        }
    } else {
        unlink("temp_courses.dat"); // Clean up temp file if no update or error
        if (!found) {
            strcpy(buffer, "Course not found.\n");
            send_message(client_socket, buffer);
            fflush(stdout); // Flush output
        }
    }

    sem_post(&course_sem); // Release semaphore
}

// Update course details function
void update_course_details(int client_socket, int faculty_id) {
    char buffer[BUFFER_SIZE];
    Course course;
    int course_id, fd, found = 0;

    // List courses offered by this faculty
    view_offering_courses(client_socket, faculty_id);

    // Get course ID to update
    strcpy(buffer, "Enter course ID: ");
    send_message_and_prompt(client_socket, buffer);
    fflush(stdout); // Flush output
    memset(buffer, 0, BUFFER_SIZE);
    {int check = recv_message(client_socket, buffer);
if(check<0){
          send_message(client_socket, "Socket Error\n");
}}
    course_id = atoi(buffer);

    if (course_id == 0) {
        strcpy(buffer, "Operation canceled.\n");
        send_message(client_socket, buffer);
        fflush(stdout); // Flush output
        return;
    }

    // Acquire semaphore for writing
    sem_wait(&course_sem);

    // Create a temporary file for writing
    int temp_fd = open("temp_courses.dat", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (temp_fd == -1) {
        strcpy(buffer, "Error: Could not create temporary file.\n");
        send_message(client_socket, buffer);
        fflush(stdout); // Flush output
        sem_post(&course_sem); // Release semaphore
        return;
    }

    fd = open("courses.dat", O_RDONLY);
    if (fd == -1) {
        strcpy(buffer, "Error: Could not open course database.\n");
        send_message(client_socket, buffer);
        fflush(stdout); // Flush output
        close(temp_fd);
        unlink("temp_courses.dat"); // Clean up temp file
        sem_post(&course_sem); // Release semaphore
        return;
    }

    found = 0;
    int authorized = 0;
    int updated_successfully = 0;

    while (read(fd, &course, sizeof(Course)) > 0) {
        if (course.id == course_id) {
            found = 1;

            if (course.faculty_id == faculty_id) {
                authorized = 1;
                
                // Display current course details
                sprintf(buffer, "Current course details:\nName: %s\nTotal Seats: %d\nAvailable Seats: %d\n",
                        course.name, course.total_seats, course.available_seats);
                send_message(client_socket, buffer);
                fflush(stdout); // Flush output
                
                // Get updated course name
                strcpy(buffer, "Enter new course name: ");
                send_message_and_prompt(client_socket, buffer);
                fflush(stdout); // Flush output
                memset(buffer, 0, BUFFER_SIZE);
                {int check = recv_message(client_socket, buffer);
                if(check<0){
                    send_message(client_socket, "Socket Error\n");
                }}
                
                if (strlen(buffer) > 0) {
                    strcpy(course.name, buffer);
                }
                
                // Get updated total seats
                strcpy(buffer, "Enter new total seats: ");
                send_message_and_prompt(client_socket, buffer);
                fflush(stdout); // Flush output
                memset(buffer, 0, BUFFER_SIZE);
                {int check = recv_message(client_socket, buffer);
                if(check<0){
                    send_message(client_socket, "Socket Error\n");
                }}
                
                int new_total_seats = atoi(buffer);
                if (new_total_seats > 0) {
                    if (new_total_seats < course.enrollment_count) {
                        strcpy(buffer, "Error: Cannot set total seats less than current enrollment.\n");
                        send_message(client_socket, buffer);
                        fflush(stdout); // Flush output
                    } else {
                        int seats_difference = new_total_seats - course.total_seats;
                        course.total_seats = new_total_seats;
                        course.available_seats += seats_difference;
                    }
                }
                
                updated_successfully = 1;
                sprintf(buffer, "Course with ID: %d updated.\n", course_id);
                send_message(client_socket, buffer);
                fflush(stdout); // Flush output
            } else {
                strcpy(buffer, "Error: You are not authorized to update this course.\n");
                send_message(client_socket, buffer);
                fflush(stdout); // Flush output
            }
        }
        write(temp_fd, &course, sizeof(Course)); // Write all courses to temp file
    }

    close(fd);
    close(temp_fd);

    // Update course file
    if (found && authorized && updated_successfully) {
        if (rename("temp_courses.dat", "courses.dat") != 0) {
             strcpy(buffer, "Error: Failed to update course file.\n");
             send_message(client_socket, buffer);
             fflush(stdout); // Flush output
             unlink("temp_courses.dat"); // Clean up temp file on error
        }
    } else {
        unlink("temp_courses.dat"); // Clean up temp file if no update or error
        if (!found) {
            strcpy(buffer, "Course not found.\n");
            send_message(client_socket, buffer);
            fflush(stdout); // Flush output
        }
    }

    sem_post(&course_sem); // Release semaphore
}

// Faculty menu function
void faculty_menu(int client_socket, int faculty_id) {
    char buffer[BUFFER_SIZE];
    int choice;

    while (1) {
        strcpy(buffer, "\n=== Faculty Menu ===\n1. View Offered Courses\n2. Add New Course\n3. Remove Course from Catalog\n4. Update Course Details\n5. Change Password\n6. Logout & Exit\n\nYour Choice: ");
        send_message_and_prompt(client_socket, buffer);
        fflush(stdout); // Flush output
        memset(buffer, 0, BUFFER_SIZE);

        {int check = recv_message(client_socket, buffer);
if(check<0){
          send_message(client_socket, "Socket Error\n");
}}
        choice = atoi(buffer);

        switch (choice) {
            case 1:
                view_offering_courses(client_socket, faculty_id);
                break;
            case 2:
                add_course(client_socket, faculty_id);
                break;
            case 3:
                remove_course(client_socket, faculty_id);
                break;
            case 4:
                update_course_details(client_socket, faculty_id);
                break;
            case 5:
                change_password(client_socket, faculty_id, 2); // 2 for faculty
                break;
            case 6:
                log_out(client_socket);
                return;
            default:
                strcpy(buffer, "Invalid choice. Please try again.\n");
                send_message(client_socket, buffer);
                fflush(stdout); // Flush output
        }
    }
}