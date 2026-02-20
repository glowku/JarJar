#ifndef JARJARVIS_NETWORK_H
#define JARJARVIS_NETWORK_H

#include <kernel/types.h>

#ifdef __cplusplus
extern "C" {
#endif

// Adresse MAC
typedef struct {
    u8 addr[6];
} mac_address_t;

// Adresse IP
typedef struct {
    u8 addr[4];
} ipv4_address_t;

// Adresse IPv6
typedef struct {
    u8 addr[16];
} ipv6_address_t;

// Types de paquets Ethernet
define ETHERTYPE_IP 0x0800
define ETHERTYPE_ARP 0x0806
define ETHERTYPE_IPV6 0x86DD

// Types ARP
define ARP_REQUEST 1
define ARP_REPLY 2

// Types IP
define IP_PROTOCOL_ICMP 1
define IP_PROTOCOL_TCP 6
define IP_PROTOCOL_UDP 17

// Ports communs
define PORT_HTTP 80
define PORT_HTTPS 443
define PORT_FTP 21
define PORT_SSH 22
define PORT_DNS 53
define PORT_DHCP_CLIENT 68
define PORT_DHCP_SERVER 67

// Structure de paquet Ethernet
typedef struct __packed {
    mac_address_t dest;
    mac_address_t src;
    u16 type;
    u8 data[];
} ethernet_frame_t;

// Structure ARP
typedef struct __packed {
    u16 hardware_type;
    u16 protocol_type;
    u8 hardware_size;
    u8 protocol_size;
    u16 opcode;
    mac_address_t sender_mac;
    ipv4_address_t sender_ip;
    mac_address_t target_mac;
    ipv4_address_t target_ip;
} arp_packet_t;

// Structure IP
typedef struct __packed {
    u8 version_ihl;
    u8 tos;
    u16 total_length;
    u16 identification;
    u16 flags_fragment;
    u8 ttl;
    u8 protocol;
    u16 checksum;
    ipv4_address_t src;
    ipv4_address_t dest;
    u8 data[];
} ipv4_packet_t;

// Structure ICMP
typedef struct __packed {
    u8 type;
    u8 code;
    u16 checksum;
    u16 identifier;
    u16 sequence;
    u8 data[];
} icmp_packet_t;

// Structure TCP
typedef struct __packed {
    u16 src_port;
    u16 dest_port;
    u32 sequence;
    u32 acknowledgment;
    u8 data_offset;
    u8 flags;
    u16 window;
    u16 checksum;
    u16 urgent;
    u8 data[];
} tcp_header_t;

// Flags TCP
define TCP_FLAG_FIN 0x01
define TCP_FLAG_SYN 0x02
define TCP_FLAG_RST 0x04
define TCP_FLAG_PSH 0x08
define TCP_FLAG_ACK 0x10
define TCP_FLAG_URG 0x20

// Structure UDP
typedef struct __packed {
    u16 src_port;
    u16 dest_port;
    u16 length;
    u16 checksum;
    u8 data[];
} udp_header_t;

// Structure de socket
typedef struct socket {
    s32 domain;
    s32 type;
    s32 protocol;
    ipv4_address_t local_addr;
    u16 local_port;
    ipv4_address_t remote_addr;
    u16 remote_port;
    s32 state;
    void* receive_buffer;
    s32 receive_buffer_size;
    s32 receive_buffer_head;
    s32 receive_buffer_tail;
    struct socket* next;
} socket_t;

// États TCP
typedef enum {
    TCP_STATE_CLOSED = 0,
    TCP_STATE_LISTEN,
    TCP_STATE_SYN_SENT,
    TCP_STATE_SYN_RECEIVED,
    TCP_STATE_ESTABLISHED,
    TCP_STATE_FIN_WAIT_1,
    TCP_STATE_FIN_WAIT_2,
    TCP_STATE_CLOSE_WAIT,
    TCP_STATE_CLOSING,
    TCP_STATE_LAST_ACK,
    TCP_STATE_TIME_WAIT,
} tcp_state_t;

// Types de socket
define SOCK_STREAM 1
define SOCK_DGRAM 2
define SOCK_RAW 3

// Domaines
define AF_INET 2
define AF_INET6 10

// Structure de carte réseau
typedef struct net_device {
    char name[32];
    mac_address_t mac;
    ipv4_address_t ip;
    ipv4_address_t netmask;
    ipv4_address_t gateway;
    ipv4_address_t dns;
    bool dhcp_enabled;
    bool link_up;
    u64 rx_packets;
    u64 tx_packets;
    u64 rx_bytes;
    u64 tx_bytes;
    u64 rx_errors;
    u64 tx_errors;
    
    // Fonctions du pilote
    bool (*init)(struct net_device* dev);
    void (*shutdown)(struct net_device* dev);
    bool (*send)(struct net_device* dev, void* data, size_t len);
    bool (*receive)(struct net_device* dev, void* buffer, size_t* len);
    void (*get_mac)(struct net_device* dev, mac_address_t* mac);
    bool (*set_mac)(struct net_device* dev, mac_address_t* mac);
    
    struct net_device* next;
} net_device_t;

// Structure de connexion TCP
typedef struct tcp_connection {
    ipv4_address_t local_addr;
    u16 local_port;
    ipv4_address_t remote_addr;
    u16 remote_port;
    tcp_state_t state;
    u32 sequence;
    u32 acknowledgment;
    u16 window;
    void* send_buffer;
    void* receive_buffer;
    struct tcp_connection* next;
} tcp_connection_t;

// Initialisation
void network_init(void);
void network_shutdown(void);

// Gestion des périphériques
void network_register_device(net_device_t* device);
void network_unregister_device(net_device_t* device);
net_device_t* network_get_default_device(void);
net_device_t* network_find_device(const char* name);

// Configuration
jar_result_t network_configure_interface(const char* name, ipv4_address_t ip, 
                                         ipv4_address_t netmask, ipv4_address_t gateway);
jar_result_t network_set_dns(ipv4_address_t dns);
jar_result_t network_enable_dhcp(const char* name);

// ARP
jar_result_t arp_resolve(ipv4_address_t ip, mac_address_t* mac);
void arp_insert(ipv4_address_t ip, mac_address_t mac);
void arp_remove(ipv4_address_t ip);

// IP
u16 ip_checksum(void* data, size_t len);
jar_result_t ip_send(ipv4_address_t dest, u8 protocol, void* data, size_t len);
jar_result_t ip_receive(ipv4_packet_t* packet, size_t len);

// ICMP
jar_result_t icmp_ping(ipv4_address_t dest, u32 timeout_ms);
void icmp_handle(icmp_packet_t* packet, size_t len, ipv4_address_t src);

// TCP
socket_t* tcp_socket(void);
jar_result_t tcp_bind(socket_t* sock, ipv4_address_t addr, u16 port);
jar_result_t tcp_listen(socket_t* sock, s32 backlog);
socket_t* tcp_accept(socket_t* sock);
jar_result_t tcp_connect(socket_t* sock, ipv4_address_t addr, u16 port);
ssize_t tcp_send(socket_t* sock, void* data, size_t len);
ssize_t tcp_receive(socket_t* sock, void* buffer, size_t len);
jar_result_t tcp_close(socket_t* sock);
void tcp_handle(ipv4_packet_t* packet, size_t len);

// UDP
socket_t* udp_socket(void);
jar_result_t udp_bind(socket_t* sock, ipv4_address_t addr, u16 port);
ssize_t udp_sendto(socket_t* sock, ipv4_address_t dest, u16 port, void* data, size_t len);
ssize_t udp_recvfrom(socket_t* sock, ipv4_address_t* src, u16* port, void* buffer, size_t len);
jar_result_t udp_close(socket_t* sock);
void udp_handle(ipv4_packet_t* packet, size_t len);

// Socket API
typedef struct {
    socket_t* (*socket)(s32 domain, s32 type, s32 protocol);
    jar_result_t (*bind)(socket_t* sock, void* addr, s32 addrlen);
    jar_result_t (*listen)(socket_t* sock, s32 backlog);
    socket_t* (*accept)(socket_t* sock, void* addr, s32* addrlen);
    jar_result_t (*connect)(socket_t* sock, void* addr, s32 addrlen);
    ssize_t (*send)(socket_t* sock, void* buf, size_t len, s32 flags);
    ssize_t (*recv)(socket_t* sock, void* buf, size_t len, s32 flags);
    jar_result_t (*close)(socket_t* sock);
} socket_api_t;

// HTTP
jar_result_t http_get(const char* url, void* buffer, size_t* len);
jar_result_t http_post(const char* url, void* data, size_t data_len, void* buffer, size_t* len);

// DNS
jar_result_t dns_resolve(const char* hostname, ipv4_address_t* ip);

// DHCP
jar_result_t dhcp_request(net_device_t* device);

// Statistiques
typedef struct {
    u64 rx_packets;
    u64 tx_packets;
    u64 rx_bytes;
    u64 tx_bytes;
    u64 rx_errors;
    u64 tx_errors;
    u64 dropped_packets;
} network_stats_t;

void network_get_stats(network_stats_t* stats);
void network_dump_stats(void);

// Utilitaires
void mac_to_string(mac_address_t* mac, char* str);
void ip_to_string(ipv4_address_t* ip, char* str);
bool string_to_ip(const char* str, ipv4_address_t* ip);
u32 ip_to_u32(ipv4_address_t* ip);
ipv4_address_t u32_to_ip(u32 ip);

#ifdef __cplusplus
}
#endif

#endif // JARJARVIS_NETWORK_H
