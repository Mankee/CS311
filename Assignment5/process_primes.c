#define _BSD_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <semaphore.h>

//#define TOTAL_BITS 1000000000
#define NUM_PROCESSES 10

#define BITMASK(b) (1 << ((b) % CHAR_BIT))
#define BITSLOT(b) ((b) / CHAR_BIT)
#define BITSET(a, b) ((a)[BITSLOT(b)] |= BITMASK(b))
#define BITCLEAR(a, b) ((a)[BITSLOT(b)] &= ~BITMASK(b))
#define BITTEST(a, b) ((a)[BITSLOT(b)] & BITMASK(b))
#define BITNSLOTS(nb) ((nb + CHAR_BIT - 1) / CHAR_BIT)

unsigned long long TOTAL_BITS = UINT32_MAX;

char *program_bitmap_array[NUM_PROCESSES];
unsigned char *bitmap_array;

struct process_args
	{
        uint64_t process_id;
        uint64_t start_bit;
        uint64_t end_bit;
	};

void mark_primes(struct process_args arg)
{
    uint64_t j;
    uint64_t k;
    uint64_t i;
    uint64_t counter = 0;
    uint64_t n;

    //struct thread_args *data = (struct thread_args *) arguments;
    n = ((arg.end_bit - arg.start_bit) + 1);

    //printf("This is process %lld working [%lld - %lld] n is %lld \n", arg.process_id, arg.start_bit, arg.end_bit, n);

    if(arg.start_bit == 1){
        arg.start_bit = 2;
    }

    for(i = arg.start_bit; i < arg.end_bit; i += 3) {
		if(!BITTEST(bitmap_array, i)){
			for(j = i * i; j < arg.end_bit; j += i)
				BITSET(bitmap_array, j);
				//printf("%lld is not prime\n", j);
		}
	}

    for(i = arg.start_bit; i < arg.end_bit; i++){
        if(!BITTEST(bitmap_array, i)){
            counter++;
            //printf("%lld is prime\n", i);
        }
    }

    //printf("number of primes for process %lld = %lld\n\n",arg.process_id, counter);
    exit(EXIT_SUCCESS);
}

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

	addr = mmap(NULL, object_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmem_fd, 0);
	if (addr == MAP_FAILED){
		fprintf(stdout, "failed to map shared memory object\n");
		exit(EXIT_FAILURE);
	}

	return addr;
}

int main (int argc, char *argv[])
{
    uint64_t thread_array_size = 0;
    uint64_t remainder = 0;
    uint64_t array_offset = 1;
    uint64_t err;
    uint64_t i;
    uint64_t j;
    uint64_t k;
    uint64_t total_num_primes = 0;
    double total_time;
    clock_t start;
    clock_t stop;
    pid_t pid[NUM_PROCESSES];

	int object_size = 1024 * 1024 * 600;

	void *addr = mount_shmem("/weila", object_size);

    bitmap_array = addr;
    memset(bitmap_array, 0, BITNSLOTS(TOTAL_BITS));

    struct process_args bitmap_data;

    thread_array_size = (BITNSLOTS(TOTAL_BITS) / NUM_PROCESSES);
    remainder = (BITNSLOTS(TOTAL_BITS) % NUM_PROCESSES);

    //printf("size of each thread = %lld\n", thread_array_size);
    //printf("size of remainder = %lld\n", remainder);
    //printf("total number chars = %d\n", BITNSLOTS(TOTAL_BITS));
    fprintf(stderr, "finding primes up to UINT32 please wait (aprox 109 seconds)...");
    start = clock();

    //marks all base prime numbers up to sqrt(TOTAL_BITS)
    for(i = 2; i < sqrt(TOTAL_BITS); i++) {
		if(!BITTEST(bitmap_array, i)) {
			for(j = i * i; j < TOTAL_BITS; j += i)
				BITSET(bitmap_array, j);
		}
	}

    for(i = 0; i < NUM_PROCESSES; i++){
        if(remainder > 0){
            //printf("(uneven)creating thread %d of size %d\n", i, (thread_array_size + 1));
            bitmap_data.start_bit = array_offset;
            array_offset += (thread_array_size + 1) * CHAR_BIT;
            bitmap_data.end_bit = array_offset - 1;
            remainder--;
        }
        else{
            //pass the size of the thread as an argument
            //printf("creating thread %d of size %d\n", i, (thread_array_size));
            bitmap_data.start_bit = array_offset;
            array_offset += thread_array_size * CHAR_BIT;
            bitmap_data.end_bit = array_offset - 1;
        }
        bitmap_data.process_id = i;

        switch(pid[i] = fork()){
        case -1:
            strerror(err);
        case 0:
            mark_primes(bitmap_data);
        default:
            break;
        }
    }

    stop = clock();

    for(i = 2; i < TOTAL_BITS; i++){
        if(!BITTEST(bitmap_array, i)){
            total_num_primes += 1;
            //printf("%lld is prime\n", i);
        }
    }

    total_time = (double)(stop - start) / (double)CLOCKS_PER_SEC;

    printf("Found %lld primes in %f seconds\n", total_num_primes - 1, total_time);
    printf("Main: program completed. Exiting.\n");

    while(wait(NULL) != -1);
    shm_unlink("/weila");
    return 0;
}
