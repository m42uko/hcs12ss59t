/*
 * HCS12SS59T AVR Library
 *   Copyright (c) 2017 Markus Koch <markus@notsyncing.net>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "hcs12ss59t.h"

/**
 * @brief hcs12ss59t_get_char Converts an ASCII character to the code used by the display
 * @param c the character to be converted
 * @return the converted character
 */
char hcs12ss59t_get_char(char c)
{
	if (c >= '@' && c <= '_')
		c -= 48;
	else if (c >= ' ' && c <= '?')
		c += 16;
	else if (c >= 'a' && c <= 'z')
		c -= 80;
	else
		c = 79;
	return c;
}

/**
 * @brief hc12ss59t_init Initializes the I/O ports and SPI master
 */
void hcs12ss59t_init()
{
	HCS12SS59T_SPI_DDR = (1 << HCS12SS59T_SPI_MOSI) |
			(1 << HCS12SS59T_SPI_SCK) |
			(1 << HCS12SS59T_SPI_SS_N) |
			(1 << HCS12SS59T_SPI_RESET_N);
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0) |
			(1 << SPR1) | (1 << DORD);

	hcs12ss59t_reset();
}

/**
 * @brief hcs12ss59t_send_byte sends a byte over SPI
 * @param c the data byte
 */
void hcs12ss59t_send_byte(char c)
{
	SPDR = c;
	while(!(SPSR & (1<<SPIF)));
	_delay_us(8);
}

/**
 * @brief hcs12ss59t_send_cmd Selects the device, sends the command and deselects the device
 * @param cmd the command
 * @param arg any arguments. Use 0 if none are required
 */
void hcs12ss59t_send_cmd(char cmd, char arg)
{
	HCS12SS59T_SELECT;
	hcs12ss59t_send_byte(cmd | arg);
	HCS12SS59T_DESELECT;
}

/**
 * @brief hcs12ss59t_set_brightness Set the brightness of the VFD
 * @param brightness brightness level (0 - 15)
 */
void hcs12ss59t_set_brightness(char brightness)
{
	hcs12ss59t_send_cmd(HCS12SS59T_DUTY, brightness);
}

/**
 * @brief hcs12ss59t_reset Performs a reset of the display controller
 */
void hcs12ss59t_reset()
{
	/* Perform a HW reset */
	HCS12SS59T_SPI_PORT &= ~(1 << HCS12SS59T_SPI_RESET_N);
	_delay_us(1); /* > 250 ns */
	HCS12SS59T_SPI_PORT |= (1 << HCS12SS59T_SPI_RESET_N);
	_delay_us(1); /* > 250 ns */

	/* Configure controlller for VFD */
	hcs12ss59t_send_cmd(HCS12SS59T_NUMDIGIT, HCS12SS59T_NUMDIGITS);
	hcs12ss59t_send_cmd(HCS12SS59T_LIGHTS, HCS12SS59T_LINORM);
	hcs12ss59t_set_brightness(HCS12SS59T_DEFAULT_BRIGHTNESS);
}

/**
 * @brief hcs12ss59t_set_text update the text on the display
 * @param text a text buffer with size HCS12SS59T_NUMDIGITS
 */
void hcs12ss59t_set_text(char *text)
{
	char *ptr;

	HCS12SS59T_SELECT;
	hcs12ss59t_send_byte(HCS12SS59T_DCRAM_WR);
	for (ptr = text + HCS12SS59T_NUMDIGITS - 1; ptr >= text; --ptr) {
		hcs12ss59t_send_byte(hcs12ss59t_get_char(*ptr));
	}
	HCS12SS59T_DESELECT;
}

/**
 * @brief hcs12ss59t_set_text update the text on the display without applying any character transformations
 * @param data a buffer with size HCS12SS59T_NUMDIGITS
 */
void hcs12ss59t_set_buffer(char *data)
{
	char *ptr;

	HCS12SS59T_SELECT;
	hcs12ss59t_send_byte(HCS12SS59T_DCRAM_WR);
	for (ptr = data + HCS12SS59T_NUMDIGITS - 1; ptr >= data; --ptr) {
		hcs12ss59t_send_byte(*ptr);
	}
	HCS12SS59T_DESELECT;
}
