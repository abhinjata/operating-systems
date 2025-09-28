#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define NUM_ITEMS 10

void consumer(int id, int read_fd) {
    int item;
    while (read(read_fd, &item, sizeof(item)) == sizeof(item)) {
        printf("Consumer %d got: %d\n", id, item);
    }
    close(read_fd);
    exit(0);
}

int main(void) {
    int fd[2];
    pipe(fd);

    if (fork() == 0) {
        close(fd[1]);
        consumer(1, fd[0]);
    }
    if (fork() == 0) {
        close(fd[1]);
        consumer(2, fd[0]);
    }

    // Producer
    close(fd[0]);
    for (int i = 1; i <= NUM_ITEMS; ++i) {
        write(fd[1], &i, sizeof(i));
        printf("Producer sent: %d\n", i);
    }
    close(fd[1]);

    // Wait for both consumers
    wait(NULL);
    wait(NULL);
    return 0;
}
