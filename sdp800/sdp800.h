#ifndef _SDP800_H
#define _SDP800_H
#include<stdint.h>
namespace SDP800{
    int init();
    int readSample();
    float getDifferentialPressure();
    float getTemperature();
    int8_t i2c_write(uint8_t addr, const uint8_t *data, uint16_t count, bool appendCrc = false);
    int8_t i2c_read(uint8_t addr, uint8_t *data, uint16_t count);
    uint8_t crc8(const uint8_t *data, uint8_t len);
};
#endif
