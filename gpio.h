#ifndef __GPIO_H__
#define __GPIO_H__

/*
typedef enum{
	GPIO_HI = 1,
	GPIO_LO = 0
} gpiovalue_t;

typedef enum{
	GPIO_OUT = 1,
	GPIO_IN = 0 
} gpiodirection_t;
*/

#define GPIO_OUT "out"
#define GPIO_IN  "in"
#define GPIO_HI  "1"
#define GPIO_LO  "0"

void setup_gpio(int, char *);
void free_gpio(int);
void gpio_value(int, char *);
#endif
