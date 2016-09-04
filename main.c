#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <errno.h>
#include <sys/types.h>
#include <string.h>

#include "si4703.h"
#include "gpio.h"

#define I2C_ADDR 0x10
#define RST	 4
#define SDIO 0

//----------- TODO------------------//
// Seek up and seek down improve
// RDS data reading
// Command handling and cleaning up
//----------------------------------//

u16_t si4703_regs[16] = {0};

int main(int argc, char argv[])
{
	int fd;
	char buff[2];
	char command[20] = {""};

	fd = open( "/dev/i2c-1",O_RDWR );
	if( fd < 0 ){
		printf("Error opening file: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	if( ioctl(fd, I2C_SLAVE, I2C_ADDR) < 0 ){
		printf("ioctl error");
		return EXIT_FAILURE;
	}

	printf("\033[1;31mInitializing device...\033[0m\n");
	si4703_init(fd);
	printf("\033[1;31mDone.\033[0m\n");
	// Clearing the screen
	printf("\033[2J");
	print_dial(0);
	while( strcmp(command, "0") != 0 ){
		//sleep(1);
		printf("\033[0;0H");
		print_options(0);
		printf("\033[K");
		printf("\033[1;34mOption: ");
		scanf("%s", command);
		//command[strlen(command) - 1] = '\0';
		if( strcmp(command, "1") == 0 ){
			printf("\033[K");
			printf("Freq.(Mhz): ");	
			scanf("%s", command);
			//printf("\033[K");
			//command[strlen(command) - 1] = '\0';
			//printf("%s\n", command);
			remove_dot(command);
			//printf("%s\n", command);
			//command[20] = {""};
			if( atoi(command) > 875 && atoi(command) < 1080 ){
				si4703_tune(fd, atoi(command));
				si4703_print_meta(fd);
				int range = 1080 - 875;
				int absval = atoi(command) - 875;
				//printf("%d\n", range);
				//printf("%d\n", absval);
				print_dial((absval * 100)/range);
			}else{
				printf("Invalid frequency\n");
			}
			command[0] = '\0'; 
		}

		if( strcmp(command, "4") == 0 ){
			printf("Mute toggle.\n");
			si4703_mute(fd);
			command[0] = '\0'; 
		}

		if( strcmp(command, "5") == 0 ){
			si4703_print_meta(fd);
			command[0] = '\0'; 
		}

		if( strcmp(command, "6") == 0 ){
			si4704_volup(fd);
			command[0] = '\0'; 
		}
		if( strcmp(command, "7") == 0 ){
			si4704_voldown(fd);
			command[0] = '\0'; 
		}
		printf("\033[0m");
		// Clearing the screen
		//printf("\033[2J");
		//si4703_seek_up(fd);
		//si4703_seek_up(fd);
		//si4703_seek_up(fd);
		//si4703_seek_up(fd);
		//si4703_seek_up(fd);
		//si4703_seek_up(fd);
		//si4703_tune(fd, 995);
		//si4703_tune(fd, 927);
		//si4703_read(fd, &si4703_regs[0]);
		//si4703_print_regs(&si4703_regs[0]);
	}
	//sleep(2);
	//while(1);
	si4703_powerdown(fd);
	free_gpio(4);
	free_gpio(0);


	close(fd);
	printf("\033[0m");
	// Clearing the screen
	printf("\033[2J");
	printf("\033[0;0H");

	return 0;
}

void remove_dot(char *str)
{
	char *dot = NULL;
	dot = strrchr(str, '.');
	if(dot != NULL && dot != '\0'){
		char *tmp = dot;	
		tmp++;
		*dot = *tmp;
		*tmp = '\0';
	}else{
		str = strcat(str, "0");
	}
}

void print_options(u8_t type)
{
	/*
	if(type == 0){
		printf("\033[1;33m=========================\n");
		printf("0) Quit\n");
		printf("1) Tune\n");
		printf("2) Seek up\n");
		printf("3) Seek down\n");
		printf("4) Mute\n");
		printf("5) Info.\n");
		printf("6) Volume +\n");
		printf("7) Volume -\n");
		printf("=========================\033[0m\n");
	}
	*/
	if(type == 0){
		printf("\033[1;33m");
		printf("+------------------------------------------------------------------------------+\n");
		printf("| (0) Quit | ");
		printf("(1) Tune | ");
		printf("(2) Seek up | ");
		printf("(3) Seek down | ");
		printf("(4) Mute |\n");
		printf("| (5) Info. | ");
		printf("(6) Volume + | ");
		printf("(7) Volume - |\n");
		printf("+------------------------------------------------------------------------------+");
		printf("\033[0m\n");
	}
}

void print_dial(u8_t per){
	int i;
	printf("\033[20;14H");
	printf("\033[K");
		printf("|");
	for(i=0;i<per/2;i++){
		printf("=");
	}
	for(i=0;i<(100 - per)/2;i++){
		printf("-");
	}
	printf("|\n");
}

void si4703_mute(int fd)
{
	si4703_read(fd, &si4703_regs[0]);
	si4703_regs[0x2] = si4703_regs[0x2] ^ 0x4000;
	si4703_write_regs(fd, &si4703_regs[0]);
}

void si4703_powerdown(int fd)
{
	si4703_regs[0x7] |= 1 << 14;
	si4703_write_regs(fd, &si4703_regs[0]);
	si4703_regs[0x4] |= 0x002A;
	si4703_write_regs(fd, &si4703_regs[0]);
	si4703_regs[0x2] &= 0xBFFF;
	si4703_write_regs(fd, &si4703_regs[0]);
	si4703_regs[0x2] = 0x0041;
	si4703_write_regs(fd, &si4703_regs[0]);
	usleep(100000);
}

void si4703_tune(int fd, int chn)
{
	//int chn = 883;
	chn *= 10;
	chn -= 8750;
	chn /= 20;

	si4703_regs[0x3] = 0x8000 | chn;
	si4703_write_regs(fd, &si4703_regs[0]);
	usleep(100000);
	si4703_regs[0x3] = 0x0000 | chn;
	si4703_write_regs(fd, &si4703_regs[0]);
}

void si4704_volup(int fd)
{
	si4703_read(fd, &si4703_regs[0]);
	u8_t cvol = si4703_regs[0x5] & 0x000F;
	cvol = ++cvol % 0xF;

	si4703_regs[0x5] |= cvol;
	si4703_write_regs(fd, &si4703_regs[0]);
}

void si4704_voldown(int fd)
{
	si4703_read(fd, &si4703_regs[0]);
	u8_t cvol = si4703_regs[0x5] & 0x000F;
	if(cvol > 0)
		--cvol;

	si4703_regs[0x5] = (si4703_regs[0x5] & 0xFFF0) + cvol;
	si4703_write_regs(fd, &si4703_regs[0]);
}

void si4703_print_meta(int fd)
{
	u16_t rchan, rssi;

	si4703_read(fd, &si4703_regs[0]);
	rchan = si4703_regs[0xB] & 0x03FF;
	rssi = si4703_regs[0xA] & 0x01FF;
	//printf("Channel value: %x\n", rchan);
	printf("\033[1;31m");
	printf("* Frequency: %3.1f Mhz\n", (rchan * 0.2) + 87.5);
	printf("* Channel value: %d\n", rchan);
	printf("* RSSI: %d\n", rssi & 0x00FF);
	printf("* Mode: %s\n", ((rssi & 0x0100) > 0) ? "Stereo" : "Mono");
	printf("\033[0m");
}

void si4703_seek_up(int fd)
{
	u16_t rssi, rchan;

	si4703_read(fd, &si4703_regs[0]);
	rchan = si4703_regs[0xB];
	si4703_regs[0x2] = si4703_regs[0x2] | (1 << 8);
	si4703_regs[0x2] = si4703_regs[0x2] | (1 << 7);
	si4703_write_regs(fd, &si4703_regs[0]);

	rssi = si4703_regs[0xA];
	printf("rchan = %x\n", rchan & 0x03FF);

	while( (rssi & 0x4000) == 0 ){
		//si4703_write_regs(fd, &si4703_regs[0]);
		printf("seek rchan = %x\n", rchan & 0x03FF);
		printf("seek rssi = %x\n", rssi);
		//si4703_write_regs(fd, &si4703_regs[0]);
		//int freq = ((rchan & 0x01FF) * 20) + 87.5;
		//printf("freq = %d\n", freq);
		//si4703_tune(fd, freq);
		//si4703_write_regs(fd, &si4703_regs[0]);
		usleep(1000000);	
		si4703_read(fd, &si4703_regs[0]);
		rssi = si4703_regs[0xA];
		rchan = si4703_regs[0xB];
	}

	
	si4703_regs[0x2] = si4703_regs[0x2] & 0xFEFF;
	si4703_write_regs(fd, &si4703_regs[0]);
}



void si4703_set_volume(int fd, u8_t val)
{
	//si4703_regs[0x5] = 0x000A;
	si4703_regs[0x5] = (0x0F & val);
	si4703_write_regs(fd, &si4703_regs[0]);
	si4703_read(fd, &si4703_regs[0]);
}

void si4703_init(int fd)
{
	u8_t buff[16] = {0};

	setup_gpio(SDIO, GPIO_OUT);
	setup_gpio(RST, GPIO_OUT);
	si4703_setup_2wire();

	si4703_read(fd, &si4703_regs[0]);
	//si4703_print_regs(&si4703_regs[0]);

	buff[10] = 0x81;	
	if(write( fd, buff, 12 ) < 12){
		printf("Error writing registers: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	usleep(500000);

	buff[0] = 0x40;	
	buff[1] = 0x01;	
	if(write( fd, buff, 2 ) < 2){
		printf("Error writing registers: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	usleep(100000);

	si4703_read(fd, &si4703_regs[0]);
	//si4703_print_regs(&si4703_regs[0]);

	//si4703_regs[0x5] = 0x000A;
	//si4703_write_regs(fd, &si4703_regs[0]);
	si4703_set_volume(fd, 0x9);
}

void si4703_read(int fd, u16_t *r)
{
	u8_t buff[32];

	if(read( fd, buff, 32 ) < 32){
		printf("Error reading registers: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	int i;
	int x = 0xA;
	for(i=0;i<16;i++){
		//printf("0x%x\n", buff[i]);
		*(r + x) = (u16_t)buff[i * 2] << 8 | buff[(i * 2) + 1];

		if( x == 0xF ){
			x = 0;
		}else{
			x++;
		}

		//printf("0x%x\n", *(r + i));
	}
}

void si4703_write_regs(int fd, u16_t *r)
{
	u8_t buff[12] = {0};

	int i, x = 0;
	for(i=0x2;i<0x8;i++){
		u16_t v = *(r + i);
		buff[x * 2]       = (u8_t)((v >> 8) & 0xFF); 
		buff[(x * 2) + 1] = (u8_t)(v & 0xFF);
		x++;
	}
	//printf("\n");
	
	if(write( fd, buff, 12 ) < 12){
		printf("Error writing registers: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
}

void si4703_print_regs(u16_t *r)
{
	int i;
	printf("=========================================\n");
	for(i=0;i<16;i++){
		printf("0x%x - 0x%x\n", i, *(r + i));
	}
	printf("=========================================\n");
}

void si4703_setup_2wire()
{
	gpio_value( SDIO, GPIO_LO );
	usleep(1000);
	gpio_value( RST, GPIO_LO );
	usleep(1000);
	gpio_value( RST, GPIO_HI );
	usleep(100000);
}

