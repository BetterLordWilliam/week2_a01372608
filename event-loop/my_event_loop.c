#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <poll.h>


#define POLL_TIMEOUT    (3000)
#define BUF_SIZE        (256)
#define EXIT_COMMAND    exit


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
    for (;;) {
        if (pDone) break;

        int pollRes = poll(&s, 1, POLL_TIMEOUT);

        if (pollRes > 0) {
            char* buf = (char*)calloc(BUF_SIZE, sizeof(char));
            int by;
            
            // read up to a maximum of BUF_SIZE - 1
            // so that \0 can be injected later for safe printing            
            by = read(s.fd, buf, BUF_SIZE - 1);

            printf("read return: %d\n", by);

            // handle errors
            if (by < 0) {
                printf("READ FAILED");
                free(buf); // error occured but we still need to free this
                return 1;

            // read was successful, in sofar as it read something
            } else if (by > 0) {
                // printf("%c %c %c %c\n", buf[0], buf[1], buf[2], buf[3]);
            
                // check that the input buffer begins with sequence 'exit'
                // set the program done, or `pDone` flag
                if (    buf[0] == 'e' 
                        && buf[1] == 'x'
                        && buf[2] == 'i'
                        && buf[3] == 't'
                )
                    pDone = 1;

                // set the last thing to null terminator
                // safe to log with `printf`
                buf[by] = '\0';
                printf("\e[32m[ECHO]\e[0m: %s\n", buf);
                // printf("\e[32m[ECHO]\e[0m:DUMMY\n");
            } else {
                // read returned 0 bytes read
            }

            free(buf); // free buffer

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

