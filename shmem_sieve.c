/* Name: Jaron Thatcher
 * Date: 11/27/2013
 * 
 * This program is the shared memory and process version of the parallelization  * of the Sieve of Eratosthenes. Compliant with POSIX 2008
 *
 */

#define _POSIX_C_SOURCE 200809L
#define _BSD_SOURCE

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <signal.h>
#include "shmemsieve.h"



void *mount_shmem(char *path, int object_size){
	int shmem_fd;
	void *addr;

	/* create and resize it */
	shmem_fd = shm_open(path, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	if (shmem_fd == -1){
		fprintf(stdout, "failed to open shared memory object\n");
		exit(EXIT_FAILURE);
	}
	/* resize it to something reasonable */
	if (ftruncate(shmem_fd, object_size) == -1){
		fprintf(stdout, "failed to resize shared memory object\n");
		exit(EXIT_FAILURE);
	}
	

	//0xdeadbeef
	addr = mmap(NULL, object_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmem_fd, 0);
	if (addr == MAP_FAILED){
		fprintf(stdout, "failed to map shared memory object\n");
		exit(EXIT_FAILURE);
	}


	return addr;
}

static void handle_signal(int signum) {
	psignal(signum, " Signal caught");
	exit(EXIT_FAILURE);
}

static void setup_signal_handlers() {
	struct sigaction s;
	memset(&s, 0, sizeof(s));
	s.sa_handler = handle_signal;
	sigaction(SIGINT, &s, NULL);
}

int main(int argc, char **argv){
	unsigned char *bitmap;
	int object_size = 1024 * 1024 * 512; /* 6 MB, since we need just over 4 for bit map */
	void *addr = mount_shmem("/thatchej", object_size);
	
	setup_signal_handlers();
	
	queue = createLinkedList();
	int num_procs = atoi(argv[1]);
	unsigned int max = UINT_MAX;

	/* store the bitmap (which we just use as bits) at the beginning */
	bitmap = addr;
	memset(bitmap, 0, sizeof(bitmap));
	set_bit(bitmap, 0);
	set_bit(bitmap, 1);

	//first prime is 2
	addFrontList(queue, 2);
	
	//store the struct used to pass into the sieving function
	struct data data[num_procs];

	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
	pthread_mutex_init(&mutex_primes, &attr);
	pthread_mutexattr_destroy(&attr);

	for(int i = 0; i < num_procs; i++) {
		data[i].max = max;
		data[i].bitmap = bitmap;
	}
	
	for(int i = 0; i < num_procs; i++) {
		int pid;
		
		switch((pid = fork())) {
		
		case -1:
			printf("Something wen't very wrong\n");
			break;
		case 0:
			//child case
			//calls the parallel sieving function for all children
			data[i].pid = getpid();
			sieve((void*)&data[i]);
		default:
			//parent case
			wait(NULL);
		}
	}
	
	char *output_file = argv[3];
	FILE *file = fopen(output_file, "w");
	printf("Unlinking Shared Memory\n");
	shm_unlink("/thatchej");	

	printf("Printing to %s\n", output_file);
	//prints the bitmap into a file - comment out for pure prime generation time
	for(long i = 2; i < max; i++) {
		if(get_bit(bitmap, i) == 0) {
			fprintf(file, "%ld\n", i); //PUT A NEWLINE HERE IF NOT PRINTING HAPPY NUMBERS

			//uncomment for happy generation
			/* if(is_happy((int)i) == 1){ */
			/* 	fprintf(file, "%s\n", "happy"); */
			/* } */
			/* else{ */
			/* 	fprintf(file, "%s\n", "sad"); */
			/* } */
		}
	}
	printf("Done printing\n");
	
	pthread_mutex_destroy(&mutex_primes);
	return 0;
}

