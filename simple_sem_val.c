//source: GFG

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t curr_sem;
void *thread(void *arg)
{
    //wait
    sem_wait(&curr_sem);
    printf("Entering, the thread id = %ld\n", pthread_self()); //get current thread id

    //critical section
    sleep(5);

    //signal
    printf("\nExiting, the thread id = %ld\n=====\n", pthread_self());
    sem_post(&curr_sem);
    return NULL;
}

int main()
{
    sem_init(&curr_sem, 0, 2);
    pthread_t t1, t2, t3, t4;
    pthread_create(&t1, NULL, thread, NULL);
    sleep(1);
    pthread_create(&t2, NULL, thread, NULL);
    sleep(1);
    pthread_create(&t3, NULL, thread, NULL);
    sleep(1);
    pthread_create(&t4, NULL, thread, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    pthread_join(t4, NULL);
    sem_destroy(&curr_sem);
    return 0;
}
