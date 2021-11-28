#include "headers.h"

/*********************ZONE_WAITING**************************/
/************************************************************/
void *homeseat_wait(void *inp) //zone-1
{
    int ID = ((struct thread_details *)inp)->ID;
    int patience = ((struct thread_details *)inp)->patience;
    struct timespec ts;

    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += patience;
    //it will search for seats till only certain time
    int ans = sem_timedwait(&home_sem, &ts);
    if (ans != 0 && errno == ETIMEDOUT)
    {
        pthread_cond_signal(&seat_came[ID]);
        return NULL;
    }

    if (flags[ID] == 0)
    {
        pthread_mutex_lock(&seat_mutex[ID]);
        flags[ID] = 1;
        pthread_mutex_unlock(&seat_mutex[ID]);
    }
    else
    {
        sem_post(&home_sem);
    }

    pthread_cond_signal(&seat_came[ID]);
    return NULL;
}

void *neutralseat_wait(void *inp) //zone-2
{
    int ID = ((struct thread_details *)inp)->ID;
    int patience = ((struct thread_details *)inp)->patience;
    struct timespec ts;

    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += patience;
    //it will search for seats till only certain time
    int ans = sem_timedwait(&neutral_sem, &ts);
    if (ans != 0 && errno == ETIMEDOUT)
    {
        pthread_cond_signal(&seat_came[ID]);
        return NULL;
    }

    if (flags[ID] == 0)
    {
        pthread_mutex_lock(&seat_mutex[ID]);
        flags[ID] = 2;
        pthread_mutex_unlock(&seat_mutex[ID]);
    }
    else
    {
        sem_post(&neutral_sem);
    }

    pthread_cond_signal(&seat_came[ID]);
    return NULL;
}

void *awayseat_wait(void *inp) //zone-3
{
    int ID = ((struct thread_details *)inp)->ID;
    int patience = ((struct thread_details *)inp)->patience;
    struct timespec ts;

    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += patience;
    //it will search for seats till only certain time
    int ans = sem_timedwait(&away_sem, &ts);
    if (ans != 0 && errno == ETIMEDOUT)
    {
        pthread_cond_signal(&seat_came[ID]);
        return NULL;
    }
    if (flags[ID] == 0)
    {
        pthread_mutex_lock(&seat_mutex[ID]);
        flags[ID] = 3;
        pthread_mutex_unlock(&seat_mutex[ID]);
    }
    else
    {
        sem_post(&away_sem);
    }

    pthread_cond_signal(&seat_came[ID]);
    return NULL;
}

/***********************************************************************/
/***********************************************************************/

