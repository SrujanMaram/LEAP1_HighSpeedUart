#include "highSpeedUART_DSOL_ver1.0.h"

// uint8_t historyTMDataFrame[D_HISTORY_TM_ONE_FRAME_SIZE];
// uint8_t payloadDataFrame[D_PAYLOAD_ONE_FRAME_SIZE];
uint8_t fileReadBuff[D_HIGH_SPEED_UART_FIFO_SIZE] = {0};
uint32_t highSpeedUartTxFifoOccupancy = 0, highSpeedUartRxFifoOccupancy = 0;
void *map_base;

void readCurentHighSpeedUartTxFifoOccupancy()
{
    highSpeedUartTxFifoOccupancy = *((volatile uint32_t *)(map_base + ((D_HIGH_SPEED_UART_BASEADDR + D_HIGH_SPEED_UART_TX_FIFO_OCCUPANCY_REG_OFFSET) & (sysconf(_SC_PAGESIZE) - 1))));
}

uint32_t readCurentHighSpeedUartRxFifoOccupancy()
{
    highSpeedUartRxFifoOccupancy = *((volatile uint32_t *)(map_base + ((D_HIGH_SPEED_UART_BASEADDR + D_HIGH_SPEED_UART_RX_FIFO_OCCUPANCY_REG_OFFSET) & (sysconf(_SC_PAGESIZE) - 1))));
}

void writeToHighSpeedUartDataReg(uint8_t value)
{
    *((volatile uint8_t *)(map_base + ((D_HIGH_SPEED_UART_BASEADDR + D_HIGH_SPEED_UART_TX_DATA_REG_OFFSET) & (sysconf(_SC_PAGESIZE) - 1)))) = value;
}

void writeToHighSpeedUartCtrlReg(uint8_t value)
{
    *((volatile uint8_t *)(map_base + ((D_HIGH_SPEED_UART_BASEADDR + D_HIGH_SPEED_UART_CONTROL_REG_OFFSET) & (sysconf(_SC_PAGESIZE) - 1)))) = value;
}

void setHighSpeedUartTxFifoThresholdOffset(uint16_t value) {
    *((volatile uint8_t *)(map_base + ((D_HIGH_SPEED_UART_BASEADDR + D_HIGH_SPEED_UART_TX_FIFO_THRESHOLD_REG_OFFSET) & (sysconf(_SC_PAGESIZE) - 1)))) = value;
}

void loadDatatoHighSpeedUart(uint8_t *data, uint16_t dataSize) {
    writeToHighSpeedUartCtrlReg(0x01);
    for (size_t i = 0; i < dataSize; i++) {
        writeToHighSpeedUartDataReg(data[i]);
    }
    writeToHighSpeedUartCtrlReg(0x00);
}

void transmitFileOnHighSpeedUart(const char *filename, uint16_t frameSize) {
    uint16_t bytesRead = 0;
    int fd;

    // Open the memory device
    if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) == -1)
    {
        printf("X HIGH SPEED UART FIFO :Error opening /dev/mem \n");
    }

    // Map the memory device to user space
    map_base = mmap(0, sysconf(_SC_PAGESIZE), PROT_READ | PROT_WRITE, MAP_SHARED, fd, D_HIGH_SPEED_UART_BASEADDR & ~(sysconf(_SC_PAGESIZE) - 1));
    if (map_base == (void *)-1)
    {
        printf("X HIGH SPEED UART FIFO: Error mapping the memory to user space \n");
    }

    // open the file, read contents and initiate high speed uart transmission
    FILE *file = fopen(filename, "rb"); // Open file in binary mode
    if (!file) {
        printf("Error opening file\n");
        return 1;
    }

    while ((bytesRead = fread(fileReadBuff, 1, frameSize, file)) > 0) {
        do {
            readCurentHighSpeedUartTxFifoOccupancy();
        } while ((D_HIGH_SPEED_UART_FIFO_SIZE - highSpeedUartTxFifoOccupancy) < bytesRead);  // Wait until enough space is available

        loadDatatoHighSpeedUart(fileReadBuff, bytesRead);  // ToDo: '0' fills for last frame? (To meet DSOL expectation)
    }   

    // Unmap the memory
    if (munmap(map_base, MAP_SIZE) == -1)
    {
        perror("Error unmapping the memory");
    }

    // Close the memory device
    close(fd);
}

int main() {

    uint8_t fileType = 0;
    printf ("Enter file type (1 -> HistoryTM, 2-> PL1, 3 -> PL2):");
    // scanf("%hhd", &fileType);
    fileType = 2;

    // ToDo: 2 - To switch to High Speed UART Mode

    switch(fileType) {
        case 1:
            printf("You selected HistoryTM.\n");
            transmitFileOnHighSpeedUart(D_HISTORY_TM_FILE, D_HISTORY_TM_ONE_FRAME_SIZE);
            break;
        case 2:
            printf("You selected PL-1.\n");
            transmitFileOnHighSpeedUart(D_PL1_FILE, D_PAYLOAD_ONE_FRAME_SIZE);
            break;
        case 3:
            printf("You selected PL2.\n");
            transmitFileOnHighSpeedUart(D_PL2_FILE, D_PAYLOAD_ONE_FRAME_SIZE);
            break;
        default:
            printf("Invalid choice! Please select a number between 1 and 3.\n");
    } 

    // ToDo: set GPIO LOW - To switch back to Modbus UART Mode

    return 0;
}
