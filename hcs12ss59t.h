/*
 * HCS12SS59T AVR Library
 *   Copyright (c) 2017 Markus Koch <markus@notsyncing.net>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Thanks to qrti on GitHub for his research into this display!
 *   https://github.com/qrti/VFD-HCS-12SS59T
 */

#ifndef HCS12SS59T_H
#define HCS12SS59T_H

#include <avr/io.h>
#include <util/delay.h>

/* AVR hardware configuration */
#define HCS12SS59T_SPI_DDR DDRB
#define HCS12SS59T_SPI_PORT PORTB
#define HCS12SS59T_SPI_MOSI PB3
#define HCS12SS59T_SPI_SCK PB5
#define HCS12SS59T_SPI_SS_N PB2
#define HCS12SS59T_SPI_RESET_N PB1

/* Display commands and information */
#define HCS12SS59T_DCRAM_WR 0x10 /* ccccaaaa dddddddd dddddddd .. */
#define HCS12SS59T_CGRAM_WR 0x20 /* ccccaaaa dddddddd dddddddd .. */
#define HCS12SS59T_ADRAM_WR 0x30 /* ccccaaaa ******dd ******dd .. */
#define HCS12SS59T_DUTY 0x50 /* ccccdddd (dddd 0 to 15)*/
#define HCS12SS59T_NUMDIGIT 0x60 /* ccccdddd */
#define HCS12SS59T_LIGHTS 0x70 /* cccc**dd */

#define HCS12SS59T_LINORM 0x00 /* Display normal */
#define HCS12SS59T_LIOFF 0x01 /* Display off */
#define HCS12SS59T_LION 0x02 /* Display all on (test) */

#define HCS12SS59T_NUMDIGITS 12 /* Display digit width */
#define HCS12SS59T_DEFAULT_BRIGHTNESS 4 /* Default brightness */

/* Macros */
#define HCS12SS59T_SELECT {HCS12SS59T_SPI_PORT &= ~(1 << HCS12SS59T_SPI_SS_N);\
	_delay_us(1); /* > 250 ns */}
#define HCS12SS59T_DESELECT {HCS12SS59T_SPI_PORT |= (1 << HCS12SS59T_SPI_SS_N);\
	_delay_us(1); /* > 250 ns */}

/* User functions */
void hcs12ss59t_init();
void hcs12ss59t_reset();
void hcs12ss59t_set_text(char *text);
void hcs12ss59t_set_buffer(char *data);
void hcs12ss59t_set_brightness(char brightness);
void hcs12ss59t_set_lights(char lights);
void hcs12ss59t_set_character(int addr, char *data, int nchar);
void hcs12ss59t_set_progress_peak(uint8_t top, uint8_t bottom, uint8_t peak_top, uint8_t peak_bottom);
#define hcs12ss59t_set_progress(top,bottom) hcs12ss59t_set_progress_peak(top, bottom, 0, 0)
void hcs12ss59t_en_progress();

/* Low level functions */
void hcs12ss59t_send_byte(char c);
char hcs12ss59t_get_char(char c);

#endif // HCS12SS59T_H
