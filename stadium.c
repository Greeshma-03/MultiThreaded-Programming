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
int spec_time;
int flags[1024];
int time_up[1024];
int home_team = 0, away_team = 0;
pthread_mutex_t seat_mutex[1024];
int reading = 0;
pthread_cond_t signal = PTHREAD_COND_INITIALIZER;
pthread_cond_t seat_came[1024] = {PTHREAD_COND_INITIALIZER};
/**************************************************************/

typedef struct spectator
{
    char name[16];
    char support;
    int time;
    int patience;
    int enrage;
    int ID;
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

/*******************SLEEPING THREAD************************/
void *sleeping_thread(void *inp)
{
    int ID = ((struct thread_details *)inp)->ID;
    sleep(spec_time);
    //changing the flag
    time_up[ID] = 1;
    return NULL;
}
/**********************************************************/

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

    pthread_mutex_lock(&seat_mutex[ID]);
    if (flags[ID] == 0)
    {
        flags[ID] = 1;
        pthread_cond_signal(&seat_came[ID]);
    }
    else
    {
        sem_post(&home_sem);
    }
    pthread_mutex_unlock(&seat_mutex[ID]);

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

    pthread_mutex_lock(&seat_mutex[ID]);
    if (flags[ID] == 0)
    {
        flags[ID] = 2;
        pthread_cond_signal(&seat_came[ID]);
    }
    else
    {
        sem_post(&neutral_sem);
    }
    pthread_mutex_unlock(&seat_mutex[ID]);

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
    pthread_mutex_lock(&seat_mutex[ID]);
    if (flags[ID] == 0)
    {
        flags[ID] = 3;
        pthread_cond_signal(&seat_came[ID]);
    }
    else
    {
        sem_post(&away_sem);
    }
    pthread_mutex_unlock(&seat_mutex[ID]);
    return NULL;
}

/***********************************************************************/
/***********************************************************************/

/*************************ZONE_SPECTATORS*******************************/
/***********************************************************************/
void *home_spec(void *inp)
{
    while (reading)
        ;
    char *name = ((struct spectator *)inp)->name;
    int time = ((struct spectator *)inp)->time;
    int patience = ((struct spectator *)inp)->patience;
    int enrage = ((struct spectator *)inp)->enrage;
    int ID = ((struct spectator *)inp)->ID;

    sleep(time);
    printf(BLUE "%s has reached the stadium\n" NORMAL, name);

    pthread_t p1, p2;

    td *thread_input = (td *)(malloc(sizeof(td)));
    thread_input->ID = ID;
    thread_input->patience = patience;

    pthread_create(&p1, NULL, homeseat_wait, (void *)(thread_input));
    pthread_create(&p2, NULL, neutralseat_wait, (void *)(thread_input));

    //if a seat is found by any of the threads in home or neutral zones we shall go
    pthread_cond_wait(&seat_came[ID], &seat_mutex[ID]);

    if (flags[ID] == 0)
    {
        printf(RED "%s couldn’t get a seat\n" NORMAL, name);
        return NULL;
    }

    else if (flags[ID] == 1)
    {
        printf(GREEN "Person %s has got a seat in zone H\n" NORMAL, name);
    }
    else
    {
        printf(GREEN "Person %s has got a seat in zone N\n" NORMAL, name);
    }

    pthread_t p3;
    pthread_create(&p3, NULL, sleeping_thread, NULL);

    pthread_mutex_lock(&(((struct spectator *)inp)->spec_mutex));

    while (time_up[ID] != 1)
    {
        if (enrage <= away_team)
        {
            printf(RED "%s is leaving due to the bad defensive performance of his team %d\n" NORMAL, name, enrage);
            // pthread_mutex_unlock(&(((struct spectator *)inp)->spec_mutex));
            return NULL;
        }
        else
        {
            pthread_cond_wait(&signal, &(((struct spectator *)inp)->spec_mutex));
        }
    }
    printf(CYAN "%s watched the match for %d seconds and is leaving\n" NORMAL, name, spec_time);
    pthread_join(p3, NULL);

    return NULL;
}

void *neutral_spec(void *inp)
{
    while (reading)
        ;
    char *name = ((struct spectator *)inp)->name;
    int time = ((struct spectator *)inp)->time;
    int patience = ((struct spectator *)inp)->patience;
    int enrage = ((struct spectator *)inp)->enrage;
    int ID = ((struct spectator *)inp)->ID;

    sleep(time);
    printf(RED "%s has reached the stadium\n" NORMAL, name);

    pthread_t p1, p2, p3;

    td *thread_input = (td *)(malloc(sizeof(td)));
    thread_input->ID = ID;
    thread_input->patience = patience;

    pthread_create(&p1, NULL, homeseat_wait, (void *)(thread_input));
    pthread_create(&p2, NULL, neutralseat_wait, (void *)(thread_input));
    pthread_create(&p3, NULL, awayseat_wait, (void *)(thread_input));

    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    pthread_join(p3, NULL);

    if (flags[ID] == 0)
    {
        printf(RED "%s couldn’t get a seat\n" NORMAL, name);
        return NULL;
    }
    else if (flags[ID] == 1)
    {
        printf(GREEN "Person %s has got a seat in zone H\n" NORMAL, name);
    }
    else if (flags[ID] == 2)
    {
        printf(GREEN "Person %s has got a seat in zone N\n" NORMAL, name);
    }
    else
    {
        printf(GREEN "Person %s has got a seat in zone A\n" NORMAL, name);
    }

    //neutral spec watches the entire match for spec time
    sleep(spec_time);
    printf(CYAN "%s watched the match for %d seconds and is leaving\n" NORMAL, name, spec_time);

    return NULL;
}

void *away_spec(void *inp)
{
    while (reading)
        ;
    char *name = ((struct spectator *)inp)->name;
    int time = ((struct spectator *)inp)->time;
    int patience = ((struct spectator *)inp)->patience;
    int enrage = ((struct spectator *)inp)->enrage;
    int ID = ((struct spectator *)inp)->ID;

    sleep(time);
    printf(BLUE "%s has reached the stadium\n" NORMAL, name);

    pthread_t p1, p2;

    td *thread_input = (td *)(malloc(sizeof(td)));
    thread_input->ID = ID;
    thread_input->patience = patience;

    pthread_create(&p1, NULL, awayseat_wait, (void *)(thread_input));
    pthread_join(p1, NULL);

    if (flags[ID] == 0)
    {
        printf(RED "%s couldn’t get a seat\n" NORMAL, name);
        return NULL;
    }
    else
    {
        printf(GREEN "Person %s has got a seat in zone N\n" NORMAL, name);
    }

    pthread_t p3;
    pthread_create(&p3, NULL, sleeping_thread, NULL);

    pthread_mutex_lock(&(((struct spectator *)inp)->spec_mutex));
    while (time_up[ID] != 1)
    {
        if (enrage <= home_team)
        {
            printf(RED "%s is leaving due to the bad defensive performance of his team %d\n" NORMAL, name, enrage);
            pthread_mutex_unlock(&(((struct spectator *)inp)->spec_mutex));
            return NULL;
        }
        else
        {
            pthread_cond_wait(&signal, &(((struct spectator *)inp)->spec_mutex));
        }
    }
    printf(CYAN "%s watched the match for %d seconds and is leaving\n" NORMAL, name, spec_time);
    pthread_join(p3, NULL);

    return NULL;
}

/***********************************************************************/
/***********************************************************************/

void *goal_thread(void *inp)
{
    char team = ((struct goal *)inp)->team;
    int time = ((struct goal *)inp)->goal_time;
    float prob = ((struct goal *)inp)->prob;

    sleep(time);
    float prob_occured = rand() % 100;
    prob_occured = prob_occured / 100;

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

int main()
{
    srand(time(NULL));

    scanf("%d %d %d", &cap_h, &cap_a, &cap_n);
    sem_init(&home_sem, 0, cap_h);
    sem_init(&neutral_sem, 0, cap_n);
    sem_init(&away_sem, 0, cap_a);

    int groups, Total_peep = 0;
    pthread_t sthread[1024];
    scanf("%d %d", &spec_time, &groups);

    for (int i = 0; i < groups; i++)
    {
        int num_peep;
        pthread_mutex_init(&(seat_mutex[i]), NULL);

        scanf("%d", &num_peep);
        for (int j = 0; j < num_peep; j++)
        {
            Total_peep++;
            Spectator *thread_input = (Spectator *)(malloc(sizeof(Spectator)));
            scanf("%s", thread_input->name);
            getchar();
            scanf("%c%d%d%d", &thread_input->support, &thread_input->time, &thread_input->patience, &thread_input->enrage);
            int ID = Total_peep;
            thread_input->ID = ID;
            flags[ID] = 0;
            time_up[ID] = 0;
            if (thread_input->support == 'H')
                pthread_create(&sthread[i], NULL, home_spec, (void *)(thread_input));
            else if (thread_input->support == 'N')
                pthread_create(&sthread[i], NULL, neutral_spec, (void *)(thread_input));
            else if (thread_input->support == 'A')
                pthread_create(&sthread[i], NULL, away_spec, (void *)(thread_input));
            else
            {
                printf(MAGENTA "Wrong spec entered\n" NORMAL);
                return 0;
            }
        }
    }

    int num_goals;
    pthread_t sgoal[num_goals];
    scanf("%d", &num_goals);

    for (int i = 0; i < num_goals; i++)
    {
        Goal *thread_input = (Goal *)(malloc(sizeof(Goal)));
        getchar();
        scanf("%c%d%f", &thread_input->team, &thread_input->goal_time, &thread_input->prob);
        pthread_create(&sgoal[i], NULL, goal_thread, (void *)(thread_input));
    }

    reading = 1;

    for (int i = 0; i < num_goals; i++)
        pthread_join(sgoal[i], NULL);

    for (int i = 0; i < Total_peep; i++)
        pthread_join(sthread[i], NULL);

    return 0;
}