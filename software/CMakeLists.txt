CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

SET(PROJECT_NAME master-brick)
SET(CHIP sam3s4)
SET(MCU cortex-m3\ -mthumb)
PROJECT(${PROJECT_NAME})

# Optimization level, can be [0, 1, 2, 3, s]. 
# 0 = turn off optimization. s = optimize for size.
#SET(OPTIMIZATION_LEVEL 0)
#SET(DEBUG "-g -ggdb")

SET(OPTIMIZATION_LEVEL 3)
SET(DEBUG "")

SET(TRACE_LEVEL 0)
SET(CMAKE_BUILD_TYPE None)

SET(USE_SPI_DMA 1)

INCLUDE_DIRECTORIES(
	"${PROJECT_SOURCE_DIR}/src/"
)

# find source files
#FILE(GLOB_RECURSE SOURCES "src/*.c")

SET(SOURCES
	"${PROJECT_SOURCE_DIR}/src/communication.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/logging/logging.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/drivers/crc/crc.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/drivers/dacc/dacc.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/drivers/pwmc/pwmc.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/drivers/usart/uart_console.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/drivers/usart/usart.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/drivers/twi/twid.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/drivers/twi/twi.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/drivers/adc/adc.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/drivers/pio/pio.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/drivers/pio/pio_it.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/drivers/efc/efc.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/drivers/wdt/wdt.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/drivers/flash/flashd.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/drivers/board/board_cstartup_gnu.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/drivers/board/board_lowlevel.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/drivers/board/exceptions.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/drivers/tc/tc.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/drivers/usb/USBD.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/drivers/usb/USBDescriptors.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/drivers/usb/USBRequests.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/drivers/usb/USBDCallbacks.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/drivers/usb/USBD_HAL.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/drivers/usb/USBDDriver.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/drivers/usb/USBDDriverCallbacks.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/drivers/pmc/pmc.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/drivers/spi/spi.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/drivers/uid/uid.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/free_rtos/croutine.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/free_rtos/tasks.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/free_rtos/portable/MemMang/heap_tf.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/free_rtos/portable/GCC/ARM_CM3/port.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/free_rtos/queue.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/free_rtos/list.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/utility/pearson_hash.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/utility/rand.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/utility/init.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/utility/led.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/utility/syscalls.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/utility/profiling.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/utility/trace.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/utility/sqrt.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/utility/mutex.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/utility/sha_1.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/utility/base64_encode.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/utility/ringbuffer.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/utility/system_timer.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/com/com_messages.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/com/i2c/i2c_eeprom/i2c_eeprom_common.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/com/i2c/i2c_eeprom/i2c_eeprom_master.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/com/none/none.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/com/com.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/com/usb/usb_descriptors.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/com/usb/usb_callbacks.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/com/usb/usb.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/com/com_common.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/com/i2c/i2c_clear_bus.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/bricklet/bricklet_communication.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/bricklet/bricklet_init.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/bricklet/bricklet_co_mcu.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/bricklet/bricklet_xmc.c"
	"${PROJECT_SOURCE_DIR}/src/extensions/extension_init.c"
	"${PROJECT_SOURCE_DIR}/src/extensions/extension_i2c.c"
	"${PROJECT_SOURCE_DIR}/src/extensions/brickd.c"
	"${PROJECT_SOURCE_DIR}/src/extensions/websocket.c"
	"${PROJECT_SOURCE_DIR}/src/extensions/chibi/chibi_low_level.c"
	"${PROJECT_SOURCE_DIR}/src/extensions/chibi/chibi_master.c"
	"${PROJECT_SOURCE_DIR}/src/extensions/chibi/chibi_slave.c"
	"${PROJECT_SOURCE_DIR}/src/extensions/chibi/chibi_init.c"
	"${PROJECT_SOURCE_DIR}/src/extensions/chibi/chibi.c"
	"${PROJECT_SOURCE_DIR}/src/extensions/rs485/rs485_low_level.c"
	"${PROJECT_SOURCE_DIR}/src/extensions/rs485/rs485_slave.c"
	"${PROJECT_SOURCE_DIR}/src/extensions/rs485/rs485_master.c"
	"${PROJECT_SOURCE_DIR}/src/extensions/rs485/rs485.c"
	"${PROJECT_SOURCE_DIR}/src/extensions/wifi/wifi_low_level.c"
	"${PROJECT_SOURCE_DIR}/src/extensions/wifi/wifi_command.c"
	"${PROJECT_SOURCE_DIR}/src/extensions/wifi/wifi_data.c"
	"${PROJECT_SOURCE_DIR}/src/extensions/wifi/wifi_ringbuffer.c"
	"${PROJECT_SOURCE_DIR}/src/extensions/wifi/wifi.c"
	"${PROJECT_SOURCE_DIR}/src/extensions/wifi2/wifi2.c"
	"${PROJECT_SOURCE_DIR}/src/extensions/wifi2/wifi2_uart.c"
	"${PROJECT_SOURCE_DIR}/src/extensions/wifi2/wifi2_bootloader.c"
	"${PROJECT_SOURCE_DIR}/src/extensions/ethernet/ethernet_websocket.c"
	"${PROJECT_SOURCE_DIR}/src/extensions/ethernet/ethernet_low_level.c"
	"${PROJECT_SOURCE_DIR}/src/extensions/ethernet/ethernet_dhcp.c"
	"${PROJECT_SOURCE_DIR}/src/extensions/ethernet/ethernet.c"
	"${PROJECT_SOURCE_DIR}/src/extensions/ethernet/ethernet_w5200.c"
	"${PROJECT_SOURCE_DIR}/src/extensions/ethernet/ethernet_w5500.c"
	"${PROJECT_SOURCE_DIR}/src/main.c"
	"${PROJECT_SOURCE_DIR}/src/master.c"
)

IF(USE_SPI_DMA)
LIST(APPEND SOURCES
	"${PROJECT_SOURCE_DIR}/src/bricklib/com/spi/spi_stack/spi_stack_select_dma.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/com/spi/spi_stack/spi_stack_common_dma.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/com/spi/spi_stack/spi_stack_slave_dma.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/com/spi/spi_stack/spi_stack_master_dma.c"
	"${PROJECT_SOURCE_DIR}/src/routing_dma.c"
)
ELSE()
LIST(APPEND SOURCES
	"${PROJECT_SOURCE_DIR}/src/bricklib/com/spi/spi_stack/spi_stack_select.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/com/spi/spi_stack/spi_stack_common.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/com/spi/spi_stack/spi_stack_slave.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib/com/spi/spi_stack/spi_stack_master.c"
	"${PROJECT_SOURCE_DIR}/src/routing.c"
)
ENDIF()

MESSAGE(STATUS "\nFound following source files:\n ${SOURCES}\n")

# define executable
ADD_EXECUTABLE(${PROJECT_NAME}.elf ${SOURCES})
SET_TARGET_PROPERTIES(${PROJECT_NAME}.elf PROPERTIES LINKER_LANGUAGE C)

# TARGET_LINK_LIBRARIES(${PROJECT_NAME}.elf -lgcc -lc)

# define compile flags
SET_TARGET_PROPERTIES(${PROJECT_NAME}.elf PROPERTIES COMPILE_FLAGS
	"${DEBUG} -mcpu=${MCU} -std=gnu99  -Wall -mlong-calls -ffunction-sections -O${OPTIMIZATION_LEVEL}"
)

#define linker flags
SET_TARGET_PROPERTIES(${PROJECT_NAME}.elf PROPERTIES LINK_FLAGS
	"-mcpu=${MCU} -Wl,--gc-sections -T\"${PROJECT_SOURCE_DIR}/src/bricklib/drivers/board/sam3s/flash_${CHIP}.ld\" "
)

ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME}.elf POST_BUILD COMMAND 
                   ${CMAKE_OBJCOPY} -S -O binary 
                   ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin)

ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME}.elf POST_BUILD COMMAND 
                   ${CMAKE_SIZE} 
                   ${OBJECT} ${PROJECT_NAME}.elf)

# add preprocessor defines
ADD_DEFINITIONS(-D${CHIP} -DTRACE_LEVEL=${TRACE_LEVEL} -DUSE_SPI_DMA=${USE_SPI_DMA} -Dflash -Dprintf=iprintf) 
