#include <Arduino.h>
// Declared weak in Arduino.h to allow user redefinitions.

// Weak empty variant initialization function.
// may be redefined by variant files.

int main(void)
{
	init();


#if defined(usbcon)
	usbdevice.attach();
#endif
	setup();
	for (;;) {
		loop();
		/* if (serialeventrun) serialeventrun(); */
	}
	return 0;
}

