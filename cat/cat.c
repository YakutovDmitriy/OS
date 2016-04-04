#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
	char buffer[2007];
	int len;
	while (len = read(STDIN_FILENO, buffer, sizeof buffer)) {
		if (len < 0) {
			fputs("Error occured while reading", stderr);
			return EXIT_FAILURE;
		}
		int written = 0;
		while (written < len) {
			int cur = write(STDOUT_FILENO, buffer + written, len - written);
			if (cur < 0) {
				fputs("Error occured while writing", stderr);
				return EXIT_FAILURE;
			}
			written += cur;
		}
	}
	return EXIT_SUCCESS;
}
