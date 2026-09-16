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
    int pDone;
    struct pollfd s = {
        .fd     = STDIN_FILENO,
        .events = POLLIN,
        .revents = 0
    };
    
    pDone = 0; 
    printf("PROGRAM STARTED\n");
    fflush(stdout);
    for (;;) {
        if (pDone) break;

        int pollRes = poll(&s, 1, POLL_TIMEOUT);

        if (pollRes > 0) {
            char* buf = (char*)calloc(256, 1);
            int by;
            
            by = read(s.fd, buf, 256 - 1);

            // handle errors
            if (by < 0) {
                printf("READ FAILED");
                return 1;
            }

            if (by > 0) {
                // end of input
                // printf("READ NOTHING")
            } else {
                // end of input
                // printf("READ NORMAL")
            
                // check that the input buffer begins with sequence 'exit'
                if (    buf[0] == 'e' 
                        && buf[1] == 'x'
                        && buf[2] == 'i'
                        && buf[3] == 't') { pDone = 1; }

                // set the last thing to null terminator
                // safe to log with `printf`
                buf[by] = '\0';
            }

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

