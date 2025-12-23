#ifndef AVR_PROGRAMMER_H
#define AVR_PROGRAMMER_H

typedef struct{
	const char *fileName;
	uint8_t spiPresc;
	uint8_t targetSignature[3];
	uint16_t buf_s;
}AVR_Prog_TypeDef;

#endif
