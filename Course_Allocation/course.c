#include "headers.h"

pthread_mutex_t lababsent_mutex = PTHREAD_MUTEX_INITIALIZER; //allocating a seat to student

int Get_availabe_TA(int id)
{
    int num = TA_labs[id].num_Ta;
    int absent = 0;
    for (int i = 0; i < num; i++)
    {
        pthread_mutex_lock(&TA_labs[id].ta_mutex[i]);
        if ((TA_labs[id].TA[i]) == 0)
            absent++;

        if ((TA_labs[id].TA[i]) > 0)
        {
            //when ta is already taking tut value will be -1 though he has taships in future
            TA_labs[id].TA[i]--;
            pthread_mutex_unlock(&TA_labs[id].ta_mutex[i]);
            return i; //ith TA is available
        }
        pthread_mutex_unlock(&TA_labs[id].ta_mutex[i]);
    }

    if (absent == num)
    {
        return -2; //indicates absence of TAs in lab!!!
    }
    return -1; //No cuurently available TAs found
}


void *course(void *inp)
{

    while (reading)
        ; //delay until input is read

    float interst = ((struct course *)inp)->interest;
    int max_slots = ((struct course *)inp)->max_slots;
    int num_labs = ((struct course *)inp)->num_labs; // total no.of labs allocated
    int ID = ((struct course *)inp)->ID;             // course id
    int *lab_ID = ((struct course *)inp)->lab_IDs;   //list of lab_ids for that course

    int non_existence = 0; // is lab alive

    while (non_existence != num_labs)
    {
        non_existence = 0;
        for (int i = 0; i < num_labs; i++)
        {
            pthread_mutex_lock(&lababsent_mutex);
            int use = absent_labs[lab_ID[i]];
            pthread_mutex_unlock(&lababsent_mutex);

            if (use != 1)
            {
                int x = Get_availabe_TA(lab_ID[i]); //x is TA_ID

                if (x == -2)
                {
                    pthread_mutex_lock(&lababsent_mutex);
                    absent_labs[lab_ID[i]] = 1;
                    pthread_mutex_unlock(&lababsent_mutex);

                    printf(CYAN "Lab %s no longer has students available for TA ship\n" NORMAL, TA_labs[lab_ID[i]].name);
                    non_existence++;
                }

                else if (x >= 0)
                {
                    pthread_mutex_lock(&TA_labs[lab_ID[i]].ta_mutex[x]);
                    int nth_ta_ship = TA_labs[lab_ID[i]].Max_Taship - TA_labs[lab_ID[i]].TA[x];
                    printf(BLUE "TA %d from lab %s has been allocated to course %s for his %d TA ship\n" NORMAL, x, TA_labs[lab_ID[i]].name, course_name[ID], nth_ta_ship);

                    /*******Conducting tutorial*******/
                    int store = TA_labs[lab_ID[i]].TA[x];
                    TA_labs[lab_ID[i]].TA[x] = -1;
                    pthread_mutex_unlock(&TA_labs[lab_ID[i]].ta_mutex[x]);

                    int p = rand();
                    p = p % max_slots;
                    p += 1; //the randomly selected seats range from 0+1 to w-1+1 inclusive

                    printf(YELLOW "Course %s has been allocated %d seats\n" NORMAL, course_name[ID], p);

                    pthread_mutex_lock(&course_seat_mutex[ID]);
                    alloted[ID] = p;
                    pthread_mutex_unlock(&course_seat_mutex[ID]);

                    sleep(3); //don't sleep in tut :P

                    pthread_mutex_lock(&course_seat_mutex[ID]);
                    alloted[ID] = 0;
                    pthread_mutex_unlock(&course_seat_mutex[ID]);

                    pthread_mutex_lock(&TA_labs[lab_ID[i]].ta_mutex[x]);
                    TA_labs[lab_ID[i]].TA[x] = store;
                    pthread_mutex_unlock(&TA_labs[lab_ID[i]].ta_mutex[x]);

                    printf(BLUE "TA %d from lab %s has completed the tutorial and left the course %s\n" NORMAL, x, TA_labs[lab_ID[i]].name, course_name[ID]);
                }
            }
            else
                non_existence++;
        }
    }

    pthread_mutex_lock(&course_mutex);
    course_interest[ID] = -1; //course no longer exists
    pthread_mutex_unlock(&course_mutex);

    printf(RED "Course %s does not have any TA mentors eligible and is removed from course offerings\n" NORMAL, course_name[ID]);

    return NULL;
}
