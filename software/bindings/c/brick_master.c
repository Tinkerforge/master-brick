/*************************************************************
 * This file was automatically generated on 2011-08-23.      *
 *                                                           *
 * If you have a bugfix for this file and want to commit it, *
 * please fix the bug in the generator. You can find a link  *
 * to the generator git on tinkerforge.com                   *
 *************************************************************/

#include "brick_master.h"

#include <string.h>

#define TYPE_GET_STACK_VOLTAGE 1
#define TYPE_GET_STACK_CURRENT 2


#ifdef _MSC_VER
	#pragma pack(push)
	#pragma pack(1)

	#define PACKED
#else
	#define PACKED __attribute__((packed))
#endif

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
} PACKED GetStackVoltage;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint16_t voltage;
} PACKED GetStackVoltageReturn;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
} PACKED GetStackCurrent;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint16_t current;
} PACKED GetStackCurrentReturn;

#ifdef _MSC_VER
	#pragma pack(pop)
#endif

int master_get_stack_voltage(Master *master, uint16_t *ret_voltage) {
	if(master->ipcon == NULL) {
		return E_NOT_ADDED;
	}

	ipcon_sem_wait_write(master);

	master->answer.type = TYPE_GET_STACK_VOLTAGE;
	master->answer.length = sizeof(GetStackVoltageReturn);
	GetStackVoltage gsv;
	gsv.stack_id = master->stack_id;
	gsv.type = TYPE_GET_STACK_VOLTAGE;
	gsv.length = sizeof(GetStackVoltage);

	ipcon_device_write(master, (char *)&gsv, sizeof(GetStackVoltage));

	if(ipcon_answer_sem_wait_timeout(master) != 0) {
		ipcon_sem_post_write(master);
		return E_TIMEOUT;
	}

	GetStackVoltageReturn *gsvr = (GetStackVoltageReturn *)master->answer.buffer;
	*ret_voltage = gsvr->voltage;

	ipcon_sem_post_write(master);

	return E_OK;
}

int master_get_stack_current(Master *master, uint16_t *ret_current) {
	if(master->ipcon == NULL) {
		return E_NOT_ADDED;
	}

	ipcon_sem_wait_write(master);

	master->answer.type = TYPE_GET_STACK_CURRENT;
	master->answer.length = sizeof(GetStackCurrentReturn);
	GetStackCurrent gsc;
	gsc.stack_id = master->stack_id;
	gsc.type = TYPE_GET_STACK_CURRENT;
	gsc.length = sizeof(GetStackCurrent);

	ipcon_device_write(master, (char *)&gsc, sizeof(GetStackCurrent));

	if(ipcon_answer_sem_wait_timeout(master) != 0) {
		ipcon_sem_post_write(master);
		return E_TIMEOUT;
	}

	GetStackCurrentReturn *gscr = (GetStackCurrentReturn *)master->answer.buffer;
	*ret_current = gscr->current;

	ipcon_sem_post_write(master);

	return E_OK;
}

void master_register_callback(Master *master, uint8_t cb, void *func) {
    master->callbacks[cb] = func;
}

void master_create(Master *master, const char *uid) {
	ipcon_device_create(master, uid);

}
