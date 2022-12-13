#include <Arduino.h>

int main(void)
{
	init();


#if defined(usbcon)
	usbdevice.attach();
#endif
	setup();
	for (;;) {
		loop();
		if (serialEventRun) serialEventRun();
	}
	return 0;
}

