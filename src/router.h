/* IPXWrapper - Router header
 * Copyright (C) 2011 Daniel Collins <solemnwarning@solemnwarning.net>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef IPXWRAPPER_ROUTER_H
#define IPXWRAPPER_ROUTER_H

#include <windows.h>
#include <winsock2.h>
#include <wsipx.h>
#include <stdint.h>

#define MAX_ROUTER_CLIENTS 128

struct router_call {
	enum {
		rc_bind,
		rc_unbind,
		rc_port,
		rc_filter,
		rc_reuse
	} call;
	
	SOCKET sock;
	
	struct sockaddr_ipx arg_addr;
	int arg_int;
};

struct router_ret {
	int err_code;	/* ERROR_SUCCESS on success */
	
	struct sockaddr_ipx ret_addr;
	int ret_int;
};

/* Represents a bound IPX address */
struct router_addr {
	struct sockaddr_ipx addr;
	
	uint16_t local_port;	/* Local UDP port (NETWORK BYTE ORDER) */
	SOCKET ws_socket;	/* Application socket */
	SOCKET control_socket;	/* Control socket */
	int filter_ptype;	/* Packet type filter, negative to disable */
	BOOL reuse;		/* SO_REUSEADDR */
	
	struct router_addr *next;
};

struct router_client {
	SOCKET sock;
	
	struct router_call recvbuf;
	int recvbuf_len;
};

struct router_vars {
	BOOL running;
	
	struct ipx_interface *interfaces;
	
	SOCKET udp_sock;
	SOCKET listener;
	
	struct router_client clients[MAX_ROUTER_CLIENTS];
	int client_count;
	
	WSAEVENT wsa_event;
	
	CRITICAL_SECTION crit_sec;
	BOOL crit_sec_init;
	
	struct router_addr *addrs;
	
	char *recvbuf;
};

struct router_vars *router_init(BOOL global);
void router_destroy(struct router_vars *router);

DWORD router_main(void *arg);

int router_bind(struct router_vars *router, SOCKET control, SOCKET sock, struct sockaddr_ipx *addr, uint32_t *nic_bcast, BOOL reuse);
void router_set_port(struct router_vars *router, SOCKET control, SOCKET sock, uint16_t port);
void router_unbind(struct router_vars *router, SOCKET control, SOCKET sock);
void router_set_filter(struct router_vars *router, SOCKET control, SOCKET sock, int ptype);
int router_set_reuse(struct router_vars *router, SOCKET control, SOCKET sock, BOOL reuse);

#endif /* !IPXWRAPPER_ROUTER_H */
