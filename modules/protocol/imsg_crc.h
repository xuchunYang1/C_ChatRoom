//CRC校验
#pragma once
#include <stdint.h>
#ifdef __cplusplus
extern "C"{
#endif

static inline uint16_t update_crc16(uint16_t crc_in, uint8_t byte)
{
	uint32_t crc = crc_in;
	uint32_t in = byte|0x100;
	do
	{
		crc <<= 1;
		in <<= 1;
		if (in&0x100)
			++crc;
		if (crc&0x10000)
			crc ^= 0x1021;
	}while (!(in&0x10000));
	
	return crc&0xffffu;
}

static inline uint16_t crc16(const uint8_t *data, uint32_t size)
{
	uint32_t crc = 0;
	const uint8_t *data_end = data + size; //指针移动size长度
	while (data < data_end)
		crc = update_crc16(crc, *data++);
	
	crc = update_crc16(crc, 0);
	crc = update_crc16(crc, 0);
	return crc&0xffffu;
}
#ifdef __cplusplus
}
#endif
