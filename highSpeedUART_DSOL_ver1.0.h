#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#define MAP_SIZE                                                4096

#define D_BASE_SOURCE_FILES_PATH                                "/home/srujan/Desktop/HighSpeedUART"
#define D_HISTORY_TM_FILE                                       D_BASE_SOURCE_FILES_PATH"/HISTORY_TM_TRANSFERFRAMES.txt"
#define D_PL1_FILE                                              D_BASE_SOURCE_FILES_PATH"/PL1_TRANSFERFRAMES.txt"
#define D_PL2_FILE                                              D_BASE_SOURCE_FILES_PATH"/PL2_TRANSFERFRAMES.txt"

#define D_HIGH_SPEED_UART_FIFO_SIZE                             1024
#define D_MODBUS_OVERHEADS_SIZE                                 8
#define D_HISTORY_TM_ONE_FRAME_SIZE                             564 + D_MODBUS_OVERHEADS_SIZE
#define D_PAYLOAD_ONE_FRAME_SIZE                                564 + D_MODBUS_OVERHEADS_SIZE          // 1460

#define D_HIGH_SPEED_UART_BASEADDR                              0xA00B0000
#define D_HIGH_SPEED_UART_STATUS_REG_OFFSET                     0x00000000
#define D_HIGH_SPEED_UART_CONTROL_REG_OFFSET                    0x00000004
#define D_HIGH_SPEED_UART_TX_DATA_REG_OFFSET                    0x00000008
#define D_HIGH_SPEED_UART_RX_DATA_REG_OFFSET                    0x0000000C
#define D_HIGH_SPEED_UART_TX_FIFO_OCCUPANCY_REG_OFFSET          0x00000010
#define D_HIGH_SPEED_UART_RX_FIFO_OCCUPANCY_REG_OFFSET          0x00000014
#define D_HIGH_SPEED_UART_TX_FIFO_THRESHOLD_REG_OFFSET          0x00000018
#define D_HIGH_SPEED_UART_FIFO_RESET_REG_OFFSET                 0x0000001C