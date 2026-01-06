//********************************************************************
//
// Isaac Godsey
// Operating Systems
// Programming Project #4: Process Synchronization Using Pthreads: The Producer / Consumer Problem With Prime Number Detector
// October 4th, 2025
// Instructor: Dr. Siming Liu
// used with wsl (ubuntu) linux system
//
//********************************************************************

#include <stdlib.h>
#include "buffer.h"
#include <semaphore.h>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <string>
#include <map>

#define MAX_THREADS 700

// function definitions
//********************************************************************
// is_prime Function
//
// This function simply checks if a number is a prime number
//
// Return Value
// ------------
// bool                         True/False if number is prime
//
// Value Parameters
// ----------------
// num		int		The number to be assessed
//********************************************************************
bool is_prime(int num);

//********************************************************************
//
// Producer Function
//
// This function inserts a random number into the buffer
//
// Return Value
// ------------
// void*                 always returns nullptr, The return value is required for compatibility
//                       with pthreads
//
// Value Parameters
// ----------------
// param		void*		a general parameter to pass in data
//*******************************************************************
void *producer(void *param);

//********************************************************************
//
// consumer Function
//
// This function consumes a number from the buffer and then uses is_prime
// to see if it is prime
//
// Return Value
// ------------
// void*                 always returns nullptr, The return value is required for compatibility
//                        with pthreads
//
// Value Parameters
// ----------------
// param		void*		a general parameter to pass in data
//*******************************************************************
void *consumer(void *param);

//********************************************************************
// initialize function
//
// This function initializes the buffer, mutex and both semaphores
//
// Return Value
// ------------
// void                         does not return anything
//********************************************************************
void initialize();

buffer_item buffer[BUFFER_SIZE];

pthread_mutex_t mutex;

sem_t full;
sem_t empty;

bool sim_flag = 1;

// variables for buffer index
int out = 0;
int in = 0;

// global variables for statistics
int items_produced = 0;
int items_consumed = 0;
bool snap_shots = 0;
int buffers_occupied = 0;
int buffers_full = 0;
int buffers_empty = 0;

std::map<long, int> produced_items;
std::map<long, int> consumed_items;

int main(int argc, char *argv[])
{

    int sim_length = atoi(argv[1]);
    int max_thread_sleep_time = atoi(argv[2]);
    int producer_threads = atoi(argv[3]);
    int consumer_threads = atoi(argv[4]);
    std::string yes_no = argv[5];
    snap_shots = (yes_no == "yes");

    // seed the time so that rand generates a different random number each time
    srand(time(nullptr));

    // initialize buffer, mutex and semaphores
    initialize();

    // arrays to store thread IDs and how many items each produced
    pthread_t producers[MAX_THREADS];
    pthread_t consumers[MAX_THREADS];

    // default pthread attributes
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    std::cout << "starting threads..." << std::endl;

    // create producer and consumer threads
    for (int x = 0; x < producer_threads; x++)
    {
        pthread_create(&producers[x], &attr, producer, argv);
    }

    for (int x = 0; x < consumer_threads; x++)
    {
        pthread_create(&consumers[x], &attr, consumer, argv);
    }

    // main thread sleep
    sleep(sim_length);
    sim_flag = 0;

    // wake producers waiting on empty VIBE CODE
    for (int x = 0; x < producer_threads; x++)
    {
        sem_post(&empty);
    }

    // wake consumers waiting on full VIBE CODE
    for (int x = 0; x < consumer_threads; x++)
    {
        sem_post(&full);
    }

    //  join producer and consumer threads
    for (int x = 0; x < producer_threads; x++)
    {
        pthread_join(producers[x], nullptr);
    }

    for (int x = 0; x < consumer_threads; x++)
    {
        pthread_join(consumers[x], nullptr);
    }

    //                     Display Statistics Exit
    std::cout
        << "PRODUCER / CONSUMER SIMULATION COMPLETE    " << std::endl
        << "=======================================    " << std::endl
        << "Simulation Time: " << sim_length << std::endl
        << "Maximum Thread Sleep Time:                 " << max_thread_sleep_time << std::endl
        << "Number of Producer Threads:                " << producer_threads << std::endl
        << "Number of Consumer Threads:                " << consumer_threads << std::endl
        << "Size of Buffer:                            " << BUFFER_SIZE << std::endl
        << "                                           " << std::endl
        << "Total Number of Items Produced:            " << items_produced << std::endl;
    for (int x = 1; x < producer_threads + 1; x++)
    {
        std::cout << "Thread " << x << ": " << produced_items[producers[x - 1]] << std::endl;
    }
    std::cout << "                                           " << std::endl;

    std::cout
        << "Total Number of Items Consumed:            " << items_consumed << std::endl;
    for (int x = 1; x < consumer_threads + 1; x++)
    {
        std::cout << "Thread " << x << ": " << consumed_items[consumers[x - 1]] << std::endl;
    }
    std::cout
        << "                                           " << std::endl
        << "Number of Items remaining in buffer:       " << items_produced - items_consumed << std::endl
        << "Number of Times Buffer Was Full:           " << buffers_full << std::endl
        << "Number of Times Buffer Was Empty:          " << buffers_empty << std::endl;

    return 0;
}
void *producer(void *param)
{
    char **argv = static_cast<char **>(param);
    int max_sleep_time = atoi(argv[2]);

    buffer_item rand;

    while (sim_flag)
    {
        int sleep_time = std::rand() % (max_sleep_time + 1);

        // sleep for a random period of time
        sleep(sleep_time);

        // generate a random number between 0-100
        rand = std::rand() % 101;

        // if the buffer is able to insert an item and the simulation is still running
        if (buffer_insert_item(rand) && sim_flag)
        {
            produced_items[pthread_self()]++;
        }
        // else if the simulation is running and it wasnt able to insert item throw error
        else if (sim_flag)
        {
            std::cout << "Producer: " << pthread_self() << " was not able to insert item";
            pthread_exit(0);
        }
    }
    pthread_exit(0);
}
void *consumer(void *param)
{
    char **argv = static_cast<char **>(param);
    int max_sleep_time = atoi(argv[2]);

    buffer_item rand;

    while (sim_flag)
    {

        int sleep_time = std::rand() % (max_sleep_time + 1);

        // sleep for a random period of time
        sleep(sleep_time);

        // if the buffer is able to remove an item and the simulation is still running
        if (buffer_remove_item(&rand) && sim_flag)
        {
            consumed_items[pthread_self()]++;
        }
        // else if the simulation is running and it wasnt able to remove item throw error
        else if (sim_flag)
        {
            std::cout << "consumer: " << pthread_self() << " could not remove item " << std::endl;
            pthread_exit(0);
        }
    }
    pthread_exit(0);
}

bool buffer_insert_item(buffer_item item)
{
    bool success = false;

    // if buffer is full wait here, if empty is 0
    sem_wait(&empty);
    if (sim_flag)
    {
        /* aquire the mutex lock */
        pthread_mutex_lock(&mutex);

        // add item to buffer
        buffer[in] = item;
        buffers_occupied++;
        items_produced++;
        if (buffers_occupied == BUFFER_SIZE)
        {
            buffers_full++;
        }
        if (snap_shots == true)
        {
            std::cout << "Producer: " << pthread_self() << " produced: " << item << std::endl;
            std::cout << "(buffers occupied: " << buffers_occupied << ")" << std::endl;
            std::cout << "buffers: ";

            // display buffer
            for (int x = 0; x < BUFFER_SIZE; x++)
            {
                std::cout << buffer[x] << "    ";
            }
            std::cout << std::endl
                      << "        ";
            for (int x = 0; x < BUFFER_SIZE; x++)
            {
                std::cout << "____  ";
            }
            std::cout << std::endl;
            std::cout << "         ";

            for (int x = 0; x < BUFFER_SIZE; x++)
            {
                if (x == in)
                {
                    std::cout << " W";
                }
                else
                {
                    std::cout << "      ";
                }
            }
            std::cout << std::endl;
        }

        success = (buffer[in] == item);
        in = (in + 1) % BUFFER_SIZE;
        /* release the mutex lock */
        pthread_mutex_unlock(&mutex);
    }
    // tells consumer " we put an item in the buffer"
    sem_post(&full);

    return success;
}

bool buffer_remove_item(buffer_item *item)
{
    bool success = false;

    // if buffer is empty wait here
    sem_wait(&full);
    if (sim_flag)
    {
        /* aquire the mutex lock */
        pthread_mutex_lock(&mutex);

        // consume item
        *item = buffer[out];
        // set slot to -1 to indicate it is empty
        buffer[out] = -1;
        buffers_occupied--;
        items_consumed++;
        if (buffers_occupied == 0)
        {
            buffers_empty++;
        }
        // check if item was removed successfully
        success = buffer[out] == -1;

        if (snap_shots == true)
        {
            std::cout << "consumer: " << pthread_self() << " consumed " << *item;
            if (is_prime(buffer[out]))
            {
                std::cout << " ***** PRIME *****";
            }
            std::cout << std::endl;
            std::cout << "(buffers occupied: " << buffers_occupied << ")" << std::endl;
            std::cout << "buffers: ";

            // display buffer
            for (int x = 0; x < BUFFER_SIZE; x++)
            {
                std::cout << buffer[x] << "    ";
            }
            std::cout << std::endl
                      << "         ";
            for (int x = 0; x < BUFFER_SIZE; x++)
            {
                std::cout << "____  ";
            }
            std::cout << std::endl;
            std::cout << "         ";
            for (int x = 0; x < BUFFER_SIZE; x++)
            {
                if (x == out)
                {
                    std::cout << " R";
                }
                else
                {
                    std::cout << "      ";
                }
            }
            std::cout << std::endl;
        }
        // move to next buffer
        out = (out + 1) % BUFFER_SIZE;

        /* release the mutex lock */
        pthread_mutex_unlock(&mutex);
    }
    // tells producer "we removed an item from the buffer"
    sem_post(&empty);

    return success;
}
void initialize()
{
    for (int x = 0; x < BUFFER_SIZE; x++)
    {
        buffer[x] = -1;
    }

    // semaphore initilization
    sem_init(&full, 0, 0);
    sem_init(&empty, 0, BUFFER_SIZE);

    // mutex initilization
    pthread_mutex_init(&mutex, NULL);
}

bool is_prime(int num)
{
    if (num < 2)
    {
        return false;
    }
    for (int x = 2; x < num; x++)
    {
        if (num % x == 0)
        {
            return false;
        }
    }
    return true;
}