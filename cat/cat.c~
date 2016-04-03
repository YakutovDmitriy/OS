#include <stdio.h>

int const BUF_SIZE = 2007;

int main() {
	char buf[BUF_SIZE];
	int len;
	while ((len = read(0, buf, BUF_SIZE)) > 0) {
		int cnt = 0;
		while ((cnt += write(1, buf + cnt, len)) < len);
	}
}
