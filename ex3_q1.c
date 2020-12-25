#include "ex3_q1.h"


pthread_mutex_t *mtx_list;
pthread_mutex_t *mtx_rand;
pthread_mutex_t *mtx_print;
pthread_mutex_t *mtx_counter;

pthread_cond_t *count_thread_created;
pthread_cond_t *count_nodes_added_to_list;
pthread_cond_t *check_if_items_to_handle_exists;

int num_of_threads_created=0;
int num_of_items_create=0;
int num_of_messages_in_list=0;
int num_of_proccessed_in_list=0;

void wait_for_threads_to_finish(pthread_t* threads, int num_of_threads)
{
    for(int i=0; i < num_of_threads; i++)
    {
        pthread_join(threads[i], NULL);
    }
}


int main()
{
    pthread_t producers[N_PROD];
    pthread_t consumers[N_PROD];

    allocate_mutex();
    mtx_list = malloc(sizeof(pthread_mutex_t*));
    mtx_rand = malloc(sizeof(pthread_mutex_t*));
    allocate_cond();
    initiate_mutex();
    initiate_cond();
    printf("befpre mtx_rand lock - c1111!!!!!!!!!!!!!\n");

    pthread_mutex_lock(mtx_rand);
    printf("inside mtx_rand lock - c1111!!!!!!!!!!!!!\n");
    pthread_mutex_unlock(mtx_rand);
    printf("END!");
/*
    create_producers(producers);
    create_consumers(consumers);
    pthread_cond_broadcast(count_thread_created); //TODO: check if it releases all threads

    //Waiting for threads to finish
    wait_for_threads_to_finish(producers, N_PROD);
    wait_for_threads_to_finish(consumers, N_CONS);

    print_list();*/
    destroy_mutex();
    destroy_cond();

    printf(PROD_TERMINATED);
    printf(CONS_TERMINATED);

    free_mutex_allocations();
    free_cond_allocations();
}

void wait_until_all_thread_created()
{
    printf("wait_until_all_thread_created: before-mtx_counter\n");
    pthread_mutex_lock(mtx_counter);
    printf("wait_until_all_thread_created-in-mtx_counter\n");
    while(num_of_threads_created < (N_CONS + N_PROD))
    {
        printf("wait_until_all_thread_created-3\n");
        pthread_cond_wait(count_thread_created, mtx_counter);
        printf("wait_until_all_thread_created-4\n");
    }
    printf("wait_until_all_thread_created-5\n");
    pthread_mutex_unlock(mtx_counter);

}

void * producer(void *ptr)
{
    wait_until_all_thread_created();

    int randNums[2];
    struct item* new_item = NULL;
    int * thread_num = (int*) ptr;
    printf("Before mtx_rand lock!!!!\n");
    pthread_mutex_lock(mtx_rand);
    printf("inside mtx_rand lock - c1!!!!!!!!!!!!!\n");
    pthread_mutex_unlock(mtx_rand);

    printf("A1\n");
    /*getting_random_numbers(randNums);
    printf("A2\n");
    create_item_with_lock(randNums, new_item, *thread_num);
    printf("A3\n");
    adding_item_to_list_with_lock(*thread_num, new_item);
    printf("A4\n");
    end_producer(*thread_num); 
    printf("A5\n");
    */
    pthread_exit(NULL);
}

void * consumer(void *ptr)
{
    wait_for_enough_items_in_list();
    int * thread_num = (int*) ptr;
    handle_getting_item(*thread_num);
    ////wait_if_no_items_to_handle();
    //check_if_proccessed_items_equal_to_finish(thread_num);
    //get_and_handle_item_in_list(thread_num);
    end_consumer(*thread_num);
    pthread_exit(NULL);  
}


void handle_getting_item(int thread_num)
{
    pthread_mutex_lock(mtx_counter);
    while(num_of_proccessed_in_list == num_of_messages_in_list)
    {
        pthread_cond_wait(check_if_items_to_handle_exists, mtx_counter);
    }

    if(num_of_proccessed_in_list==TOTAL_ITEMS)
    {
        pthread_mutex_unlock(mtx_counter);
        end_consumer(thread_num);
    }
    else
    {
        pthread_mutex_lock(mtx_list);
        get_and_handle_item_in_list(thread_num);
        pthread_mutex_unlock(mtx_counter);
        pthread_mutex_unlock(mtx_list);
    }
}

void update_new_item_fields(int* randNums, struct item* new_item)
{
    new_item->prod = randNums[0] * randNums[1];
    new_item->status = NOT_DONE;
}

void create_item_with_lock(int* randNums, struct item* new_item, int thread_num)
{
    pthread_mutex_lock(mtx_counter);
    if(num_of_items_create < TOTAL_ITEMS)
    {
        new_item = malloc(sizeof(item));
        update_new_item_fields(randNums, new_item);
        num_of_items_create++;
        pthread_mutex_unlock(mtx_counter);
    }
    else
    {
        pthread_mutex_unlock(mtx_counter);
        end_producer(thread_num);
    }
}

void check_and_wake_consumers()
{
    if(num_of_messages_in_list == ITEM_START_CNT && num_of_items_create == ITEM_START_CNT)
    {
        pthread_cond_broadcast(count_nodes_added_to_list);
    }
}

void wait_for_enough_items_in_list()
{
    pthread_mutex_lock(mtx_counter);
    while(num_of_messages_in_list < ITEM_START_CNT || num_of_items_create < ITEM_START_CNT)
    {
        pthread_cond_wait(count_nodes_added_to_list, mtx_counter);
    }
    pthread_mutex_unlock(mtx_counter);

}

void wait_if_no_items_to_handle()
{
    pthread_mutex_lock(mtx_counter);
    while(num_of_proccessed_in_list == num_of_messages_in_list)
    {
        pthread_cond_wait(check_if_items_to_handle_exists, mtx_counter);
    }
    pthread_mutex_unlock(mtx_counter);
}

void get_and_handle_item_in_list(int thread_num)
{
    item * item_got = get_undone_from_list();
    write_getting_item_with_lock(thread_num,item_got);
    set_two_factors(item_got);
    item_got->status = DONE;
    num_of_proccessed_in_list++;
}


void adding_item_to_list_with_lock(int thread_num, struct item* new_item)
{
    pthread_mutex_lock(mtx_list);
    pthread_mutex_lock(mtx_counter);
    if(num_of_messages_in_list==TOTAL_ITEMS)
    {
        end_producer(thread_num);
    }
    else
    {
        write_adding_item_with_lock(thread_num, new_item);
        add_to_list(new_item);
        num_of_messages_in_list++;
        check_and_wake_consumers();
    }  
    pthread_mutex_unlock(mtx_counter);
    pthread_mutex_unlock(mtx_list);    
}

void getting_random_numbers(int* randNums)
{
    for(int i=0; i<2; i++)
    {
        printf("%d: before mtx_rand lock\n", i);
        pthread_mutex_lock(mtx_rand);
        printf("%d: inside mtx_rand lock - c1\n", i);
        pthread_mutex_unlock(mtx_rand);
        randNums[i] = get_random_in_range();
        printf("%d: inside mtx_rand lock - c2\n", i);
        //pthread_mutex_unlock(mtx_rand);
        printf("%d: outside mtx_rand lock\n", i);
        while(!is_prime(randNums[i]))
        {
            pthread_mutex_lock(mtx_rand);
            randNums[i] = get_random_in_range();
            pthread_mutex_unlock(mtx_rand);
        }
    }
}

//end threads functions
void end_producer(int thread_num)
{
    write_producer_is_done_with_lock(thread_num);
    pthread_exit(NULL);
}

void end_consumer(int thread_num)
{
    write_consumer_is_done_with_lock(thread_num);
    pthread_exit(NULL);
}

//print functions

void write_producer_is_done_with_lock(int thread_num)
{
    pthread_mutex_lock(mtx_print);
    write_producer_is_done(thread_num);
    pthread_mutex_unlock(mtx_print);
}

void write_consumer_is_done_with_lock(int thread_num)
{
    pthread_mutex_lock(mtx_print);
    write_consumer_is_done(thread_num);
    pthread_mutex_unlock(mtx_print);
}

void write_getting_item_with_lock(int thread_num, struct item* item_got)
{
    pthread_mutex_lock(mtx_print);
    write_getting_item(thread_num, item_got);
    pthread_mutex_unlock(mtx_print);
}

void write_adding_item_with_lock(int thread_num, struct item* new_item)
{
    pthread_mutex_lock(mtx_print);
    write_adding_item(thread_num, new_item);
    pthread_mutex_unlock(mtx_print);
}


//allocate functions
void allocate_mutex()
{


    mtx_print = malloc(sizeof(pthread_mutex_t*));
    mtx_counter = malloc(sizeof(pthread_mutex_t*));
}

void allocate_cond()
{
    count_thread_created = malloc(sizeof(pthread_cond_t*));
    count_nodes_added_to_list = malloc(sizeof(pthread_cond_t*));
    check_if_items_to_handle_exists = malloc(sizeof(pthread_cond_t*));
}

//initiate functions
void initiate_mutex()
{
    pthread_mutex_init(mtx_list, NULL);
    pthread_mutex_init(mtx_rand, NULL);
    pthread_mutex_init(mtx_print, NULL);
    pthread_mutex_init(mtx_counter, NULL);
}

void initiate_cond()
{
    pthread_cond_init(count_thread_created, NULL);
    pthread_cond_init(count_nodes_added_to_list, NULL);
    pthread_cond_init(check_if_items_to_handle_exists, NULL);
}

//destory functions
void destroy_mutex()
{
    pthread_mutex_destroy(mtx_list);
    pthread_mutex_destroy(mtx_rand);
    pthread_mutex_destroy(mtx_print);
    pthread_mutex_destroy(mtx_counter);
}

void destroy_cond()
{
    pthread_cond_destroy(count_thread_created);
    pthread_cond_destroy(count_nodes_added_to_list);
    pthread_cond_destroy(check_if_items_to_handle_exists);
}

//creating threads functions
void create_producers(pthread_t* producers)
{
    //Creating producer threads
    for(int i=0, thread_num = i+1; i < N_PROD; i++)
    {
        pthread_create( &(producers[i]), NULL, producer, &thread_num);
        num_of_threads_created++;
        printf("created producer\n");
    }
    printf(ALL_PROD_CREATED);
}

void create_consumers(pthread_t* consumers)
{
    //Creating consumer threads
    for(int i=0, thread_num = i+1; i < N_CONS; i++)
    {
        pthread_create( &(consumers[i]), NULL, consumer, &thread_num);
        num_of_threads_created++;
        printf("created consumer\n");
    }
    printf(ALL_CONS_CREATED);
}

//free memory functions
void free_mutex_allocations()
{
    free(mtx_list);
    free(mtx_rand);
    free(mtx_print);
    free(mtx_counter);
}

void free_cond_allocations()
{
    free(count_thread_created);
    free(count_nodes_added_to_list);
    free(check_if_items_to_handle_exists);
}