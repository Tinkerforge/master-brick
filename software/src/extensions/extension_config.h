/* master-brick
 * Copyright (C) 2011 Olaf Lüke <olaf@tinkerforge.com>
 *
 * extension_config.h: Configuration for extensions
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

#ifndef EXTENSION_CONFIG_H
#define EXTENSION_CONFIG_H

#define EXTENSION_TYPE_NONE 0
#define EXTENSION_TYPE_CHIBI 1
#define EXTENSION_TYPE_RS485 2

#define EXTENSION_POS_TYPE 0
#define EXTENSION_POS_ADDRESS 4
#define EXTENSION_POS_RECEIVER_ADDRESS_START 8

#define DEBUG_EXTENSION 1

#if(DEBUG_EXTENSION)
#define logextd(str, ...) do{logd("ext: " str, ##__VA_ARGS__);}while(0)
#define logexti(str, ...) do{logi("ext: " str, ##__VA_ARGS__);}while(0)
#define logextw(str, ...) do{logw("ext: " str, ##__VA_ARGS__);}while(0)
#define logexte(str, ...) do{loge("ext: " str, ##__VA_ARGS__);}while(0)
#define logextf(str, ...) do{logf("ext: " str, ##__VA_ARGS__);}while(0)
#else
#define logextd(str, ...) {}
#define logexti(str, ...) {}
#define logextw(str, ...) {}
#define logexte(str, ...) {}
#define logextf(str, ...) {}
#endif

#endif
