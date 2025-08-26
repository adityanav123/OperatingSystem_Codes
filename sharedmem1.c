#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SHARED_MEM_SZ 1024 // Bytes

int main(int argc, char *argv[]) {
    key_t key = 1234; // Shared Mem Key
    int shmid; // shared mem id
    char *sharedMem;

    // Creating a shared mem segment
    shmid = shmget(key, SHARED_MEM_SZ, IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget failed!");
        exit(1);
    }

    // Child Process
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed!");
        exit(1);
    }

    // Child process
    if (pid == 0) {
        //attaching shared mem
        sharedMem = (char*)shmat(shmid, NULL, 0);
        if (sharedMem == (char*)-1) {
            perror("shmat failed in child!");
            exit(1);
        }

        sleep(1);

        // reading from shared mem
        printf("Child Reads: %s\n", sharedMem);

        // Detaching Shared Mem
        if (shmdt(sharedMem) == -1) {
            perror("shmdt failed in child!");
            exit(1);
        }
    } else {
        // Parent
        sharedMem = (char*)shmat(shmid, NULL, 0);
        if (sharedMem == (char*)-1) {
            perror("shmat failed in parent!");
            exit(1);
        }

        // Writing in Shared Memory
        char *message = "Hello_from_parent!";
        strncpy(sharedMem, message, SHARED_MEM_SZ);

        wait(NULL); // wait for child process to read

        if (shmdt(sharedMem) == -1) {
            perror("shmdt failed in parent!");
            exit(1);
        }

        if (shmctl(shmid, IPC_RMID, NULL) == -1) {
            perror("shmctl IPC_RMID failed!");
            exit(1);
        }
    }

    return 0;
}
