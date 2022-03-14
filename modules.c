#include <loadfile.h>

void loadModules(void){
	SifLoadModule("rom0:SIO2MAN", 0, NULL);
	SifLoadModule("rom0:PADMAN", 0, NULL);
	SifLoadModule("rom0:MCMAN", 0, NULL);
}
