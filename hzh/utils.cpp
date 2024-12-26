#include "utils.h"


namespace hzh {
    


uint64_t GetCurrentMS() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000ul  + tv.tv_usec / 1000;
}

uint64_t GetCurrentUS() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 * 1000ul  + tv.tv_usec;
}

}