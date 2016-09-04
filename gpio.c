#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <errno.h>
#include <sys/types.h>

#include "gpio.h"

void gpio_value(int pin, char *val)
{
	char buff[80];

	char *dir = "/sys/class/gpio/gpio%d/value";	
	sprintf( buff, dir, pin );	
	FILE *e = fopen( buff, "w" );
	if( e == NULL ){
		printf("Error opening GPIO value: %s - %s\n", strerror(errno), buff);
		exit(EXIT_FAILURE);
		//return -1;
	}


	if( fputs(val, e) < 0){
		printf("Error writing GPIO value: %s - %d\n", strerror(errno), pin);
		exit(EXIT_FAILURE);
		//return -1;
	}

	fflush(e);
	fclose(e);
}

void setup_gpio(int pin, char *direc)
{
	char buff0[4];
	char *exp = "/sys/class/gpio/export";
	FILE *e = fopen(exp, "w");
	if( e == NULL ){
		printf("Error opening GPIO export: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
		//return -1;
	}

	sprintf( buff0, "%d", pin );
	if( fputs(buff0, e) < 0){
		printf("Error writing GPIO export: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
		//return -1;
	}

	fflush(e);
	fclose(e);

	// Setting GPIO direction
	char buff[80];

	char *dir = "/sys/class/gpio/gpio%d/direction";	
	sprintf( buff, dir, pin );	
	e = fopen( buff, "w" );
	if( e == NULL ){
		printf("Error opening GPIO direction: %s - %s\n", strerror(errno), buff);
		exit(EXIT_FAILURE);
		//return -1;
	}

	if( fputs(direc, e) < 0){
		printf("Error writing GPIO direction RST: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
		//return -1;
	}

	fclose(e);
}

void free_gpio(int pin)
{
	char buff[4];
	char *exp = "/sys/class/gpio/unexport";
	FILE *e = fopen(exp, "w");
	if( e == NULL ){
		printf("Error opening GPIO unexport: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
		//return -1;
	}

	sprintf( buff, "%d", pin );
	if(fputs(buff, e) < 0){
		printf("Error writing GPIO unexport RST: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
		//return -1;
	}

	fflush(e);
	fclose(e);
}
