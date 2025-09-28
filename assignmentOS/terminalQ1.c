#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#define MAX_TOKENS 128

int main(void) {

    char* line = NULL;
    size_t len = 0; //size or len of the actual buffer
    ssize_t nread; //s represents signed size_t (can be negative), return type of getline(). no. of chars typed/read

    while(1 == 1){
        printf("gash> ");
        fflush(stdout); //optimization technique, not important

        nread = getline(&line, &len, stdin);
        if (nread == -1) { //ctrl + D         
            printf("\n");
            break;
        }

        // strip trailing newline
        if (nread > 0 && line[nread-1] == '\n') line[nread-1] = '\0';

        // skip empty lines
        char* p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') continue;

        // exit builtin
        if (strcmp(p, "exit") == 0) break;

        // tokenise into argv style array
        char* argv[MAX_TOKENS];
        int argc = 0;
        char* tok = strtok(p, " \t"); //actual tokenization, strtok

        while (tok != NULL && argc < MAX_TOKENS-1) {
            argv[argc++] = tok;
            tok = strtok(NULL, " \t");
        }
        argv[argc] = NULL; // argv must be NULL-terminated

        //process forking
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            continue;
        } 
        
        else if (pid == 0) {

            //execute vector with path search
            execvp(argv[0], argv);

            // If execvp returns, an error occurred
            fprintf(stderr, "execvp(%s) failed: %s\n", argv[0], strerror(errno));
            _exit(127);
        } 
        
        else {

            // parent: wait for child to finish to avoid zombies/ABANDONMENT!
            int status;
            pid_t w = waitpid(pid, &status, 0);

            if (w == -1) {
                perror("waitpid");
            } 
            
            else {

                if (WIFEXITED(status)) {
                    printf("child %d exited with status %d\n", pid, WEXITSTATUS(status));
                } 
                
                else if (WIFSIGNALED(status)) {
                    printf("child %d killed by signal %d\n", pid, WTERMSIG(status));
                } 
                
                else {
                    printf("child %d ended (status 0x%x)\n", pid, status);
                }
            }
        }
    }

    free(line);
    return 0;
}
