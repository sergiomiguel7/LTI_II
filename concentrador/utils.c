
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/time.h>
#include <fcntl.h>
#include <time.h>
#include "rs232/rs232.h"
#include "api.h"
#include "server_api.h"

uint32_t current_timestamp()
{
	struct timeval te;
	gettimeofday(&te, NULL); // get current time
	return (uint32_t)te.tv_sec;
}
void split32(char *buffer, uint32_t value)
{
	UINT32UNION_t aux;
	aux.number = value;
	for (int i = 0; i < 4; i++)
	{
		buffer[i] = aux.bytes[i];
	}
}
void split16(char *buffer, uint16_t value)
{
	UINT16UNION_t aux;
	aux.number = value;
	for (int i = 0; i < 2; i++)
	{
		buffer[i] = aux.bytes[i];
	}
}
uint16_t join16(char *buffer)
{
	UINT16UNION_t aux;
	for (int i = 0; i < 2; i++)
	{
		aux.bytes[i] = buffer[i];
	}
	return aux.number;
}
uint32_t join32(char *buffer)
{
	UINT32UNION_t aux;
	for (int i = 0; i < 4; i++)
	{
		aux.bytes[i] = buffer[i];
	}
	return aux.number;
}
void splitFloat(char *buffer, float value)
{
	FLOATUNION_t aux;
	aux.number = value;
	for (int i = 0; i < 4; i++)
	{
		buffer[i] = aux.bytes[i];
	}
}
float joinFloat(char *bufffer)
{
	FLOATUNION_t aux;
	for (int i = 0; i < 4; i++)
	{
		aux.bytes[i] = bufffer[i];
	}
	return aux.number;
}

long transform_data(char *date, char *hour)
{

	struct tm t;
	time_t t_day;

	char *token = strtok(date, "/");
	int counter = 0;

	t.tm_isdst = -1;

	while (token != NULL)
	{
		switch (counter)
		{
		case 0:
			t.tm_mday = atoi(token); //dia
			break;
		case 1:
			t.tm_mon = atoi(token) - 1; //mes
			break;
		case 2:
			t.tm_year = atoi(token) - 1900;
			break;
		}
		token = strtok(NULL, "/");
		counter++;
		
	}

	if (counter != 3) //incorret date
	{
		return 0;
	}

	counter = 0;
	token = strtok(hour, ":");
	while (token != NULL)
	{
		switch (counter)
		{
		case 0:
			t.tm_hour = atoi(token) + 1;
			break;
		case 1:
			t.tm_min = atoi(token);
			break;
		}
		token = strtok(NULL, ":");
		counter++;
	}

	t.tm_sec = 00;
	t_day = mktime(&t);
	return (long)t_day;
}