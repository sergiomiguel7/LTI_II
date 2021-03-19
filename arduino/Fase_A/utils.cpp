#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "string.h"
#include "fcntl.h"
#include "api.h"

void split32(uint8_t *buffer, uint32_t value)
{
	UINT32UNION_t aux;
	aux.number = value;
	for (int i = 0; i < 4; i++)
	{
		buffer[i] = aux.bytes[i];
	}
}
void split16(uint8_t *buffer, uint16_t value)
{
	UINT16UNION_t aux;
	aux.number = value;
	for (int i = 0; i < 2; i++)
	{
		buffer[i] = aux.bytes[i];
	}
}
uint16_t join16(uint8_t *buffer)
{
	UINT16UNION_t aux;
	for (int i = 0; i < 2; i++)
	{
		aux.bytes[i] = buffer[i];
	}
	return aux.number;
}
uint32_t join32(uint8_t *buffer)
{
	UINT32UNION_t aux;
	for (int i = 0; i < 4; i++)
	{
		aux.bytes[i] = buffer[i];
	}
	return aux.number;
}
void splitFloat(uint8_t *buffer, float value)
{
	FLOATUNION_t aux;
	aux.number = value;
	for (int i = 0; i < 4; i++)
	{
		buffer[i] = aux.bytes[i];
	}
}
float joinFloat(uint8_t *bufffer)
{
	FLOATUNION_t aux;
	for (int i = 0; i < 4; i++)
	{
		aux.bytes[i] = bufffer[i];
	}
	return aux.number;
}
