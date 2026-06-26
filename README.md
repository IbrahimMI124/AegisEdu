# AegisEdu

A client-server based Academic Course Registration System developed in **C** as an Operating Systems mini project. The system enables secure interaction between students, faculty, and administrators through role-based authentication, concurrent client handling, and persistent file-based storage.

## Features

- 🔐 Role-Based Authentication
  - Student
  - Faculty
  - Administrator

- 📚 Course Management
  - Add, update, and remove courses
  - View available courses
  - Manage course enrollments

- 👨‍🎓 Student Portal
  - Enroll in courses
  - Drop courses
  - View enrolled courses
  - Change password

- 👨‍🏫 Faculty Portal
  - Add new courses
  - Remove courses
  - Update course details
  - View offered courses
  - Change password

- 👨‍💼 Administrator Portal
  - Add students
  - Modify student details
  - Add faculty members
  - Modify faculty details
  - Activate student accounts
  - Block student accounts

- ⚡ Concurrent Client Support
  - Multithreaded server
  - File locking
  - Semaphores
  - Read/Write synchronization

- 💾 Persistent Storage
  - File-based database
  - Student records
  - Faculty records
  - Course records

---

# Technologies Used

- C Programming
- Socket Programming
- POSIX Threads (Pthreads)
- Semaphores
- File Locking
- Linux System Calls
- File I/O

---

# Operating System Concepts Demonstrated

This project demonstrates several core Operating Systems concepts:

- Client-Server Architecture
- Socket Programming
- Multithreading
- Process Management
- File Management
- File Locking
- Synchronization using Semaphores
- Concurrent Access Control
- Inter-Process Communication
- System Calls

---

# System Architecture

```
                  +-------------------+
                  |      Client       |
                  |-------------------|
                  | Student Portal    |
                  | Faculty Portal    |
                  | Admin Portal      |
                  +---------+---------+
                            |
                      Socket Connection
                            |
                            v
                  +-------------------+
                  |      Server       |
                  |-------------------|
                  | Authentication    |
                  | Course Manager    |
                  | Student Manager   |
                  | Faculty Manager   |
                  | Admin Manager     |
                  +---------+---------+
                            |
                --------------------------
                |           |            |
                v           v            v
          Student File Faculty File Course File
```

---

# Project Structure

```
.
├── Admin.c
├── Admin.h
├── Client.c
├── Client.h
├── Faculty.c
├── Faculty.h
├── Student.c
├── Student.h
├── Server.c
├── Server.h
├── Utilities.c
├── Utilities.h
└── README.md
```

---

# User Roles

## Administrator

- Add students
- Modify student information
- Add faculty
- Modify faculty information
- Activate student accounts
- Block student accounts

## Faculty

- Add courses
- Remove courses
- Update course details
- View offered courses
- Change password

## Student

- View available courses
- Enroll in courses
- Drop courses
- View enrolled courses
- Change password

---

# Authentication

The server authenticates every user before granting access.

Supported login roles:

- Administrator
- Faculty
- Student

After successful authentication, users are redirected to their respective dashboards.

---

# Concurrency Control

To ensure data consistency during simultaneous client access, the project implements:

- POSIX Semaphores
- Read Locks
- Write Locks
- Thread Synchronization

Course enrollment and drop operations are protected using write locks to prevent race conditions.

---

# Data Storage

The application uses persistent file storage for:

- Students
- Faculty
- Courses

All CRUD operations are performed directly on these files using low-level system calls.

---

# System Calls Used

The project makes extensive use of Linux system calls, including:

- `open()`
- `read()`
- `write()`
- `close()`
- `socket()`
- `bind()`
- `listen()`
- `accept()`
- `connect()`
- `fork()`
- `pthread_create()`
- `sem_wait()`
- `sem_post()`
- `flock()`

---

# Screenshots

## Client-Server Connection

_Add screenshot here_

## Add Student

_Add screenshot here_

## Edit Student Details

_Add screenshot here_

## Faculty Portal

_Add screenshot here_

## Add Course & Change Password

_Add screenshot here_

## Student Login

_Add screenshot here_

## Course Enrollment

_Add screenshot here_

## View & Drop Course

_Add screenshot here_

## Logout & Exit

_Add screenshot here_

---

# Future Improvements

- Database integration (MySQL/PostgreSQL)
- Password hashing
- GUI/Desktop application
- Web interface
- Logging and audit trails
- Course search and filtering
- Student registration history
- Email notifications

---

# Author

**Mohammed Ibrahim**

- IMT2023112
- Operating Systems Mini Project
- International Institute of Information Technology Bangalore (IIIT-B)

---

# License

This project was developed for academic purposes as part of an Operating Systems course.
