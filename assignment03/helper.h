#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef STRUCTS_H
#define STRUCTS_H

#define RR 1
#define SJF 2
#define PR_NO_PREMP 3
#define PR_PREMP 4


typedef struct Process {
    int pid;
    int arrival_time;
    int burst_time;
    int priority;
    int wait_time;
    int start_time;
    int has_cpu;
} Process;

typedef struct Header {
    int algorithm;
    int time_quantum;
    int num_processes;
    int reserved;
} Header;

/* Forward Declarations */
Process* load_file(char* filename);
int append_to_outbuf(Process *process, char *buffer, int offset);
void write_file(char* filename, char* outbuf);


/**
 * Loads the input file and returns a pointer to the process array
 * 
 * @param filename The name of the input file
 * @return A pointer to the process array
**/
Process* load_file(char* filename) {
    Header header;
    Process* process_array;
    FILE* fp;
    size_t len = 0;
    ssize_t read;
    char* line = NULL;
    char* token;
    int line_number = 1;
    int algorithm, time_quantum, num_processes;

    fp = fopen(filename, "r");
    
    if (fp == NULL) {
        printf("Error opening file");
        exit(EXIT_FAILURE);
    }

    while ((read = getline(&line, &len, fp)) != -1) {
        
        if (line_number > 2) {
            int index = line_number - 2;
            Process process;
            token = strtok(line, " ");
            process.pid = atoi(token);
            token = strtok(NULL, " ");
            process.arrival_time = atoi(token);
            token = strtok(NULL, " ");
            process.burst_time = atoi(token);
            token = strtok(NULL, " ");
            process.priority = atoi(token);
            process.start_time = -1;
            process.has_cpu = 0;
            process.wait_time = 0;
            process_array[index] = process;
        }

        if (line_number == 1) {
            token = strtok(line, " ");
            
            if (strcmp(token, "RR") == 0) {
                algorithm = RR;
            } else if (strcmp(token, "SJF\n") == 0) {
                algorithm = SJF;
            } else if (strcmp(token, "PR_noPREMP\n") == 0) {
                algorithm = PR_NO_PREMP;
            } else if (strcmp(token, "PR_withPREMP\n") == 0) {
                algorithm = PR_PREMP;
            } else {
                printf("Invalid algorithm");
                exit(EXIT_FAILURE);
            }

            if (algorithm == RR) {
                token = strtok(NULL, " ");
                time_quantum = atoi(token);
            }
        }

        if (line_number == 2) {
            token = strtok(line, " ");
            num_processes = atoi(token);
            process_array = malloc((num_processes + 1) * sizeof(Process)); // +1 for 16 bytes of header at process_array[0]
        }
        
        line_number++;
    }

    fclose(fp);
    
    if (line) {
        free(line);
    }

    header.algorithm = algorithm;
    header.time_quantum = time_quantum;
    header.num_processes = num_processes;
    header.reserved = 0;

    memcpy(&process_array[0], &header, sizeof(Header));

    return process_array;
}

/**
 * Appends the process information to the output outbuf
 * 
 * @param process Pointer to a process to append to the output outbuf
 * @param outbuf The outbuf to append to
 * @param offset The offset to append at
 * @return The new offset
*/
int append_to_outbuf(Process *process, char *outbuf, int offset) {
    offset += sprintf(outbuf + offset, "%d\t\t%d\n", process->start_time, process->pid);
    return offset;
}

/**
 * Writes the output buffer to the output file
 * 
 * @param filename The name of the output file
 * @param outbuf The output buffer
*/
void write_file(char* filename, char* outbuf) {
    FILE *fp;
    fp = fopen(filename, "w");
    // write to file
    fprintf(fp, "%s", outbuf);
    fclose(fp);
}

#endif