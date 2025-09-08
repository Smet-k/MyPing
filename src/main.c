#include <stdio.h>

#include "ping.h"

int main() {
    // TODO: parse arguments
    char* dst = "8.8.8.8";
    
    ping(dst);

    return 0;
}
