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
	else if (c == 0) {
		c = ' ' - 48;
	}
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
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << DORD) | (1 << SPR0);

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
	hcs12ss59t_send_byte(cmd | (arg & 0x0f));
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
	hcs12ss59t_set_lights(HCS12SS59T_LINORM);
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

/**
 * @brief hcs12ss59t_set_character set user configurable characters
 * @param addr character number (0 - 15)
 * @param data data array, 2 bytes per character
 * @param nchar Amount of characters to update (auto increment)
 */
void hcs12ss59t_set_character(int addr, char *data, int nchar)
{
	char *ptr;

	nchar = nchar * 2;

	HCS12SS59T_SELECT;
	hcs12ss59t_send_byte(HCS12SS59T_CGRAM_WR | (addr & 0x0f));
	for (ptr = data; ptr < data + nchar; ++ptr) {
		hcs12ss59t_send_byte(*ptr);
	}
	HCS12SS59T_DESELECT;
}

/**
 * @brief hcs12ss59t_set_lights Set the display mode to on, off or test
 * @param lights value, use constants HCS12SS59T_LI*
 */
void hcs12ss59t_set_lights(char lights)
{
	hcs12ss59t_send_cmd(HCS12SS59T_LIGHTS, lights);
}

const char hcs12ss59t_bar_styles[2][5][2] = {
	{
		{0x84, 0x83}, // Full on
		{0x80, 0x00}, // 1
		{0x80, 0x80}, // 2
		{0x80, 0x81}, // 3
		{0x80, 0x83} // 4
	},
	{
		{0x48, 0x38}, // Full on
		{0x40, 0x00}, // 1
		{0x40, 0x20}, // 2
		{0x40, 0x30}, // 3
		{0x40, 0x38} // 4
	}
};

const char hcs12ss59t_tick_styles[2][5][2] = {
	{
		{0x80, 0x00}, // 1
		{0x00, 0x80}, // 2
		{0x00, 0x01}, // 3
		{0x00, 0x02}, // 4
		{0x04, 0x00} // 5
	},
	{
		{0x40, 0x00}, // 1
		{0x00, 0x20}, // 2
		{0x00, 0x10}, // 3
		{0x00, 0x08}, // 4
		{0x08, 0x00} // 5
	}
};

/**
 * @brief hcs12ss59t_set_progress Set the progress bar value
 * @param top Top progress bar value. 0 - HCS12SS59T_NUMDIGITS * 5
 * @param bottom Bottom progress bar value. 0 - HCS12SS59T_NUMDIGITS * 5
 */
void hcs12ss59t_set_progress_peak(uint8_t top, uint8_t bottom,
				  uint8_t peak_top, uint8_t peak_bottom)
{
	char charconf[HCS12SS59T_NUMDIGITS * 2];
	int full;
	int last;
	char *ptr;
	int row;

	// Sanitize input
	if (top > HCS12SS59T_NUMDIGITS * 5)
		top = HCS12SS59T_NUMDIGITS * 5;
	if (bottom > HCS12SS59T_NUMDIGITS * 5)
		bottom = HCS12SS59T_NUMDIGITS * 5;
	if (peak_top > HCS12SS59T_NUMDIGITS * 5)
		peak_top = HCS12SS59T_NUMDIGITS * 5;
	if (peak_bottom > HCS12SS59T_NUMDIGITS * 5)
		peak_bottom = HCS12SS59T_NUMDIGITS * 5;

	// Clear memory
	for (ptr = charconf; ptr < charconf + HCS12SS59T_NUMDIGITS * 2; ++ptr) {
		*ptr = 0;
	}

	// Calculate ticks
	if (peak_top != 0) {
		peak_top--;
		ptr = charconf + ((peak_top / 5) * 2);
		*(ptr++) |= hcs12ss59t_tick_styles[0][peak_top % 5][0];
		*ptr |= hcs12ss59t_tick_styles[0][peak_top % 5][1];
	}
	if (peak_bottom != 0) {
		peak_bottom--;
		ptr = charconf + ((peak_bottom / 5) * 2);
		*(ptr++) |= hcs12ss59t_tick_styles[1][peak_bottom % 5][0];
		*ptr |= hcs12ss59t_tick_styles[1][peak_bottom % 5][1];
	}

	// Calculate top memory
	for (row = 0; row < 2; ++row) {
		if (row == 0) {
			full = top / 5;
			last = top % 5;
		} else {
			full = bottom / 5;
			last = bottom % 5;
		}
		for (ptr = charconf; ptr < charconf + full * 2; ++ptr) {
			*(ptr++) |= hcs12ss59t_bar_styles[row][0][0];
			*ptr |= hcs12ss59t_bar_styles[row][0][1];
		}
		if (last != 0) {
			*(ptr++) |= hcs12ss59t_bar_styles[row][last][0];
			*ptr |= hcs12ss59t_bar_styles[row][last][1];
		}
	}

	hcs12ss59t_set_character(0, charconf, HCS12SS59T_NUMDIGITS);
}

/**
 * @brief hcs12ss59t_en_progress Prepare display for progress bar mode.
 */
void hcs12ss59t_en_progress()
{
	hcs12ss59t_set_buffer("\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B");
}
