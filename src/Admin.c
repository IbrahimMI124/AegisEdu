#include "Admin.h"



// Add student function
void add_student(int client_socket)
{
    char buffer[BUFFER_SIZE];
    Student student;
    int fd, last_id = 0;
    int duplicate_found = 0;

    strcpy(buffer, "Enter student name: ");
    send_message_and_prompt(client_socket, buffer);
    memset(buffer, 0, BUFFER_SIZE);
    
    int check = recv_message(client_socket, buffer);
    if (check < 0)
    {
        send_message(client_socket, "Socket Error\n");
        return;
    }
    buffer[strcspn(buffer, "\n")] = 0; // Remove newline
    strcpy(student.name, buffer);

    sem_wait(&student_sem);
    fd = open("students.dat", O_RDONLY);
    if (fd != -1)
    {
        Student temp;
        while (read(fd, &temp, sizeof(Student)) > 0)
        {
            if (strcmp(student.name, temp.name) == 0)
            {
                duplicate_found = 1;
                break;
            }
            last_id = temp.id;
        }
        close(fd);
    }

    if (duplicate_found)
    {
        strcpy(buffer, "Error: Student username already exists.\n");
        send_message(client_socket, buffer);
        sem_post(&student_sem);
        return;
    }

    strcpy(buffer, "Enter student password: ");
    send_message_and_prompt(client_socket, buffer);
    memset(buffer, 0, BUFFER_SIZE);
    
    check = recv_message(client_socket, buffer);
    if (check < 0)
    {
        send_message(client_socket, "Socket Error\n");
        sem_post(&student_sem);
        return;
    }
    buffer[strcspn(buffer, "\n")] = 0; // Remove newline
    strcpy(student.password, buffer);

    student.active = 1;

    student.id = last_id + 1;

    fd = open("students.dat", O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd == -1)
    {
        strcpy(buffer, "Error: Could not open student database for writing.\n");
        send_message(client_socket, buffer);
        sem_post(&student_sem);
        return;
    }

    write(fd, &student, sizeof(Student));
    close(fd);
    sem_post(&student_sem);

    sprintf(buffer, "Student with ID: %d\n added.", student.id);
    send_message(client_socket, buffer);
}

// View student details function
void view_student_details(int client_socket) {
    char buffer[BUFFER_SIZE];
    int fd;
    Student student;
    int found = 0;

    sem_wait(&student_sem);

    fd = open("students.dat", O_RDONLY);
    if (fd == -1) {
        strcpy(buffer, "Error: Could not open student database.\n");
        send_message(client_socket, buffer);
        sem_post(&student_sem);
        return;
    }

    send_message(client_socket, "Student Details:\n");
    while (read(fd, &student, sizeof(Student)) > 0) {
        sprintf(buffer, "ID: %d | Name: %s | Status: %s\n", student.id, student.name, student.active ? "Active" : "Blocked");
        send_message(client_socket, buffer);
        found = 1;
    }

    if (!found) {
        send_message(client_socket, "No students found.\n");
    }

    close(fd);
    sem_post(&student_sem);
}

// Add faculty function
void add_faculty(int client_socket)
{
    char buffer[BUFFER_SIZE];
    Faculty faculty;
    int fd, last_id = 0;
    int duplicate_found = 0;

    strcpy(buffer, "Enter faculty name: ");
    send_message_and_prompt(client_socket, buffer);
    memset(buffer, 0, BUFFER_SIZE);
    
    int check = recv_message(client_socket, buffer);
    if (check < 0)
    {
        send_message(client_socket, "Error\n");
        return;
    }
    buffer[strcspn(buffer, "\n")] = 0; // Remove newline
    strcpy(faculty.name, buffer);

    sem_wait(&faculty_sem);
    fd = open("faculty.dat", O_RDONLY);
    if (fd != -1)
    {
        Faculty temp;
        while (read(fd, &temp, sizeof(Faculty)) > 0)
        {
            if (strcmp(faculty.name, temp.name) == 0)
            {
                duplicate_found = 1;
                break;
            }
            last_id = temp.id;
        }
        close(fd);
    }

    if (duplicate_found)
    {
        strcpy(buffer, "Error: Faculty username already exists.\n");
        send_message(client_socket, buffer);
        sem_post(&faculty_sem);
        return;
    }

    strcpy(buffer, "Enter faculty password: ");
    send_message_and_prompt(client_socket, buffer);
    memset(buffer, 0, BUFFER_SIZE);
    
    check = recv_message(client_socket, buffer);
    if (check < 0)
    {
        send_message(client_socket, "Socket Error\n");
        sem_post(&faculty_sem);
        return;
    }
    buffer[strcspn(buffer, "\n")] = 0; // Remove newline
    strcpy(faculty.password, buffer);

    faculty.id = last_id + 1;

    fd = open("faculty.dat", O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd == -1)
    {
        strcpy(buffer, "Error: Could not open faculty database for writing.\n");
        send_message(client_socket, buffer);
        sem_post(&faculty_sem);
        return;
    }

    write(fd, &faculty, sizeof(Faculty));
    close(fd);
    sem_post(&faculty_sem);

    sprintf(buffer, "Faculty with ID: %d\n added.", faculty.id);
    send_message(client_socket, buffer);
}

// View faculty details function
void view_faculty_details(int client_socket) {
    char buffer[BUFFER_SIZE];
    int fd;
    Faculty faculty;
    int found = 0;

    sem_wait(&faculty_sem);

    fd = open("faculty.dat", O_RDONLY);
    if (fd == -1) {
        strcpy(buffer, "Error: Could not open faculty database.\n");
        send_message(client_socket, buffer);
        sem_post(&faculty_sem);
        return;
    }

    send_message(client_socket, "Faculty Details:\n");
    while (read(fd, &faculty, sizeof(Faculty)) > 0) {
        sprintf(buffer, "ID: %d | Name: %s\n", faculty.id, faculty.name);
        send_message(client_socket, buffer);
        found = 1;
    }

    if (!found) {
        send_message(client_socket, "No faculty found.\n");
    }

    close(fd);
    sem_post(&faculty_sem);
}


// Activate student function
void activate_student(int client_socket)
{
    char buffer[BUFFER_SIZE];
    int student_id, fd, found = 0;
    Student student;

    strcpy(buffer, "Enter student ID: ");
    send_message_and_prompt(client_socket, buffer);
    memset(buffer, 0, BUFFER_SIZE);
    
    int check = recv_message(client_socket, buffer);
    if (check < 0)
    {
        send_message(client_socket, "Socket Error\n");
        return;
    }
    buffer[strcspn(buffer, "\n")] = 0; // Remove newline
    
    student_id = atoi(buffer);
    if (student_id <= 0) {
        send_message(client_socket, "Invalid Student ID.\n");
        return;
    }

    sem_wait(&student_sem);

    int temp_fd = open("temp_students.dat", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (temp_fd == -1)
    {
        strcpy(buffer, "Error: Could not create temporary file.\n");
        send_message(client_socket, buffer);
        sem_post(&student_sem);
        return;
    }

    fd = open("students.dat", O_RDONLY);
    if (fd == -1)
    {
        strcpy(buffer, "Error: Could not open student database.\n");
        send_message(client_socket, buffer);
        close(temp_fd);
        unlink("temp_students.dat");
        sem_post(&student_sem);
        return;
    }

    while (read(fd, &student, sizeof(Student)) > 0)
    {
        if (student.id == student_id)
        {
            found = 1;
            student.active = 1; // Set status to active
        }
        write(temp_fd, &student, sizeof(Student));
    }

    close(fd);
    close(temp_fd);

    if (found)
    {
        if (rename("temp_students.dat", "students.dat") == 0)
        {
            sprintf(buffer, "Student ID %d activated successfully.\n", student_id);
        }
        else
        {
            strcpy(buffer, "Error: Failed to activate student.\n");
            unlink("temp_students.dat");
        }
    }
    else
    {
        unlink("temp_students.dat");
        strcpy(buffer, "Error: Student not found.\n");
    }

    sem_post(&student_sem);
    send_message(client_socket, buffer);
}

// Block student function
void block_student(int client_socket)
{
    char buffer[BUFFER_SIZE];
    int student_id, fd, found = 0;
    Student student;

    strcpy(buffer, "Enter student ID: ");
    send_message_and_prompt(client_socket, buffer);
    memset(buffer, 0, BUFFER_SIZE);
    
    int check = recv_message(client_socket, buffer);
    if (check < 0)
    {
        send_message(client_socket, "Socket Error\n");
        return;
    }
    buffer[strcspn(buffer, "\n")] = 0; // Remove newline
    
    student_id = atoi(buffer);
    if (student_id <= 0) {
        send_message(client_socket, "Invalid Student ID.\n");
        return;
    }

    sem_wait(&student_sem);

    int temp_fd = open("temp_students.dat", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (temp_fd == -1)
    {
        strcpy(buffer, "Error: Could not create temporary file.\n");
        send_message(client_socket, buffer);
        sem_post(&student_sem);
        return;
    }

    fd = open("students.dat", O_RDONLY);
    if (fd == -1)
    {
        strcpy(buffer, "Error: Could not open student database.\n");
        send_message(client_socket, buffer);
        close(temp_fd);
        unlink("temp_students.dat");
        sem_post(&student_sem);
        return;
    }

    while (read(fd, &student, sizeof(Student)) > 0)
    {
        if (student.id == student_id)
        {
            found = 1;
            student.active = 0; // Set status to blocked
        }
        write(temp_fd, &student, sizeof(Student));
    }

    close(fd);
    close(temp_fd);

    if (found)
    {
        if (rename("temp_students.dat", "students.dat") == 0)
        {
            sprintf(buffer, "Student ID %d blocked successfully.\n", student_id);
        }
        else
        {
            strcpy(buffer, "Error: Failed to block student.\n");
            unlink("temp_students.dat");
        }
    }
    else
    {
        unlink("temp_students.dat");
        strcpy(buffer, "Error: Student not found.\n");
    }

    sem_post(&student_sem);
    send_message(client_socket, buffer);
}

// Modify student details function
void modify_student_details(int client_socket)
{
    char buffer[BUFFER_SIZE];
    int student_id, fd, found = 0;
    Student student;
    char new_name[50];
    char new_password[50];
    int duplicate_found = 0;

    strcpy(buffer, "Enter Student ID: ");
    send_message_and_prompt(client_socket, buffer);
    memset(buffer, 0, BUFFER_SIZE);
    
    int check = recv_message(client_socket, buffer);
    if (check < 0)
    {
        send_message(client_socket, "Socket Error\n");
        return;
    }
    buffer[strcspn(buffer, "\n")] = 0; // Remove newline
    
    student_id = atoi(buffer);
    if (student_id <= 0) {
        send_message(client_socket, "Invalid Student ID.\n");
        return;
    }

    strcpy(buffer, "Enter new name: ");
    send_message_and_prompt(client_socket, buffer);
    memset(buffer, 0, BUFFER_SIZE);
    
    check = recv_message(client_socket, buffer);
    if (check < 0)
    {
        send_message(client_socket, "Socket Error\n");
        return;
    }
    buffer[strcspn(buffer, "\n")] = 0; // Remove newline
    strcpy(new_name, buffer);

    if (strlen(new_name) > 0)
    {
        sem_wait(&student_sem);
        fd = open("students.dat", O_RDONLY);
        if (fd != -1)
        {
            Student temp_student;
            while (read(fd, &temp_student, sizeof(Student)) > 0)
            {
                if (temp_student.id != student_id && strcmp(new_name, temp_student.name) == 0)
                {
                    duplicate_found = 1;
                    break;
                }
            }
            close(fd);
        }
        sem_post(&student_sem);
    }

    if (duplicate_found)
    {
        strcpy(buffer, "Error: A user with this name already exists.\n");
        send_message(client_socket, buffer);
        return;
    }

    strcpy(buffer, "Enter new password: ");
    send_message_and_prompt(client_socket, buffer);
    memset(buffer, 0, BUFFER_SIZE);
    
    check = recv_message(client_socket, buffer);
    if (check < 0)
    {
        send_message(client_socket, "Socket Error\n");
        return;
    }
    buffer[strcspn(buffer, "\n")] = 0; // Remove newline
    strcpy(new_password, buffer);

    sem_wait(&student_sem);

    int temp_fd = open("temp_students.dat", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (temp_fd == -1)
    {
        strcpy(buffer, "Error: Could not create temporary file.\n");
        send_message(client_socket, buffer);
        sem_post(&student_sem);
        return;
    }

    fd = open("students.dat", O_RDONLY);
    if (fd == -1)
    {
        strcpy(buffer, "Error: Could not open student database.\n");
        send_message(client_socket, buffer);
        close(temp_fd);
        unlink("temp_students.dat");
        sem_post(&student_sem);
        return;
    }

    while (read(fd, &student, sizeof(Student)) > 0)
    {
        if (student.id == student_id)
        {
            found = 1;
            if (strlen(new_name) > 0)
            {
                strcpy(student.name, new_name);
            }
            if (strlen(new_password) > 0)
            {
                strcpy(student.password, new_password);
            }
        }
        write(temp_fd, &student, sizeof(Student));
    }

    close(fd);
    close(temp_fd);

    if (found)
    {
        if (rename("temp_students.dat", "students.dat") == 0)
        {
            strcpy(buffer, "Student details updated.\n");
        }
        else
        {
            strcpy(buffer, "Error: Failed to update student details.\n");
            unlink("temp_students.dat");
        }
    }
    else
    {
        unlink("temp_students.dat");
        strcpy(buffer, "Error: Student not found.\n");
    }

    sem_post(&student_sem);
    send_message(client_socket, buffer);
}

// Modify faculty details function
void modify_faculty_details(int client_socket)
{
    char buffer[BUFFER_SIZE];
    int faculty_id, fd, found = 0;
    Faculty faculty;
    char new_name[50];
    char new_password[50];
    int duplicate_found = 0;

    strcpy(buffer, "Enter Faculty ID to modify: ");
    send_message_and_prompt(client_socket, buffer);
    memset(buffer, 0, BUFFER_SIZE);
    
    int check = recv_message(client_socket, buffer);
    if (check < 0)
    {
        send_message(client_socket, "Socket Error\n");
        return;
    }
    buffer[strcspn(buffer, "\n")] = 0; // Remove newline
    
    faculty_id = atoi(buffer);
    if (faculty_id <= 0) {
        send_message(client_socket, "Invalid Faculty ID.\n");
        return;
    }

    strcpy(buffer, "Enter new name: ");
    send_message_and_prompt(client_socket, buffer);
    memset(buffer, 0, BUFFER_SIZE);
    
    check = recv_message(client_socket, buffer);
    if (check < 0)
    {
        send_message(client_socket, "Socket Error\n");
        return;
    }
    buffer[strcspn(buffer, "\n")] = 0; // Remove newline
    strcpy(new_name, buffer);

    if (strlen(new_name) > 0)
    {
        sem_wait(&faculty_sem);
        fd = open("faculty.dat", O_RDONLY);
        if (fd != -1)
        {
            Faculty temp_faculty;
            while (read(fd, &temp_faculty, sizeof(Faculty)) > 0)
            {
                if (temp_faculty.id != faculty_id && strcmp(new_name, temp_faculty.name) == 0)
                {
                    duplicate_found = 1;
                    break;
                }
            }
            close(fd);
        }
        sem_post(&faculty_sem);
    }

    if (duplicate_found)
    {
        strcpy(buffer, "Error: A user with this name already exists.\n");
        send_message(client_socket, buffer);
        return;
    }

    strcpy(buffer, "Enter new password: ");
    send_message_and_prompt(client_socket, buffer);
    memset(buffer, 0, BUFFER_SIZE);
    
    check = recv_message(client_socket, buffer);
    if (check < 0)
    {
        send_message(client_socket, "Socket Error\n");
        return;
    }
    buffer[strcspn(buffer, "\n")] = 0; // Remove newline
    strcpy(new_password, buffer);

    sem_wait(&faculty_sem);

    int temp_fd = open("temp_faculty.dat", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (temp_fd == -1)
    {
        strcpy(buffer, "Error: Could not create temporary file.\n");
        send_message(client_socket, buffer);
        sem_post(&faculty_sem);
        return;
    }

    fd = open("faculty.dat", O_RDONLY);
    if (fd == -1)
    {
        strcpy(buffer, "Error: Could not open faculty database.\n");
        send_message(client_socket, buffer);
        close(temp_fd);
        unlink("temp_faculty.dat");
        sem_post(&faculty_sem);
        return;
    }

    while (read(fd, &faculty, sizeof(Faculty)) > 0)
    {
        if (faculty.id == faculty_id)
        {
            found = 1;
            if (strlen(new_name) > 0)
            {
                strcpy(faculty.name, new_name);
            }
            if (strlen(new_password) > 0)
            {
                strcpy(faculty.password, new_password);
            }
        }
        write(temp_fd, &faculty, sizeof(Faculty));
    }

    close(fd);
    close(temp_fd);

    if (found)
    {
        if (rename("temp_faculty.dat", "faculty.dat") == 0)
        {
            strcpy(buffer, "Faculty details updated.\n");
        }
        else
        {
            strcpy(buffer, "Error: Failed to update faculty details.\n");
            unlink("temp_faculty.dat");
        }
    }
    else
    {
        unlink("temp_faculty.dat");
        strcpy(buffer, "Error: Faculty not found.\n");
    }

    sem_post(&faculty_sem);
    send_message(client_socket, buffer);
}


// Admin menu function
void admin_menu(int client_socket)
{
    char buffer[BUFFER_SIZE];
    int choice;

    while (1)
    {
        strcpy(buffer, "=== Admin Menu ===\n1. Add Student\n2. View Student Details\n3. Add Faculty\n4. View Faculty Details\n5. Activate Student\n6. Block Student\n7. Edit Student Details\n8. Edit Faculty Details\n9. Logout & Exit\nYour Choice: ");
        
        send_message_and_prompt(client_socket, buffer);
        memset(buffer, 0, BUFFER_SIZE);

        int check = recv_message(client_socket, buffer);
        if (check < 0)
        {
            send_message(client_socket, "Socket Error\n");
            return; // Exit menu loop on communication error
        }
        
        // Remove trailing newline character from the received choice
        buffer[strcspn(buffer, "\n")] = 0;

        // Basic validation to ensure input is a number
        int is_valid_number = 1;
        if (strlen(buffer) == 0) {
            is_valid_number = 0;
        } else {
            for (int i = 0; buffer[i] != '\0'; i++) {
                if (!isdigit(buffer[i])) {
                    is_valid_number = 0;
                    break;
                }
            }
        }

        if (!is_valid_number) {
            choice = 0; // Assign 0 to fall into the default case
        } else {
            choice = atoi(buffer);
        }

        switch (choice)
        {
            case 1:
                add_student(client_socket);
                break;
            case 2:
                view_student_details(client_socket);
                break;
            case 3:
                add_faculty(client_socket);
                break;
            case 4:
                view_faculty_details(client_socket);
                break;
            case 5:
                activate_student(client_socket);
                break;
            case 6:
                block_student(client_socket);
                break;
            case 7:
                modify_student_details(client_socket);
                break;
            case 8:
                modify_faculty_details(client_socket);
                break;
            case 9:
                send_message(client_socket, "Logging off...\n");
                log_out(client_socket);
                return; // Exit the admin menu loop
            default:
                strcpy(buffer, "Invalid choice. Please try again.\n");
                send_message(client_socket, buffer);
                break;  // Add break statement to avoid falling through
        }
        
        // Ensure the buffer is cleared after each operation
        memset(buffer, 0, BUFFER_SIZE);
    }
}