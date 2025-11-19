#include <tamtypes.h>
#include <kernel.h>
#include <sifrpc.h>
#include <stdio.h>
#include <libpad.h>
#include <sbv_patches.h>
#include <string.h>
#include "pad.h"
#include "modules.h"
#include "display.h"
#include "math.h"
#include "menus.h"
#include "fileio.h"
#include "config.h"

int main(){
	int ret;
	int port, slot;					// controller port and slot.
	int menu, old_menu, last_menu, x, y;		// current menu, previous menu and cursor position (x, y).
	struct padButtonStatus buttons;
	u32 paddata;
	u32 old_pad = 0;
	u32 new_pad;
	app_state_t state;				// Single structure containing all application state

	x = y = 0;							// Initialize some variables...
	menu = old_menu = last_menu = MAIN_MENU;
	init_app_state(&state);				// Initialize all config data at once

	SifInitRpc(0);
	sbv_patch_disable_prefix_check();

	loadModules();		// Load IOP modules
	padInit(0);

	port = 0; // 0 -> Connector 1, 1 -> Connector 2
	slot = 0; // Always zero if not using multitap

	openPort(port, slot);
	initializePad(port, slot);

	initDisplay();
	displayMenu(menu, state.mcport, &state.smb, &state.ipconf, state.file_chosen);

	for (;;){
		ret = padRead(port, slot, &buttons); // port, slot, buttons

		if (ret != 0) {
			paddata = 0xffff ^ buttons.btns;

			new_pad = paddata & ~old_pad;
			old_pad = paddata;

			if(old_menu != menu){				// only draw the menu when it should change, if menu doen't change then it doesn't get drawn again.
				displayMenu(menu, state.mcport, &state.smb, &state.ipconf, state.file_chosen);
				old_menu = menu;
			}

			switch(menu){			// depending on the current menu, the functions vary. See display.c for al the menus.
				case MAIN_MENU:				// main menu, MC port selection.
					if((new_pad & PAD_UP) && y > 0) {		// moves the selection cursor up.
						y--;
						updateMain(y);
					}
					if((new_pad & PAD_DOWN) && y < 1) {				// moves the selection cursor down.
						y++;
						updateMain(y);
					}
					if(new_pad & PAD_CROSS) {						// If cross is pressed, depending on what option is selected it set's the MC port to either slot 1 or slot 2.
						state.mcport = y;  // y is either 0 or 1
						x = y = 0;						// resets the position of the cursor for the next screen.
						old_menu = last_menu = MAIN_MENU;		// saves the menu number.
						menu = FILE_MENU;						// changes to the next menu.
					}
					if(new_pad & PAD_SELECT) {		// if select is pressed it shows the info screen.
						old_menu = last_menu = MAIN_MENU;
						menu = INFO_MENU;
					}
					if(new_pad & PAD_START) {			// if start is pressed it takes you to the exit confirmation.
						x = y = 0;
						old_menu = last_menu = MAIN_MENU;
						menu = EXIT_MENU;
					}
					break;
				case FILE_MENU:			// file selection menu.
					if((new_pad & PAD_UP) && y > 0) {		// moves the selection cursor up.
						y--;
						updateSelectedFile(y);
					}
					if((new_pad & PAD_DOWN) && y < 1) {		// moves the selection cursor down.
						y++;
						updateSelectedFile(y);
					}
					if(new_pad & PAD_CROSS) {				// If cross is pressed, depending on the selected option, it will load and read the file and proceed to the next menu. If the file does not exist then it will show an error.
						int read_result;
						switch(y){
							case 0:
								strcpy(state.file_chosen, "SMBCONFIG.DAT");
								break;
							case 1:
								strcpy(state.file_chosen, "IPCONFIG.DAT");
								break;
						}
						sprintf(state.path, "mc%d:/POPSTARTER/%s", state.mcport, state.file_chosen);

						switch(y){
							case 0:
								read_result = read_smb_config(state.path, &state.smb);
								if (read_result == 0) {
									// Success
									x = y = 0;
									old_menu = last_menu = FILE_MENU;
									menu = SMB_EDIT_MENU;
								} else if (read_result == -2) {
									// Corrupt file
									x = y = 0;
									old_menu = last_menu = FILE_MENU;
									menu = READ_CORR_ERROR;
								} else {
									// File not found
									x = y = 0;
									old_menu = last_menu = FILE_MENU;
									menu = READ_ERROR;
								}
								break;
							case 1:
								read_result = read_ip_config(state.path, &state.ipconf);
								if (read_result == 0) {
									// Success
									x = y = 0;
									old_menu = last_menu = FILE_MENU;
									menu = IP_EDIT_MENU;
								} else if (read_result == -2) {
									// Corrupt file
									x = y = 0;
									old_menu = last_menu = FILE_MENU;
									menu = READ_CORR_ERROR;
								} else {
									// File not found
									x = y = 0;
									old_menu = last_menu = FILE_MENU;
									menu = READ_ERROR;
								}
								break;
						}
					}
					if(new_pad & PAD_CIRCLE) {		// if circle is pressed then it will go back to the previous menu.
						x = y = 0;
						state.mcport = 0;
						old_menu = last_menu = FILE_MENU;
						menu = MAIN_MENU;
					}
					break;
				case INFO_MENU:								// info menu
					if(new_pad & PAD_CIRCLE) {		// circle takes you back to the main menu
						x = y = 0;
						old_menu = last_menu = INFO_MENU;
						menu = MAIN_MENU;
					}
					break;
				case SMB_EDIT_MENU:			// SMB editing menu.
					if((new_pad & PAD_LEFT) && x > 0) {											// When the arrows are pressed, the UI gets updated.
						x--;
						updateSMB(&state.smb, x);
					}
					if((new_pad & PAD_RIGHT) && x < 4) {
						x++;
						updateSMB(&state.smb, x);
					}
					if(new_pad & PAD_R1) {														// R1, L1, R2 and L2 are the buttons responsible for changing the IP values. When pressed, the math gets done and the UI gets updated with the new values.
						if (x == 4) {
							plusOne(&state.smb.port, x);  // Port is a single int, not array
						} else {
							plusOne(state.smb.ip, x);
						}
						updateSMB(&state.smb, x);
					}
					if(new_pad & PAD_L1) {
						if (x == 4) {
							subsOne(&state.smb.port, x);
						} else {
							subsOne(state.smb.ip, x);
						}
						updateSMB(&state.smb, x);
					}
					if(new_pad & PAD_L2) {
						if (x == 4) {
							subsTen(&state.smb.port, x);
						} else {
							subsTen(state.smb.ip, x);
						}
						updateSMB(&state.smb, x);
					}
					if(new_pad & PAD_R2) {
						if (x == 4) {
							plusTen(&state.smb.port, x);
						} else {
							plusTen(state.smb.ip, x);
						}
						updateSMB(&state.smb, x);
					}
					if(new_pad & PAD_CIRCLE) {				// If circle is pressed the variables get reset and the previous menu gets loaded
						x = y = 0;
						init_smb_config(&state.smb);
						state.path[0] = '\0';
						state.file_chosen[0] = '\0';
						old_menu = last_menu = SMB_EDIT_MENU;
						menu = FILE_MENU;
					}
					if(new_pad & PAD_CROSS) {			// If cross gets pressed, the write confirmation menu gets shown
						x = y = 0;
						old_menu = last_menu = SMB_EDIT_MENU;
						menu = WRITE_MENU;
					}
					break;
				case IP_EDIT_MENU:			// IPCONFIG editing menu.
					if((new_pad & PAD_LEFT) && x > 0) {																		// When the arrows are pressed, the UI gets updated.
						x--;
						updateIPCONF(&state.ipconf, x, y);
					}
					if((new_pad & PAD_RIGHT) && x < 3) {
						x++;
						updateIPCONF(&state.ipconf, x, y);
					}
					if((new_pad & PAD_UP) && y > 0) {
						y--;
						updateIPCONF(&state.ipconf, x, y);
					}
					if((new_pad & PAD_DOWN) && y < 2) {
						y++;
						updateIPCONF(&state.ipconf, x, y);
					}
					if(new_pad & PAD_R1) {												// R1, L1, R2 and L2 are the buttons responsible for changing the IP values. When pressed, the math gets done and the UI gets updated with the new values.
						switch(y){
							case 0:
								plusOne(state.ipconf.ip, x);
								break;
							case 1:
								plusOne(state.ipconf.netmask, x);
								break;
							case 2:
								plusOne(state.ipconf.gateway, x);
								break;
						}
						updateIPCONF(&state.ipconf, x, y);
					}
					if(new_pad & PAD_L1) {
						switch(y){
							case 0:
								subsOne(state.ipconf.ip, x);
								break;
							case 1:
								subsOne(state.ipconf.netmask, x);
								break;
							case 2:
								subsOne(state.ipconf.gateway, x);
								break;
						}
						updateIPCONF(&state.ipconf, x, y);
					}
					if(new_pad & PAD_L2) {
						switch(y){
							case 0:
								subsTen(state.ipconf.ip, x);
								break;
							case 1:
								subsTen(state.ipconf.netmask, x);
								break;
							case 2:
								subsTen(state.ipconf.gateway, x);
								break;
						}
						updateIPCONF(&state.ipconf, x, y);
					}
					if(new_pad & PAD_R2) {
						switch(y){
							case 0:
								plusTen(state.ipconf.ip, x);
								break;
							case 1:
								plusTen(state.ipconf.netmask, x);
								break;
							case 2:
								plusTen(state.ipconf.gateway, x);
								break;
						}
						updateIPCONF(&state.ipconf, x, y);
					}
					if(new_pad & PAD_CIRCLE) {					// If circle is pressed the variables get reset and the previous menu gets loaded
						x = y = 0;
						init_ip_config(&state.ipconf);
						state.path[0] = '\0';
						state.file_chosen[0] = '\0';
						old_menu = last_menu = IP_EDIT_MENU;
						menu = FILE_MENU;
					}
					if(new_pad & PAD_CROSS) {			// If cross gets pressed, the write confirmation menu gets shown
						x = y = 0;
						old_menu = last_menu = IP_EDIT_MENU;
						menu = WRITE_MENU;
					}
					break;
				case WRITE_MENU:									// Write confirmation menu
					if((new_pad & PAD_UP) && y > 0) {		// Up and Down change the selection
						y--;
						updateYN(y);
					}
					if((new_pad & PAD_DOWN) && y < 1) {
						y++;
						updateYN(y);
					}
					if((new_pad & PAD_CROSS) && y == 0) {		// When X is pressed and the chosen option is Yes then it writes the new file
						int write_result;
						switch(last_menu){
							case SMB_EDIT_MENU:
								write_result = write_smb_config(state.path, &state.smb);
								if (write_result == 0) {
									old_menu = last_menu = WRITE_MENU;
									menu = SMB_EDIT_MENU;
								} else {
									x = y = 0;
									old_menu = last_menu = WRITE_MENU;
									menu = WRITE_ERROR;
								}
								break;
							case IP_EDIT_MENU:
								write_result = write_ip_config(state.path, &state.ipconf);
								if (write_result == 0) {
									old_menu = last_menu = WRITE_MENU;
									menu = IP_EDIT_MENU;
								} else {
									x = y = 0;
									old_menu = last_menu = WRITE_MENU;
									menu = WRITE_ERROR;
								}
								break;
						}
 					}
					if((new_pad & PAD_CIRCLE) || ((new_pad & PAD_CROSS) && y == 1)) {		// If either circle or the No option are chosen then it returns to the previous menu
						x = y = 0;
						switch(last_menu){
							case SMB_EDIT_MENU:
								old_menu = last_menu = WRITE_MENU;
								menu = SMB_EDIT_MENU;
								break;
							case IP_EDIT_MENU:
								old_menu = last_menu = WRITE_MENU;
								menu = IP_EDIT_MENU;
								break;
						}
					}
					break;
				case EXIT_MENU:										// Exit confirmation menu
					if((new_pad & PAD_UP) && y > 0) {			// Up and Down to choose either Yes or No.
						y--;
						updateExit(y);
					}
					if((new_pad & PAD_DOWN) && y < 2) {
						y++;
						updateExit(y);
					}
					if((new_pad & PAD_CROSS) && y == 0) {		// If Browser is selected and X is pressed then console launches the OSDSYS (Browser).
						Exit(0);
					}
					if((new_pad & PAD_CROSS) && y == 1) {		// If OPL is selected and X is pressed then console launches OpenPS2Loader.
						strcpy(state.file_chosen, "OPNPS2LD.ELF");
						if(checkFile("mc0:/OPL/OPNPS2LD.ELF")){
							padEnd();
							ExitElf("mc0:/OPL/OPNPS2LD.ELF", "mc0:/OPL/");
						}
						else if(checkFile("mc1:/OPL/OPNPS2LD.ELF")){
							padEnd();
							ExitElf("mc1:/OPL/OPNPS2LD.ELF", "mc1:/OPL/");
						}
						else if(checkFile("mc0:/APPS/OPNPS2LD.ELF")){
							padEnd();
							ExitElf("mc0:/APPS/OPNPS2LD.ELF", "mc0:/APPS/");
						}
						else if(checkFile("mc1:/APPS/OPNPS2LD.ELF")){
							padEnd();
							ExitElf("mc1:/APPS/OPNPS2LD.ELF", "mc1:/APPS/");
						}
						else{
							x = y = 0;
							old_menu = last_menu = EXIT_MENU;
							menu = EXIT_ERROR;
						}
					}
					if((new_pad & PAD_CROSS) && y == 2) {		// If wLE is selected and X is pressed then console launches wLaunchELF.
						strcpy(state.file_chosen, "BOOT.ELF or ULE.ELF");
						if(checkFile("mc0:/BOOT/BOOT.ELF")){
							padEnd();
							ExitElf("mc0:/BOOT/BOOT.ELF", "mc0:/BOOT/");
						}
						else if(checkFile("mc1:/BOOT/BOOT.ELF")){
							padEnd();
							ExitElf("mc1:/BOOT/BOOT.ELF", "mc1:/BOOT/");
						}
						else if(checkFile("mc0:/APPS/ULE.ELF")){
							padEnd();
							ExitElf("mc0:/APPS/ULE.ELF", "mc0:/APPS/");
						}
						else if(checkFile("mc1:/APPS/ULE.ELF")){
							padEnd();
							ExitElf("mc1:/APPS/ULE.ELF", "mc1:/APPS/");
						}
						else{
							x = y = 0;
							old_menu = last_menu = EXIT_MENU;
							menu = EXIT_ERROR;
						}
					}
					if(new_pad & PAD_CIRCLE) {		//If circle is pressed then it goes back to the main menu
						x = y = 0;
						old_menu = last_menu = EXIT_MENU;
						menu = MAIN_MENU;
					}
					break;
				case READ_ERROR:			//Error file does not exist dialog.
					if((new_pad & PAD_CIRCLE) || (new_pad & PAD_CROSS)) {		// Circle or X take you back to the previous menu.
						state.path[0] = '\0';
						state.file_chosen[0] = '\0';
						old_menu = last_menu = READ_ERROR;
						menu = FILE_MENU;
					}
					break;
				case READ_CORR_ERROR:			//Error file is corrupt dialog.
					if((new_pad & PAD_CIRCLE) || (new_pad & PAD_CROSS)) {		// Circle or X take you back to the previous menu.
						state.path[0] = '\0';
						state.file_chosen[0] = '\0';
						old_menu = last_menu = READ_CORR_ERROR;
						menu = FILE_MENU;
					}
					break;
				case WRITE_ERROR:			// Error can't write file dialog.
					if((new_pad & PAD_CIRCLE) || (new_pad & PAD_CROSS)) {		// Circle or X take you back to the previous menu.
						switch(state.file_chosen[0]){
							case 'S':
								old_menu = last_menu = WRITE_ERROR;
								menu = SMB_EDIT_MENU;
								break;
							case 'I':
								old_menu = last_menu = WRITE_ERROR;
								menu = IP_EDIT_MENU;
								break;
						}
					}
					break;
				case EXIT_ERROR:
					if((new_pad & PAD_CIRCLE) || (new_pad & PAD_CROSS)) {		// Circle or X take you back to the previous menu.
								old_menu = last_menu = EXIT_ERROR;
								menu = EXIT_MENU;
								break;
					}
					break;
			}
		}
	}
	return 0;
}
