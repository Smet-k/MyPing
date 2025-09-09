#include "ping.h"
#include "sock.h"

#include <sys/time.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#define WORD_LENGTH_IN_BYTES 16

static double calc_time_diff_ms(struct timeval *start, struct timeval *end);
static int fill_header(struct icmphdr* icmph, const int seq);
static uint16_t calculate_checksum(void* buffer, uint16_t length);

void ping(const struct Options options) {
    if(!options.destination){
        perror("Received NULL dst");
        return;
    }
    int sock = create_socket();
    struct icmphdr icmph;
    struct sockaddr_in destAddr = resolve_host(options.destination);
    struct timeval startTime, endTime;
    int seq = 1;
    printf("Seq\tDestination\tTime\n");
    while(1){
        struct sockaddr_in replyAddr;
        char packet[PACKET_SIZE];

        if(fill_header(&icmph, seq++) < 0){
            perror("Failed to fill icmp header");
            continue;
        }

        gettimeofday(&startTime, NULL);
        if(send_packet(sock, &icmph, &destAddr) < 0)
            perror("Failed to send packet");

        if(recv_packet(sock, packet, &replyAddr) < 0)
            perror("Failed to receive packet");
            
        gettimeofday(&endTime, NULL);

        printf("%3d\t%-15s\t%3.fms\n",seq ,inet_ntoa(replyAddr.sin_addr), calc_time_diff_ms(&startTime, &endTime));
        sleep(options.delay);
    }
}



static int fill_header(struct icmphdr* icmph, const int seq){
    if(!icmph) {
        perror("ICMP header pointer is null");
        return -1;
    }

    memset(icmph, 0, sizeof(*icmph));

    icmph->type = ICMP_ECHO;
    icmph->code = 0;
    icmph->un.echo.id = (uint16_t)getpid();
    icmph->un.echo.sequence = seq;
    icmph->checksum = calculate_checksum(icmph, sizeof(icmph));
    return 0;
}



static uint16_t calculate_checksum(void* buffer, uint16_t length){
    if(!buffer) return 0;

    uint16_t* word_pointer = buffer;
    uint32_t sum = 0;

    for(sum = 0; length > 1; length -= 2) {
        sum += *word_pointer++;
    }

    if (length == 1) {
        sum += *(uint8_t*)word_pointer;
    }

    sum = (sum >> WORD_LENGTH_IN_BYTES) + (sum & 0xFFFF);
    sum += sum >> WORD_LENGTH_IN_BYTES;

    return (uint16_t)~sum;
}



static double calc_time_diff_ms(struct timeval *start, struct timeval *end){
    double start_ms = (start->tv_sec * 1000.0) + (start->tv_usec / 1000.0);
    double end_ms = (end->tv_sec * 1000.0) + (end->tv_usec / 1000.0);
    return end_ms - start_ms;
}