/*************************************************************
 * This file was automatically generated on 23.08.2011.      *
 *                                                           *
 * If you have a bugfix for this file and want to commit it, *
 * please fix the bug in the generator. You can find a link  *
 * to the generator git on tinkerforge.com                   *
 *************************************************************/

#ifndef BRICK_MASTER_H
#define BRICK_MASTER_H

#include "ip_connection.h"

typedef Device Master;


void master_create(Master *master, const char *uid);

int master_get_stack_voltage(Master *master, uint16_t *ret_voltage);
int master_get_stack_current(Master *master, uint16_t *ret_current);

void master_register_callback(Master *master, uint8_t cb, void *func);

#endif