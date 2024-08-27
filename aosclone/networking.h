#pragma once

extern bool aos_network_connected;

long aos_network_init();

long aos_network_connect(const char *);

long aos_network_service();
void aos_network_send(unsigned char packetID, const void *data, size_t size, bool reliable);

long aos_network_get_ping();

void aos_network_disconnect();

void aos_network_uninit();