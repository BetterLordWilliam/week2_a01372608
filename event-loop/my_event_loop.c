#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <poll.h>
#include <time.h>


#define POLL_TIMEOUT        (3000)
#define MONOTIME_RETRY_MAX  (100)
#define BUF_SIZE            (256)
#define EXIT_COMMAND        exit

#define PROGRAM_START_MSG "\e[34mPROGRAM STARTED\e[0m\n"
#define PROGRAM_END_MSG "\e[34mPROGRAM END\e[0m\n"
#define PROGRAM_ERROR_EXIT_MSG "\e[31mERROR EXECUTING PROGRAM\e[0m\n"

#define PRCSTDIN_CTRLD_EXIT_MSG \
    "\e[31m"                                    \
    "ctrl+d detected, program will now exit"    \
    "\e[0m\n"

#define PRCSTDIN_EXIT_IN_INPUT_MSG \
    "\e[31m"                                                    \
    "string 'exit' detected within the first 4 bytes of input," \
    "program will now exit"                                     \
    "\e[0m\n"

#define ECHO_MSG \
    "\e[32m"    \
    "[ECHO]"    \
    "\e[0m"     \
    ": "

#define HEARTBEAT_MSG \
    "\e[33m"        \
    "[HEARTBEAT]"   \
    "\e[0m"         \
    ": poll timeout\n"

#define POLLERR_MSG \
    "\e[30m"    \
    "[ERROR]"   \
    "\e[0m"     \
    "there was an error polling\n"

#define POLL_INV_VAL_MSG \
    "\e[30m"    \
    "[ERROR]"   \
    "\e[0m"     \
    ": poll returned invalid value\n"


/**
global flag
    indicates that the program is done & will terminate from the main loop
*/
int pDone = 0;


/**
time helper getting current monotonic time value.
    returns 0 if successfully retrieves time, sets value of long r
    returns -1 if underlying `clock_gettime` calls fail & exceed retries
*/
int monotime(int64_t* r) {
    int clockr = 0, s = 0;
    struct timespec t;
    for (;;) {
        clockr = clock_gettime(CLOCK_MONOTONIC, &t);
        if (clockr == -1) {
            ++s; continue;
        }
        if (s == MONOTIME_RETRY_MAX)
            return -1;
        *r = (int64_t)t.tv_sec * 1000 + t.tv_nsec / 1000000;
        return 0;
    }
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
    ssize_t stdinrr;        // result of reading from stdin
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
        
        // insert the null terminator
        buf[stdinrr] = '\0';

        if (strncmp(buf, "exit", 4) == 0) {
            // check that the input buffer begins with sequence 'exit'
            // set the program done, or `pDone` flag
            printf(PRCSTDIN_EXIT_IN_INPUT_MSG);
            pDone = 1;

        } else {
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
    // start time (not necessary), running time, deadline time
    int64_t stime  = 0, rtime = 0, dead = 0, left = 0, wait = 0;
    int pollRes = 0;
    if (monotime(&stime) < 0)
        goto error;
    
    printf("%s\t%ld\n", PROGRAM_START_MSG, stime);

    struct pollfd s = {
        .fd     = STDIN_FILENO,
        .events = POLLIN,
        .revents = 0
    };
    
    pDone   = 0;  // [WO] reset the pDone flag before entering the main loop
    dead    = stime;
    dead    += POLL_TIMEOUT;

    for (;;) {
        // [WO] check the value of pDone flag before the iteration
        // if the flag is set exit
        if (pDone) break;
        // read current iteration time
        if (monotime(&rtime) < 0)
            goto error;

        // [WO] the rest of the main loop is dealing with the polling cycle
        // minimum poll time of 0, incase rtime has elapsed the deadline
        left = dead - rtime;
        wait = (left > 0) ? left : 0;
        pollRes = poll(&s, 1, (int)wait);

        // int pollRes = poll(&s, 1, dead - rtime);
        if (pollRes > 0) {
            if (s.revents & (POLLNVAL)) {
                // invalid value, error w/ the poll call
                // fd closed begin error processing
                printf(POLL_INV_VAL_MSG);
                goto error;
            }
            if (s.revents & (POLLERR | POLLHUP)) {
                // [WO] do these return events apply to file descriptors or just sockets?
                // for now do nothing
            }
            if (s.revents & POLLIN) {
                if (_processStdin(s.fd) != PRCSTDIN_OK) {
                    // `_processStdin` call failed begin error processing
                    goto error;
                }
            }

        } else if (pollRes < 0) {
            printf(POLLERR_MSG);
            goto error;
        }
        
        // compute next deadline while interval time is greater than
        // current deadline
        if (monotime(&rtime) < 0)
            goto error;
        if (rtime >= dead) {
            printf(HEARTBEAT_MSG);
            do { dead += POLL_TIMEOUT; }
            while (dead <= rtime);
        }
    }


    printf(PROGRAM_END_MSG);


    return 0;


error:
    printf(PROGRAM_ERROR_EXIT_MSG);
    return 1;
}

