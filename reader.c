/**********************************************************
 *
 * reader.c
 * CIS 451 Lab 5
 *
 * !!!NAMES!!!
 * Connor Valley
 *************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h> 
#include <sys/shm.h>
#include <signal.h>
#include <sys/stat.h>

#define TEXT_SIZE 256
#define SHM_SIZE (sizeof(int) * 3 + TEXT_SIZE)

int shmId;
char *shmPtr;

void sigHandler(int sig) {
	printf("\nCaught signal %d, cleaning up...\n", sig);

	if(shmdt (shmPtr) < 0) { 
      perror ("Unable to detach\n"); 
      exit (1); 
   }

   exit(0);
}

int main() {

	key_t key = ftok("shmkey", 'a');

	if ((shmId = shmget(key, SHM_SIZE, 0)) < 0) {
		perror("Unable to access shared memory\n");
		exit(1);
	}

	if ((shmPtr = shmat(shmId, 0, 0)) == (void*) - 1) {
		perror("Unable to attach\n");
		exit(1);
	}

	signal(SIGINT, sigHandler);

	printf("Shared memory found and attached.\n");

	int *lock = (int*) shmPtr;
	int *reader1Done = (int*) (shmPtr + sizeof(int));
	int *reader2Done = (int*) (shmPtr + 2 * sizeof(int));
	char *text = (char*) (shmPtr + 3 * sizeof(int));

	int *flag = NULL;

	if (*reader1Done == 0) {
		flag = reader1Done;
		*reader1Done = -1;
		printf("I am reader 1\n");
	} else {
		flag = reader2Done;
		*reader2Done = -1;
		printf("I am reader 2\n");
	}

	while(1) {

		printf("Waiting on writer...\n");

		while(*lock == 0) {
			// wait for writer
		}

		printf("Reader reads: %s\n", text);

		*flag = 1;

		if ((*reader1Done == 1) && (*reader2Done == 1)) {
			*lock = 0;
		}

		while(*lock == 1) {
			// wait for other reader
		}
	}

}