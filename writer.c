/**********************************************************
 *
 * writer.c
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

// Catch exit signal and clean up shared memory space
void sigHandler(int sig) {
	printf("\nCaught signal %d, cleaning up...\n", sig);

	if(shmdt (shmPtr) < 0) { 
      perror ("Unable to detach\n"); 
      exit (1); 
   } 

   if(shmctl (shmId, IPC_RMID, 0) < 0) { 
      perror ("Unable to deallocate\n"); 
      exit(1); 
   }

   exit(0);
}

int main() {

	key_t key = ftok("shmkey", 'a');

	if ((shmId = shmget(key, SHM_SIZE, IPC_CREAT|S_IRUSR|S_IWUSR)) < 0) {
		perror("Unable to create shared memory\n");
		exit(1);
	}

	if ((shmPtr = shmat(shmId, 0, 0)) == (void*) - 1) {
		perror("Unable to attach\n");
		exit(1);
	}

	signal(SIGINT, sigHandler);

	int *lock = (int*) shmPtr;
	int *reader1Done = (int*) (shmPtr + sizeof(int));
	int *reader2Done = (int*) (shmPtr + 2 * sizeof(int));
	char *text = (char*) (shmPtr + 3 * sizeof(int));

	printf("Shared memory space created & attached.\n");

	*lock = 0;

	while(1) {

		while (*lock == 1) {
			// wait for reader
		}

		*reader1Done = 0;
		*reader2Done = 0;

		printf("No active readers...\n");

		printf("Enter text: ");

		fgets(text, TEXT_SIZE, stdin);

		printf("Text written to shared memory, setting lock...\n");

		*lock = 1;

	}

}