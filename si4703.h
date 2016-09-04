#ifndef __SI4703_H__
#define __SI4703_H__

typedef char  u8_t;
typedef unsigned short u16_t;

void si4703_setup_2wire();
void si4703_read(int fd, u16_t *r);
void si4703_print_regs(u16_t *r);
void si4703_write_regs(int fd, u16_t *r);
void si4703_init(int fd);
void si4703_tune(int fd, u16_t chn);
void si4703_set_volume(int fd, u8_t val);
void si4703_seek_up(int fd);
void si4703_print_meta(int fd);
void si4703_mute(int fd);
void si4703_powerdown(int fd);
void si4704_volup(int fd);
void si4704_voldown(int fd);
void print_options(u8_t type);
void print_dial(u8_t per);
void remove_dot(char *str);
u16_t si4703_freq2chan(u16_t freq);
u16_t si4703_chan2freq(u16_t freq);
#endif
