#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/select.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

double calc_time_diff_ms(struct timeval *start, struct timeval *end){
    double start_ms = (start->tv_sec * 1000.0) + (start->tv_usec / 1000.0);
    double end_ms = (end->tv_sec * 1000.0) + (end->tv_usec / 1000.0);
    return end_ms - start_ms;
}

void ping(struct in_addr* dst) {
    struct icmphdr icmp_hdr;
    struct sockaddr_in addr;
    char dst_string[INET_ADDRSTRLEN];
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

    inet_ntop(AF_INET, &(addr.sin_addr), dst_string, INET_ADDRSTRLEN);

    for (;;) {
        unsigned char data[2048];
        double rtt;
        int rc;
        struct timeval timeout = {3, 0}, start, end;
        fd_set read_set;
        socklen_t slen;
        struct icmphdr rcv_hdr;

        icmp_hdr.un.echo.sequence = sequence++;
        memcpy(data, &icmp_hdr, sizeof icmp_hdr);
        memcpy(data + sizeof icmp_hdr, "ping", 4);
        rc = sendto(sock, data, sizeof icmp_hdr + 4,
                    0, (struct sockaddr*)&addr, sizeof addr);
        gettimeofday(&start, NULL);

        if (rc <= 0) {
            perror("Sendto");
            break;
        }

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
            gettimeofday(&end, NULL);
            rtt = calc_time_diff_ms(&start, &end);
            printf("%d Bytes from %s, id=0x%x: sequence=%d, Response time: %.3fms\n",
                   rc,dst_string,icmp_hdr.un.echo.id, icmp_hdr.un.echo.sequence, rtt);
        } else {
            printf("Got ICMP packet with type 0x%x ?!?\n", rcv_hdr.type);
        }
        sleep(2);
    }
}