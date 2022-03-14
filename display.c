#include <debug.h>
#include <unistd.h>
#include "menus.h"

//80x28 characters in the screen
/* List of menus:
 * 0 - Main menu (MC Selection)
 * 1 - File selection menu
 * 2 - Info Screen
 * 10 - Write confirmation
 * 11 - Exit confirmation
 * 20 - SMBCONFIG editing
 * 21 - IPCONFIG editing
 * 30 - Error: File does not exist/cannot read
 * 31 - Error: can't save/file cannot be accessed for writing
 */

/* Menus are just text based, they get drawn whenever a new menu is chosen.
 * The update functions just check for the position of the cursor or the state of the variable sand update the content accordingly.
 */

void initDisplay(){
	init_scr();			// initialize scr for text output.
	scr_setCursor(0);	// Disable the cursor to prevent it from sticking.
	scr_clear();		// clear the screen just to be safe and then draw the first menu.
}

void displayMenu(int menu, int mcport, int ipa, int ipb, int ipc, int ipd, int ipe, int neta, int netb, int netc, int netd, int gatea, int gateb, int gatec, int gated, char *share, char *username, char *smbpassword, char *file_chosen){
	scr_setXY(0, 0);
	scr_clear();
	scr_printf("\n  POPSTARTER SMB CONFIG TOOL\n");
	scr_printf("________________________________________________________________________________");
	scr_setXY(0, 22);
	scr_printf("________________________________________________________________________________\n");
	scr_setXY(0, 3);
	switch(menu){
		case MAIN_MENU:
			scr_printf("\n\n  Choose Memory Card to work on:\n\n\n");
			scr_printf("    > Memory Card on Slot 1 <\n");
			scr_printf("\n");
			scr_printf("      Memory Card on Slot 2\n");
			scr_setXY(0, 24);
			scr_printf("  Use UP and DOWN to navigate                            Press TRIANGLE for info\n");
			scr_printf("  Press X to select an option                     Press START to exit to Browser");
			break;
		case FILE_MENU:
			scr_printf("\n\n  Choose the file you want to edit from Slot %d:\n\n\n", mcport+1);
			scr_printf("    > SMBCONFIG.DAT <\n");
			scr_printf("\n");
			scr_printf("      IPCONFIG.DAT\n");
			scr_setXY(0, 24);
			scr_printf("  Use UP and DOWN to navigate\n\n");
			scr_printf("  Press X to select an option                                 Press O to go back");
			break;
		case INFO_MENU:
			scr_printf("\n\n  In order to use this software you need to already have the\n");
			scr_printf("  POPSTARTER files on your Memory Card as the purpose of this software\n");
			scr_printf("  is to be able to update the configuration without the use of a PC.\n\n");
			scr_printf("  Due to the lack of text input, this software only allows you to change IP\n");
			scr_printf("  addresses (the SMB Share Name, User and Password must be already set).\n\n");
			scr_printf("  Thanks to pukko for the great pad.c example.");
			scr_setXY(0, 24);
			scr_printf("  Press O to go back");
			break;
		case SMB_EDIT_MENU:
			scr_printf("\n\n  Editing SMBCONFIG.DAT on Slot %d:\n\n\n", mcport+1);
			scr_printf("      IP Address: %03d.%03d.%03d.%03d:%03d\n", ipa, ipb, ipc, ipd, ipe);
			scr_printf("                  ^\n");
			scr_printf("      Share: %s\n\n      User: %s\n\n      Password: %s\n", share, username, smbpassword);
			scr_setXY(0, 24);
			scr_printf("  Use <- and -> to move the cursor                        Press X to save config\n");
			scr_printf("  R1 = +1 / L1 = -1 / R2 = +10 / L2 = -10                     Press O to go back");
			break;
		case IP_EDIT_MENU:
			scr_printf("\n\n  Editing IPCONFIG.DAT on Slot %d:\n\n\n", mcport+1);
			scr_printf("    > IP Address: %03d.%03d.%03d.%03d <\n", ipa, ipb, ipc, ipd);
			scr_printf("                  ^\n");
			scr_printf("      Netmask:    %03d.%03d.%03d.%03d\n", neta, netb, netc, netd);
			scr_printf("\n");
			scr_printf("      Gateway:    %03d.%03d.%03d.%03d\n", gatea, gateb, gatec, gated);
			scr_setXY(0, 24);
			scr_printf("  Use UP, DOWN, <- and -> to move the cursor              Press X to save config\n");
			scr_printf("  R1 = +1 / L1 = -1 / R2 = +10 / L2 = -10                     Press O to go back");
			break;
		case WRITE_MENU:
			scr_printf("\n\n  Do you want to save %s to the Memory Card on Slot %d?\n\n\n", file_chosen, mcport+1);
			scr_printf("    > Yes <\n\n      No\n");
			scr_setXY(0, 24);
			scr_printf("  Use UP and DOWN to navigate\n\n");
			scr_printf("  Press X to select an option                                 Press O to go back");
			break;
		case EXIT_MENU:
			scr_printf("\n\n  Do you want to exit to the Browser?\n\n\n");
			scr_printf("    > Yes <\n\n      No\n");
			scr_setXY(0, 24);
			scr_printf("  Use UP and DOWN to navigate\n\n");
			scr_printf("  Press X to select an option                                 Press O to go back");
			break;
		case READ_ERROR:
			scr_printf("\n\n  Error: Can't load %s from Memory Card on Slot %d.\n         The file does not exist or Memory Card is not present.\n\n\n", file_chosen, mcport+1);
			scr_printf("    > OK <\n");
			scr_setXY(0, 24);
			scr_printf("  Press X to confirm\n\n");
			scr_printf("  Press O to go back");
			break;
		case WRITE_ERROR:
			scr_printf("\n\n  Error: Can't write %s to Memory Card on Slot %d.\n         The file cannot be accessed.\n\n\n", file_chosen, mcport+1);
			scr_printf("    > OK <\n");
			scr_setXY(0, 24);
			scr_printf("  Press X to confirm\n\n");
			scr_printf("  Press O to go back");
			break;
	}
}

void updateMain(int y){
	scr_setXY(0, 8);
	switch (y){
		case 0:
			scr_printf("    > Memory Card on Slot 1 <\n");
			scr_printf("\n");
			scr_printf("      Memory Card on Slot 2  \n");
			break;
		case 1:
			scr_printf("      Memory Card on Slot 1  \n");
			scr_printf("\n");
			scr_printf("    > Memory Card on Slot 2 <\n");
			break;
	}
}

void updateSelectedFile(int y){
	scr_setXY(0, 8);
	switch (y){
		case 0:
			scr_printf("    > SMBCONFIG.DAT <\n");
			scr_printf("\n");
			scr_printf("      IPCONFIG.DAT  \n");
			break;
		case 1:
			scr_printf("      SMBCONFIG.DAT  \n");
			scr_printf("\n");
			scr_printf("    > IPCONFIG.DAT <\n");
			break;
	}
}

void updateSMB(int ipa, int ipb, int ipc, int ipd, int ipe, char *share, char *username, char *smbpassword, int x){
	scr_setXY(0, 8);
	scr_printf("      IP Address: %03d.%03d.%03d.%03d:%03d\n", ipa, ipb, ipc, ipd, ipe);
	switch(x){
		case 0:
			scr_printf("                   ^");
			break;
		case 1:
			scr_printf("                       ^");
			break;
		case 2:
			scr_printf("                           ^");
			break;
		case 3:
			scr_printf("                               ^");
			break;
		case 4:
			scr_printf("                                   ^");
			break;
	}
}

void updateIPCONF(int ipa, int ipb, int ipc, int ipd, int ipe, int neta, int netb, int netc, int netd, int gatea, int gateb, int gatec, int gated, int x, int y){
	scr_setXY(0, 8);
	if(y == 0){
		scr_printf("    > IP Address: %03d.%03d.%03d.%03d <\n", ipa, ipb, ipc, ipd);
		switch(x){
			case 0:
				scr_printf("                   ^\n");
				break;
			case 1:
				scr_printf("                       ^\n");
				break;
			case 2:
				scr_printf("                           ^\n");
				break;
			case 3:
				scr_printf("                               ^\n");
				break;
		}
	}
	else{
		scr_printf("      IP Address: %03d.%03d.%03d.%03d  \n", ipa, ipb, ipc, ipd);
		scr_printf("\n");
	}
	if(y == 1){
		scr_printf("    > Netmask:    %03d.%03d.%03d.%03d <\n", neta, netb, netc, netd);
		switch(x){
			case 0:
				scr_printf("                   ^\n");
				break;
			case 1:
				scr_printf("                       ^\n");
				break;
			case 2:
				scr_printf("                           ^\n");
				break;
			case 3:
				scr_printf("                               ^\n");
				break;
		}
	}
	else{
		scr_printf("      Netmask:    %03d.%03d.%03d.%03d\n", neta, netb, netc, netd);
		scr_printf("\n");
	}
	if(y == 2){
		scr_printf("    > Gateway:    %03d.%03d.%03d.%03d <\n", gatea, gateb, gatec, gated);
		switch(x){
			case 0:
				scr_printf("                   ^\n");
				break;
			case 1:
				scr_printf("                       ^\n");
				break;
			case 2:
				scr_printf("                           ^\n");
				break;
			case 3:
				scr_printf("                               ^\n");
				break;
		}
	}
	else{
		scr_printf("      Gateway:    %03d.%03d.%03d.%03d\n", gatea, gateb, gatec, gated);
		scr_printf("\n");
	}
}

void updateYN(int y){
	scr_setXY(0, 8);
	switch(y){
		case 0:
			scr_printf("    > Yes <\n\n      No\n");
			break;
		case 1:
			scr_printf("      Yes  \n\n    > No <\n");
			break;
	}
}
