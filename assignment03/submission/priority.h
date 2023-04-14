/* Written by Justin Heyman */

#include <sys/queue.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include "helper.h"


struct ps_q_item {
    Process *process;
    TAILQ_ENTRY(ps_q_item) entries; /* Singly linked tail queue. */
};

TAILQ_HEAD(ps_q_head, ps_q_item); // Declare the head of the queue

void priority(Process* process_array, int num_processes, char* outbuf, int preemption) {
    struct ps_q_head head; // Head of the waiting queue
    struct ps_q_item *q_item, *q_tmp;
    struct ps_q_item *q_preempted = NULL;
    struct ps_q_item *rqp = NULL;
    Process *running_process;
    int i;
    int offset = 0;
    int t = -1; // Time
    int num_processed = 0;

    TAILQ_INIT(&head); /* Initialize the queue. */
    if (preemption) {
        offset += sprintf(outbuf + offset, "PR_withPREMP\n");
    } else {
        offset += sprintf(outbuf + offset, "PR_noPREMP\n");
    }

    while (num_processed != num_processes) {
        t += 1;

        // Queue processes that have arrived, ordered by priority
        // If a process is preempted, re-insert it into the correct queue position
        for (i = 1; i <= num_processes; i++) {
            if (process_array[i].arrival_time == t || q_preempted != NULL) {
                //TODO: If q_tmp is not null, re-insert it into the queue
                Process *process;
                if (q_preempted != NULL) {
                    process = q_preempted->process;
                    q_preempted = NULL;
                    i -= 1;
                } else {
                    process = &process_array[i];
                }
                if (TAILQ_EMPTY(&head)) {
                    q_tmp = malloc(sizeof(struct ps_q_item));
                    q_tmp->process = malloc(sizeof(Process*));
                    q_tmp->process = process;
                    TAILQ_INSERT_HEAD(&head, q_tmp, entries);
                    continue;
                }
                TAILQ_FOREACH(q_item, &head, entries) {
                    Process *q_process = q_item->process;
                    q_tmp = malloc(sizeof(struct ps_q_item));
                    q_tmp->process = malloc(sizeof(Process*));
                    q_tmp->process = process;
                    if (process->priority == q_process->priority) {
                        if (process->pid > q_process->pid) {
                            TAILQ_INSERT_AFTER(&head, q_item, q_tmp, entries);
                            break;
                        } else if (process->pid < q_process->pid) {
                            TAILQ_INSERT_BEFORE(q_item, q_tmp, entries);
                        } else {
                            printf("Error: Duplicate process ID");
                            exit(EXIT_FAILURE);
                        }
                    } else if (process->priority < q_process->priority) {
                        TAILQ_INSERT_BEFORE(q_item, q_tmp, entries);
                        break;
                    } else if (process->priority > q_process->priority) {
                        if (TAILQ_NEXT(q_item, entries) == NULL) {
                            TAILQ_INSERT_TAIL(&head, q_tmp, entries);
                            break;
                        }
                        continue;
                    }
                } // End of TAILQ_FOREACH
            }
        } // End of queueing processes

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

        if (preemption && !TAILQ_EMPTY(&head) && (running_process->priority > TAILQ_FIRST(&head)->process->priority)) {
            if (running_process->burst_time == 0) {
                running_process->has_cpu = 0;
                printf("Process %d terminated\n", running_process->pid);
                rqp = TAILQ_FIRST(&head);
                TAILQ_REMOVE(&head, rqp, entries);
                running_process = rqp->process;
                running_process->has_cpu = 1;
                running_process->start_time = t;
                offset = append_to_outbuf(running_process, outbuf, offset);
                num_processed += 1;
            } else {
                running_process->has_cpu = 0;
                printf("Process %d preempted\n", running_process->pid);
                
                // Store the preempted process in a tmp variable to be reinserted into the queue next iteration
                q_preempted = malloc(sizeof(struct ps_q_item));
                q_preempted->process = malloc(sizeof(Process*));
                q_preempted->process = running_process;

                rqp = TAILQ_FIRST(&head);
                TAILQ_REMOVE(&head, rqp, entries);
                running_process = rqp->process;
                running_process->has_cpu = 1;
                running_process->start_time = t;
                offset = append_to_outbuf(running_process, outbuf, offset);
            }
        } else if (running_process->burst_time == 0) {
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
        if (q_preempted != NULL) {
            q_preempted->process->wait_time += 1;
        }

        running_process->burst_time -= 1;


    } // End of while

    int total_wait_time = 0;
    for (i = 1; i <= num_processes; i++) {
        total_wait_time += process_array[i].wait_time;
    }
    // printf("Average wait time: %f\n", (float)total_wait_time / num_processes);
    offset += sprintf(outbuf + offset, "AVG Waiting Time: %.2f\n", (float)total_wait_time / num_processes);


}