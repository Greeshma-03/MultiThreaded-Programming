#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#include <errno.h>

#define RED "\033[1;91m"
#define GREEN "\033[1;92m"
#define YELLOW "\033[1;93m"
#define BLUE "\033[1;94m"
#define MAGENTA "\033[1;95m"
#define CYAN "\033[1;96m"
#define NORMAL "\033[0m"

#define max_people 1000

pthread_mutex_t clasico_lock;

typedef struct person{
    char Name[20];
    char type;
    int appearence_time;
    int patience_time;
    int num_goals; 
    pthread_mutex_t plock;
    pthread_mutex_t prlock;
    pthread_cond_t cond; 
}Person;

typedef struct goal{
    char team;
    int chance_time;
    float probability;
}Goal;

int H_cap, A_cap, N_cap;
int max_time;           // max spectating time 

int H_score, A_score;   

sem_t A_zone;
sem_t H_zone;
sem_t N_zone;

void* Nue_thrfun(void* arg)
{
    time_t* time_taken = (time_t*)malloc(sizeof(time_t));
    (*time_taken) = time(NULL);
    int check = sem_timedwait(&N_zone, (struct timespec*)arg);
    (*time_taken) = time(NULL) - (*time_taken); 

    return time_taken;
}

void* Hme_thrfun(void* arg)
{
    time_t* time_taken = (time_t*)malloc(sizeof(time_t));
    (*time_taken) = time(NULL);
    int check = sem_timedwait(&H_zone, (struct timespec*)arg);
    (*time_taken) = time(NULL) - (*time_taken); 

    return time_taken;
}

void* allocate_seats(void* arg)
{
    int check; 
    struct timespec ts;
    sleep(((Person*)arg)->appearence_time);
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += ((Person*)arg)->patience_time; 
    if(((Person*)arg)->type == 'A')
    {
        check = sem_timedwait(&A_zone, &ts);
        if(check == -1 && errno == ETIMEDOUT)
        {
            printf(RED"%s couldn’t get a seat"NORMAL,((Person*)arg)->Name);
            return NULL; 
        }
        printf(BLUE"%s has got a seat in zone A\n", ((Person*)arg)->Name);
    }
    else if(((Person*)arg)->type == 'H')
    {
        time_t *H_time; 
        time_t* N_time; 
        pthread_t Nue_zone, Hme_zone;
        pthread_create(&Nue_zone, NULL,Nue_thrfun,(void*)(&ts));
        pthread_create(&Hme_zone, NULL,Hme_thrfun,(void*)(&ts));
        
        pthread_join(Hme_thrfun,(void *)&H_time);       // *H_time = time_taken by the Hme_thread
        pthread_join(Nue_thrfun, (void*)&N_time); 

        if((H_time) >= ((Person)arg)->patience_time && (N_time) >= ((Person)arg)->patience_time)
        {
            printf(RED"%s couldn’t get a seat"NORMAL,((Person*)arg)->Name);
            return NULL;
        }
        else if((*H_time) > (*N_time))
        {
            printf(BLUE"%s has got a seat in zone N\n", ((Person*)arg)->Name);
            if((H_time) < ((Person)arg)->patience_time)
            {
                sem_post(&H_zone);
            }
        }
        else if((*N_time) > (*H_time))
        {
            printf(BLUE"%s has got a seat in zone H\n", ((Person*)arg)->Name);
            if((N_time) < ((Person)arg)->patience_time)
            {
                sem_post(&N_zone);
            }
        }
        else if((*N_time) == (*H_time))
        {
            if((*N_time)%2 == 0)
            {
                printf(BLUE"%s has got a seat in zone N\n", ((Person*)arg)->Name);
                sem_post(&H_zone);
            }
            else 
            {
                printf(BLUE"%s has got a seat in zone H\n", ((Person*)arg)->Name); 
                sem_post(&N_zone);
            }
        }
    }
    else 
    {
           
    }

}

int main(void)
{
    scanf("%d %d %d", &H_cap, &A_cap, &N_cap);
    scanf("%d", &max_time);
    int num_groups;
    scanf("%d", &num_groups);

    int friends[num_groups];
    Person Audiance[max_people];

    int num_people = 0;

    for(int i = 0;i < num_groups; i++)
    {
        scanf("%d", &friends[i]);
        for(int j = 0; j < friends[i]; j++)
        {
            scanf("%s", Audiance[num_people].Name);
            getchar();
            scanf("%c %d %d %d", &Audiance[num_people].type, &Audiance[num_people].appearence_time, &Audiance[num_people].patience_time, &Audiance[num_people].num_goals);
            // Audiance[num_people].grp_idx = i;
            num_people++;
        }
    }
    int goals; 
    scanf("%d", &goals);
    Goal chances[goals];
    for(int i = 0;i < goals; i++)
    {
        getchar();
        scanf("%c %d %f", &chances[i].team, &chances[i].chance_time, &chances[i].probability);
    }

    sem_init(&A_zone, 0, A_cap);       
    sem_init(&H_zone, 0, H_cap);
    sem_init(&N_zone , 0, N_cap);

    pthread_t people[num_people];

    for(int i = 0;i < num_people; i++)
    {
        pthread_create(&people[i], NULL, allocate_seats, (void *)(&Audiance[i]));
    }

    for(int i; i < num_people; i++)
    {
        pthread_join(people[i],NULL);
    }
    return 0;
}