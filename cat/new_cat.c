#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

int const READ_MODE = 0;
int const WRITE_MODE = 1;

int try_rw(int mode, int fd, char* buffer, int count) {
	while (1) {
		int len;
		if (mode == READ_MODE) {
			len = read(fd, buffer, count);
		} else {
			len = write(fd, buffer, count);
		}
		if (len >= 0 || errno != EINTR) {
			return len;
		}
	}
}

int in_fd, out_fd;

void oops(char const* msg) {
	fputs(msg, stderr);
	close(in_fd);
	close(out_fd);
	exit(EXIT_FAILURE);
}

int main(int argc, char** argv) {

	in_fd = argc >= 2 ? open(argv[1], O_RDONLY) : STDIN_FILENO;
	out_fd = argc >= 3 ? open(argv[2], O_WRONLY | O_CREAT, S_IREAD | S_IWRITE) : STDOUT_FILENO;
	
	if (in_fd < 0) {
		oops("Cannot open input file");
	}

	if (out_fd < 0) {
		oops("Cannot open output file");
	}

	char buffer[2007];
	
	int len;
	while (len = try_rw(READ_MODE, in_fd, buffer, sizeof buffer)) {
		
		if (len < 0) {
			oops("Error occured while reading");
		}
		
		int written = 0;
		while (written < len) {
			int cur = try_rw(WRITE_MODE, out_fd, buffer + written, len - written);
			if (cur < 0) {
				oops("Error occured while writing");
			}
			written += cur;
		}
	}
	close(in_fd);
	close(out_fd);
	return EXIT_SUCCESS;
}
