# Define the compiler
CC = gcc

# Define compiler flags
CFLAGS = -Wall -Wextra -pthread -g

# Source files
SERVER_SRCS = Server.c Utilities.c Admin.c Faculty.c Student.c
CLIENT_SRCS = Client.c Utilities.c

# Object files
SERVER_OBJS = $(SERVER_SRCS:.c=.o)
CLIENT_OBJS = $(CLIENT_SRCS:.c=.o)

# Executables
SERVER_EXEC = server
CLIENT_EXEC = client

# Data files
DATAFILES = students.dat faculty.dat courses.dat

# Default target
all: $(SERVER_EXEC) $(CLIENT_EXEC) $(DATAFILES)

# Rule to build server
$(SERVER_EXEC): $(SERVER_OBJS)
	$(CC) $(CFLAGS) $(SERVER_OBJS) -o $(SERVER_EXEC)

# Rule to build client — include utils.o here!
$(CLIENT_EXEC): Client.o Utilities.o
	$(CC) $(CFLAGS) Client.o Utilities.o -o $(CLIENT_EXEC)

# Generic rule to compile .c to .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to create empty .dat files if they do not exist
$(DATAFILES):
	touch $(DATAFILES)

# Clean rule
clean:
	rm -f $(SERVER_OBJS) $(CLIENT_OBJS) $(SERVER_EXEC) $(CLIENT_EXEC) *.dat temp_*.dat

# Phony targets
.PHONY: all clean
