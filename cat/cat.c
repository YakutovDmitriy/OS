#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
	char buf[2007];
	int len;
	while (len = read(STDIN_FILENO, buf, sizeof buf)) {
		if (len < 0) {
			fprintf(stderr, "Error occured while reading\n");
			return EXIT_FAILURE;
		}
		int wrote = 0;
		while (wrote < len) {
			int cur = write(STDOUT_FILENO, buf + wrote, len - wrote);
			if (cur < 0) {
				fprintf(stderr, "Error occured while writing\n");
				return EXIT_FAILURE;
			}
			wrote += cur;
		}
	}
	return EXIT_SUCCESS;
}
