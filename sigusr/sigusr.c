#include <stdio.h>
#include <signal.h>

typedef struct sigaction act_t;

void handler(int sig, siginfo_t* info, void* ctx) {
    printf("%s from %d\n", (SIGUSR1 ? "SIGUSR1" : "SIGUSR2"), info->si_pid);
}

int main() {
    act_t act;
    act.sa_sigaction = handler;
    act.sa_flags = SA_SIGINFO;

    if (sigaction(SIGUSR1, &act, NULL) || sigaction(SIGUSR2, &act, NULL)) {
        puts("Error occured while setting sigaction");
    }

    if (!sleep(10)) {
        puts("No signals were caught");
    }

    return 0;
}