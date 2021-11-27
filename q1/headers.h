#ifndef __HEADERS_H
#define __HEADERS_H

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



/***************************Global variables*************************/
extern Lab TA_labs[64]; //Lab in the college that consists of TA's
extern int absent_labs[64];
extern int alloted[64];           //alloted slots for each of the course
extern float course_interest[64]; //interest in each course
extern char course_name[64][32];  //name of the courses are stored
extern int reading;           //input reading

/*******************synchornization variables************************/
extern pthread_mutex_t course_seat_mutex[64];                       //allocating a seat to student
extern pthread_mutex_t course_mutex;    //course_interest changing mutex
/********************************************************************/

/********************************************************************/

int Get_availabe_TA(int id);
void *course(void *inp);
void *student(void *inp);


#endif