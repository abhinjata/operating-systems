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
    long mtype;           // message type; must be > 0
    char mtext[MAX_TEXT]; // message text (null-terminated)
};

void caesar_encrypt(char *s) {
    for (; *s; ++s) {

        if ('A' <= *s && *s <= 'Z') {
            *s = ('A' + ( (*s - 'A' + 3) % 26 ));
        } 
        
        else if ('a' <= *s && *s <= 'z') {
            *s = ('a' + ( (*s - 'a' + 3) % 26 ));
        }
    }
}

int main(void) {

    key_t key = ftok(PATH_FOR_FTOK, PROJECT_ID);

    if (key == (key_t)-1) {
        perror("ftok");
        return 1;
    }

    // Create or get the message queue with rw for user/group/other
    int msgid = msgget(key, IPC_CREAT | 0666);
    if (msgid == -1) {
        perror("msgget");
        return 1;
    }

    struct msgbuf sbuf;
    sbuf.mtype = 1; //random positive type

    printf("Enter message to send (max %d bytes). Ctrl+D to quit:\n", MAX_TEXT-1);

    if (!fgets(sbuf.mtext, sizeof sbuf.mtext, stdin)) {
        //EOF or error reading — exit gracefully
        if (ferror(stdin)) perror("fgets");
        return 0;
    }

    //remove trailing newline if present
    size_t len = strlen(sbuf.mtext);
    if (len > 0 && sbuf.mtext[len-1] == '\n') sbuf.mtext[len-1] = '\0';

    //encrypt in place
    caesar_encrypt(sbuf.mtext);

    //send message
    if (msgsnd(msgid, &sbuf, strlen(sbuf.mtext) + 1, 0) == -1) {
        perror("msgsnd");
        return 1;
    }

    printf("Encrypted message sent.\n");
    return 0;
}
