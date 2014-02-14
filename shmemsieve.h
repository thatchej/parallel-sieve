#define _POSIX_C_SOURCE 200809L

#include "linked_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>



struct data {
	unsigned int max;
	int pid;
	unsigned char *bitmap;
};


pthread_mutex_t mutex_primes;
struct linkedList *queue;
int done = 0;

void set_bit(unsigned char A[], long k) {
	A[k/8] |= 1 << (k%8);
}

void clear_bit(unsigned char A[], int k) {
	A[k/8] &= ~(1 << (k%32));
}

int get_bit(unsigned char A[], unsigned int k) {
	return ((A[k/8] & (1 << (k%8))) != 0);
}

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


void mark_multiples(unsigned char *bitmap, long start, unsigned int end) {
	
	long i = 2, num;
	while((num = i*start) <= end) {
		set_bit(bitmap, num); //bitmap[num] = 1;
		++i;
	}
}

int is_prime(unsigned long n) {
	if(n == 4) return 0;
	long i;
	for(i = 2; i*i<n; i++) {
		if (n % i == 0) return 0;
	}
	return 1;
}

void *sieve(void *args) { 
	//every process will run this
   
	struct data *data = (struct data*)args;
	unsigned int max = data->max;
	unsigned char *bitmap = data->bitmap;
	long cur_prime;
	long to_queue;
	int pid = data->pid;

	while(done != 1) {
		//atomically grabs and removes a prime from the queue
		
		
		pthread_mutex_lock(&mutex_primes);
		
		cur_prime = frontList(queue);
		
		if(cur_prime * cur_prime > max) {
			printf("Process %d exiting\n", pid);
			done = 1;
			exit(EXIT_SUCCESS);
			return NULL;
		}
	
		removeFrontList(queue);
	
		//finds next number needed to go onto the queue
		to_queue = cur_prime+1;
		while(!is_prime(to_queue)) to_queue++;
		addFrontList(queue, to_queue);
		pthread_mutex_unlock(&mutex_primes);
		
		//simple check to determine if cur_prime has picked up junk from the queue
		//stops this from becoming an infinite loop
		if(cur_prime){
			mark_multiples(bitmap, cur_prime, max);
		}
	}
	return NULL;
} 




