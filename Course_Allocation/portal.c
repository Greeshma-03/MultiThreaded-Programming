#include "headers.h"

/***************************Extern variables**************************/
int absent_labs[64];//Checking if the lab is dead or still working
int alloted[64];           //alloted slots for each of the course
float course_interest[64]; //interest in each course
char course_name[64][32];  //name of the courses are stored
int reading=1;

//MUTEX LOCKS
Lab TA_labs[64]; //Lab in the college that consists of TA's
pthread_mutex_t course_seat_mutex[64];//to update the information of course
pthread_mutex_t course_mutex = PTHREAD_MUTEX_INITIALIZER;

/*********************************************************************/

int main()
{

    srand(time(NULL));
    int num_students, num_labs /*Max labs are 64*/, num_courses;
    scanf("%d%d%d", &num_students, &num_labs, &num_courses);

    //creating course threads and intialising all variables
    pthread_t cthread[num_courses];

    for (int i = 0; i < num_courses; i++)
    {
        pthread_mutex_init(&(course_seat_mutex[i]), NULL);
        int lab;
        Course *thread_input = (Course *)(malloc(sizeof(Course)));
        char name[32];
        scanf("%s %f %d %d", name, &thread_input->interest, &thread_input->max_slots, &lab);
        thread_input->num_labs = lab;
        thread_input->ID = i;
        alloted[i] = 0;
        course_interest[i] = thread_input->interest;
        strcpy(course_name[i], name);
        for (int j = 0; j < lab; j++)
            scanf("%d", &thread_input->lab_IDs[j]);

        pthread_create(&cthread[i], NULL, course, (void *)(thread_input));
    }

    //creating student threads and intialising all variables
    pthread_t sthread[num_students];
    for (int i = 0; i < num_students; i++)
    {

        Student *thread_input = (Student *)(malloc(sizeof(Student)));
        scanf("%f%d%d%d%d", &thread_input->calibre, &thread_input->pref1, &thread_input->pref2, &thread_input->pref3, &thread_input->time);
        thread_input->ID = i;
        pthread_create(&sthread[i], NULL, student, (void *)(thread_input));
    }

    //Taking input for the labs
    for (int i = 0; i < num_labs; i++)
    {
        scanf("%s%d%d", TA_labs[i].name, &TA_labs[i].num_Ta, &TA_labs[i].Max_Taship);
        for (int j = 0; j < TA_labs[i].num_Ta; j++)
        {
            TA_labs[i].TA[j] = TA_labs[i].Max_Taship;
            pthread_mutex_init(&(TA_labs[i].ta_mutex[j]), NULL);
        }
        absent_labs[i] = 0; //intially all are present
    }

    reading = 0;

    // the program terminates after terminating of all student threads
    for (int i = 0; i < num_students; i++)
    {
        pthread_join(sthread[i], NULL);
    }

    return 0;
}