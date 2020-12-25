#pragma once
#include "ex3_q1_given.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <signal.h>

list_node* list_head;
list_node* list_tail;

void wait_for_threads_to_finish(pthread_t* threads, int num_of_threads);
void wait_until_all_thread_created();
void* producer(void *ptr);
void* consumer(void *ptr);
void handle_getting_item(int thread_num);
void update_new_item_fields(int* randNums, struct item* new_item);
void create_item_with_lock(int* randNums, struct item* new_item, int thread_num);
void check_and_wake_consumers();
void wait_for_enough_items_in_list();
void wait_if_no_items_to_handle();
void get_and_handle_item_in_list(int thread_num);
void adding_item_to_list_with_lock(int thread_num, struct item* new_item);
void getting_random_numbers(int* randNums);

//end threads functions
void end_producer(int thread_num);

void end_consumer(int thread_num);

//print functions
void write_producer_is_done_with_lock(int thread_num);
void write_consumer_is_done_with_lock(int thread_num);
void write_getting_item_with_lock(int thread_num, struct item* item_got);
void write_adding_item_with_lock(int thread_num, struct item* new_item);

//allocate functions
void allocate_mutex();
void allocate_cond();

//initiate functions
void initiate_mutex();
void initiate_cond();

//destory functions
void destroy_mutex();
void destroy_cond();

//creating threads functions
void create_producers(pthread_t* producers);
void create_consumers(pthread_t* consumers);

//free memory functions
void free_mutex_allocations();
void free_cond_allocations();