//
// Created by strinejc on 10/6/2026.
//

#ifndef CODE_PROBLEM1_H
#define CODE_PROBLEM1_H
#include "helpers.h"

void problemOne();

void firstComeFirstServed(struct Node *head, int count);
void shortestJobFirst(struct Node *head, int count);
void shortestTimeToCompletionFirst(struct Node *head, int count);
void roundRobin(struct Node *head, int count);
void randomSchedule(struct Node *head, int count);

#endif //CODE_PROBLEM1_H
