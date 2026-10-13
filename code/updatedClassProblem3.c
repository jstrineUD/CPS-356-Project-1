#include <stdio.h>
#include <stdlib.h>


#define MAX_CPUS 8
#define MAX_PROCESSES 30
#define MAX_TIME 1700

struct Process {
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

void enqueue(struct CPU *cpu, int index) {
    cpu->readyQueue[cpu->queueSize++] = index;
}

int dequeue(struct CPU *cpu) {
    if (cpu->queueSize == 0)
        return -1;

    int index = cpu->readyQueue[0];

    for (int i = 0; i < cpu->queueSize - 1; i++)
        cpu->readyQueue[i] = cpu->readyQueue[i + 1];

    cpu->queueSize--;
    return index;
}

int main(void) {
    srandom(1234);

    int cpuCount, processCount;
    struct Process processes[MAX_PROCESSES];
    struct CPU cpus[MAX_CPUS];

    // Timeline stores the process running on each CPU
    int timeline[MAX_CPUS][MAX_TIME];

    int successfulSteals = 0;
    int unsuccessfulSteals = 0;
    int completed = 0;
    int time = 0;

    printf("Enter number of CPUs: ");
    if (scanf("%d", &cpuCount) != 1 ||
        cpuCount < 2 || cpuCount > 8) {
        printf("Invalid CPU count.\n");
        return 1;
    }

    printf("Enter number of processes: ");
    if (scanf("%d", &processCount) != 1 ||
        processCount < 1 || processCount > 30) {
        printf("Invalid process count.\n");
        return 1;
    }

    for (int c = 0; c < cpuCount; c++) {
        cpus[c].queueSize = 0;
        cpus[c].runningProcess = -1;
    }

    // Read process information
    for (int i = 0; i < processCount; i++) {
        printf("Enter ID, arrival time, run time: ");

        if (scanf("%d %d %d",
                  &processes[i].id,
                  &processes[i].arrivalTime,
                  &processes[i].runTime) != 3) {
            printf("Invalid input.\n");
            return 1;
        }

        if (processes[i].id < 1 ||
            processes[i].arrivalTime < 0 ||
            processes[i].arrivalTime > 100 ||
            processes[i].runTime < 1 ||
            processes[i].runTime > 50) {
            printf("Invalid process data.\n");
            return 1;
        }

        for (int j = 0; j < i; j++) {
            if (processes[j].id == processes[i].id) {
                printf("Duplicate process ID.\n");
                return 1;
            }
        }

        processes[i].assignedCPU = i % cpuCount;
        processes[i].remainingTime = processes[i].runTime;
        processes[i].startTime = -1;
        processes[i].completionTime = -1;
    }

    printf("\n--- Scheduling Simulation ---\n");

    while (completed < processCount && time < MAX_TIME) {

        // Step 1: Add newly arrived processes
        for (int i = 0; i < processCount; i++) {
            if (processes[i].arrivalTime == time) {
                int cpu = processes[i].assignedCPU;

                enqueue(&cpus[cpu], i);

                printf("Time %d: P%d arrived at CPU %d\n",
                       time, processes[i].id, cpu + 1);
            }
        }

        // Step 2: Idle CPUs take local ready processes
        for (int c = 0; c < cpuCount; c++) {
            if (cpus[c].runningProcess == -1 &&
                cpus[c].queueSize > 0) {

                cpus[c].runningProcess = dequeue(&cpus[c]);
            }
        }




        // Step 3: Randomized work stealing
        for (int c = 0; c < cpuCount; c++) {

            // Only idle CPUs can steal
            if (cpus[c].runningProcess != -1) {
                continue;
            }

            // Randomly choose another CPU
            int victim = random() % cpuCount;

            // Do not select the same CPU
            while (victim == c) {
                victim = random() % cpuCount;
            }

            // Check if the selected CPU has waiting processes
            if (cpus[victim].queueSize > 0) {

                int stolen = dequeue(&cpus[victim]);
                cpus[c].runningProcess = stolen;

                successfulSteals++;

                printf("Time %d: CPU %d stole P%d from CPU %d\n",
                       time, c + 1,
                       processes[stolen].id, victim + 1);

            } else {

                unsuccessfulSteals++;

                printf("Time %d: CPU %d failed to steal from CPU %d\n",
                       time, c + 1, victim + 1);
            }
        }
        // Step 4: Execute one time unit on each CPU
        for (int c = 0; c < cpuCount; c++) {
            int p = cpus[c].runningProcess;

            timeline[c][time] = p;

            if (p == -1)
                continue;

            if (processes[p].startTime == -1)
                processes[p].startTime = time;

            processes[p].remainingTime--;

            if (processes[p].remainingTime == 0) {
                processes[p].completionTime = time + 1;
                completed++;

                printf("Time %d: P%d completed on CPU %d\n",
                       time + 1, processes[p].id, c + 1);

                cpus[c].runningProcess = -1;
            }
        }

        time++;
    }

    if (completed != processCount) {
        printf("Simulation did not finish.\n");
        return 1;
    }

    // Print Gantt charts
    printf("\n--- Gantt Charts ---\n");

    for (int c = 0; c < cpuCount; c++) {
        printf("\nCPU %d: ", c + 1);

        int start = 0;

        while (start < time) {
            int p = timeline[c][start];
            int end = start + 1;

            while (end < time && timeline[c][end] == p)
                end++;

            if (p == -1)
                printf("[%d-%d: Idle] ", start, end);
            else
                printf("[%d-%d: P%d] ",
                       start, end, processes[p].id);

            start = end;
        }

        printf("\n");
    }

    // Print process statistics
    printf("\n--- Process Statistics ---\n");
    printf("PID  Arrival  Run  Start  Completion  Waiting  Turnaround\n");

    double totalWaiting = 0;
    double totalTurnaround = 0;

    for (int i = 0; i < processCount; i++) {
        int turnaround =
            processes[i].completionTime - processes[i].arrivalTime;

        int waiting = turnaround - processes[i].runTime;

        totalWaiting += waiting;
        totalTurnaround += turnaround;

        printf("P%-3d %-8d %-4d %-6d %-11d %-8d %d\n",
               processes[i].id,
               processes[i].arrivalTime,
               processes[i].runTime,
               processes[i].startTime,
               processes[i].completionTime,
               waiting,
               turnaround);
    }

    printf("\nAverage waiting time: %.2f\n",
           totalWaiting / processCount);

    printf("Average turnaround time: %.2f\n",
           totalTurnaround / processCount);

    printf("Successful steals: %d\n", successfulSteals);
    printf("Unsuccessful steal attempts: %d\n",
           unsuccessfulSteals);

    return 0;
}
