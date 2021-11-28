#ifndef __HEADERS_H
#define __HEADERS_H

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>

/*************COLORS***************/
#define RED "\033[1;91m"
#define GREEN "\033[1;92m"
#define YELLOW "\033[1;93m"
#define BLUE "\033[1;94m"
#define MAGENTA "\033[1;95m"
#define CYAN "\033[1;96m"
#define NORMAL "\033[0m"
/**********************************/

/***********************Global variables***********************/
sem_t neutral_sem;
sem_t home_sem;
sem_t away_sem;
int cap_h, cap_a, cap_n;
extern int spec_time;
extern int flags[1024];
int time_up[1024];
extern int home_team, away_team;
extern pthread_mutex_t seat_mutex[1024];
extern int reading;
extern pthread_cond_t signal;
extern pthread_cond_t seat_came[1024];
extern pthread_mutex_t friend_mutex;
int friends_groups[256];
/**************************************************************/

typedef struct spectator
{
    char name[16];
    char support;
    int time;
    int patience;
    int enrage;
    int ID;
    int Group_ID;
    pthread_mutex_t spec_mutex;
} Spectator;

typedef struct goal
{
    char team;
    int goal_time;
    float prob;
} Goal;

typedef struct thread_details
{
    int ID;
    int patience;
} td;

void *homeseat_wait(void *inp); //zone-1
void *neutralseat_wait(void *inp); //zone-2
void *awayseat_wait(void *inp); //zone-3

void add_seat(int flag);
void *sleeping_thread(void *inp);
void *goal_thread(void *inp);

void *home_spec(void *inp);
void *neutral_spec(void *inp);
void *away_spec(void *inp);

#endif