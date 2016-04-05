#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include <algorithm>
#include <string>
#include <vector>

int const bsn = '\n';

int get_char() {
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
	int ch = get_char();
	if (ch < 0) {
		return false;
	}
	while (ch <= bsn) {
		ch = get_char();
		if (ch < 0) {
			return false;
		}
	}
	while (ch > bsn) {
		to.push_back((char) ch);
		ch = get_char();
		if (ch < 0) {
			return false;
		}
	}
	return true;
}

bool print_line(int fd, std::string const& line, bool eoln) {
	char const* buf = eoln ? (line + (char)bsn).data() : line.data();
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
	bool q1 = false, q2 = false;
	for (char x : s) {
		if (sep == ' ' && x == '\'' && !q2) {
			q1 ^= true;
			continue;
		}
		if (sep == ' ' && x == '"' && !q1) {
			q2 ^= true;
			continue;
		}
		if (x == sep && !q1 && !q2) {
			while (!cur.empty() && cur.back() == ' ') {
				cur.pop_back();
			}
			ret.push_back(cur);
			cur.clear();
		} else {
			if (x != ' ' || !cur.empty() || q1 || q2) {
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

std::vector<pid_t> pids;

void process(std::string const& cmd) {
	auto subcmds = split(cmd, '|');
	int cnt = subcmds.size();
	pids.clear();

	int p[2];

	for (int i = 0; i < cnt; ++i) {
		int new_p[2];
		if (i + 1 < cnt) {
			pipe2(new_p, O_CLOEXEC);
		}

		int rfd = i == 0 ? STDIN_FILENO : p[0];
		int wfd = i + 1 == cnt ? STDOUT_FILENO : new_p[1];
		pids.push_back(run_subcmd(subcmds[i], rfd, wfd));

		if (i > 0) {
			close(p[0]);
			close(p[1]);
		}

		std::swap(p, new_p);
	}
}

void handler(int sig) {
	for (auto pid : pids) {
		kill(pid, sig);
	}
}

int main() {
	struct sigaction sigact;
	sigact.sa_handler = handler;
	sigaction(SIGINT, &sigact, NULL);

	while (true) {
		for (auto pid : pids) {
			waitpid(pid, NULL, 0);
		}
		print_line(STDOUT_FILENO, "$ ", false);
		std::string cmd;
		if (!next_line(cmd)) {
			break;
		}
		process(cmd);
	}
	return EXIT_SUCCESS;
}
