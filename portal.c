#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

/*************COLORS***************/
#define RED "\033[1;91m"
#define GREEN "\033[1;92m"
#define YELLOW "\033[1;93m"
#define BLUE "\033[1;94m"
#define MAGENTA "\033[1;95m"
#define CYAN "\033[1;96m"
#define NORMAL "\033[0m"
/**********************************/

typedef struct course
{
    float interest;
    int max_slots;
    int num_labs;
    int lab_IDs[32]; //the number of labs for each course doesn't exceed 32
    int ID;
} Course;

typedef struct student
{
    float calibre;
    int pref1;
    int pref2;
    int pref3;
    int time;
    int ID;
} Student;

typedef struct lab
{
    pthread_mutex_t lab_mutex;
    char name[32];
    int num_Ta;
    int TA[32]; //stores the number of times a xth TA can do his TA_ship
    int Max_Taship;
    pthread_mutex_t ta_mutex[32]; //mutexes for each of the TA

} Lab;

/*******************synchornization variables************************/
pthread_mutex_t seat_mutex = PTHREAD_MUTEX_INITIALIZER;   //allocating a seat to student
pthread_mutex_t course_mutex = PTHREAD_MUTEX_INITIALIZER; //course_interest changing mutex
pthread_cond_t seatallocated = PTHREAD_COND_INITIALIZER;  //seats are allocated
/********************************************************************/

/***************************Global variables*************************/
Lab TA_labs[64]; //Lab in the college that consists of TA's
int absent_labs[64];
int alloted[64];           //alloted slots for each of the course
float course_interest[64]; //interest in each course
char course_name[64][32];  //name of the courses are stored
int reading = 1;           //input reading
/********************************************************************/

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
    int num_labs = ((struct course *)inp)->num_labs;        // total no.of labs allocated
    int ID = ((struct course *)inp)->ID;                    // course id 
    int *lab_ID = ((struct course *)inp)->lab_IDs;          //list of lab_ids for that course

    int non_existence = 0;              // is lab alive 

    while (non_existence != num_labs)
    {
        for (int i = 0; i < num_labs; i++)
        {
            if (absent_labs[lab_ID[i]] != 1)
            {
                int x = Get_availabe_TA(lab_ID[i]); //x is TA_ID

                if (x == -2)
                {
                    absent_labs[lab_ID[i]] = 1;
                    printf(CYAN"Lab %s no longer has students available for TA ship\n"NORMAL,TA_labs[lab_ID[i]].name);
                    non_existence++;
                    continue;
                }

                if (x >= 0)
                {
                    pthread_mutex_lock(&TA_labs[lab_ID[i]].ta_mutex[x]);
                    int nth_ta_ship = TA_labs[lab_ID[i]].Max_Taship - TA_labs[lab_ID[i]].TA[x];
                    //get the available TA from given lab_id
                    printf(BLUE "TA %d from lab %s has been allocated to course %s for his %d TA ship\n" NORMAL, x, TA_labs[lab_ID[i]].name, course_name[ID], nth_ta_ship);

                    /*******Conducting tutorial*******/
                    int store = TA_labs[lab_ID[i]].TA[x];
                    TA_labs[lab_ID[i]].TA[x] = -1;

                    int p = rand();
                    p = p % max_slots;
                    p += 1; //the randomly selected seats range from 0+1 to w-1+1 inclusive
                    pthread_mutex_unlock(&TA_labs[lab_ID[i]].ta_mutex[x]);

                    printf(YELLOW "Course %s has been allocated %d seats\n" NORMAL, course_name[ID], p);

                    pthread_mutex_lock(&seat_mutex);
                    alloted[ID] = p;
                    pthread_mutex_unlock(&seat_mutex);

                    pthread_cond_signal(&seatallocated);
                    sleep(2); //don't sleep in tut :P

                    pthread_mutex_lock(&seat_mutex);
                    alloted[ID] = 0;
                    pthread_mutex_unlock(&seat_mutex);

                    pthread_mutex_lock(&TA_labs[lab_ID[i]].ta_mutex[x]);
                    TA_labs[lab_ID[i]].TA[x] = store;
                    /*********************************/
                    printf(BLUE "TA %d from lab %s has completed the tutorial and left the course %s\n" NORMAL, x, TA_labs[lab_ID[i]].name, course_name[ID]);
                    pthread_mutex_unlock(&TA_labs[lab_ID[i]].ta_mutex[x]);
                }
            }
        }
    }

    pthread_mutex_lock(&course_mutex);
    course_interest[ID] = -1; //course no longer exists
    pthread_mutex_unlock(&course_mutex);

    printf(RED "Course %s does not have any TA mentors eligible and is removed from course offerings\n" NORMAL, course_name[ID]);

    return NULL;
}

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
        //waiting for tutorial slot
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
            pthread_mutex_lock(&seat_mutex);

            if (alloted[pref[i]] > 0)
            {
                alloted[pref[i]]--; //Student was allocated a seat
                printf(BLUE "Student %d has been allocated a seat in course %s\n" NORMAL, ID, course_name[pref[i]]);
                pthread_mutex_unlock(&seat_mutex);
                break;
            }
            else
            {
                pthread_cond_wait(&seatallocated, &seat_mutex); //you will again wait for the next signal
            }
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

int main()
{

    srand(time(NULL));
    int num_students, num_labs /*Max labs are 64*/, num_courses;
    scanf("%d%d%d", &num_students, &num_labs, &num_courses);

    //creating course threads and intialising all variables
    pthread_t cthread[num_courses];

    for (int i = 0; i < num_courses; i++)
    {
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
