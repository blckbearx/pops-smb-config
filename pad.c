#include <libpad.h>

char padBuf[256] __attribute__((aligned(64)));
char actAlign[6];
int actuators;

int waitPadReady(int port, int slot){
	int state;
	int lastState;
	char stateString[16];

	state = padGetState(port, slot);
	lastState = -1;
	while((state != PAD_STATE_STABLE) && (state != PAD_STATE_FINDCTP1)) {
		if (state != lastState) {
			padStateInt2String(state, stateString);
			//scr_printf("Please wait, pad(%d,%d) is in state %s\n", port, slot, stateString);
		}
		lastState = state;
		state=padGetState(port, slot);
	}
	// Were the pad ever 'out of sync'?
	if (lastState != -1) {
		//scr_printf("Pad OK!\n");
	}
	return 0;
}

int initializePad(int port, int slot){

	int ret;
	int modes;
	int i;

	waitPadReady(port, slot);

	// How many different modes can this device operate in?
	// i.e. get # entries in the modetable
	modes = padInfoMode(port, slot, PAD_MODETABLE, -1);
	//scr_printf("The device has %d modes\n", modes);

	if (modes > 0) {
		//scr_printf("( ");
		for (i = 0; i < modes; i++) {
			//scr_printf("%d ", padInfoMode(port, slot, PAD_MODETABLE, i));
		}
		//scr_printf(")");
	}

	//scr_printf("It is currently using mode %d\n", padInfoMode(port, slot, PAD_MODECURID, 0));

	// If modes == 0, this is not a Dual shock controller
	// (it has no actuator engines)
	if (modes == 0) {
		//scr_printf("This is a digital controller?\n");
		return 1;
	}

	// Verify that the controller has a DUAL SHOCK mode
	i = 0;
	do {
		if (padInfoMode(port, slot, PAD_MODETABLE, i) == PAD_TYPE_DUALSHOCK)
			break;
		i++;
	} while (i < modes);
	if (i >= modes) {
		//scr_printf("This is no Dual Shock controller\n");
		return 1;
	}

	// If ExId != 0x0 => This controller has actuator engines
	// This check should always pass if the Dual Shock test above passed
	ret = padInfoMode(port, slot, PAD_MODECUREXID, 0);
	if (ret == 0) {
		//scr_printf("This is no Dual Shock controller??\n");
		return 1;
	}

	//scr_printf("Enabling dual shock functions\n");

	// When using MMODE_LOCK, user cant change mode with Select button
	padSetMainMode(port, slot, PAD_MMODE_DUALSHOCK, PAD_MMODE_LOCK);

	waitPadReady(port, slot);
	//scr_printf("infoPressMode: %d\n", padInfoPressMode(port, slot));

	waitPadReady(port, slot);
	//scr_printf("enterPressMode: %d\n", padEnterPressMode(port, slot));

	waitPadReady(port, slot);
	actuators = padInfoAct(port, slot, -1, 0);
	//scr_printf("# of actuators: %d\n",actuators);

	if (actuators != 0) {
		actAlign[0] = 0;   // Enable small engine
		actAlign[1] = 1;   // Enable big engine
		actAlign[2] = 0xff;
		actAlign[3] = 0xff;
		actAlign[4] = 0xff;
		actAlign[5] = 0xff;

		waitPadReady(port, slot);
		//scr_printf("padSetActAlign: %d\n", padSetActAlign(port, slot, actAlign));
	}
	else {
		//scr_printf("Did not find any actuators.\n");
	}
	waitPadReady(port, slot);
	return 1;
}

int openPort(int port, int slot){
	int ret;
	if((ret = padPortOpen(port, slot, padBuf)) == 0) {
		return ret;
	}
	return 1;
}
