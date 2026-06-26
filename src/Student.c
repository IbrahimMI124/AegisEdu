#include "Student.h"



// View all courses function
void view_all_courses(int client_socket) {
    char buffer[BUFFER_SIZE];
    Course course;
    int fd;

    // List available courses (read access, protected by semaphore)
    strcpy(buffer, "Courses:\n");
    send_message(client_socket, buffer);

    sem_wait(&course_sem); // Acquire semaphore for reading
    fd = open("courses.dat", O_RDONLY);
    if (fd == -1) {
        strcpy(buffer, "No courses available.\n");
        send_message(client_socket, buffer);
        fflush(stdout); // Flush output
        sem_post(&course_sem); // Release semaphore
        return;
    }

    // Display courses
    while (read(fd, &course, sizeof(Course)) > 0) {
        sprintf(buffer, "ID: %d | Name: %s | Seats: %d\n",
                course.id, course.name, course.available_seats);
        send_message(client_socket, buffer);
        fflush(stdout); // Flush output
    }

    close(fd);
    sem_post(&course_sem); // Release semaphore after reading
}

// Enroll in course function
void enroll_course(int client_socket, int student_id) {
    char buffer[BUFFER_SIZE];
    Course course;
    int course_id, fd, found = 0, already_enrolled = 0;

    // First display all available courses
    view_all_courses(client_socket);

    // Get course ID to enroll
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
        strcpy(buffer, "Enrollment canceled.\n");
        send_message(client_socket, buffer);
        fflush(stdout); // Flush output
        return;
    }

    // Now acquire semaphore for writing
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

    // Open original file for reading
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

    // Find and update the course
    while (read(fd, &course, sizeof(Course)) > 0) {
        if (course.id == course_id) {
            found = 1;

            // Check if student is already enrolled
            for (int i = 0; i < course.enrollment_count; i++) {
                if (course.enrolled_students[i] == student_id) {
                    already_enrolled = 1;
                    break;
                }
            }

            if (already_enrolled) {
                strcpy(buffer, "You are already enrolled in this course.\n");
                send_message(client_socket, buffer);
                fflush(stdout); // Flush output
            } else if (course.available_seats <= 0) {
                strcpy(buffer, "Not enough seats in course.\n");
                send_message(client_socket, buffer);
                fflush(stdout); // Flush output
            } else {
                // Enroll student
                course.enrolled_students[course.enrollment_count] = student_id;
                course.enrollment_count++;
                course.available_seats--;

                sprintf(buffer, "Enrolled in the course with ID: %d\n", course_id);
                send_message(client_socket, buffer);
                fflush(stdout); // Flush output
            }
        }
        write(temp_fd, &course, sizeof(Course)); // Write all courses to temp file
    }

    close(fd);
    close(temp_fd);

    // Update course file
    if (found && !already_enrolled && course.available_seats >= 0) { // Check available_seats >= 0 just in case
        if (rename("temp_courses.dat", "courses.dat") != 0) {
             sprintf(buffer, "Error: Failed to update course file after enrollment.\n");
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

// Drop course function (renamed from unenroll_course)
void drop_course(int client_socket, int student_id) {
    char buffer[BUFFER_SIZE];
    Course course;
    int course_id, fd, found = 0, enrolled = 0;

    // List enrolled courses (read access, protected by semaphore)
    strcpy(buffer, "Courses enrolled:\n");
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

    // Display enrolled courses
    int has_courses = 0;
    while (read(fd, &course, sizeof(Course)) > 0) {
        for (int i = 0; i < course.enrollment_count; i++) {
            if (course.enrolled_students[i] == student_id) {
                sprintf(buffer, "ID: %d | Name: %s\n", course.id, course.name);
                send_message(client_socket, buffer);
                fflush(stdout); // Flush output
                has_courses = 1;
                break;
            }
        }
    }

    close(fd);
    sem_post(&course_sem); // Release semaphore after reading

    if (!has_courses) {
        strcpy(buffer, "You are not enrolled in any courses.\n");
        send_message(client_socket, buffer);
        fflush(stdout); // Flush output
        return;
    }

    // Get course ID to drop
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
        strcpy(buffer, "Cancelled drop course.\n");
        send_message(client_socket, buffer);
        fflush(stdout); // Flush output
        return;
    }

    // Now acquire semaphore for writing
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

    // Open original file for reading
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

    // Find and update the course
    while (read(fd, &course, sizeof(Course)) > 0) {
        if (course.id == course_id) {
            found = 1;

            // Check if student is enrolled
            for (int i = 0; i < course.enrollment_count; i++) {
                if (course.enrolled_students[i] == student_id) {
                    enrolled = 1;

                    // Remove student from enrollment
                    for (int j = i; j < course.enrollment_count - 1; j++) {
                        course.enrolled_students[j] = course.enrolled_students[j + 1];
                    }
                    course.enrollment_count--;
                    course.available_seats++;

                    sprintf(buffer, "Dropped course with ID: %d\n", course_id);
                    send_message(client_socket, buffer);
                    fflush(stdout); // Flush output
                    break;
                }
            }

            if (!enrolled) {
                strcpy(buffer, "You are not enrolled in this course.\n");
                send_message(client_socket, buffer);
                fflush(stdout); // Flush output
            }
        }
        write(temp_fd, &course, sizeof(Course)); // Write all courses to temp file
    }

    close(fd);
    close(temp_fd);

    // Update course file
    if (found && enrolled) {
        if (rename("temp_courses.dat", "courses.dat") != 0) {
             strcpy(buffer, "Error: Failed to update course file after dropping course.\n");
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

// View enrolled courses function
void view_enrolled_courses(int client_socket, int student_id) {
    char buffer[BUFFER_SIZE];
    Course course;
    int fd, found_course = 0; 

    // Apply semaphore for viewing courses
    sem_wait(&course_sem);
    fd = open("courses.dat", O_RDONLY);
    if (fd == -1) {
        strcpy(buffer, "No courses available.\n");
        send_message(client_socket, buffer);
        fflush(stdout); // Flush output
        sem_post(&course_sem); // Release semaphore
        return;
    }

    // Display enrolled courses
    strcpy(buffer, "Courses Enrolled:\n");
    send_message(client_socket, buffer);
    fflush(stdout); // Flush output

    while (read(fd, &course, sizeof(Course)) > 0) {
        for (int i = 0; i < course.enrollment_count; i++) {
            if (course.enrolled_students[i] == student_id) {
                found_course = 1;
                sprintf(buffer, "ID: %d | Name: %s | Faculty ID: %d\n",
                        course.id, course.name, course.faculty_id);
                send_message(client_socket, buffer);
                fflush(stdout); // Flush output
                break;
            }
        }
    }

    close(fd);
    sem_post(&course_sem); // Release semaphore

    if (!found_course) {
        strcpy(buffer, "You are not enrolled in any courses.\n");
        send_message(client_socket, buffer);
        fflush(stdout); // Flush output
    }
}

// Student menu function
void student_menu(int client_socket, int student_id) {
    char buffer[BUFFER_SIZE];
    int choice;

    while (1) {
        strcpy(buffer, "\n=== Student Menu ===\n1. View Available Courses\n2. Enroll in a Course\n3. Drop a Course\n4. View Enrolled Courses\n5. Change Password\n6. Logout & Exit\n\nYour Choice: ");
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
                view_all_courses(client_socket);
                break;
            case 2:
                enroll_course(client_socket, student_id);
                break;
            case 3:
                drop_course(client_socket, student_id);
                break;
            case 4:
                view_enrolled_courses(client_socket, student_id);
                break;
            case 5:
                change_password(client_socket, student_id, 3); // 3 for student
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