#include "ex3_q1.h"
#include "ex3_q1_given.h"

pthread_mutex_t *mtx_producer;
pthread_mutex_t *mtx_consumer;
pthread_cond_t *count_thread_created;
pthread_cond_t *count_nodes_added_to_list;

int num_of_threads_created=0;
int num_of_messages_in_list=0;

void main()
{
    pthread_t producers[N_PROD];
    pthread_t consumers[N_PROD];
    mtx_producer = malloc(sizeof(pthread_mutex_t*));
    mtx_consumer = malloc(sizeof(pthread_mutex_t*));
    count_thread_created = malloc(sizeof(pthread_cond_t*));

    pthread_mutex_init(mtx_producer, NULL);
    pthread_mutex_init(mtx_consumer, NULL);
    pthread_cond_init(count_thread_created, NULL);

    //Creating producer threads
    for(int i=0, thread_num = i+1; i < N_PROD; i++)
    {
        pthread_create( &(producers[i]), NULL, producer, &thread_num);
        num_of_threads_created++;
    }
    printf(ALL_PROD_CREATED);

    //Creating consumer threads
    for(int i=0, thread_num = i+1; i < N_CONS; i++)
    {
        pthread_create( &(consumers[i]), NULL, consumer, &thread_num);
        num_of_threads_created++;
    }
    printf(ALL_CONS_CREATED);
      
    pthread_cond_signal(count_thread_created); //TODO: check if it releases all threads

    //Waiting for threads to finish
    for(int i=0; i < N_PROD; i++)
    {
        pthread_join(producers[i], NULL);
    }

    //TODO: Producers need to wait until all threads created
    //TODO: Consumers need to wait until CNT_START_ITEM message written to list

    for(int i=0; i < N_CONS; i++)
    {
        pthread_join(consumers[i], NULL);
    }

    print_list();

    printf(PROD_TERMINATED);
    printf(CONS_TERMINATED);

    pthread_mutex_destroy(mtx_producer);
    pthread_mutex_destroy(mtx_consumer);
    pthread_cond_destroy(count_thread_created);

    free(mtx_producer);
    free(mtx_consumer);
    free(count_thread_created);
}

void producer(void *ptr)
{
    pthread_mutex_lock(mtx_producer);
    while(num_of_threads_created < (N_CONS + N_PROD))
    {
        pthread_cond_wait(num_of_threads_created, mtx_producer);
    }
    pthread_mutex_unlock(mtx_producer);

    int randNums[2];
    struct item* new_item = malloc(sizeof(item));
    int * thread_num = (int*) ptr;

    for(int i=0; i<2; i++)
    {
        pthread_mutex_lock(mtx_producer);
        randNums[i] = get_random_in_range();
        pthread_mutex_unlock(mtx_producer);

        while(!is_prime(randNums[i]))
        {
            pthread_mutex_lock(mtx_producer);
            randNums[i] = get_random_in_range();
            pthread_mutex_unlock(mtx_producer);
        }
    }

    new_item->prod = randNums[0] * randNums[1];
    new_item->status = NOT_DONE;

    pthread_mutex_lock(mtx_producer);
    write_adding_item(new_item, thread_num);
    add_to_list(new_item);
    num_of_messages_in_list++;
    pthread_mutex_unlock(mtx_producer);

    pthread_mutex_lock(mtx_producer);
    write_producer_is_done(thread_num);
    pthread_mutex_unlock(mtx_producer);

    pthread_exit(NULL);  
}


void consumer(void *ptr)
{
    pthread_mutex_lock(mtx_consumer);
    while(num_of_messages_in_list < ITEM_START_CNT)
    {
        pthread_cond_wait(num_of_messages_in_list, mtx_producer);
    }
    pthread_mutex_unlock(mtx_consumer);

    int * thread_num = (int*) ptr;

    pthread_mutex_lock(mtx_consumer);
    item * item_got = get_undone_from_list();
    write_getting_item(thread_num, item_got);
    pthread_mutex_unlock(mtx_consumer);

    set_two_factors(item_got);
    item_got->status = DONE;

    pthread_mutex_lock(mtx_consumer);
    write_consumer_is_done(thread_num);
    pthread_mutex_unlock(mtx_consumer);
    pthread_exit(NULL);  
}