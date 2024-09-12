#include "dps310.h"
#include <TWELITE>

#define DPS310_address 0x77

#define reset 0x0c
#define PSR_B0 0x02
#define PSR_B1 0x01
#define PSR_B2 0x00
#define TMP_B0 0x05
#define TMP_B1 0x04
#define TMP_B2 0x03

#define MEAS_CFG 0x08
#define TempMeasurement 0x06
#define PMeasurement 0x05
#define PRS_CFG 0x06
#define SportsP 0x37 // 0 011 - 8 measurements pr. sec. 0111 - 128 times (High Precision).
#define TMP_CFG 0x07
#define SportsT 0xc7 // 1-MEMS 100 - 16 measurements pr. sec 0111-oversampling 128
#define CFG_REG 0x09
#define SHIFT 0x0c	 // temp & P shift
#define Kt 2088960.0 // 128times Compensation Scale Factors
#define Kp 2088960.0 // 128times Compensation Scale Factors
#define COEF 0x10   // 18 coefficients


namespace
{
    uint8_t readbuffer[30];
    int16_t c0, c1, c01, c11, c20, c21, c30;
    int32_t c00, c10;
    int32_t Temp0=0x00;
};

// 2の補数計算(マイナス値を算出する）
int32_t DPS310::two_complement(uint32_t val, uint8_t bits)
{
    int32_t val_out;

    if ((val) & ((uint32_t)1 << (bits - 1)))
    {
        val_out = val - (1U << bits);
    }
    else
    {
        val_out = val;
    }
    return val_out;
}

void DPS310::writeReg(uint8_t reg, uint8_t data)
{
    if (auto wrt = Wire.get_writer(DPS310_address))
    {
        wrt << reg;
        wrt << data;
    }
    else
    {
        Serial << "write error!" << mwx::crlf;
    }
}

void DPS310::readReg(uint8_t reg, uint8_t *data, uint8_t len)
{
    if (auto wrt = Wire.get_writer(DPS310_address))
    {
        wrt << reg;
    }
    else
    {
        Serial << "read error" << mwx::crlf;
    }
    if (auto rd = Wire.get_reader(DPS310_address, len))
    {
        for (int i = 0; i < len; i++)
        {
            rd >> data[i];
        }
    }
}

void DPS310::init()
{
    if (auto wrt = Wire.get_writer(DPS310_address))
    {
        wrt << reset;
    }
    delay(100);
    readReg(COEF, readbuffer, 18); // 係数の読み込み
    c0 = (readbuffer[0] << 8 | readbuffer[1] & 0xf0) >> 4;
    c0 = -(c0 & 0b100000000000) | (c0 & 0b011111111111);
    c1 = (readbuffer[1] & 0x0f) << 8 | readbuffer[2];
    c1 = -(c1 & 0b100000000000) | (c1 & 0b011111111111);
    c00 = (readbuffer[3] << 16 | readbuffer[4] << 8 | readbuffer[5] & 0xf0) >> 4;
    c00 = -(c00 & 0b10000000000000000000) | (c00 & 0b01111111111111111111);
    c10 = ((readbuffer[5] & 0x0f) << 16 | readbuffer[6] << 8 | readbuffer[7]);
    c10 = -(c10 & 0b10000000000000000000) | (c10 & 0b01111111111111111111);
    c01 = readbuffer[8] << 8 | readbuffer[9];
    c01 = -(c01 & 0b1000000000000000) | (c01 & 0b0111111111111111);
    c11 = readbuffer[10] << 8 | readbuffer[11];
    c11 = -(c11 & 0b1000000000000000) | (c11 & 0b0111111111111111);
    c20 = readbuffer[12] << 8 | readbuffer[13];
    c20 = -(c20 & 0b1000000000000000) | (c20 & 0b0111111111111111);
    c21 = readbuffer[14] << 8 | readbuffer[15];
    c21 = -(c21 & 0b1000000000000000) | (c21 & 0b0111111111111111);
    c30 = readbuffer[16] << 8 | readbuffer[17];
    c30 = -(c30 & 0b1000000000000000) | (c30 & 0b0111111111111111);
    
	writeReg(MEAS_CFG, 0xF7);
	writeReg(PRS_CFG, SportsP);
	writeReg(TMP_CFG, SportsT);
	writeReg(CFG_REG, SHIFT);
}

double DPS310::readTemp(){
	// read temperature
	writeReg(MEAS_CFG, TempMeasurement);
	delay(100);
	readReg(TMP_B2, readbuffer, 3);
	Temp0 = readbuffer[0] << 16 | readbuffer[1] << 8 | readbuffer[2];
	Temp0 = -(Temp0 & 0b100000000000000000000000) | (Temp0 & 0b011111111111111111111111);
    return c0 * 0.5 + c1 * (float)Temp0 / Kt;
}

double DPS310::readPressure(){
    if(Temp0==0x00){
        readTemp();
    }
	// read pressure
	writeReg(MEAS_CFG, PMeasurement);
	delay(100);
	readReg(PSR_B2, readbuffer, 3);
	int32_t P0 = readbuffer[0] << 16 | readbuffer[1] << 8 | readbuffer[2];
	P0 = -(P0 & 0b100000000000000000000000) | (P0 & 0b011111111111111111111111);
	double Praw = ((float)P0) / Kp;
	double p= c00 + Praw * (c10 + Praw * (c20 + Praw * c30)) + (((float)Temp0) / Kt) * c01 + (((float)Temp0) / Kt) * Praw * (c11 + Praw * c21);

    Temp0=0x00;
    return p;
}
