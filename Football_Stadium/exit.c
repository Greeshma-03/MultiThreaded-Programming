#include "headers.h"

/*********************************************************/
void add_seat(int flag)
{
    if (flag == 1)
        sem_post(&home_sem);
    else if (flag == 2)
        sem_post(&neutral_sem);
    else if (flag == 3)
        sem_post(&away_sem);
    else
        printf(RED "Bruhhhhhh!!!\n" NORMAL);
}

/*******************SLEEPING THREAD************************/
void *sleeping_thread(void *inp)
{
    int ID = ((struct thread_details *)inp)->ID;
    sleep(spec_time);
    //changing the flag
    time_up[ID] = 1;
    pthread_cond_signal(&signal);
    return NULL;
}

void *goal_thread(void *inp)
{
    char team = ((struct goal *)inp)->team;
    int time = ((struct goal *)inp)->goal_time;
    float prob = ((struct goal *)inp)->prob;

    sleep(time);
    float prob_occured = rand() % 100;
    prob_occured = prob_occured / 100.0;

    //check
    if (prob_occured <= prob)
    {
        pthread_cond_signal(&signal);
        if (team == 'H')
        {
            home_team++;
            printf(MAGENTA "Team H have scored their %d goal\n" NORMAL, home_team);
        }
        else
        {
            away_team++;
            printf(MAGENTA "Team A have scored their %d goal\n" NORMAL, away_team);
        }
    }
    else
    {
        if (team == 'H')
        {
            printf(MAGENTA "Team H missed the chance to score their %d goal\n" NORMAL, home_team + 1);
        }
        else
        {
            printf(MAGENTA "Team A missed the chance to score their %d goal\n" NORMAL, away_team + 1);
        }
    }

    return NULL;
}

/**********************************************************/
