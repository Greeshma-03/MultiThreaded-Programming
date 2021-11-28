#include "headers.h"

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
    int Group_ID = ((struct spectator *)inp)->Group_ID;

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
        printf(RED "%s is waiting for their friends at the exit\n" NORMAL, name);
        pthread_mutex_lock(&friend_mutex);
        friends_groups[Group_ID]--;
        if (friends_groups[Group_ID] == 0)
        {
            printf(RED "Group %d is leaving for dinner\n" NORMAL, Group_ID);
        }
        pthread_mutex_unlock(&friend_mutex);

        return NULL;
    }

    else if (flags[ID] == 1)
    {
        printf(CYAN "%s has got a seat in zone H\n" NORMAL, name);
    }
    else
    {
        printf(CYAN "%s has got a seat in zone N\n" NORMAL, name);
    }

    pthread_t p3;

    pthread_create(&p3, NULL, sleeping_thread, (void *)(thread_input));

    pthread_mutex_lock(&(((struct spectator *)inp)->spec_mutex));
    while (time_up[ID] != 1)
    {
        if (enrage <= away_team)
        {
            printf(RED "%s is leaving due to the bad defensive performance of his team\n" NORMAL, name);
            printf(RED "%s is waiting for their friends at the exit\n" NORMAL, name);
            pthread_join(p3, NULL);

            pthread_mutex_lock(&friend_mutex);
            friends_groups[Group_ID]--;
            if (friends_groups[Group_ID] == 0)
            {
                printf(RED "Group %d is leaving for dinner\n" NORMAL, Group_ID);
            }
            pthread_mutex_unlock(&friend_mutex);

            pthread_mutex_unlock(&(((struct spectator *)inp)->spec_mutex));
            add_seat(flags[ID]);
            return NULL;
        }
        else
        {
            pthread_cond_wait(&signal, &(((struct spectator *)inp)->spec_mutex)); //signals are sent by goals and time-up
        }
    }
    printf(GREEN "%s watched the match for %d seconds and is leaving\n" NORMAL, name, spec_time);
    printf(RED "%s is waiting for their friends at the exit\n" NORMAL, name);

    pthread_join(p3, NULL);
    add_seat(flags[ID]);

    pthread_mutex_lock(&friend_mutex);
    friends_groups[Group_ID]--;
    if (friends_groups[Group_ID] == 0)
    {
        printf(RED "Group %d is leaving for dinner\n" NORMAL, Group_ID);
    }
    pthread_mutex_unlock(&friend_mutex);

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
    int Group_ID = ((struct spectator *)inp)->Group_ID;

    sleep(time);
    printf(BLUE "%s has reached the stadium\n" NORMAL, name);

    pthread_t p1, p2, p3;

    td *thread_input = (td *)(malloc(sizeof(td)));
    thread_input->ID = ID;
    thread_input->patience = patience;

    pthread_create(&p1, NULL, homeseat_wait, (void *)(thread_input));
    pthread_create(&p2, NULL, neutralseat_wait, (void *)(thread_input));
    pthread_create(&p3, NULL, awayseat_wait, (void *)(thread_input));

    pthread_cond_wait(&seat_came[ID], &seat_mutex[ID]);

    if (flags[ID] == 0)
    {
        printf(RED "%s couldn’t get a seat\n" NORMAL, name);
        pthread_mutex_lock(&friend_mutex);
        friends_groups[Group_ID]--;
        if (friends_groups[Group_ID] == 0)
        {
            printf(RED "Group %d is leaving for dinner\n" NORMAL, Group_ID);
        }
        pthread_mutex_unlock(&friend_mutex);
        return NULL;
    }
    else if (flags[ID] == 1)
    {
        printf(CYAN "%s has got a seat in zone H\n" NORMAL, name);
    }
    else if (flags[ID] == 2)
    {
        printf(CYAN "%s has got a seat in zone N\n" NORMAL, name);
    }
    else
    {
        printf(CYAN "%s has got a seat in zone A\n" NORMAL, name);
    }

    //neutral spec watches the entire match for spec time
    sleep(spec_time);
    printf(GREEN "%s watched the match for %d seconds and is leaving\n" NORMAL, name, spec_time);
    pthread_join(p3, NULL);
    printf(RED "%s is waiting for their friends at the exit\n" NORMAL, name);

    add_seat(flags[ID]);

    pthread_mutex_lock(&friend_mutex);
    friends_groups[Group_ID]--;
    if (friends_groups[Group_ID] == 0)
    {
        printf(RED "Group %d is leaving for dinner\n" NORMAL, Group_ID);
    }
    pthread_mutex_unlock(&friend_mutex);

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
    int Group_ID = ((struct spectator *)inp)->Group_ID;

    sleep(time);
    printf(BLUE "%s has reached the stadium\n" NORMAL, name);

    pthread_t p1, p2;

    td *thread_input = (td *)(malloc(sizeof(td)));
    thread_input->ID = ID;
    thread_input->patience = patience;

    pthread_create(&p1, NULL, awayseat_wait, (void *)(thread_input));
    pthread_cond_wait(&seat_came[ID], &seat_mutex[ID]); //signal is sent till someone got us a seat or time expired

    if (flags[ID] == 0)
    {
        printf(RED "%s couldn’t get a seat\n" NORMAL, name);
        pthread_mutex_lock(&friend_mutex);
        friends_groups[Group_ID]--;
        if (friends_groups[Group_ID] == 0)
        {
            printf(RED "Group %d is leaving for dinner\n" NORMAL, Group_ID);
        }
        pthread_mutex_unlock(&friend_mutex);
        return NULL;
    }
    else
    {
        printf(CYAN "%s has got a seat in zone A\n" NORMAL, name);
    }

    pthread_t p3;
    pthread_create(&p3, NULL, sleeping_thread, (void *)(thread_input));

    pthread_mutex_lock(&(((struct spectator *)inp)->spec_mutex));
    while (time_up[ID] != 1)
    {
        if (enrage <= home_team)
        {
            printf(RED "%s is leaving due to the bad defensive performance of his team \n" NORMAL, name);
            printf(RED "%s is waiting for their friends at the exit\n" NORMAL, name);

            pthread_mutex_unlock(&(((struct spectator *)inp)->spec_mutex));
            add_seat(flags[ID]);

            pthread_mutex_lock(&friend_mutex);
            friends_groups[Group_ID]--;
            if (friends_groups[Group_ID] == 0)
            {
                printf(RED "Group %d is leaving for dinner\n" NORMAL, Group_ID + 1);
            }
            pthread_mutex_unlock(&friend_mutex);
            pthread_join(p3, NULL);

            return NULL;
        }
        else
        {
            pthread_cond_wait(&signal, &(((struct spectator *)inp)->spec_mutex));
        }
    }
    printf(GREEN "%s watched the match for %d seconds and is leaving\n" NORMAL, name, spec_time);
    pthread_join(p3, NULL);
    printf(RED "%s is waiting for their friends at the exit\n" NORMAL, name);

    add_seat(flags[ID]);

    pthread_mutex_lock(&friend_mutex);
    friends_groups[Group_ID]--;
    if (friends_groups[Group_ID] == 0)
    {
        printf(RED "Group %d is leaving for dinner\n" NORMAL, Group_ID + 1);
    }
    pthread_mutex_unlock(&friend_mutex);

    return NULL;
}

/***********************************************************************/
/***********************************************************************/
