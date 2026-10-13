# CPS-356-Project-1
Project Repository for Project 1
CPS 356 – Operating Systems
Fall 2026

## Instructions for Problem 1
Open the code folder inside CLion, and build the project. Running the program will begin problem 1 and prompt the user to input a count of processes and then input runtimes for that many processes. The program will then perform the analysis via the 4 algorithms implemented and display the outcome in the console.

Problem 3: Multiprocessor Scheduling with Work Stealing

**Project Description**

This program simulates multiprocessor CPU scheduling using work stealing. Each CPU has its own ready queue. Processes are initially assigned using round-robin assignment and executed using non-preemptive FCFS scheduling.

When a CPU becomes idle, it attempts to steal a waiting process from another CPU.

**Files**
- main.c – C source code
- CMakeLists.txt – Build configuration

**How to Compile**

gcc -std=c11 main.c -o problem3

**How to Run the code**

Ubuntu/Linux: ./problem3

Or open the project in CLion and click Run.

**Input Example**

3
5
1 0 8
2 0 4
3 0 9
4 1 5
5 2 3

**Output Specification**

The program displays a Gantt charts for each CPU, process execution times, waiting times, turnaround times, average waiting time and turnaround time and successful/unsuccessful steal attempts.
