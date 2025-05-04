// /*
//  * Copyright (C) 2025 pdnguyen of HCMC University of Technology VNU-HCM
//  */

// /* Sierra release
//  * Source Code License Grant: The authors hereby grant to Licensee
//  * personal permission to use and modify the Licensed Source Code
//  * for the sole purpose of studying while attending the course CO2018.
//  */

// #include "common.h"
// #include "syscall.h"
// #include "stdio.h"
// #include "libmem.h"
// #include "queue.h"

// int __sys_killall(struct pcb_t *caller, struct sc_regs* regs)
// {
//     char proc_name[100];
//     uint32_t data;

//     //hardcode for demo only
//     uint32_t memrg = regs->a1;

//     /* TODO: Get name of the target proc */
//     //proc_name = libread..
//     int i = 0;
//     data = 0;
//     while(data != -1){
//         libread(caller, memrg, i, &data);
//         proc_name[i]= data;
//         if(data == -1) proc_name[i]='\0';
//         i++;
//     }
//     printf("The procname retrieved from memregionid %d is \"%s\"\n", memrg, proc_name);

//     /* TODO: Traverse proclist to terminate the proc
//      *       stcmp to check the process match proc_name
//      */
//     //caller->running_list
//     //caller->mlq_ready_queu

//     for(int i = 0; i < MAX_PRIO; i++){
//         struct queue_t *q = &caller->mlq_ready_queue[i];
//         if(empty(q))
//             continue;
//         struct queue_t tmp_q;
//         tmp_q.size = 0;
//         while(!empty(q)){
//             struct pcb_t *proc = dequeue(q);
//             char* pname = strrchr(proc->path, '/');
//             if(pname != NULL)
//                 pname++;
//             else
//                 pname = proc->path;
//             if(strcmp(pname, proc_name) == 0){
//                 enqueue(&tmp_q, proc);
//             }
//             else{
//                 libfree(q, proc);
//             }
//             *q = tmp_q;
//     }

//     /* TODO Maching and terminating
//      *       all processes with given
//      *        name in var proc_name
//      */

//     return 0;
// }

// NEW FILE

/*
 * Copyright (C) 2025 pdnguyen of HCMC University of Technology VNU-HCM
 */

/* Sierra release
 * Source Code License Grant: The authors hereby grant to Licensee
 * personal permission to use and modify the Licensed Source Code
 * for the sole purpose of studying while attending the course CO2018.
 */

#include "common.h"
#include "syscall.h"
#include "stdio.h"
#include "libmem.h"
#include "queue.h"
#include <string.h>

int __sys_killall(struct pcb_t *caller, struct sc_regs *regs)
{
    char proc_name[100];
    uint32_t data;

    // hardcode for demo only
    uint32_t memrg = regs->a1;

    /* TODO: Get name of the target proc */
    // proc_name = libread..
    int i = 0;
    data = 0;
    while (data != -1)
    {
        libread(caller, memrg, i, &data);
        proc_name[i] = data;
        if (data == -1)
            proc_name[i] = '\0';
        i++;
    }
    printf("The procname retrieved from memregionid %d is \"%s\"\n", memrg, proc_name);

    /* Traverse running_list to terminate matching processes */
    struct pcb_t *proc;
    struct queue_t *running_list = caller->running_list;

    if (running_list != NULL)
    {
        for (int i = 0; i < running_list->size; i++)
        {
            proc = running_list->proc[i];
            if (strcmp(proc->path, proc_name) == 0)
            {
                printf("Terminating process %d with name %s from running list\n",
                       proc->pid, proc_name);
                // Remove from running list
                for (int j = i; j < running_list->size - 1; j++)
                {
                    running_list->proc[j] = running_list->proc[j + 1];
                }
                running_list->size--;
                i--; // Adjust index since we removed an element
            }
        }
    }

#ifdef MLQ_SCHED
    /* Traverse mlq_ready_queue to terminate matching processes */
    struct queue_t *mlq_ready_queue = caller->mlq_ready_queue;
    if (mlq_ready_queue != NULL)
    {
        for (int i = 0; i < MAX_PRIO; i++)
        {
            struct queue_t *queue = &mlq_ready_queue[i];
            for (int j = 0; j < queue->size; j++)
            {
                proc = queue->proc[j];
                if (strcmp(proc->path, proc_name) == 0)
                {
                    printf("Terminating process %d with name %s from ready queue\n",
                           proc->pid, proc_name);
                    // Remove from ready queue
                    for (int k = j; k < queue->size - 1; k++)
                    {
                        queue->proc[k] = queue->proc[k + 1];
                    }
                    queue->size--;
                    j--; // Adjust index since we removed an element
                }
            }
        }
    }
#endif

    return 0;
}