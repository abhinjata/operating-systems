#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

#define PROJECT_ID 'M'
#define PATH_FOR_FTOK "/tmp"
#define MAX_TEXT 512

struct msgbuf {
    long mtype;
    char mtext[MAX_TEXT];
};

// Caesar decrypt: shift letters by -3 (wrap inside A-Z and a-z). 
void caesar_decrypt(char *s) {

    for (; *s; ++s) {

        if ('A' <= *s && *s <= 'Z') {

            *s = (char)('A' + ( ( (*s - 'A' - 3) % 26 + 26 ) % 26 ));
        } 
        
        else if ('a' <= *s && *s <= 'z') {

            *s = (char)('a' + ( ( (*s - 'a' - 3) % 26 + 26 ) % 26 ));

        }
    }
}

int main(void){

    key_t key = ftok(PATH_FOR_FTOK, PROJECT_ID);
    if (key == (key_t)-1){
        perror("ftok");
        return 1;
    }

    // Get (or create) the queue. The sender also uses IPC_CREAT, so they match.
    int msgid = msgget(key, IPC_CREAT | 0666);
    if (msgid == -1) {
        perror("msgget");
        return 1;
    }

    printf("Receiver: waiting for messages...\n");
    struct msgbuf rbuf;

    for (;;) {
        ssize_t r = msgrcv(msgid, &rbuf, sizeof rbuf.mtext, 0, 0);
        if (r == -1) {
            if (errno == EINTR) continue; // interrupted by signal, try again 
            perror("msgrcv");
            break;
        }

        //Ensure NUL termination (msgrcv returns number of bytes placed in mtext) 
        if (r >= (ssize_t)sizeof rbuf.mtext) rbuf.mtext[sizeof rbuf.mtext - 1] = '\0';
        else rbuf.mtext[r] = '\0';

        //decrypt, then print
        caesar_decrypt(rbuf.mtext);
        printf("Received (decrypted): %s\n", rbuf.mtext);

        //If the special message arrived, remove the queue and exit
        if (strcmp(rbuf.mtext, "Over and out") == 0) {
            printf("Received shutdown message. Removing message queue and exiting.\n");
            if (msgctl(msgid, IPC_RMID, NULL) == -1) {
                perror("msgctl(IPC_RMID)");
                return 1;
            }
            return 0;
        }
    }

    return 0;
}
