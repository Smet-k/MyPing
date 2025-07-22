#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <time.h>
#include <sys/select.h>
#include <stdio.h>
#include <string.h>

void delay(int secs){
    unsigned int retTime = time(0) + secs;
    while (time(0) < retTime);
}

void ping(struct in_addr* dst) {
    struct icmphdr icmp_hdr;
    struct sockaddr_in addr;
    int sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_ICMP);
    int sequence = 0;
    if (sock < 0) {
        perror("socket");
        return;
    }

    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_addr = *dst;

    memset(&icmp_hdr, 0, sizeof icmp_hdr);
    icmp_hdr.type = ICMP_ECHO;
    icmp_hdr.un.echo.id = 1234;
    for (;;) {
        unsigned char data[2048];
        int rc;
        struct timeval timeout = {3, 0};
        fd_set read_set;
        socklen_t slen;
        struct icmphdr rcv_hdr;

        icmp_hdr.un.echo.sequence = sequence++;
        memcpy(data, &icmp_hdr, sizeof icmp_hdr);
        memcpy(data + sizeof icmp_hdr, "ping", 4);
        rc = sendto(sock, data, sizeof icmp_hdr + 4,
                    0, (struct sockaddr*)&addr, sizeof addr);
        if (rc <= 0) {
            perror("Sendto");
            break;
        }
        puts("Sent ICMP\n");

        memset(&read_set, 0, sizeof read_set);
        FD_SET(sock, &read_set);

        // Waiting for reply
        rc = select(sock + 1, &read_set, NULL, NULL, &timeout);
        if (rc == 0) {
            puts("Got no reply\n");
            continue;
        } else if (rc < 0) {
            perror("Select");
            break;
        }

        slen = 0;
        rc = recvfrom(sock, data, sizeof data, 0, NULL, &slen);
        if (rc <= 0) {
            perror("recvfrom");
            break;
        } else if (rc < sizeof rcv_hdr) {
            printf("Error, got short ICMP packet, %d bytes\n", rc);
            break;
        }
        memcpy(&rcv_hdr, data, sizeof rcv_hdr);
        if (rcv_hdr.type == ICMP_ECHOREPLY) {
            printf("ICMP Reply, id=0x%x, sequence = 0x%x\n",
                   icmp_hdr.un.echo.id, icmp_hdr.un.echo.sequence);
        } else {
            printf("Got ICMP packet with type 0x%x ?!?\n", rcv_hdr.type);
        }
        delay(2);
    }
}