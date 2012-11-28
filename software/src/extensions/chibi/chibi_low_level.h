/* master-brick
 * Copyright (C) 2011 Olaf Lüke <olaf@tinkerforge.com>
 *
 * chibi_low_level.h: Low-level chibi protocol implementation
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

#ifndef CHIBI_LOW_LEVEL_H
#define CHIBI_LOW_LEVEL_H

#include <stdint.h>
#include <stdbool.h>

#include "bricklib/drivers/pio/pio.h"

#include "config.h"
#include "chibi_config.h"

// Header
typedef struct {
	uint16_t frame_control_field;
	uint8_t sequence;
	uint16_t pan_id;
	uint16_t short_destination_address;
	uint16_t short_source_address;
} __attribute__((__packed__)) ChibiHeaderMPDU; // MAC Protocol Data Unit

#define CHIBI_MAX_FRAME_LENGTH   127
#define CHIBI_HEADER_LENGTH      sizeof(ChibiHeaderMPDU)
#define CHIBI_FCS_LENGTH         2
#define CHIBI_LENGTH_LENGTH      1
#define CHIBI_MIN_FRAME_LENGTH   3
#define CHIBI_MAX_DATA_LENGTH    (CHIBI_MAX_FRAME_LENGTH - \
                                  CHIBI_HEADER_LENGTH    - \
                                  CHIBI_FCS_LENGTH       - \
                                  CHIBI_LENGTH_LENGTH)

#define CHIBI_TYPE_NONE    0
#define CHIBI_TYPE_SLAVE   1
#define CHIBI_TYPE_MASTER  2

#define CHIBI_MDPU_BEACON                 0x00
#define CHIBI_MDPU_DATA                   0x01
#define CHIBI_MDPU_ACK                    0x02
#define CHIBI_MDPU_MAC_COMMAND            0x03
#define CHIBI_MDPU_SECURITY_PROCESSNG     (1 << 3)
#define CHIBI_MDPU_FRAME_PENDING          (1 << 4)
#define CHIBI_MDPU_ACK_REQUEST            (1 << 5)
#define CHIBI_MDPU_PAN_COMPRESSION        (1 << 6)
#define CHIBI_MDPU_DEST_NO_PAN_ADDRESS    (0x00 << 10)
#define CHIBI_MDPU_DEST_SHORT_ADDRESS     (0x02 << 10)
#define CHIBI_MDPU_DEST_EXTENDED_ADDRESS  (0x03 << 10)
#define CHIBI_MDPU_FRAME_VERSION_03       (0x00 << 12)
#define CHIBI_MDPU_FRAME_VERSION_06       (0x01 << 12)
#define CHIBI_MDPU_SRC_NO_PAN_ADDRESS     (0x00 << 14)
#define CHIBI_MDPU_SRC_SHORT_ADDRESS      (0x02 << 14)
#define CHIBI_MDPU_SRC_EXTENDED_ADDRESS   (0x03 << 14)

// Chibi error states
#define CHIBI_ERROR_OK           0
#define CHIBI_ERROR_WRONG_STATE  1
#define CHIBI_ERROR_TIMEOUT      2
#define CHIBI_ERROR_NO_ACK       3
#define CHIBI_ERROR_UNEXPECTED   4
#define CHIBI_ERROR_WAIT_FOR_ACK 5

// Frequencies
#define OQPSK_868MHZ_EUROPE 0
#define OQPSK_915MHZ_US     1
#define OQPSK_780MHZ_CHINA  2
#define BPSK40_915MHZ       3

// AT86RF212 timings (in ns)
#define CHIBI_TIME_P_ON_TO_CLKM_AVAIL   330000
#define CHIBI_TIME_SLEEP_TO_TRX_OFF     380000
#define CHIBI_TIME_TRX_OFF_TO_SLEEP     (35*125)
#define CHIBI_TIME_TRX_OFF_PLL_ON       200000
#define CHIBI_TIME_PLL_ON_TRX_OFF       1000
#define CHIBI_TIME_TRX_OFF_RX_ON        200000
#define CHIBI_TIME_RX_ON_TRX_OFF        1000
#define CHIBI_TIME_PLL_ON_RX_ON         1000
#define CHIBI_TIME_RX_ON_PLL_ON         1000
#define CHIBI_TIME_PLL_LOCK_TIME        110000
#define CHIBI_TIME_BUSY_TX_PLL_ON       32000
#define CHIBI_TIME_ALL_STATES_TRX_OFF   1000
#define CHIBI_TIME_RESET_TRX_OFF        26000
#define CHIBI_TIME_TRX_IRQ_DELAY        9000
#define CHIBI_TIME_IRQ_PROCESSING_DLY   32000
#define CHIBI_TIME_SLEEP_PULSE          63
#define CHIBI_TIME_RESET_PULSE          625
#define CHIBI_TIME_SPI_AFTER_RESET      625
#define CHIBI_TIME_BETWEEN_SELECT       250

// AT86RF212 SPI masks
#define CHIBI_SPI_RW      0xC0    //  Register Write (short mode)
#define CHIBI_SPI_RR      0x80    //  Register Read (short mode)
#define CHIBI_SPI_FW      0x60    //  Frame Transmit Mode (long mode)
#define CHIBI_SPI_FR      0x20    //  Frame Receive Mode (long mode)
#define CHIBI_SPI_SW      0x40    //  SRAM Write
#define CHIBI_SPI_SR      0x00    //  SRAM Read

// AT86RF212 registers
#define CHIBI_REGISTER_TRX_STATUS     0x01
#define CHIBI_REGISTER_TRX_STATE      0x02
#define CHIBI_REGISTER_TRX_CTRL_0     0x03
#define CHIBI_REGISTER_TRX_CTRL_1     0x04
#define CHIBI_REGISTER_PHY_TX_PWR     0x05
#define CHIBI_REGISTER_PHY_RSSI       0x06
#define CHIBI_REGISTER_PHY_ED_LEVEL   0x07
#define CHIBI_REGISTER_PHY_CC_CCA     0x08
#define CHIBI_REGISTER_CCA_THRES      0x09
#define CHIBI_REGISTER_RX_CTRL        0x0A
#define CHIBI_REGISTER_SFD_VALUE      0x0B
#define CHIBI_REGISTER_TRX_CTRL_2     0x0C
#define CHIBI_REGISTER_ANT_DIV        0x0D
#define CHIBI_REGISTER_IRQ_MASK       0x0E
#define CHIBI_REGISTER_IRQ_STATUS     0x0F
#define CHIBI_REGISTER_VREG_CTRL      0x10
#define CHIBI_REGISTER_BATMON         0x11
#define CHIBI_REGISTER_XOSC_CTRL      0x12
#define CHIBI_REGISTER_CC_CTRL_0      0x13
#define CHIBI_REGISTER_CC_CTRL_1      0x14
#define CHIBI_REGISTER_RX_SYN         0x15
#define CHIBI_REGISTER_RF_CTRL_0      0x16
#define CHIBI_REGISTER_XAH_CTRL_1     0x17
#define CHIBI_REGISTER_FTN_CTRL       0x18
#define CHIBI_REGISTER_RF_CTRL_1      0x19
#define CHIBI_REGISTER_PLL_CF         0x1A
#define CHIBI_REGISTER_PLL_DCU        0x1B
#define CHIBI_REGISTER_PART_NUM       0x1C
#define CHIBI_REGISTER_VERSION_NUM    0x1D
#define CHIBI_REGISTER_MAN_ID_0       0x1E
#define CHIBI_REGISTER_MAN_ID_1       0x1F
#define CHIBI_REGISTER_SHORT_ADDR_0   0x20
#define CHIBI_REGISTER_SHORT_ADDR_1   0x21
#define CHIBI_REGISTER_PAN_ID_0       0x22
#define CHIBI_REGISTER_PAN_ID_1       0x23
#define CHIBI_REGISTER_IEEE_ADDR_0    0x24
#define CHIBI_REGISTER_IEEE_ADDR_1    0x25
#define CHIBI_REGISTER_IEEE_ADDR_2    0x26
#define CHIBI_REGISTER_IEEE_ADDR_3    0x27
#define CHIBI_REGISTER_IEEE_ADDR_4    0x28
#define CHIBI_REGISTER_IEEE_ADDR_5    0x29
#define CHIBI_REGISTER_IEEE_ADDR_6    0x2A
#define CHIBI_REGISTER_IEEE_ADDR_7    0x2B
#define CHIBI_REGISTER_XAH_CTRL_0     0x2C
#define CHIBI_REGISTER_CSMA_SEED_0    0x2D
#define CHIBI_REGISTER_CSMA_SEED_1    0x2E
#define CHIBI_REGISTER_CSMA_BE        0x2F
#define CHIBI_REGISTER_NUM            0x30

// AT86RF212 status
#define CHIBI_STATUS_P_ON                    0x00
#define CHIBI_STATUS_BUSY_RX                 0x01
#define CHIBI_STATUS_BUSY_TX                 0x02
#define CHIBI_STATUS_RX_ON                   0x06
#define CHIBI_STATUS_TRX_OFF                 0x08
#define CHIBI_STATUS_PLL_ON                  0x09
#define CHIBI_STATUS_SLEEP                   0x0F
#define CHIBI_STATUS_BUSY_RX_AACK            0x11
#define CHIBI_STATUS_BUSY_TX_ARET            0x12
#define CHIBI_STATUS_RX_AACK_ON              0x16
#define CHIBI_STATUS_TX_ARET_ON              0x19
#define CHIBI_STATUS_RX_ON_NOCLK             0x1C
#define CHIBI_STATUS_RX_AACK_ON_NOCLK        0x1D
#define CHIBI_STATUS_BUSY_RX_AACK_NOCLK      0x1E
#define CHIBI_STATUS_TRANSITION_IN_PROGRESS  0x1F

#define CHIBI_STATUS_MASK                    0x1F
#define CHIBI_STATUS(v)                      ((v) & CHIBI_STATUS_MASK)


// AT86RF212 state
#define CHIBI_STATE_NOP                 0x00
#define CHIBI_STATE_TX_START            0x02
#define CHIBI_STATE_FORCE_TRX_OFF       0x03
#define CHIBI_STATE_FORCE_PLL_ON        0x04
#define CHIBI_STATE_RX_ON               0x06
#define CHIBI_STATE_TRX_OFF             0x08
#define CHIBI_STATE_PLL_ON              0x09
#define CHIBI_STATE_RX_AACK_ON          0x16
#define CHIBI_STATE_TX_ARET_ON          0x19

#define CHIBI_STATE_MASK                0x1F
#define CHIBI_STATE(v)                  ((v) & CHIBI_STATE_MASK)
#define CHIBI_STATE_TRAC_STATUS_POS     5
#define CHIBI_STATE_TRAC_STATUS(v)      ((v) >> CHIBI_STATE_TRAC_STATUS_POS)

#define CHIBI_TRAC_STATUS_SUCCESS                  0
#define CHIBI_TRAC_STATUS_SUCCESS_DATA_PENDING     1
#define CHIBI_TRAC_STATUS_SUCCESS_WAIT_FOR_ACK     2
#define CHIBI_TRAC_STATUS_CHANNEL_ACCESS_FAILURE   3
#define CHIBI_TRAC_STATUS_NO_ACK                   5
#define CHIBI_TRAC_STATUS_INVALID                  7

// AT86RF212 csma seed
#define CHIBI_CSMA_SEED_1_AACK_I_AM_COORD     (1 << 3)
#define CHIBI_CSMA_SEED_1_AACK_DIS_ACK        (1 << 4)
#define CHIBI_CSMA_SEED_1_AACK_SET_PD         (1 << 5)
#define CHIBI_CSMA_SEED_1_AACK_FVN_MODE_0     (0 << 6)
#define CHIBI_CSMA_SEED_1_AACK_FVN_MODE_01    (1 << 6)
#define CHIBI_CSMA_SEED_1_AACK_FVN_MODE_012   (2 << 6)
#define CHIBI_CSMA_SEED_1_AACK_FVN_MODE_IND   (3 << 6)

#define CHIBI_CSMA_SEED_1_AACK_FVN_MODE_MASK  (3 << 6)

// AT86RF212 irq mask
#define CHIBI_IRQ_MASK_PLL_LOCK       (1 << 0)
#define CHIBI_IRQ_MASK_PLL_UNLOCK     (1 << 1)
#define CHIBI_IRQ_MASK_RX_START       (1 << 2)
#define CHIBI_IRQ_MASK_TRX_END        (1 << 3)
#define CHIBI_IRQ_MASK_CCA_ED_DONE    (1 << 4)
#define CHIBI_IRQ_MASK_AMI            (1 << 5)
#define CHIBI_IRQ_MASK_TRX_UR         (1 << 6)
#define CHIBI_IRQ_MASK_BAT_LOW        (1 << 7)

// AT86RF212 trx ctrl 2
#define CHIBI_TRX_CTRL_2_OQPSK_DATA_RATE(v)    (v)
#define CHIBI_TRX_CTRL_2_SUB_MODE              (1 << 2)
#define CHIBI_TRX_CTRL_2_BPSK_OQPSK            (1 << 3)
#define CHIBI_TRX_CTRL_2_OQPSK_SUB1_RC_EN      (1 << 4)
#define CHIBI_TRX_CTRL_2_OQPSK_SCRAM_EN        (1 << 5)
#define CHIBI_TRX_CTRL_2_TRX_OFF_AVDD_EN       (1 << 6)
#define CHIBI_TRX_CTRL_2_RX_SAFE_MODE          (1 << 7)
#define CHIBI_TRX_CTRL_2_OQPSK_DATA_RATE_MASK  0x03

// AT86RF212 trx ctrl 1
#define CHIBI_TRX_CTRL_1_IRQ_POLARITY          (1 << 0)
#define CHIBI_TRX_CTRL_1_IRQ_MASK_MODE         (1 << 1)
#define CHIBI_TRX_CTRL_1_SPI_CMD_MODE_0        (0 << 2)
#define CHIBI_TRX_CTRL_1_SPI_CMD_MODE_TRX      (1 << 2)
#define CHIBI_TRX_CTRL_1_SPI_CMD_MODE_PHY      (2 << 2)
#define CHIBI_TRX_CTRL_1_SPI_CMD_MODE_IRQ      (3 << 2)
#define CHIBI_TRX_CTRL_1_RX_BL_CTRL            (1 << 4)
#define CHIBI_TRX_CTRL_1_TX_AUTO_CRC_ON        (1 << 5)
#define CHIBI_TRX_CTRL_1_IRQ_2_EXT_EN          (1 << 6)
#define CHIBI_TRX_CTRL_1_PA_EXT_EN             (1 << 7)
#define CHIBI_TRX_CTRL_1_SPI_CMD_MODE_MASK     (3 << 2)

// AT86RF212 rf ctrl 0
#define CHIBI_RF_CTRL_0_PA_LT_2             (0 << 6)
#define CHIBI_RF_CTRL_0_PA_LT_4             (1 << 6)
#define CHIBI_RF_CTRL_0_PA_LT_6             (2 << 6)
#define CHIBI_RF_CTRL_0_PA_LT_8             (3 << 6)
#define CHIBI_RF_CTRL_0_GC_TX_OFFS_M1       0x00
#define CHIBI_RF_CTRL_0_GC_TX_OFFS_0        0x01
#define CHIBI_RF_CTRL_0_GC_TX_OFFS_P1       0x02
#define CHIBI_RF_CTRL_0_GC_TX_OFFS_P2       0x03
// Table 7-16 in at86rf212 datasheet
#define CHIBI_OQPSK_TX_OFFSET               0x02
#define CHIBI_BPSK_TX_OFFSET                0x03

#define CHIBI_RF_CTRL_0_PA_LT_MASK          (3 << 6)
#define CHIBI_RF_CTRL_0_GC_TX_OFFS_MASK     0x03

// AT86RF212 phy cc cca
#define CHIBI_PHY_CC_CCA_CHANNEL_868        0x00
#define CHIBI_PHY_CC_CCA_CHANNEL_906        0x01
#define CHIBI_PHY_CC_CCA_CHANNEL_908        0x02
#define CHIBI_PHY_CC_CCA_CHANNEL_910        0x03
#define CHIBI_PHY_CC_CCA_CHANNEL_912        0x04
#define CHIBI_PHY_CC_CCA_CHANNEL_914        0x05
#define CHIBI_PHY_CC_CCA_CHANNEL_916        0x06
#define CHIBI_PHY_CC_CCA_CHANNEL_918        0x07
#define CHIBI_PHY_CC_CCA_CHANNEL_920        0x08
#define CHIBI_PHY_CC_CCA_CHANNEL_922        0x09
#define CHIBI_PHY_CC_CCA_CHANNEL_924        0x0A

#define CHIBI_PHY_CC_CCA_CHANNEL_MASK       0x1F

// AT86RF212 phy rssi
#define CHIBI_PHY_RSSI_RX_CRC_VALID         (1 << 7)
#define CHIBI_PHY_RSSI_RND_VALUE(v)         ((v) << 5)
#define CHIBI_PHY_RSSI_RSSI(v)              ((v))
#define CHIBI_PHY_RSSI_RND_VALUE_MASK       0x60
#define CHIBI_PHY_RSSI_RSSI_MASK            0x1F

// AT86RF212 xah ctrl 0
#define CHIBI_XAH_CTRL_0_SLOTTED_OPERATION  (1 << 0)
#define CHIBI_XAH_CTRL_0_CSMA_RETRIES(v)    ((v) << 1) // 0..5 allowed
#define CHIBI_XAH_CTRL_0_FRAME_RETRIES(v)   ((v) << 4)

// AT86RF212 xah ctrl 1
#define CHIBI_XAH_CTRL_1_AACK_PROM_MODE     (1 << 1)
#define CHIBI_XAH_CTRL_1_AACK_ACK_TIME      (1 << 2)
#define CHIBI_XAH_CTRL_1_AACK_UPLD_RES_FT   (1 << 4)
#define CHIBI_XAH_CTRL_1_AACK_FLTR_RES_FT   (1 << 5)
#define CHIBI_XAH_CTRL_1_CSMA_LBT_MODE      (1 << 6)

void chibi_select(void);
void chibi_deselect(void);
uint8_t chibi_transceive_byte(const uint8_t value);
uint8_t chibi_read_register(const uint8_t address);
void chibi_write_register(const uint8_t address, const uint8_t value);

uint8_t chibi_read_register_buffer(const uint8_t address,
                                   uint8_t *buffer,
                                   const uint8_t length);

uint8_t chibi_write_register_buffer(const uint8_t address,
                                    const uint8_t *buffer,
                                    const uint8_t length);

void chibi_write_register_mask(const uint8_t address,
                               const uint8_t value,
                               const uint8_t mask);

uint8_t chibi_read_register_mask(const uint8_t address, const uint8_t mask);

void chibi_write_frame(const uint8_t *header,
                       const uint8_t *data,
                       const uint8_t data_length);

bool chibi_is_sleeping(void);
void chibi_sleep(void);
void chibi_wakeup(void);
void chibi_reset_enable(void);
void chibi_reset_disable(void);
void chibi_reset(void);

void chibi_set_mode(const uint8_t mode);
void chibi_set_channel(const uint8_t channel);

uint8_t chibi_get_status();
uint8_t chibi_set_state(const uint8_t state);
uint8_t chibi_transfer(const uint8_t *header,
                       const uint8_t *data,
                       const uint8_t length);
void chibi_read_frame(void);
void chibi_interrupt(const Pin *pin);
void chibi_low_level_init(void);
void chibi_low_level_insert_uid(void* data);

#endif
