/* master-brick
 * Copyright (C) 2012-2013 Olaf Lüke <olaf@tinkerforge.com>
 *
 * brickd.c: Simplistic reimplementation of brickd for WIFI Extension
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "brickd.h"

#include "config.h"

#include <stddef.h>

#include "bricklib/utility/sha_1.h"

#include "bricklib/logging/logging.h"
#include "extensions/ethernet/ethernet_config.h"
#include "extensions/ethernet/ethernet_low_level.h"

#define MAX_SOCKETS 16

BrickdRouting brickd_routing_table[BRICKD_ROUTING_TABLE_SIZE];

uint32_t brickd_counter = 0;
uint32_t brickd_authentication_nonce = 42; // Will be randomized

BrickdAuthenticationState brickd_authentication_state[MAX_SOCKETS] = {
	BRICKD_AUTHENTICATION_STATE_ENABLED
};

void brickd_init(void) {
	for(uint8_t i = 0; i < BRICKD_ROUTING_TABLE_SIZE; i++) {
		brickd_routing_table[i].uid = 0;
		brickd_routing_table[i].counter = 0;
		brickd_routing_table[i].func_id = 0;
		brickd_routing_table[i].seqence_number = 0;
		brickd_routing_table[i].cid = -1;
	}
}

uint32_t brickd_counter_diff(const uint32_t new, const uint32_t old) {
	if(new > old) {
		return new - old;
	}

	return new + (0xFFFFFFFF - old);
}

void brickd_route_from(const void *data, const uint8_t cid) {
	MessageHeader *data_header = (MessageHeader*)data;

	// Broadcast
	if(data_header->sequence_num == 0 || data_header->uid == 0 || !data_header->return_expected) {
		return;
	}

	BrickdRouting *smallest = &brickd_routing_table[0];
	brickd_counter++;
	uint32_t diff = 0;

	for(uint8_t i = 0; i < BRICKD_ROUTING_TABLE_SIZE; i++) {
		if(brickd_routing_table[i].cid == -1) {
			brickd_routing_table[i].uid = data_header->uid;
			brickd_routing_table[i].func_id = data_header->fid;
			brickd_routing_table[i].seqence_number = data_header->sequence_num;
			brickd_routing_table[i].cid = cid;
			brickd_routing_table[i].counter = brickd_counter;
			return;
		} else {
			uint32_t new_diff = brickd_counter_diff((*smallest).counter, brickd_routing_table[i].counter);
			if(new_diff > diff) {
				smallest = &brickd_routing_table[i];
				diff = new_diff;
			}
		}
	}

	smallest->uid = data_header->uid;
	smallest->func_id = data_header->fid;
	smallest->cid = cid;
	smallest->counter = brickd_counter;
}

int8_t brickd_route_to(const void *data) {
	MessageHeader *data_header = (MessageHeader*)data;

	// Broadcast
	if(data_header->sequence_num == 0 || data_header->uid == 0) {
		return -1;
	}

	BrickdRouting *current_match = NULL;
	uint32_t current_diff = 0;

	for(uint8_t i = 0; i < BRICKD_ROUTING_TABLE_SIZE; i++) {
		if(brickd_routing_table[i].uid == data_header->uid &&
		   brickd_routing_table[i].func_id == data_header->fid &&
		   brickd_routing_table[i].seqence_number == data_header->sequence_num) {
			uint32_t new_diff = brickd_counter_diff(current_match->counter, brickd_counter);
			if(new_diff > current_diff) {
				new_diff = current_diff;
				current_match = &brickd_routing_table[i];
			}
		}
	}

	if(current_match != NULL) {
		int8_t cid = current_match->cid;
		current_match->uid = 0;
		current_match->func_id = 0;
		current_match->seqence_number = 0;
		current_match->cid = -1;

		return cid;
	}

	return -1;
}

void brickd_disconnect(const uint8_t cid) {
	if(cid > 0 && cid < 16) {
		for(uint8_t i = 1; i < BRICKD_ROUTING_TABLE_SIZE; i++) {
			if(brickd_routing_table[i].cid == cid) {
				brickd_routing_table[i].cid = -1;
				brickd_routing_table[i].func_id = 0;
				brickd_routing_table[i].uid = 0;
				brickd_routing_table[i].seqence_number = 0;
			}
		}
	}
}

void brickd_disconnect_by_com_and_cid(const ComType com, int8_t cid) {
	if(com == COM_ETHERNET) {
		if(cid >= 0 && cid < 7) {
			ethernet_low_level_emergency_disconnect(cid);
		}
	} else if(com == COM_WIFI) {
		// TODO
	}
}

void brickd_set_authentication_seed(const uint32_t seed) {
	logethi("Using seed %u\n\r", seed);
	brickd_authentication_nonce = seed;
}

void brickd_enable_authentication(void) {
	for(uint8_t i = 0; i < MAX_SOCKETS; i++) {
		brickd_authentication_state[i] = BRICKD_AUTHENTICATION_STATE_ENABLED;
	}
}

void brickd_disable_authentication(void) {
	for(uint8_t i = 0; i < MAX_SOCKETS; i++) {
		brickd_authentication_state[i] = BRICKD_AUTHENTICATION_STATE_DISABLED;
	}
}

void brickd_get_authentication_nonce(const ComType com, const GetAuthenticationNonce *data) {
	int8_t cid = brickd_route_to(data);
	if(cid < 0 || cid > 15) {
		logexte("brickd_get_authentication_nonce: Invalid cid %d\n\r", cid);
		// TODO: What now?
		return;
	}

	BrickdAuthenticationState state = brickd_authentication_state[cid];
	switch(state) {
		case BRICKD_AUTHENTICATION_STATE_NONCE_SEND:
			brickd_authentication_state[cid] = BRICKD_AUTHENTICATION_STATE_ENABLED;
		case BRICKD_AUTHENTICATION_STATE_DISABLED: {
			logexte("Failure during authentication nonce request: %d -> %d\n\r", state, cid);
			brickd_disconnect_by_com_and_cid(com, cid);
			return;
		}

		case BRICKD_AUTHENTICATION_STATE_DONE:
		case BRICKD_AUTHENTICATION_STATE_ENABLED: {
			logexti("Authentication nonce requested %d\n\r", cid);
			break;
		}
	}

	GetAuthenticationNonceReturn ganr;
	ganr.header        = data->header;
	ganr.header.length = sizeof(GetAuthenticationNonceReturn);

	brickd_authentication_nonce++;

	memcpy(ganr.server_nonce, &brickd_authentication_nonce, 4);
	uint8_t length = send_blocking_with_timeout(&ganr, sizeof(GetAuthenticationNonceReturn), com);
	if(length != sizeof(GetAuthenticationNonceReturn)) {
		brickd_authentication_state[cid] = BRICKD_AUTHENTICATION_STATE_ENABLED;
	} else {
		brickd_authentication_state[cid] = BRICKD_AUTHENTICATION_STATE_NONCE_SEND;
	}
}

void brickd_authenticate(const ComType com, const Authenticate *data) {
	int8_t cid = brickd_route_to(data);
	if(cid < 0 || cid > 15) {
		logexte("brickd_authenticate: Invalid cid %d\n\r", cid);
		// TODO: What now?
		com_return_setter(com, data);
		return;
	}

	BrickdAuthenticationState state = brickd_authentication_state[cid];
	switch(state) {
		case BRICKD_AUTHENTICATION_STATE_DISABLED:
		case BRICKD_AUTHENTICATION_STATE_DONE:
			brickd_authentication_state[cid] = BRICKD_AUTHENTICATION_STATE_ENABLED;
		case BRICKD_AUTHENTICATION_STATE_ENABLED: {
			logexte("Failure during authentication request: %d -> %d\n\r", state, cid);
			brickd_disconnect_by_com_and_cid(com, cid);
			com_return_setter(com, data);
			return;
		}

		case BRICKD_AUTHENTICATION_STATE_NONCE_SEND: {
			logexti("Authentication requested %d\n\r", cid);
			break;
		}
	}

	uint32_t nonces[2];
	uint8_t digest[SHA1_DIGEST_LENGTH];

	memcpy(&nonces[0], &brickd_authentication_nonce, 4);
	memcpy(&nonces[1], data->client_nonce, 4);

	char secret[AUTHENTICATION_SECRET_LENGTH+1] = {0};

	if(com == COM_ETHERNET) {
		ethernet_read_config(secret, AUTHENTICATION_SECRET_LENGTH, ETHERNET_AUTHENTICATION_SECRET_POS);
	} else if(com == COM_WIFI) {
		// TODO: Read wifi secret
	} else {
		brickd_authentication_state[cid] = BRICKD_AUTHENTICATION_STATE_ENABLED;
		com_return_setter(com, data);
		return;
	}

	hmac_sha1((uint8_t *)secret, strlen(secret), (uint8_t *)nonces, sizeof(nonces), digest);

	if(memcmp(data->digest, digest, SHA1_DIGEST_LENGTH) != 0) {
		brickd_authentication_state[cid] = BRICKD_AUTHENTICATION_STATE_ENABLED;
		brickd_disconnect_by_com_and_cid(com, cid);
	} else {
		brickd_authentication_state[cid] = BRICKD_AUTHENTICATION_STATE_DONE;
	}

	com_return_setter(com, data);
}

bool brickd_check_auth(const MessageHeader *header, const int8_t cid) {
	if(brickd_authentication_state[cid] == BRICKD_AUTHENTICATION_STATE_DISABLED ||
	   brickd_authentication_state[cid] == BRICKD_AUTHENTICATION_STATE_DONE) {
		return true;
	}

	if(header && header->uid == 1) {
		return true;
	}

	return false;
}
