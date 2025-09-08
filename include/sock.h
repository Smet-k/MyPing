#ifndef SOCK_H
#define SOCK_H

#include <netdb.h>
#include <netinet/ip_icmp.h>
#include <sys/socket.h>

#define PACKET_SIZE 64

int send_packet(const int sock, const struct icmphdr* icmph, const struct sockaddr_in* dstAddr);
int recv_packet(const int sock, char* packet, struct sockaddr_in* replyAddr);
int create_socket();
struct sockaddr_in resolve_host(const char* dst);

#endif