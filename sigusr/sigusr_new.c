#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

void handler(int sig, siginfo_t* info, void* ctx) {
    printf("%s from %d\n", (sig == SIGUSR1 ? "SIGUSR1" : "SIGUSR2"), info->si_pid);
    exit(0);
}

void empty_handler(int) {}

int main() {
    struct sigaction act;
    
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGUSR1);
    sigaddset(&act.sa_mask, SIGUSR2);
    
    act.sa_sigaction = handler;
    act.sa_flags = SA_SIGINFO;

    if (sigaction(SIGUSR1, &act, NULL) || sigaction(SIGUSR2, &act, NULL)) {
        puts("Error occured while setting sigaction");
        return 1;
    }
    
    sleep(10);
    
    struct sigaction act1;
    act1.sa_sigaction = empty_handler;
    act1.sa_flags = 0;
    
    puts("No signals were caught");
    return 0;
}
