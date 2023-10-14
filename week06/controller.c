#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>

#define PID_PATH "agent.pid"
#define AGENT_NOT_FOUND "Error: No agent found.\n"
#define AGENT_FOUND "Agent found.\n"
#define INFO "Choose a command {\"read\", \"exit\", \"stop\","\
    " \"continue\"} to send to the agent\n"
#define MAX_CMD_LEN 10 

void read_pid(pid_t* pid) {
    FILE* pid_file = fopen(PID_PATH, "r");
    if (pid_file == NULL) {
        *pid = -1;
        return;
    }
    fscanf(pid_file, "%d", pid);
    fclose(pid_file);
}

pid_t agent_id;

void sigint_handler(int signum) {
    kill(agent_id, SIGTERM);
    exit(signum);
}

int main() {
    read_pid(&agent_id);
    if (agent_id == -1 || kill(agent_id, 0)) {
        printf(AGENT_NOT_FOUND);
        return 0;
    }

    signal(SIGINT, sigint_handler);

    for (;;) {
        printf(INFO);

        char cmd[MAX_CMD_LEN];
        scanf("%s", cmd);
        
        if (!strcmp(cmd, "read")) {
            kill(agent_id, SIGUSR1);
        } else if (!strcmp(cmd, "exit")) {
            kill(agent_id, SIGUSR2);
            exit(SIGUSR2);
        } else if (!strcmp(cmd, "stop")) {
            kill(agent_id, SIGSTOP);
        } else if (!strcmp(cmd, "continue")) {
            kill(agent_id, SIGCONT);
        }
    }
}
