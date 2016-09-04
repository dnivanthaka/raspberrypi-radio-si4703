all: radio

radio: gpio.o si4703.o
	gcc si4703.o gpio.o -o radio

si4703.o: si4703.c
	gcc -c si4703.c

gpio.o: gpio.c
	gcc -c gpio.c 


clean:
	rm -f *.o radio


