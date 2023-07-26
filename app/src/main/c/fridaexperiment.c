#include <android/log.h>

#define LOG(x) \
     __android_log_print(ANDROID_LOG_VERBOSE, "Example", x)

__attribute((constructor()))
void greet() {
    LOG("Starting frida check");

    LOG("Starting debug check");
}

bool