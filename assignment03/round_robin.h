/* Written by Justin Heyman */

#include <sys/queue.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include "helper.h"

struct entry {
    Process *process;
    STAILQ_ENTRY(entry) entries; /* Singly linked tail queue. */
};

STAILQ_HEAD(stailhead, entry); // Declare the head of the queue

void print_wq(struct stailhead *head);

void round_robin(Process* process_array, int num_processes, int init_tq, char* outbuf) {
    struct stailhead head; // Head of the waiting queue
    struct entry *wqp; // Pointer to a waiting process
    struct entry *rqp; // Pointer to the running process (queue entry)
    Process *running_process;
    int i;
    int offset = 0;
    int t = -1; // Time
    int tq = init_tq; // Time quantum
    int q_size = 0; // Queue size

    STAILQ_INIT(&head); /* Initialize the queue. */
    // printf("\n\n\nRR\t\t%d\n", init_tq);
    offset += sprintf(outbuf + offset, "RR\t%d\n", init_tq);

    while (1) {    
        t += 1;
        
        // Add new processes to waiting queue
        for (i = 1; i <= num_processes; i++) {
            if (process_array[i].arrival_time == t) {
                wqp = malloc(sizeof(struct entry));
                STAILQ_INSERT_TAIL(&head, wqp, entries);
                wqp->process = malloc(sizeof(Process*));
                wqp->process = &process_array[i];
                q_size += 1;
            }
        }
        

        // Initialize
        if (t == 0) {
            rqp = STAILQ_FIRST(&head);
            running_process = rqp->process;
            STAILQ_REMOVE_HEAD(&head, entries);
            // print_wq(&head);
            running_process->has_cpu = 1;
            running_process->start_time = t;
            q_size -= 1;
            // printf("%d\t%d\n", running_process->start_time, running_process->pid);
            offset = append_to_outbuf(running_process, outbuf, offset);
        }

        if (running_process->burst_time == 0) {
             if (STAILQ_EMPTY(&head)) {
                running_process->has_cpu = 0;
                printf("Last Process %d terminated\n", running_process->pid);
                break;
             }
            running_process->has_cpu = 0;
            printf("Process %d terminated\n", running_process->pid);
            
            running_process = STAILQ_FIRST(&head)->process;
            STAILQ_REMOVE_HEAD(&head, entries);
            running_process->has_cpu = 1;
            running_process->start_time = t;
            // printf("%d\t%d\n", running_process->start_time, running_process->pid);
            offset = append_to_outbuf(running_process, outbuf, offset);
            q_size -= 1;
            tq = init_tq; // Reset time quantum
        }

        if (tq == 0) {
            if (STAILQ_EMPTY(&head)) {
                if (running_process->burst_time == 0) {
                    running_process->has_cpu = 0;
                    STAILQ_REMOVE_HEAD(&head, entries);
                    q_size -= 1;
                    printf("Last Process %d terminated\n", running_process->pid);
                    break;
                } 
            } else if (running_process->burst_time == 0) {
                running_process->has_cpu = 0;
                STAILQ_REMOVE_HEAD(&head, entries);
                q_size -= 1;
                printf("Process %d terminated\n", running_process->pid);
            } else {
                running_process->has_cpu = 0;
                struct entry *tqp = malloc(sizeof(struct entry));
                tqp->process = malloc(sizeof(Process*));
                tqp->process = running_process;
                STAILQ_INSERT_TAIL(&head, tqp, entries);
                running_process = STAILQ_FIRST(&head)->process;
                STAILQ_REMOVE_HEAD(&head, entries);

                printf("Process %d preempted\n", running_process->pid);
            }
            tq = init_tq;
            running_process->has_cpu = 1;
            running_process->start_time = t;
            // printf("%d\t%d\n", running_process->start_time, running_process->pid);
            offset = append_to_outbuf(running_process, outbuf, offset);
        }

        STAILQ_FOREACH(wqp, &head, entries) {
            Process *tmp_process = wqp->process;
            tmp_process->wait_time += 1;
        }

        running_process->burst_time -= 1;
        tq -= 1;
    }

    // Print average wait time for all processes
    int total_wait_time = 0;
    for (i = 1; i <= num_processes; i++) {
        total_wait_time += process_array[i].wait_time;
    }
    // printf("Average wait time: %f\n", (float)total_wait_time / num_processes);
    offset += sprintf(outbuf + offset, "Average wait time: %f\n", (float)total_wait_time / num_processes);
    write_file("output.txt", outbuf);
}

// Print waiting queue
void print_wq(struct stailhead *head) {
    struct entry *wqp;
    printf("Processes in waiting queues: \n");
    STAILQ_FOREACH(wqp, head, entries) {
        printf("%d\n", wqp->process->pid);
    }
}


