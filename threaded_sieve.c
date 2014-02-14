/* Author: Jaron Thatcher
 * Date: 11/27/2013
 *
 * This program is the threaded version of the parallelization of the Sieve of Eratoshenes
 * compliant with POSIX 2008
 */

#define _POSIX_C_SOURCE 200809L 

#include "linked_list.h"
#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/times.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "threaded_sieve.h"
#include <limits.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <strings.h>

void usage() {
	printf("Usage: ./prog <number_of_threads> <output_file>\n");
}

int main(int argc, char **argv) {
	if(argc != 3) {
		usage();
		exit(EXIT_FAILURE);
	}

	//queue used for parallelization of sieve
	queue = createLinkedList();
	
	//number of threads and max size to sieve until
	int num_threads = atoi(argv[1]);
	unsigned int max = UINT_MAX;
	
	//initializing all chars in bitmap to 0 - aka unmarked for the sieve
	memset(bitmap, 0, sizeof(bitmap));
	bitmap[0] = 1;
	bitmap[1] = 1;
	
	//puts the first prime number into the queue
	addFrontList(queue, 2);
	struct data data[num_threads];
	pthread_t thread[num_threads];
	
	pthread_mutex_init(&mutex_primes, NULL);
	
    for(int i = 0; i < num_threads; i++) {
	    
	    /*max number to find primes until - generally UINT_MAX*/
	    data[i].max = max;
	    data[i].thread_id = i;
		    
		if(pthread_create(&thread[i], NULL, sieve, (void*)&data[i]) != 0) {
			perror("Couldnt create thread");
			exit(EXIT_FAILURE);
		}
    }
	
    for(int i = 0; i < num_threads; i++)
	    pthread_join(thread[i], NULL);

	char *output_file = argv[2];
	FILE *file = fopen(output_file, "w");

	//code to print to a file 'output_file'
	//uncomment lines 79-84 for file output of happy and sad properties of primes		
	printf("Printing to %s\n", output_file);
	for(unsigned long i = 0; i < max; i++) {
		if(bitmap[i] == 0){
			fprintf(file, "%ld\n", i);
			
			/* if(is_happy((int)i) == 1){ */
			/* 	fprintf(file, "%s\n", "happy"); */
			/* } */
			/* else{ */
			/* 	fprintf(file, "%s\n", "sad"); */
			/* } */
		}
	}

	printf("Done Printing\n");
	pthread_mutex_destroy(&mutex_primes);
}
