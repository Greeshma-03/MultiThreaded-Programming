#include <stdio.h>
#include "headers.h"

int home_team = 0, away_team = 0;
int reading = 0;
int spec_time;
int flags[1024];
pthread_cond_t signal = PTHREAD_COND_INITIALIZER;
pthread_mutex_t seat_mutex[1024];
pthread_cond_t seat_came[1024] = {PTHREAD_COND_INITIALIZER};
pthread_mutex_t friend_mutex = PTHREAD_MUTEX_INITIALIZER;

int main()
{
    srand(time(NULL));

    scanf("%d %d %d", &cap_h, &cap_a, &cap_n);
    sem_init(&home_sem, 0, cap_h);
    sem_init(&neutral_sem, 0, cap_n);
    sem_init(&away_sem, 0, cap_a);

    int groups, Total_peep = -1;
    pthread_t sthread[1024];
    scanf("%d %d", &spec_time, &groups);

    for (int i = 0; i < groups; i++)
    {
        int num_peep;
        pthread_mutex_init(&(seat_mutex[i]), NULL);

        scanf("%d", &num_peep);
        friends_groups[i] = num_peep;

        for (int j = 0; j < num_peep; j++)
        {
            Total_peep++;
            Spectator *thread_input = (Spectator *)(malloc(sizeof(Spectator)));
            scanf("%s", thread_input->name);
            getchar();
            scanf("%c%d%d%d", &thread_input->support, &thread_input->time, &thread_input->patience, &thread_input->enrage);
            int ID = Total_peep;
            thread_input->ID = ID;
            thread_input->Group_ID = i;
            flags[ID] = 0;
            time_up[ID] = 0;
            if (thread_input->support == 'H')
                pthread_create(&sthread[ID], NULL, home_spec, (void *)(thread_input));
            else if (thread_input->support == 'N')
                pthread_create(&sthread[ID], NULL, neutral_spec, (void *)(thread_input));
            else if (thread_input->support == 'A')
                pthread_create(&sthread[ID], NULL, away_spec, (void *)(thread_input));
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

    for (int i = 0; i <= Total_peep; i++)
        pthread_join(sthread[i], NULL);

    return 0;
}