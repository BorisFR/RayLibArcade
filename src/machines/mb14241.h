/* MB14241.h
 * Github: https:\\github.com\tommojphillips
 */

#ifndef M14241_H
#define M14241_H

#include "../MyDefine.h"

// typedef struct {
// 	uint16_t data;
// 	uint8_t amount;
// } MB14241;
// 
// void mb14241_reset(MB14241* mb14241);
// void mb14241_amount(MB14241* mb14241, uint8_t value);
// void mb14241_data(MB14241* mb14241, uint8_t value);
// uint8_t mb14241_shift(MB14241* mb14241);

extern uint16_t mb14241data;
extern uint8_t mb14241amount;

void mb14241_amount(int offset, int value);
void mb14241_data(int offset, int value);
int mb14241_shift(int offset);

#endif