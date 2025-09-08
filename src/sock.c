#define _POSIX_C_SOURCE 200112L

#include "sock.h"

#include <stdio.h>

int send_packet(const int sock, const struct icmphdr* icmph, const struct sockaddr_in* dstAddr){
    if(!icmph || !dstAddr)
        return -1;
    
    int numbytes = sendto(sock, icmph, sizeof(*icmph), 0,
                        (struct sockaddr*)dstAddr, sizeof(*dstAddr));

    if(numbytes < 0)
        return -1;

    return 0;
}

int recv_packet(const int sock, char* packet, struct sockaddr_in* replyAddr) {
    if(!packet || !replyAddr)
        return -1;
    
    socklen_t addrLen = sizeof(*replyAddr);
    return recvfrom(sock, packet, PACKET_SIZE, 0, 
                    (struct sockaddr*)replyAddr, &addrLen);
    
    return 0;
}

int create_socket(){
    const int sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if(sock == -1)
        perror("Socket creation failed");

    return sock;
}

struct sockaddr_in resolve_host(const char* dst){
    if(!dst) {
        perror("Provided NULL dst to resolve");
        struct sockaddr_in addr = {0};
        return addr;
    }

    struct addrinfo hints = {0}, *result;

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    if(getaddrinfo(dst, NULL, &hints, &result) != 0)
        perror("Failed to resolve host");

    const struct sockaddr_in destAddr = *(struct sockaddr_in*)result->ai_addr;
    freeaddrinfo(result);

    return destAddr;
}