#pragma once

#include <TWELITE>
#include <cstdint>
#include "SensorPacket.h"

namespace
{
	uint32_t millis_offset1=0;
	uint32_t millis_offset2=0;
}

namespace Config
{
  const uint32_t APP_ID = 0x96fb64cd;
  const uint8_t CHANNEL = 10;

  void setup_slave(uint8_t id){
    the_twelite
      << TWENET::appid(APP_ID)	// set application ID (identify network group)
      << TWENET::channel(CHANNEL); // set channel (pysical channel)
    auto &&nwksmpl = the_twelite.network.use<NWK_SIMPLE>();
    nwksmpl << NWK_SIMPLE::logical_id(id) // set Logical ID. (0x00 means a parent device)
			<< NWK_SIMPLE::repeat_max(0);	// can repeat a packet up to three times. (being kind of a router)
    the_twelite.begin();
  }

  void setup_master(uint8_t id){
    the_twelite
      << TWENET::appid(APP_ID)	// set application ID (identify network group)
      << TWENET::channel(CHANNEL) // set channel (pysical channel)
      << TWENET::rx_when_idle();	// open receive circuit (if not set, it can't listen packts from others)
    auto &&nwksmpl = the_twelite.network.use<NWK_SIMPLE>();
    nwksmpl << NWK_SIMPLE::logical_id(id) // set Logical ID. (0x00 means a parent device)
			<< NWK_SIMPLE::repeat_max(0)	// can repeat a packet up to three times. (being kind of a router)
      << NWK_SIMPLE::dup_check(16, 50, 5);
    the_twelite.begin();
  }
}
