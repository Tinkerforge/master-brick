/* master-brick
 * Copyright (C) 2011 Olaf Lüke <olaf@tinkerforge.com>
 *
 * chibi_config.h: Configuration of chibi protocol implementation
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

#ifndef CHIBI_CONFIG_H
#define CHIBI_CONFIG_H

#define CHIBI_PAN_ID             0x1234
#define CHIBI_CSMA_RETRIES       5   // Retries for CSMA (max 5)
#define CHIBI_FRAME_RETRIES      15   // Retries if no ACK (max 15)
#define CHIBI_USE_CRC
//#define CHIBI_USE_PROMISCUOUS_MODE

#endif
