#include <kernel.h>
#include <sifrpc.h>
#include <iopcontrol.h>
#include <loadfile.h>

void loadModules(void){
	SifLoadModule("rom0:SIO2MAN", 0, NULL);
	SifLoadModule("rom0:PADMAN", 0, NULL);
	SifLoadModule("rom0:MCMAN", 0, NULL);
}

void ExitElf(char *filename, char *directory){
	char *args[1];
	t_ExecData exec;
	SifLoadElf(filename, &exec);
	if (exec.epc > 0){
		SifInitRpc(0);
		while (!SifIopReset("", 0)){
		};
		while (!SifIopSync()){
		};
		SifInitRpc(0);
		if (directory != 0){
			args[0] = directory;
			ExecPS2((void *)exec.epc, (void *)exec.gp, 1, args);
		}
		else{
			ExecPS2((void *)exec.epc, (void *)exec.gp, 0, NULL);
		}
	}
}
