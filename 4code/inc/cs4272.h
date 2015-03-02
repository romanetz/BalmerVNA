#include <stdbool.h>

bool cs4272_Init();

void cs4272_write_reg(uint8_t addr, uint8_t data);
uint8_t cs4272_read_reg(uint8_t addr);

void cs4272_start();
void cs4272_stop();

#define g_i2s_dma true

#define SOUND_BUFFER_SIZE 2048
extern uint16_t sound_buffer[SOUND_BUFFER_SIZE];

uint16_t cs4272_getPos();