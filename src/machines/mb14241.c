/* MB14241.c
 * Github: https:\\github.com\tommojphillips
 */

#include "mb14241.h"

// void mb14241_reset(MB14241* mb14241) {
// 	mb14241->amount = 0;
// 	mb14241->data = 0;
// }
// 
// void mb14241_amount(MB14241* mb14241, uint8_t value) {
// 	mb14241->amount = (~value & 0x7);
//     //I8080shift_register_offset = (~amount & 0x7);
// }
// void mb14241_data(MB14241* mb14241, uint8_t value) {
// 	mb14241->data = (value << 7) | (mb14241->data >> 8);
//     // I8080shift_register_value = (data << 7) | (I8080shift_register_value >> 8);
// }
// uint8_t mb14241_shift(MB14241* mb14241) {
// 	return (mb14241->data >> mb14241->amount) & 0xFF;
//     //return (I8080shift_register_value >> I8080shift_register_offset) & 0xFF;
// }

uint16_t mb14241data = 0;
uint8_t mb14241amount = 0;

void mb14241_shift_count_w(int offset, int value) {
    mb14241amount = (~value & 0x7);
}

void mb14241_shift_data_w(int offset, int value) {
    mb14241data = (value << 7) | (mb14241data >> 8);
}

int mb14241_shift_result_r(int offset) {
    return (mb14241data >> mb14241amount) & 0xFF;
}