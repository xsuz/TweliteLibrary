#ifndef _DPS310_H
#define _DPS310_H
#include<stdint.h>

namespace DPS310
{
    void init();
    void readReg(uint8_t reg, uint8_t *data, uint8_t len);
    void writeReg(uint8_t reg, uint8_t data);
    int32_t two_complement(uint32_t val, uint8_t bits);
    double readTemp();
    double readPressure();
} // namespace DPS310


#endif