#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

int counter = 0;
pthread_mutex_t cnt_lock;

typedef struct thread_details
{
    int idx;
} td;

void *inc_counter(void *inp)
{
    int thread_idx = ((struct thread_details *)inp)->idx;

    pthread_mutex_lock(&cnt_lock);
    counter++;
    pthread_mutex_unlock(&cnt_lock);
    return NULL;
}

int main()
{

    const int TIMES = 10000;
    pthread_t thread_ids_arr[TIMES];

    pthread_mutex_init(&cnt_lock, NULL);

    for (int i = 0; i < TIMES; i++)
    {
        pthread_t curr_tid;
        td *thread_input = (td *)(malloc(sizeof(td)));
        thread_input->idx = i;
        pthread_create(&curr_tid, NULL, inc_counter, (void *)(thread_input));
        thread_ids_arr[i] = curr_tid;
    }
    
    for (int i = 0; i < TIMES; i++)
    {
        pthread_join(thread_ids_arr[i], NULL);
    }

    pthread_mutex_destroy(&cnt_lock);
    printf("Value of counter is: %d\n", counter);
    return 0;
}
