#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "helper.h"
#include "round_robin.h"

#define RR 1
#define SJF 2
#define PR_NO_PREMP 3
#define PR_PREMP 4

// Process* load_file(char* filename);
char *outbuf;

int main() {
    outbuf = malloc(1024*1024);
    Header header;
    Process* process_array; 
    int i = 1;
    
    process_array = load_file("./test_cases/input15.txt"); //TODO: Change this to just "input.txt" before submitting
    memcpy(&header, &process_array[0], sizeof(Header));
    
    switch (header.algorithm) {
        case RR:
            printf("Algorithm: Round Robin\t\tTime Quantum: %d\t\tNumber of Processes: %d\n\n", header.time_quantum, header.num_processes);
            printf("PID\t\tArrival Time\t\tBurst Time\t\tPriority\n");
            for ( ; i <= header.num_processes; i++) {
                printf("%d\t\t%d\t\t\t%d\t\t\t%d\n", process_array[i].pid, process_array[i].arrival_time, process_array[i].burst_time, process_array[i].priority);
            }
            round_robin(process_array, header.num_processes, header.time_quantum, outbuf);
            break;
        case SJF:
            printf("Algorithm: SJF\t\tNumber of Processes: %d\n\n", header.num_processes);
            printf("PID\t\tArrival Time\t\tBurst Time\t\tPriority\n");
            for ( ; i <= header.num_processes; i++) {
                printf("%d\t\t%d\t\t\t%d\t\t\t%d\n", process_array[i].pid, process_array[i].arrival_time, process_array[i].burst_time, process_array[i].priority);
            }
            // Run algorithm
            break;
        case PR_NO_PREMP:
            printf("Algorithm: Priority with no Preemption\t\tNumber of Processes: %d\n\n", header.num_processes);
            printf("PID\t\tArrival Time\t\tBurst Time\t\tPriority\n");
            for ( ; i <= header.num_processes; i++) {
                printf("%d\t\t%d\t\t\t%d\t\t\t%d\n", process_array[i].pid, process_array[i].arrival_time, process_array[i].burst_time, process_array[i].priority);
            }
            // Run algorithm
            break;
        case PR_PREMP:
            printf("Algorithm: Priority with Preemption\t\tNumber of Processes: %d\n\n", header.num_processes);
            printf("PID\t\tArrival Time\t\tBurst Time\t\tPriority\n");
            for ( ; i <= header.num_processes; i++) {
                printf("%d\t\t%d\t\t\t%d\t\t\t%d\n", process_array[i].pid, process_array[i].arrival_time, process_array[i].burst_time, process_array[i].priority);
            }
            // Run algorithm
            break;
        default:
            free(outbuf);
            printf("Invalid algorithm\n");
            exit(EXIT_FAILURE);
    }
    free(outbuf);
    return 0;
}


