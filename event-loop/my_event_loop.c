#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <poll.h>
#include <time.h>


#define POLL_TIMEOUT    (3000)
#define BUF_SIZE        (256)
#define EXIT_COMMAND    exit

#define PROGRAM_START_MSG "\e[34mPROGRAM STARTED\e[0m\n"
#define PROGRAM_END_MSG "\e[34mPROGRAM END\e[0m\n"
#define PRCSTDIN_CTRLD_EXIT_MSG \
    "\e[31m" \
    "ctrl+d detected, program will now exit" \
    "\e[0m\n"
#define PRCSTDIN_EXIT_IN_INPUT_MSG \
    "\e[31m" \
    "string 'exit' detected within the first 4 bytes of input," \
    "program will now exit" \
    "\e[0m\n"
#define HEARTBEAT_MSG \
    "\e[33m" \
    "[HEARTBEAT]" \
    "\e[0m" \
    ": poll timeout\n"
#define POLLERR_MSG \
    "\e[30m" \
    "[ERROR]" \
    "\e[0m" \
    "there was an error polling\n"

/**
global flag
    indicates that the program is done & will terminate from the main loop
*/
int pDone = 0;


/**
time helper getting current monotonic time value.
*/
int monotime() {
    int clockr;
    struct timespec t;

    for (;;) {
        clockr = clock_gettime(CLOCK_MONOTONIC, &t);
        if (clockr == -1)
            goto error;
        return ((int32_t)t.tv_sec * 1000 + t.tv_nsec / 1000000);
    }

error:
    return -1;
}


typedef enum procStdinCode {
    /** OK or no errors */
    PRCSTDIN_OK     = 0,
    /** read error */
    PRCSTDIN_RERR   = 1 
} ProcStdinCode ;


/**
process the stdin
    this function will take a file descirptor & read the contents
    into a buffer & then echo that to stdout via `printf`, with some minimal
    formatting applied.

    if stdin is 4 bytes it will check if the bytes equal the pattern 'exit' &
    set the `pDone` global flag to have the value 1
*/
static ProcStdinCode _processStdin(
    int fd 
) {
    ProcStdinCode errcode;  // return this in the error goto
    int stdinrr;            // result of reading from stdin
    char* buf;              // buffer for stdin contents
    
    buf = (char*)calloc(BUF_SIZE, sizeof(char));
    stdinrr = read(fd, buf, sizeof(char) * BUF_SIZE - 1);

    if (stdinrr < 0) {
        // case 1 error, error while reading, so state this as the code
        errcode = PRCSTDIN_RERR;
        goto error;

    } else if (stdinrr == 0) {
        // case 2 EOF detected
        printf(PRCSTDIN_CTRLD_EXIT_MSG);
        pDone = 1;

    } else if (stdinrr > 0) {
        // case 3 some number of bytes were successfully read
        // check that the input buffer begins with sequence 'exit'
        // set the program done, or `pDone` flag
        if (strncmp(buf, "exit", 4) == 0) {
            printf(PRCSTDIN_EXIT_IN_INPUT_MSG);
            pDone = 1;
        } else {
            // use byte number to set bytes read + 1 as null terminator
            // character
            buf[stdinrr] = '\0';
            
            // write the buf to stdout
            printf("\e[32m[ECHO]\e[0m: %s\n", buf);
        }
    }    
    
    free(buf);
    return PRCSTDIN_OK;

error:
    // return from the function w/ a non-zero exit code
    free(buf);
    return errcode;
}


int main() 
{
    printf(PROGRAM_START_MSG);


    struct pollfd s = {
        .fd     = STDIN_FILENO,
        .events = POLLIN,
        .revents = 0
    };
    
    pDone = 0;  // [WO] reset the pDone flag before entering the main loop

    for (;;) {
        // [WO] check the value of pDone flag before the iteration
        // if the flag is set exit
        if (pDone) break;
        
        // [WO] the rest of the main loop is dealing with the polling cycle
        int pollRes = poll(&s, 1, POLL_TIMEOUT);
        if (pollRes > 0) {

            if (s.revents & (POLLERR | POLLHUP)) {
                // [WO] do these return events apply to file descriptors or just sockets?
                // for now do nothing
            }
            if (s.revents & POLLIN) {
                if (_processStdin(s.fd) != PRCSTDIN_OK) {
                    goto error;
                }
            }

        } else if (pollRes < 0) {
            printf(POLLERR_MSG);
            return 1;

        } else {
            printf(HEARTBEAT_MSG);
        }
    }


    printf(PROGRAM_END_MSG);


    return 0;


error:
    printf("ERROR EXITING PROGRAM W/ STATUS CODE 1");
    return 1;
}

