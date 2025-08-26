#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
    int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "fork failed!\n");
        exit(1);
    }
    else if (rc == 0) {
        // Redirect Standard Output to file
        close(STDOUT_FILENO); // Closing Standard Output
        open("./p4.outpu", O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);

        char *myArgs[3];
        myArgs[0] = strdup("wc");
        myArgs[1] = strdup("wait1.c");
        myArgs[2] = NULL;

        execvp(myArgs[0], myArgs);
    } else {
        int wc = wait(NULL);
    }
    return 0;
}
