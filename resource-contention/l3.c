#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sched.h>

#include <omp.h>
#include <inttypes.h>

#define USEC 1000000 // 1s

// returns the size of the L3 cache in bytes, otherwise -1 on error.
// if there're two NUMA nodes, then the return value is only for one -Haoran
// e.g. in dvorak-2-3, cpu 0-13,28-41 share one L3 cache; cpu 14-27,42-55 share the other one; but the return value is only one of two L3 caches -Haoran
int cache_size() {
	// return the cache size from cpu 0
	// assuming that all cpu caches having the same size
	const char* cache_size_path = "/sys/devices/system/cpu/cpu0/cache/index3/size";

	FILE *cache_size_fd;
	if (!(cache_size_fd = fopen(cache_size_path, "r"))) {
		perror("could not open cache size file");
		return -1;
	}

	char line[512];
	if(!fgets(line, 512, cache_size_fd)) {
		fclose(cache_size_fd);
		perror("could not read from cache size file");
		return -1;
	}

	// strip newline
	const int newline_pos = strlen(line) - 1;
	if (line[newline_pos] == '\n') {
		line[newline_pos] = '\0';
	}

	// get multiplier
	int multiplier = 1;
	const int multiplier_pos = newline_pos - 1;
	switch (line[multiplier_pos]) {
		case 'K':
			multiplier = 1024;
		break;
		case 'M':
			multiplier = 1024 * 1024;
		break;
		case 'G':
			multiplier = 1024 * 1024 * 1024;
		break;
	}

	// remove multiplier
	if (multiplier != 1) {
		line[multiplier_pos] = '\0';
	}

	// line should now be clear of non-numeric characters
	int value = atoi(line);
	int cache_size = value * multiplier;
	
	fclose(cache_size_fd);
	return cache_size;
}

int main(int argc, char **argv) {
	char* volatile block;
  
	// Usage: ./l3 <duration in sec>
	if (argc < 3) { 
		printf("Usage: ./l3 <duration in sec> <intensity>\n"); 
		exit(0); 
	}

	int CACHE_SIZE = cache_size(); 
	printf("Detected L3 cache size: %d bytes\n", CACHE_SIZE);

	uint32_t maxThreads = omp_get_num_procs();
	double intensity = (atoi(argv[2])) / 10.0;
	printf("Intensity: %f\n", intensity);
	maxThreads = maxThreads*intensity;
	if (intensity == 0) {
		maxThreads = 2;
	}
	omp_set_num_threads(maxThreads);
	printf("Running %" PRIu32 " threads\n", maxThreads);
#pragma omp parallel
	{

	// create a mapping to memory	
	block = (char*)mmap(NULL, CACHE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
	if (block == (char *) -1) {
		perror("error - cannot mmap");
		exit(1);
	};
	
	int usr_timer = atoi(argv[1]);
	double time_spent = 0.0; 
  	clock_t begin, end;

	while (time_spent < usr_timer) {
  		begin = clock();
		memcpy(block, block+CACHE_SIZE/2, CACHE_SIZE/2);
		// memcpy(block, block+CACHE_SIZE, CACHE_SIZE);i - segmentation fault
		// memcpy(block+CACHE_SIZE*3/4, block, CACHE_SIZE*3/4); - segmentation fault
		// note: replaced original throttling sleep with yielding that gives other threads chance to run their workloads
		// sched_yield() causes the calling thread to relinquish the CPU.
		// The thread is moved to the end of the queue for its static priority and a new thread gets to run.
		// sched_yield(); // sleep((float)(usr_timer-time_spent)/usr_timer);
		end = clock();
  		time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
		// printf("Time spent: %f\n", time_spent);

		// intensity adjuster
		// usleep(USEC*intensity);
	}
	} // #pragma omp parallel
	return 0;
}
