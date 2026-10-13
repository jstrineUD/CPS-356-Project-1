//
// Created by strinejc on 10/6/2026.
//

#include <stdio.h>
#include "problem1.h"

#include <stdlib.h>

#define MIN_PROCESSES 1
#define MAX_PROCESSES 20


//The main function of problem 1 prompts the user for N processes and the user provides the run times for those processes. Processes are assigned an arrival time and pid based on
//when they were added by the user. Each process is entered into a linked list to serve as a source of truth for the algorithms to reference when they are biulding their queues.

void problemOne() {
    int processCount;

    struct Node *head = NULL;
    struct Node *tail = NULL;

    printf("Enter the number of processes, between %d and %d: ", MIN_PROCESSES, MAX_PROCESSES);
    scanf(" %d", &processCount);
    while (processCount < MIN_PROCESSES || processCount > MAX_PROCESSES) {
        printf("Invalid process count\n");
        scanf("%d", &processCount);
    }

    for (int i = 0; i < processCount; i++) {
        struct Process *newProcess = malloc(sizeof(struct Process));
        struct Node *newNode = malloc(sizeof(struct Node));

        newProcess->pid = i + 1;
        newProcess->arrival_time = i;
        newProcess->response_time = 0;
        newProcess->turnaround_time = 0;
        newProcess->isQueued = 0;

        printf("Enter the run time for process %d: ", i + 1);
        scanf("%d", &newProcess->run_time);
        newProcess->remaining_time = newProcess->run_time;


        newNode->next = NULL;
        newNode->process = newProcess;

        if (head == NULL) {
            head = newNode;
            tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
    }

    //calls to all the scheduler methods, these methods then output their data into the console for analysis
    firstComeFirstServed(head, processCount);
    shortestJobFirst(head, processCount);
    randomSchedule(head, processCount);
    shortestTimeToCompletionFirst(head, processCount);


    struct Node *currNode = head;

    while (currNode != NULL) {
        currNode->process->remaining_time = currNode->process->run_time;
        currNode->process->turnaround_time = 0;
        currNode->process->response_time = 0;
        currNode->process->isQueued = 0;
        currNode = currNode->next;
    }
    roundRobin(head, processCount);

    //frees our data now that we've completed our analysis
    while (head != NULL) {
        struct Node *next = head->next;
        free(head->process);
        free(head);
        head = next;
    }
}

void firstComeFirstServed(struct Node *head, int count) {
    int queueTracker[MAX_PROCESSES] = {0};

    struct Queue queue = {
        .size = 0,
        .front = NULL,
        .rear = NULL
    };

    struct Algorithm algo = {
        .queue = &queue,
        .timeElapsed = 0,
        .totalResponseTime = 0,
        .totalTurnaroundTime = 0,
        .name = "FCFS"
    };

    for (int i = 0; i < count; i++) {
        struct Node *currNode = head;
        struct Node *selected = NULL;

        int currentIndex = 0;
        int selectedIndex = -1;

        while (currNode != NULL) {
            if (queueTracker[currentIndex] == 0) {
                if (selected == NULL ||
                    currNode->process->arrival_time < selected->process->arrival_time) {
                    selected = currNode;
                    selectedIndex = currentIndex;
                }
            }
            currNode = currNode->next;
            currentIndex++;
        }

        if (selected == NULL) {
            break;
        }

        struct QueueNode *newNode = malloc(sizeof(struct QueueNode));
        newNode->process = *selected->process;
        newNode->next = NULL;

        if (algo.queue->size == 0) {
            algo.queue->front = newNode;
            algo.queue->rear = newNode;
        } else {
            algo.queue->rear->next = newNode;
            algo.queue->rear = newNode;
        }

        algo.queue->size++;

        queueTracker[selectedIndex] = 1;
    }
    printGanttChart(&algo);
    freeQueue(algo.queue);
}

void shortestJobFirst(struct Node *head, int count) {
    int queueTracker[MAX_PROCESSES] = {0};

    struct Queue queue = {
        .size = 0,
        .front = NULL,
        .rear = NULL
    };

    struct Algorithm algo = {
        .queue = &queue,
        .timeElapsed = 0,
        .totalResponseTime = 0,
        .totalTurnaroundTime = 0,
        .name = "SJf"
    };

    for (int i = 0; i < count; i++) {
        struct Node *currNode = head;
        struct Node *selected = NULL;

        int currentIndex = 0;
        int selectedIndex = -1;

        while (currNode != NULL) {
            if (queueTracker[currentIndex] == 0 && currNode->process->arrival_time <= algo.timeElapsed) {
                if (selected == NULL ||
                    currNode->process->run_time < selected->process->run_time) {
                    selected = currNode;
                    selectedIndex = currentIndex;
                }
            }
            currNode = currNode->next;
            currentIndex++;
        }

        if (selected == NULL) {
            break;
        }

        struct QueueNode *newNode = malloc(sizeof(struct QueueNode));
        newNode->process = *selected->process;
        newNode->next = NULL;

        if (algo.queue->size == 0) {
            algo.queue->front = newNode;
            algo.queue->rear = newNode;
        } else {
            algo.queue->rear->next = newNode;
            algo.queue->rear = newNode;
        }

        algo.queue->size++;
        queueTracker[selectedIndex] = 1;

        algo.timeElapsed += selected->process->run_time;
    }
    printGanttChart(&algo);
    freeQueue(algo.queue);
}

void shortestTimeToCompletionFirst(struct Node *head, int count) {
    struct Queue queue = {
        .size = 0,
        .front = NULL,
        .rear = NULL
    };

    struct Algorithm algo = {
        .queue = &queue,
        .timeElapsed = 0,
        .totalResponseTime = 0,
        .totalTurnaroundTime = 0,
        .name = "STTCF"
    };

    int finishedProcesses = 0;

    while (finishedProcesses < count) {
        struct Node *currNode = head;
        struct Node *selected = NULL;

        while (currNode != NULL) {
            if (currNode->process->arrival_time <= algo.timeElapsed
                && currNode->process->remaining_time > 0) {
                if (selected == NULL ||
                    currNode->process->remaining_time < selected->process->remaining_time) {
                    selected = currNode;
                }
            }
            currNode = currNode->next;
        }

        if (selected == NULL) {
            break;
        }

        if (algo.queue->rear != NULL && algo.queue->rear->process.pid == selected->process->pid) {
            algo.queue->rear->duration++;
        } else {
            struct QueueNode *newNode = malloc(sizeof(struct QueueNode));
            newNode->process = *selected->process;
            newNode->next = NULL;
            newNode->duration = 1;

            if (algo.queue->size == 0) {
                algo.queue->front = newNode;
                algo.queue->rear = newNode;
            } else {
                algo.queue->rear->next = newNode;
                algo.queue->rear = newNode;
            }
            algo.queue->size++;
        }
        selected->process->remaining_time--;
        algo.timeElapsed++;

        if (selected->process->remaining_time == 0) {
            finishedProcesses++;

            selected->process->turnaround_time = algo.timeElapsed - selected->process->arrival_time;
        }
    }

    printPreemptiveGanttChart(&algo, head, count);
    freeQueue(algo.queue);
}

void roundRobin(struct Node *head, int count) {
    struct Queue queue = {
        .size = 0,
        .front = NULL,
        .rear = NULL
    };

    struct Queue callStack = {
        .size = 0,
        .front = NULL,
        .rear = NULL
    };

    struct Algorithm algo = {
        .queue = &queue,
        .timeElapsed = 0,
        .totalResponseTime = 0,
        .totalTurnaroundTime = 0,
        .name = "RR"
    };

    int finishedProcesses = 0;

    while (finishedProcesses < count) {
        int timeSpan = 2;
        struct Node *currNode = head;

        while (currNode != NULL) {
            if (currNode->process->arrival_time <= algo.timeElapsed &&
                currNode->process->remaining_time > 0 &&
                currNode->process->isQueued == 0) {
                struct QueueNode *newNode = malloc(sizeof(struct QueueNode));

                newNode->process = *currNode->process;
                newNode->duration = 0;
                newNode->next = NULL;

                if (callStack.rear == NULL) {
                    callStack.front = newNode;
                    callStack.rear = newNode;
                } else {
                    callStack.rear->next = newNode;
                    callStack.rear = newNode;
                }

                callStack.size++;
                currNode->process->isQueued = 1;
            }
            currNode = currNode->next;
        }

        struct QueueNode *newNode = callStack.front;
        callStack.front = newNode->next;

        if (callStack.front == NULL) {
            callStack.rear = NULL;
        }

        callStack.size--;
        newNode->next = NULL;

        if (newNode->process.remaining_time < timeSpan) {
            timeSpan = newNode->process.remaining_time;
        }

        newNode->process.remaining_time -= timeSpan;
        algo.timeElapsed += timeSpan;

        struct QueueNode *callNode = malloc(sizeof(struct QueueNode));
        callNode->process = newNode->process;
        callNode->duration = timeSpan;
        callNode->next = NULL;

        if (algo.queue->rear == NULL) {
            algo.queue->front = callNode;
            algo.queue->rear = callNode;
        }else {
            algo.queue->rear->next = callNode;
            algo.queue->rear = callNode;
        }

        algo.queue->size++;

        currNode = head;
        while (currNode != NULL) {
            if (currNode->process->arrival_time <= algo.timeElapsed && currNode->process->remaining_time > 0 && currNode->process->isQueued == 0) {
                struct QueueNode *arrivingNode = malloc(sizeof(struct QueueNode));
                arrivingNode->process = *currNode->process;
                arrivingNode->duration = 0;
                arrivingNode->next = NULL;
                if (callStack.rear == NULL) {
                    callStack.front = arrivingNode;
                    callStack.rear = arrivingNode;
                }else {
                    callStack.rear->next = arrivingNode;
                    callStack.rear = arrivingNode;
                }

                callStack.size++;
                currNode->process->isQueued = 1;
            }
            currNode = currNode->next;
        }
        if (newNode->process.remaining_time == 0) {
            finishedProcesses++;

            currNode = head;

            while (currNode != NULL) {
                if (currNode->process->pid == newNode->process.pid) {
                    currNode->process->remaining_time = 0;
                    currNode->process->turnaround_time = algo.timeElapsed - currNode->process->arrival_time;

                    break;
                }
                currNode = currNode->next;
            }
            free(newNode);
        } else {
            newNode->next = NULL;
            if (callStack.rear == NULL) {
                callStack.front = newNode;
                callStack.rear = newNode;
            } else {
                callStack.rear->next = newNode;
                callStack.rear = newNode;
            }
            callStack.size++;
        }
    }

    printPreemptiveGanttChart(&algo, head, count);
    freeQueue(&callStack);
    freeQueue(algo.queue);
}

void randomSchedule(struct Node* head, int count) {
    int randomQueue[count];
    for (int i = 0; i < count; i++) {
        randomQueue[i] = i + 1;
    }

    for (int i = count -1; i > 0; i--) {
        int indexToShuffle = rand() % count;

        int j = randomQueue[i];
        randomQueue[i] = randomQueue[indexToShuffle];
        randomQueue[indexToShuffle] = j;
    }

    struct Queue queue = {
        .size = 0,
        .front = NULL,
        .rear = NULL
    };

    struct Algorithm algo = {
        .queue = &queue,
        .timeElapsed = 0,
        .totalResponseTime = 0,
        .totalTurnaroundTime = 0,
        .name = "Random"
    };

    for (int i = 0; i < count; i++) {
        struct Node *currNode = head;
        struct Node *selected = NULL;

        while (currNode != NULL) {
            if (currNode->process->arrival_time <= algo.timeElapsed && currNode->process->pid ==  randomQueue[i]) {
                    selected = currNode;
                break;
            }
            currNode = currNode->next;
        }
        if (selected == NULL) {
            break;
        }

        struct QueueNode *newNode = malloc(sizeof(struct QueueNode));
        newNode->process = *selected->process;
        newNode->next = NULL;

        if (algo.queue->size == 0) {
            algo.queue->front = newNode;
            algo.queue->rear = newNode;
        } else {
            algo.queue->rear->next = newNode;
            algo.queue->rear = newNode;
        }
        algo.queue->size++;
        algo.timeElapsed += selected->process->run_time;
    }
    printGanttChart(&algo);
    freeQueue(algo.queue);
}
