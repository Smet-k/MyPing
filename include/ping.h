#pragma once

#include <arpa/inet.h>

void delay(int secs);
void ping(struct in_addr* dst);