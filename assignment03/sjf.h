/* Written by Justin Heyman */

#include <stdio.h>
#include <stdlib.h>
#include "helper.h"
#include <sys/queue.h>
// Process* get_shortest_process(Process *candidates[], int num_candidates);

struct sjf_q_item {
    Process *process;
    TAILQ_ENTRY(sjf_q_item) entries; /* Doubly linked tail queue. */
};

TAILQ_HEAD(sjf_q_head, sjf_q_item); // Declare the head of the queue

void sjf(Process* process_array, int num_processes, char* outbuf) {
    struct sjf_q_head head; // Head of the waiting queue
    struct sjf_q_item *rqp = NULL; // Pointer to the running process (queue sjf_q_item)
    struct sjf_q_item *q_item; // Temporary pointer to a queue item
    struct sjf_q_item *q_tmp; // Used for creating temporary queue items to be inserted into the queue
    Process *candidates[num_processes];
    Process *running_process;
    int i;
    int offset = 0;
    int t = -1; // Time
    int num_processed = 0;

    TAILQ_INIT(&head); /* Initialize the queue. */
    offset += sprintf(outbuf + offset, "SJF\n");
    
    while (!(num_processed == num_processes)) {
        t += 1;

        for (i = 1; i <= num_processes; i++) {
            if (process_array[i].arrival_time == t) {
                Process *process = &process_array[i];
                if (TAILQ_EMPTY(&head)) {
                    q_tmp = malloc(sizeof(struct sjf_q_item));
                    q_tmp->process = malloc(sizeof(Process*));
                    q_tmp->process = process;
                    TAILQ_INSERT_HEAD(&head, q_tmp, entries);
                    continue;
                }
                TAILQ_FOREACH(q_item, &head, entries) {
                    Process *q_process = q_item->process;
                    q_tmp = malloc(sizeof(struct sjf_q_item));
                    q_tmp->process = malloc(sizeof(Process*));
                    q_tmp->process = process;
                    if (process->burst_time == q_process->burst_time) {
                        if (process->arrival_time == q_process->arrival_time) {
                            if (process->pid == q_process->pid) {
                                break;
                            } else if (process->pid > q_process->pid) {
                                TAILQ_INSERT_AFTER(&head, q_item, q_tmp, entries);
                                break;
                            } else if (process->pid < q_process->pid) {
                                TAILQ_INSERT_BEFORE(q_item, q_tmp, entries);
                            } else {
                                printf("Error: Something went wrong during ordered queue insertion");
                                exit(EXIT_FAILURE);
                            }
                        } else if (process->arrival_time < q_process->arrival_time) {
                            TAILQ_INSERT_BEFORE(q_item, q_tmp, entries);
                            break;
                        } else if (process->arrival_time > q_process->arrival_time) {
                            if (TAILQ_NEXT(q_item, entries) == NULL) {
                                TAILQ_INSERT_TAIL(&head, q_tmp, entries);
                                break;
                            }
                            if (process->arrival_time < TAILQ_NEXT(q_item, entries)->process->arrival_time) {
                                TAILQ_INSERT_AFTER(&head, q_item, q_tmp, entries);
                                break;
                            }
                            continue;
                        } else {
                            printf("Error: Something went wrong during ordered queue insertion");
                            exit(EXIT_FAILURE);
                        }
                    } else if (process->burst_time < q_process->burst_time) {
                        TAILQ_INSERT_BEFORE(q_item, q_tmp, entries);
                        break;
                    } else if (process->burst_time > q_process->burst_time) {
                        if (TAILQ_NEXT(q_item, entries) == NULL) {
                            TAILQ_INSERT_TAIL(&head, q_tmp, entries);
                            break;
                        }
                        continue;
                    } else {
                        printf("Error: Something went wrong during ordered queue insertion");
                        exit(EXIT_FAILURE);
                    }
                } // End TAILQ_FOREACH
            } // End if
        } // End for loop
        

        // If the queue is empty and there are no running processes, continue
        if (TAILQ_EMPTY(&head) && rqp == NULL) {
            continue;
        }

        if (rqp == NULL) {
            rqp = TAILQ_FIRST(&head);
            TAILQ_REMOVE(&head, rqp, entries);
            running_process = rqp->process;
            running_process->has_cpu = 1;
            running_process->start_time = t;
            offset = append_to_outbuf(running_process, outbuf, offset);
            num_processed += 1;
        }


        if (running_process->burst_time == 0) {
            if (TAILQ_EMPTY(&head)) {
                running_process->has_cpu = 0;
                rqp == NULL;
                printf("Process %d terminated\n", running_process->pid);
                continue;
            }
            running_process->has_cpu = 0;
            printf("Process %d terminated\n", running_process->pid);
            rqp = TAILQ_FIRST(&head);
            TAILQ_REMOVE(&head, rqp, entries);
            running_process = rqp->process;
            running_process->has_cpu = 1;
            running_process->start_time = t;
            offset = append_to_outbuf(running_process, outbuf, offset);
            num_processed += 1;
        }
        
        q_item = TAILQ_FIRST(&head);
        TAILQ_FOREACH(q_item, &head, entries) {
            Process *process = q_item->process;
            process->wait_time += 1;
        }

        running_process->burst_time -= 1;

    } // End while loop
    
    int total_wait_time = 0;
    for (i = 1; i <= num_processes; i++) {
        total_wait_time += process_array[i].wait_time;
    }
    // printf("Average wait time: %f\n", (float)total_wait_time / num_processes);
    offset += sprintf(outbuf + offset, "AVG Waiting Time: %.2f\n", (float)total_wait_time / num_processes);
}