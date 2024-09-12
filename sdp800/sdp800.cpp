#include"sdp800.h"
#include <TWELITE>

namespace
{

    float mDifferentialPressure;
    float mTemperature;
}
int SDP800::init()
{
    const uint8_t CMD_LEN = 2;
    uint8_t cmd0[CMD_LEN] = {0x36, 0x1E};
    uint8_t ret = i2c_write(0x25, cmd0, CMD_LEN);
    if (ret != 0)
    {
        return 1;
    }
    return 0;
}

int SDP800::readSample()
{
    const uint8_t CMD_LEN = 2;
    uint8_t cmd[CMD_LEN] = {0x36, 0x2F};

    const uint8_t DATA_LEN = 9;
    uint8_t data[DATA_LEN] = {0};

    if (i2c_write(0x25, cmd, CMD_LEN) != 0)
    {
        return 1;
    }

    delay(100); // theoretically 45ms

    if (int8 ret=i2c_read(0x25, data, DATA_LEN))
    {
        return ret;
    }

    // TODO: check CRC

    int16_t dp_raw = (int16_t)data[0] << 8 | data[1];
    int16_t temp_raw = (int16_t)data[3] << 8 | data[4];
    int16_t dp_scale = (int16_t)data[6] << 8 | data[7];

    mDifferentialPressure = dp_raw / (float)dp_scale;
    mTemperature = temp_raw / 200.0;

    return 0;
}

float SDP800::getDifferentialPressure()
{
    return mDifferentialPressure;
}

float SDP800::getTemperature()
{
    return mTemperature;
}

int8_t SDP800::i2c_write(uint8_t addr, const uint8_t *data, uint16_t count, bool appendCrc)
{
    if (auto &&wrt = Wire.get_writer(addr))
    {
        for (uint8 i = 0; i < count; i++)
        {
            wrt(data[i]);
        }
        if (appendCrc)
        {
            if (wrt(crc8(data, count)) != 1)
            {
                return 2;
            }
        }
    }else{
        return 1;
    }
    return 0;
}

int8_t SDP800::i2c_read(uint8_t addr, uint8_t *data, uint16_t count)
{
    if (auto &&rdr = Wire.get_reader(addr, count))
    {
        for (uint8 i = 0; i < count; i++)
        {
            rdr >> *data;
            data++;
        }
    }else{
        return 1;
    }
    return 0;
}

uint8_t SDP800::crc8(const uint8_t *data, uint8_t len)
{
    // adapted from SHT21 sample code from http://www.sensirion.com/en/products/humidity-temperature/download-center/

    uint8_t crc = 0xff;
    uint8_t byteCtr;
    for (byteCtr = 0; byteCtr < len; ++byteCtr)
    {
        crc ^= (data[byteCtr]);
        for (uint8_t bit = 8; bit > 0; --bit)
        {
            if (crc & 0x80)
            {
                crc = (crc << 1) ^ 0x31;
            }
            else
            {
                crc = (crc << 1);
            }
        }
    }
    return crc;
}
