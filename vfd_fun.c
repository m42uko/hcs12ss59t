#include "vfd_fun.h"
#include <stdlib.h>
#include <string.h>

#define VFD_ANIM_DELAY 20

void vfd_animate_to(char *text, enum VFD_FUN_ANIMATIONS animation)
{
	int i,j;
	int done = 0;
	static char last[HCS12SS59T_NUMDIGITS];

	switch (animation) {
	case ANIMATION_RANDOM:
		for (int j = 0; j < 25; ++j) {
			HCS12SS59T_SELECT;
			hcs12ss59t_send_byte(HCS12SS59T_DCRAM_WR);
			for (i = 0; i < HCS12SS59T_NUMDIGITS; ++i) {
				hcs12ss59t_send_byte(hcs12ss59t_get_char(rand() % 26 + 'A'));
			}
			HCS12SS59T_DESELECT;
			_delay_ms(VFD_ANIM_DELAY);
			PORTC ^= (1 << PC5);
		}

		hcs12ss59t_set_text(text);
		break;

	case ANIMATION_FLIP:
		while (!done) {
			for (int j = 0; j < 25; ++j) {
				done = 1;
				for (i = 0; i < HCS12SS59T_NUMDIGITS; ++i) {
					if (last[i] > text[i]) {
						while (hcs12ss59t_get_char(--last[i]) == 79) {
							if (last[i] == '?')
								break;
						}
						done = 0;
					} else if (last[i] < text[i]) {
						while (hcs12ss59t_get_char(++last[i]) == 79) {
							if (last[i] == '?')
								break;
						}
						done = 0;
					}
				}
				hcs12ss59t_set_text(last);
				_delay_ms(VFD_ANIM_DELAY);
				PORTC ^= (1 << PC5);
			}
		}
		break;

	case ANIMATION_SLIDE:
		for (j = 0; j < HCS12SS59T_NUMDIGITS; ++j) {
			for (i = 1; i < HCS12SS59T_NUMDIGITS; ++i) {
				last[i - 1] = last[i];
			}
			last[HCS12SS59T_NUMDIGITS-1] = text[j];
			hcs12ss59t_set_text(last);
			_delay_ms(VFD_ANIM_DELAY);
			PORTC ^= (1 << PC5);
		}
		break;

	default:
		hcs12ss59t_set_text(text);
		PORTC ^= (1 << PC5);
		break;
	}

	for (i = 0; i < HCS12SS59T_NUMDIGITS; ++i) {
		last[i] = text[i];
	}
}
