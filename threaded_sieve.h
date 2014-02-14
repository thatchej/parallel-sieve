#define _POSIX_C_SOURCE 200809L

#include "linked_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <unistd.h>

struct data {
	unsigned long max;
	int thread_id;
	unsigned long cur_prime;
};

pthread_mutex_t mutex_primes;
unsigned char bitmap[UINT_MAX];
struct linkedList *queue;
unsigned long done = 0;

int is_happy(int cur_num) {

	int digit = 0;
	
	//10 digits in one billion
	int number[10];
	for(int i = 0; i < 10; i++) {
		number[i] = 0;
	}
	int square;
	int ten = 10;
	
	while((cur_num != 4) && (cur_num != 1)) {
		for(digit = 0; digit < 10; digit++) {
			number[digit] = cur_num % ten;
			cur_num = cur_num / ten;
		}
		
		cur_num = 0;
		for(digit = 0; digit < 10; digit++) {
			square = (number[digit] * number[digit]);
			cur_num += square;
		}
	}
	
	if(cur_num == 1) return 1;
	else return 0;
	
}

void mark_multiples(unsigned char bitmap[], unsigned long start, unsigned long end) {
	
	unsigned long i = 2, num;
	while((num = i*start) <= end) {
		bitmap[num] = 1;
		++i;
	}
}

int is_prime(unsigned long n) {
	if(n == 4) return 0;
	unsigned long i;
	for(i = 2; i*i<n; i++) {
		if (n % i == 0) return 0;
	}
	return 1;
}

void *sieve(void *args) { 
	//every thread will run this
   
	struct data *data = (struct data*)args;
	unsigned long max = data->max;
	int thread_id = data->thread_id;
	unsigned long cur_prime;
	unsigned long to_queue;

	printf("Thread %d sieving\n", thread_id);
	
	while(!done) {
		//atomically grabs and removes a prime from the queue
		pthread_mutex_lock(&mutex_primes);	
		cur_prime = frontList(queue);
		
		if(cur_prime * cur_prime > max) {
			done = 1;
			printf("Thread %d exiting\n", thread_id);
			pthread_exit((void *) 0);
			return NULL;
		}
	
		removeFrontList(queue);
	
		//finds next number needed to go onto the queue
		to_queue = cur_prime+1;
		while(!is_prime(to_queue)) to_queue++;
		addFrontList(queue, to_queue);
	
		pthread_mutex_unlock(&mutex_primes);
		if(cur_prime)
		mark_multiples(bitmap, cur_prime, max);
	}
	return NULL;
} 




