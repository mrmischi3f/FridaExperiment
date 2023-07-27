#include <android/log.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <threads.h>


#define LOG(x) \
     __android_log_print(ANDROID_LOG_VERBOSE, "Example", "%s", x)

#define MAX_LEN 512

const char* statusPath = "/proc/self/status";
const char* tracerPID = "TracerPid";

void detectDebugger();
void crashApp();
ssize_t readLine(int fd, char* buff, int max_len);
int checkTracerPid(char* buf);

__attribute__((constructor))
void fridaCheck() {
    LOG("Starting frida check");

    LOG("Starting debug thread");

    pthread_t debug;
    pthread_create(&debug, NULL, (void*)detectDebugger, NULL);
}

void crashApp() {
    LOG("Crashing the app");
    int* invalidAddress = (int*)0x3421ffa1;
    *invalidAddress = 0;
}

void detectDebugger() {
    LOG("Starting debug thread");
    struct timespec timereq;
    timereq.tv_sec = 5; //Changing to 5 seconds from 1 second
    timereq.tv_nsec = 0;

    while(1) {
        char buf[MAX_LEN];
        int status = openat(0, statusPath, 0);
        while (readLine(status, buf, MAX_LEN) > 0) {
            if (checkTracerPid(buf)) {
                close(status);
                crashApp();
            }
        }
        close(status);
        nanosleep(&timereq, NULL);
    }
}

ssize_t readLine(int fd, char* buff, int max_len) {
    char c;
    ssize_t ret;
    ssize_t bytes_read = 0;

    memset(buff, 0, MAX_LEN);

    do {
        ret = read(fd, &c, 1);
        if(ret != 1) {
            if(bytes_read == 0) {
                return -1;
            } else {
                return bytes_read;
            }
        }
        if(c == '\n') {
            return bytes_read;
        }
        *(buff++) = c;
        bytes_read++;
    } while(c < max_len - 1);

    return ret;
}

int checkTracerPid(char* buf) {
    const char* tracer = strstr(buf, tracerPID);
    if(tracer == NULL) {
        return 0;
    }
    char* c_pid;
    strtok_r(buf, ":", &c_pid);
    int pid = atoi(c_pid);
    if(pid != 0) {
        return -1;
    }
    return 0;
}