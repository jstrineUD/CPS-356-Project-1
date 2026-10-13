//
// Created by strinejc on 10/13/2026.
//

#ifndef CODE_PROBLEM3_H
#define CODE_PROBLEM3_H
#define MAX_CPUS 8
#define MAX_PROCESSES 30
#define MAX_TIME 1700

struct Processes {
    int id;
    int arrivalTime;
    int runTime;
    int assignedCPU;
    int startTime;
    int completionTime;
    int remainingTime;
};

struct CPU {
    int readyQueue[MAX_PROCESSES];
    int queueSize;
    int runningProcess;
};
void enqueue(struct CPU *cpu, int index);
int dequeue(struct CPU *cpu);
int problem3(void);
#endif //CODE_PROBLEM3_H
