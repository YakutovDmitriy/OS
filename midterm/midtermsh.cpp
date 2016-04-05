#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <algorithm>
#include <string>
#include <vector>

char const bsn = '\n';

char get_char() {
	static char buf[2007];
	static size_t cur = 0;
	static size_t len = 0;
	if (cur >= len) {
		len = read(0, buf, sizeof buf);
		if (len <= 0) {
			return -1;
		}
		cur = 0;
	}
	return buf[cur++];
}

bool next_line(std::string& to) {
	to = std::string();
	char ch = get_char();
	bool was_error = ch < 0;
	while (ch <= bsn) {
		ch = get_char();
		was_error |= ch < 0;
	}
	while (ch > bsn) {
		to.push_back(ch);
		ch = get_char();
		was_error |= ch < 0;
	}
	return !was_error;
}

bool print_line(int fd, std::string const& line) {
	char const* buf = (line + bsn).data();
	size_t szbuf = 1U + line.size();
	size_t cur = 0;
	while (cur < line.size()) {
		int now = write(fd, buf + cur, szbuf - cur);
		if (now < 0) {
			return false;
		}
		cur += now;
	}
	return true;
}

std::vector<std::string> split(std::string s, char sep) {
	s += sep;
	std::vector<std::string> ret;
	std::string cur;
	for (char x : s) {
		if (x == sep) {
			while (!cur.empty() && cur.back() == ' ') {
				cur.pop_back();
			}
			ret.push_back(cur);
			cur.clear();
		} else {
			if (x != ' ' || !cur.empty()) {
				cur += x;
			}
		}
	}
	return ret;
}

int run_subcmd(std::string const& cmd, int rfd, int wfd) {
	if (int child = fork()) {
		return child;
	} else {
		auto args = split(cmd, ' ');
		dup2(rfd, STDIN_FILENO);
		dup2(wfd, STDOUT_FILENO);
		char** c_args = new char*[args.size() + 1];
		c_args[args.size()] = 0;
		for (size_t q = 0; q < args.size(); ++q) {
			c_args[q] = new char[args[q].size() + 1];
			c_args[q][args[q].size()] = 0;
			memcpy(c_args[q], args[q].data(), args[q].size());
		}
		execvp(c_args[0], c_args);
	}
}

void process(std::string const& cmd) {
	auto subcmds = split(cmd, '|');
	int cnt = subcmds.size();
	std::vector<int> pids(cnt);

	int p[2];

	for (int i = 0; i < cnt; ++i) {
		int new_p[2];
		if (i + 1 < cnt) {
			pipe2(new_p, O_CLOEXEC);
		}

		int rfd = i == 0 ? STDIN_FILENO : p[0];
		int wfd = i + 1 == cnt ? STDOUT_FILENO : new_p[1];
		pids[i] = run_subcmd(subcmds[i], rfd, wfd);

		if (i > 0) {
			close(p[0]);
			close(p[1]);
		}

		std::swap(p, new_p);
	}
}

int main() {
	std::string cmd;
	while (true) {
		print_line(STDOUT_FILENO, "$");
		if (!next_line(cmd)) {
			break;
		}
		process(cmd);
	}
	return EXIT_SUCCESS;
}
