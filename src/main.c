#include "ping.h"

#include <stdio.h>
#include <getopt.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>
static struct Options parse_options(const uint8_t argc, char* argv[]);

int main(int argc, char* argv[]) {
    const struct Options options = parse_options(argc, argv);
    
    ping(options);

    return 0;
}

static struct Options parse_options(const uint8_t argc, char* argv[]) {
    int currentOption;
    struct Options options = {
        .destination = NULL,
        .delay = 2
        };

    static struct option long_options[] = {
        {"delay", required_argument, NULL, 'd'},
        {"help", no_argument, NULL, 'h'}};

    while ((currentOption = getopt_long(argc, argv, "d:h", long_options, NULL)) != -1) {
        switch (currentOption) {
            case 'd':
                char* delayEndPointer;
                const long delay = strtol(optarg, &delayEndPointer, 10);

                if (*delayEndPointer != '\0') {
                    fprintf(stderr, "Invalid delay value: %s\n", optarg);
                    break;
                }

                if (delay < 0 || delay > UINT8_MAX) {
                    fprintf(stderr, "Delay is out of range(0-255): %s\n", optarg);
                    break;
                }

                options.delay = (uint8_t)delay;
                break;
            case 'h':
                printf("Usage: MyPing [-d delay] destination\n");
                exit(EXIT_SUCCESS);
                break;
            default:
                printf("Invalid argument. use -h for help.");
                break;
        }
    }

    if (optind == argc) {
        printf("Usage: MyPing [-d delay] destination\n");
        exit(EXIT_FAILURE);
    }
    options.destination = argv[optind];

    return options;
}