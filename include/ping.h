#ifndef PING_H
#define PING_H

#include <stdint.h>

struct Options {
    char* destination;
    uint8_t delay;
};

void ping(const struct Options options);

#endif