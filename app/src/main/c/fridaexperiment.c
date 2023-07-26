#include <android/log.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>


#define LOG(x) \
     __android_log_print(ANDROID_LOG_VERBOSE, "Example", "%s", x)
#define MAX_LEN 512

const char* statusPath = "/proc/self/status";

int detectDebugger();
void crashApp();
ssize_t readLine(int fd, char* buff, int max_len);
int

__attribute((constructor()))
void fridaCheck() {
    int status = -1;
    LOG("Starting frida check");

    LOG("Starting debug check");
    status = detectDebugger();

    if(status == -1) {
        crashApp();
    }
}

void crashApp() {
    LOG("Crashing the app");
    int* invalidAddress = (int*)0x3421ffa1;
    *invalidAddress = 0;
}

int detectDebugger() {
    char buf[MAX_LEN];
    int status = openat(0, statusPath, 0);
    while(readLine(status, buf, MAX_LEN) > 0) {
        LOG(buf);
    }
    return 0;
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