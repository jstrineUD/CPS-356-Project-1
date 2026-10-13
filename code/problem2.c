#include <stdbool.h>
#include <stdio.h>

#define QUANTUM 5      // time quantum = 5 for every priority
#define BOOST   50     // boost time = 50, after this period, move all processor to highest priority

typedef struct {
    int pID;
    int arrivalTime;
    int burstTime;
    int remainingTime;
    int priorityQueue; //Q0-10, Q0 is highest priority
    int orderInQueue; //order which process assigned to the priority queue
    bool isDone;
} Process;

void problemTwo() {
    int q, n;
    Process p[20];
    int timeline[5000];
    int qLevelTimeline[5000];
    int i, currentTime = 0, done = 0, run = -1, used = 0, counter = 0;

    do {
        printf("Enter number of queues: ");
        scanf("%d",&q);
        if (q < 2 || q > 10) {
            printf("Invalid number of queues. It must be from 2 - 10.\n");
        }
    }while ( q< 2 || q >10 );


    printf("Enter number of processes: ");
    scanf("%d",&n);

    for (i = 0; i < n; i++) {
        printf("Enter pID, arrival time, and burst time: ");
        scanf("%d %d %d", &p[i].pID, &p[i].arrivalTime, &p[i].burstTime);
        p[i].remainingTime = p[i].burstTime;
        p[i].priorityQueue = 0;          /* processes starts at the highest priority  queue */
        p[i].orderInQueue = 0;
        p[i].isDone = false;
    }

    //While loop ends when completes all jobs
    while (done < n) {

        //Check for new arrivals
        for ( i = 0; i < n; i++) {
            if (p[i].arrivalTime == currentTime && p[i].isDone == false) {
                p[i].orderInQueue = ++counter;
            }
        }

        //After 50s, boost all processes to highest priority
        if (currentTime>0 && currentTime % BOOST == 0) {
            for ( i = 0; i < n; i++) {
                p[i].priorityQueue = 0;
            }
        }

        //If cpu is used, check if any new arrival processes with higher priority
        if ( run != -1) {
            for ( i = 0; i < n; i++) {
                if (i != run && p[i].arrivalTime <= currentTime
                    && p[i].isDone == false && p[i].priorityQueue < p[run].priorityQueue) {

                    p[run].orderInQueue = ++counter;
                    run = -1;
                    break;
                    }
            }
        }

        //Situtation when cpu is free, decide which process takes it
        if (run == -1) {
            int highestPriority = -1;
            for (i = 0; i < n; i++) {
                if (p[i].arrivalTime <= currentTime && p[i].isDone == false) {
                    if (highestPriority == -1 || p[i].priorityQueue < p[highestPriority].priorityQueue
                        || ( p[i].priorityQueue == p[highestPriority].priorityQueue
                        && p[i].orderInQueue < p[highestPriority].orderInQueue )) {
                        highestPriority = i;
                        }
                }
            }
            run = highestPriority; //process with highest priority takes over CPU
            used = 0; //reset time quantum
        }

        //Time management
        if (run == -1) {
            timeline[currentTime] = 0;
            qLevelTimeline[currentTime] = -1;
        } else {
            timeline[currentTime] = p[run].pID;
            qLevelTimeline[currentTime] = p[run].priorityQueue;
            p[run].remainingTime--;
            used++;
        }
        currentTime++;

        //Check if processes finished
        if (run != -1) {
            if (p[run].remainingTime == 0) {
                done++;
                p[run].isDone = true;
                run = -1;
            } else if (used == QUANTUM) { //Check if processes used all time quantum, increase its priority queue
                if (p[run].priorityQueue < q - 1) {
                    p[run].priorityQueue++;
                }
                p[run].orderInQueue = ++counter;
                run = -1;
            }
        }

    }

    /* Gantt chart Format*/
    int s = 0;
    printf("Gantt chart:\n|");
    for (i = 1; i <= currentTime; i++)
        if (i == currentTime || timeline[i] != timeline[s]) {
            if (timeline[s]) {
                printf(" P%d |", timeline[s]);
            }else {
                printf(" idle |");
            }
            s = i;
        }
    printf("\n0");
    s = 0;
    for (i = 1; i <= currentTime; i++)
        if (i == currentTime || timeline[i] != timeline[s]) {
            int w;
            if (timeline[s] == 0)       w = 7;   // " idle |"
            else if (timeline[s] >= 10) w = 6;   // " P10 |"
            else                        w = 5;   // " P1 |"
            printf("%*d", w, i);
            s = i;
        }
    printf("\n");

    s = 0;
    for (i = 1; i <= currentTime; i++)
        if (i == currentTime || timeline[i] != timeline[s]) {
            printf(" Q%d |", qLevelTimeline[s]);
            s = i;
        }
}
