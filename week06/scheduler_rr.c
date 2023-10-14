#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <limits.h>

#define PS_MAX 10
#define QUANT 3

typedef struct queue_node {
    int x;
    struct queue_node* prev;
    struct queue_node* next;
} queue_node;

typedef struct {
    size_t size;
    queue_node* back;
    queue_node* front;
} queue;

size_t queue_size(queue* q) {
    return q->size;
}

void push(queue* q, int x) {
    if (queue_size(q) == 0) {
        queue_node* new_node = (queue_node*) malloc(sizeof(queue_node));
        new_node->x = x;
        new_node->prev = new_node->next = NULL;
        q->back = q->front = new_node;
        ++q->size;
        return;
    }
    queue_node* new_node = (queue_node*) malloc(sizeof(queue_node));
    new_node->x = x;
    new_node->prev  = NULL;
    new_node->next = q->back;
    q->back->prev = new_node;
    q->back = new_node;
    ++q->size;
}

int front(queue* q) {
    return q->front->x;
}

int back(queue* q) {
    return q->back->x;
}

void pop(queue* q) {
    if (queue_size(q) == 1) {
        free(q->front);
        q->front = q->back = NULL;
        --q->size;
        return;
    }
    queue_node* new_front = q->front->prev;
    new_front->next = NULL;
    free(q->front);
    q->front = new_front;
    --q->size;
}

// holds the scheduling data of one process
typedef struct{
	int idx; // process idx (index)
	unsigned at, bt, rt, wt, ct, tat; // arrival time, burst time, response time, waiting time, completion time, turnaround time.
	int burst; // remaining burst (this should decrement when the process is being executed)
} ProcessData;

// the idx of the running process
int running_process = -1; // -1 means no running processes

// the total time of the timer
unsigned total_time; // should increment one second at a time by the scheduler

// data of the processes
ProcessData data[PS_MAX]; // array of process data

// array of all process pids
pid_t ps[PS_MAX]; // zero valued pids - means the process is terminated or not created yet

// size of data array
unsigned data_size;

// queue of processes
queue rr_queue;

// remaining time of quant
int quant;

void read_file(FILE* file){
    char header_buf[10];
    fscanf(file, "%s", header_buf);
    fscanf(file, "%s", header_buf);
    fscanf(file, "%s", header_buf);
    
    int idx;
    int at;
    int bt;
    

    for (int i = 0; i < PS_MAX; ++i) {
        ps[i] = 0;
    }

    while (fscanf(file, "%d", &idx) == 1) {
        fscanf(file, "%d", &at);
        fscanf(file, "%d", &bt);
        data[idx].idx = idx;
        data[idx].at = at;
        data[idx].bt = bt;
        data[idx].burst = bt;
        ++data_size;
    }
}

// send signal SIGCONT to the worker process
void resume(pid_t process) {
    if (ps[process] != 0) {
        kill(ps[process], SIGCONT);
    }
}

// send signal SIGTSTP to the worker process
void suspend(pid_t process) {
    if (ps[process] != 0) {
        kill(ps[process], SIGTSTP);
    }
}

// send signal SIGTERM to the worker process
void terminate(pid_t process) {
    if (ps[process] != 0)  {
        kill(ps[process], SIGTERM);
    }
}

// create a process using fork
void create_process(int new_process) {
    if ((ps[new_process] = fork())) {
        quant = QUANT;
        running_process = new_process;
        data[running_process].rt = total_time - data[running_process].at;
    } else {
        char str_process[10];
        sprintf(str_process, "%d", new_process);
        char* worker = "./worker";
        char* args[] = {worker, str_process, NULL};
        execvp(worker, args);
    }
}

// add processes that come to this moment
void add_new_processes() {
    for (size_t idx = 0; idx < data_size; ++idx) {
        if (data[idx].at == total_time) {
            push(&rr_queue, data[idx].idx);
        }
    }
}

// find next process for running
ProcessData find_next_process() {
	// if next_process did not arrive so far, 
    // then we recursively call this function after incrementing total_time
	if (queue_size(&rr_queue) == 0){
        printf("Scheduler: Runtime: %u seconds.\nNo processes yet.\n", total_time);
        
        // increment the time
        total_time++;
        add_new_processes();
        return find_next_process(); 
	}

    int location = front(&rr_queue);
    pop(&rr_queue);

  // return the data of next process
	return data[location];
}


// reports the metrics and simulation results
void report(){
	printf("Simulation results.....\n");
	int sum_wt = 0;
	int sum_tat = 0;
	for (unsigned i = 0; i < data_size; i++){
		printf("process %d: \n", i);
		printf("	at=%d\n", data[i].at);
		printf("	bt=%d\n", data[i].bt);
		printf("	ct=%d\n", data[i].ct);
		printf("	wt=%d\n", data[i].wt);
		printf("	tat=%d\n", data[i].tat);
		printf("	rt=%d\n", data[i].rt);
		sum_wt += data[i].wt;
		sum_tat += data[i].tat;
	}

	printf("data size = %d\n", data_size);
	float avg_wt = (float)sum_wt/data_size;
	float avg_tat = (float)sum_tat/data_size;
	printf("Average results for this run:\n");
	printf("	avg_wt=%f\n", avg_wt);
	printf("	avg_tat=%f\n", avg_tat);
}

void check_burst(){
	for (unsigned i = 0; i < data_size; i++) {
		if (data[i].burst > 0) {
            return;
        }
    }

    // report simulation results
    report();

    // terminate the scheduler :)
	  exit(EXIT_SUCCESS);
}

// This function is called every one second as handler for SIGALRM signal
void schedule_handler(int) {
    // increment the total time
    total_time++;
    add_new_processes();

    if (running_process != -1) {
        --data[running_process].burst;
        --quant;
        printf("Scheduler: Runtime: %u seconds\n", total_time);
        printf("Process %d is running with %d seconds left\n", running_process, data[running_process].burst);
        printf("Process %d is running with %d seconds left in quant\n", running_process, quant);
        if (data[running_process].burst == 0) {
            printf("Scheduler: Terminating Process %d (Remaining Time: %d)\n",
                    running_process, data[running_process].burst);
            terminate(running_process);
            waitpid(ps[running_process], NULL, 0);
            ps[running_process] = 0;

            data[running_process].ct = total_time;
            data[running_process].tat = data[running_process].ct - data[running_process].at;
            data[running_process].wt = data[running_process].tat - data[running_process].bt;
            running_process = -1;
        } else if (quant == 0) {
            if (queue_size(&rr_queue) == 0) {
                printf("Scheduler: no processes require processor except %d, prolonging quant for it.\n", 
                        running_process);
                quant = QUANT;
                return;
            }
            push(&rr_queue, running_process);
            suspend(running_process);
            printf("Scheduler: Stopping Process %d (Remaining Time: %d)\n", 
                    running_process, data[running_process].burst);
            running_process = -1;
        } else {
            return;
        }
    }

    check_burst();
    int next = find_next_process().idx;
    if (ps[next] == 0) {
        create_process(next);
        printf("Scheduler: Starting Process %d (Remaining Time: %d)\n",
                running_process, data[running_process].burst);
    } else {
        quant = QUANT;
        running_process = next;
        resume(running_process);
        printf("Scheduler: Resuming Process %d (Remaining Time: %d)\n", 
                running_process, data[running_process].idx);
    }
}


int main(int, char *argv[]) {
    // read the data file
    FILE *in_file  = fopen(argv[1], "r");
  	if (in_file == NULL) {   
		printf("File is not found or cannot open it!\n"); 
  		exit(EXIT_FAILURE);
    } else {
        read_file(in_file); 
    }

    // setup a queue
    rr_queue.size = 0;

    // set a timer
    struct itimerval timer;

    // the timer goes off 1 second after reset
    timer.it_value.tv_sec = 1;
    timer.it_value.tv_usec = 0;

    // timer increments 1 second at a time
    timer.it_interval.tv_sec = 1;
    timer.it_interval.tv_usec = 0;

    // this counts down and sends SIGALRM to the scheduler process after expiration.
    setitimer(ITIMER_REAL, &timer, NULL);

    // register the handler for SIGALRM signal
    signal(SIGALRM, schedule_handler);

    // Wait till all processes finish
    while(1); // infinite loop
}
