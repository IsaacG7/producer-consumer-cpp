
This is a C++ program that simulates the classic producer/consumer problem using multithreading with the pthread library. 

To run this program:

First compile by entering "g++ producer_consumer.cpp -lpthread"

This generates the executable (default: a.out).

Then, enter ./a.out <simulation_length> <max_thread_sleep> <num_producers> <num_consumers> <display_buffer>

Parameters:

simulation_length - Simulation time in seconds

max_thread_sleep - Maximum sleep time for threads (seconds)

num_producers - Number of producer threads

num_consumers - Number of consumer threads

display_buffer - "yes" to show buffer status after each production/consumption, "no" to hide

Example:

./a.out 15 3 2 4 yes

15-second simulation

Max thread sleep: 3 seconds

2 producers and 4 consumers

Display buffer status

    
