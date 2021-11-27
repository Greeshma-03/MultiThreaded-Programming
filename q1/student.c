#include "headers.h"

void *student(void *inp)
{
    while (reading)
        ; //delay until input is read

    int pref[3];
    float calibre = ((struct student *)inp)->calibre;
    pref[0] = ((struct student *)inp)->pref1;
    pref[1] = ((struct student *)inp)->pref2;
    pref[2] = ((struct student *)inp)->pref3;
    int time = ((struct student *)inp)->time;
    int ID = ((struct student *)inp)->ID;

    //filling the preferences
    sleep(time);
    printf(GREEN "Student %d has filled in preferences for course registration\n" NORMAL, ID);

    for (int i = 0; i < 3; i++)
    {
        int with_draw = 0;
        while (1)
        {
            //if course is withdrawn from portal then shifting to next preference
            pthread_mutex_lock(&course_mutex);
            float course_interst = course_interest[pref[i]];
            pthread_mutex_unlock(&course_mutex);

            if (course_interst == (float)-1)
            {
                with_draw = 1;
                break;
            }
            //if course is removed u continue to the next preference

            //waiting for tutorial slot
            pthread_mutex_lock(&course_seat_mutex[pref[i]]);
            if (alloted[pref[i]] > 0)
            {
                alloted[pref[i]]--; //Student was allocated a seat
                printf(BLUE "Student %d has been allocated a seat in course %s\n" NORMAL, ID, course_name[pref[i]]);
                pthread_mutex_unlock(&course_seat_mutex[pref[i]]);
                break;
            }
            pthread_mutex_unlock(&course_seat_mutex[pref[i]]);
        }

        if (with_draw == 0)
        {
            sleep(5); //listening to tutorial :P
            float p = rand() % 100;
            p = p / 100;
            pthread_mutex_lock(&course_mutex);
            float p_student = course_interest[pref[i]] * calibre;
            pthread_mutex_unlock(&course_mutex);

            if (p_student > p)
            { //finalized a course
                printf(RED "Student %d has selected course %s permanently\n" NORMAL, ID, course_name[pref[i]]);
                return NULL;
            }
            else
            { //moving to next preference
                printf(CYAN "Student %d has withdrawn from course %s\n" NORMAL, ID, course_name[pref[i]]);
            }
        }

        if (i != 2)
        {
            printf(CYAN "Student %d has changed current preference from %s (priority %d) to %s (priority %d)\n" NORMAL, ID, course_name[pref[i]], i, course_name[pref[i + 1]], i + 1);
        }
        //deciding to select the course
    }

    printf(RED "Student %d couldn’t get any of his preferred courses\n" NORMAL, ID);
    return NULL;
}
