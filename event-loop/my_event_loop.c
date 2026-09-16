#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <poll.h>


#define POLL_TIMEOUT    (3000)
#define BUF_SIZE        (256)
#define EXIT_COMMAND    exit


int checkExit(const char* buf, size_t len)
{
    for (int i = 0; i < len; i++) {
        
    }
}

int main() 
{
    struct pollfd s = {
        .fd     = STDIN_FILENO,
        .events = POLLIN,
        .revents = 0
    };
    
    printf("PROGRAM STARTED\n");
    fflush(stdout);
    for (;;) {
        int pollRes = poll(&s, 1, POLL_TIMEOUT);

        if (pollRes > 0) {
            char buf[256];
            read(s.fd, buf, 256);

            printf("\e[32m[ECHO]\e[0m: %s\n", buf);
            // printf("\e[32m[ECHO]\e[0m:DUMMY\n");

        } else if (pollRes < 0) {
            printf("\e[30m[ERROR]\e[0mthere was an error polling\n");
            return 1;

        } else {
            printf("\e[33m[HEARTBEAT]\e[0m: poll timeout\n");
        }
    }
    printf("PROGRAM COMPLETE\n");

    return 0;
}

