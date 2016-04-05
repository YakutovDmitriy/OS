#include <cstdio>
#include <algorithm>
#include <unistd.h>
#include <string>
#include <vector>
#include <cstring>

#include <sys/types.h>
#include <sys/wait.h>

char const bsn = '\n';

char get_char() {
	static char buf[2007];
	static size_t cur = 0; 
	static size_t len =0;
	if (cur >= len) {
		len = read(0, buf, sizeof buf);
		if (len < 0) {
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

bool print_line(std::string const& line) {
	char const* buf = (line + bsn).data();
	size_t szbuf = 1U + line.size();
	size_t cur = 0;
	while (cur < line.size()) {
		int now = write(1, buf + cur, szbuf - cur);
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

void launch(std::string const& cmd, std::vector<std::string> const& args) {
	if (!fork()) {
		char** c_args = new char*[args.size() + 1];
		c_args[args.size()] = 0;
		for (size_t i = 0; i < args.size(); ++i) {
			c_args[i] = new char[args[i].size() + 1];
			c_args[i][args[i].size()] = 0;
			memcpy(c_args[i], args[i].data(), args[i].size());
		}
		execvp(cmd.data(), c_args);
	}
}

void process(std::string const& cmd) {
	auto subcmds = split(cmd, '|');
	for (auto p : subcmds) {
		auto x = split(p, ' ');
		std::string cmd = x[0];
		launch(cmd, x);
	}
}

int main() {
	std::string cmd;
	while (true) {
		print_line("$");
		if (!next_line(cmd)) {
			break;
		}
		process(cmd);
	}
	return EXIT_SUCCESS;
}