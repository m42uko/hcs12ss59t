#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

#include "hcs12ss59t.h"
#include "vfd_fun.h"

#define DELAY 1000
#define ANIMATION ANIMATION_SLIDE

int main() {
	DDRC = (1 << PC5);

	// Setup
	hcs12ss59t_init();
	hcs12ss59t_set_brightness(8);

	while (1) {
		vfd_animate_to(" HCS12SS59T ", rand() % 3);
		_delay_ms(DELAY);
		vfd_animate_to(" VF Display ", rand() % 3);
		_delay_ms(DELAY);
		vfd_animate_to("AVRC LIBRARY", rand() % 3);
		_delay_ms(DELAY);
		vfd_animate_to("10.0.100.234", rand() % 3);
		_delay_ms(DELAY);
		vfd_animate_to("ElPsyCongroo", rand() % 3);
		_delay_ms(DELAY);
	}
}
