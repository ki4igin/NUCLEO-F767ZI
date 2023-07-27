#include "udp_server.h"

#include "udp.h"
#include "servo.h"

#define UDP_SERVER_PORT_CMD 2020
#define UDP_CLIENT_PORT_CMD 2021

static struct udp_pcb *pcb_cmd_s;
static struct udp_pcb *pcb_cmd_c;
static struct ip4_addr ip;

struct cmd_deg {
    uint32_t id;
    uint32_t deg;
};

extern struct servo servo;
static void recv_callback(
    void *arg,
    struct udp_pcb *pcb,
    struct pbuf *p,
    const ip_addr_t *addr,
    u16_t port)

{
    (void)arg;
    (void)pcb;
    (void)addr;
    (void)port;
    // udp_send(pcb_cmd_c, p);

    struct cmd_deg *cmd_deg = p->payload;
    servo_move(&servo, cmd_deg->deg);

    pbuf_free(p);
}

void udp_server_init(void)
{
    pcb_cmd_s = udp_new();
    pcb_cmd_c = udp_new();

    ipaddr_aton("192.168.0.10", &ip);

    udp_bind(pcb_cmd_s, &ip, UDP_SERVER_PORT_CMD);
    udp_recv(pcb_cmd_s, recv_callback, NULL);

    ipaddr_aton("192.168.0.6", &ip);
    udp_connect(pcb_cmd_c, &ip, UDP_CLIENT_PORT_CMD);
}
