// IPC using named pipes: the writer
// gcc -ggdb -Wall -o writer writer.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <errno.h>

#define INPUT_SIZE 100

int main(void) {
	const char* _pipe = "/tmp/my_own_pipe";

	if (mkfifo(_pipe, 0666) < 0 && errno != EEXIST) {
		perror("mkfifo");
		exit(EXIT_FAILURE);
	}

	char input[INPUT_SIZE];
	int write_fd;

	if ((write_fd = open(_pipe, O_WRONLY)) < 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }

	while (1) {
		if (!fgets(input, INPUT_SIZE, stdin)) break;

		if (strncmp(input, "exit", 4) == 0) {
			break;
		}

		if (write(write_fd, input, strlen(input) + 1) < 0) {
			perror("write");
			break;
		}
	}

	close(write_fd);
	unlink(_pipe);

	return 0;
}