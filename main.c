#include <tamtypes.h>
#include <kernel.h>
#include <sifrpc.h>
#include <stdio.h>
#include <libpad.h>
#include <sbv_patches.h>
#include "pad.h"
#include "modules.h"
#include "display.h"
#include "math.h"
#include "menus.h"
#include "fileio.h"

#define MAX_PATH 260

int mcport;							// 0 for port 1 and 1 for port 2.
int ipa, ipb, ipc, ipd, ipe;		// IP numbers ipa.ipb.ipc.ipd:ipe
int neta, netb, netc, netd;			// Netmask numbers neta.netb.netc.netd
int gatea, gateb, gatec, gated;		// Gateway numbers gatea.gateb.gatec.gated
char share[80] = {'\0'};			// SAMBA Share name, according to spec max string length is 80 chars.
char username[255] = {'\0'}; 		// SAMBA Share user, according to spec max string length is 255 chars.
char smbpassword[255] = {'\0'}; 	// SAMBA Share password, I assume max string length is 255 (why would anyone use a longer password anyways).
char *file_chosen[14] = {'\0'};		// The chosen file to edit, either SMBCONFIG.DAT or IPCONFIG.DAT
char path[MAX_PATH] = {'\0'};		// Path to read from or write to

int main(){
	int ret;
	int port, slot;					// controller port and slot.
	int menu, old_menu, last_menu, x, y;		// current menu, previous menu and cursor position (x, y).
	struct padButtonStatus buttons;
	u32 paddata;
	u32 old_pad = 0;
	u32 new_pad;

	x = y = 0;							// Initialize some variables...
	menu = old_menu = last_menu = MAIN_MENU;
	ipa = ipb = ipc = ipd = ipe = 0;
	neta = netb = netc = netd = 0;
	gatea = gateb = gatec = gated = 0;
	mcport = 0;

	sbv_patch_disable_prefix_check();
	SifInitRpc(0);

	loadModules();		// Load IOP modules
	padInit(0);

	port = 0; // 0 -> Connector 1, 1 -> Connector 2
	slot = 0; // Always zero if not using multitap

	openPort(port, slot);
	initializePad(port, slot);

	initDisplay();
	displayMenu(menu, mcport, ipa, ipb, ipc, ipd, ipe, neta, netb, netc, netd, gatea, gateb, gatec, gated, share, username, smbpassword, *file_chosen);

	for (;;){
		ret = padRead(port, slot, &buttons); // port, slot, buttons

		if (ret != 0) {
			paddata = 0xffff ^ buttons.btns;

			new_pad = paddata & ~old_pad;
			old_pad = paddata;

			if(old_menu != menu){				// only draw the menu when it should change, if menu doen't change then it doesn't get drawn again.
				displayMenu(menu, mcport, ipa, ipb, ipc, ipd, ipe, neta, netb, netc, netd, gatea, gateb, gatec, gated, share, username, smbpassword, *file_chosen);
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
						switch(y){
							case 0:
								mcport = 0;
								break;
							case 1:
								mcport = 1;
								break;
						}
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
						FILE *loadedcfg;
						switch(y){
							case 0:
								*file_chosen = "SMBCONFIG.DAT";
								break;
							case 1:
								*file_chosen = "IPCONFIG.DAT";
								break;
						}
						sprintf(path, "mc%d:/POPSTARTER/%s", mcport, *file_chosen);
						loadedcfg = fopen(path, "r");
						if(loadedcfg != NULL){
							switch(y){
								case 0:
									fscanf(loadedcfg,"%d.%d.%d.%d:%d %s\n%s\n%s", &ipa, &ipb, &ipc, &ipd, &ipe, share, username, smbpassword);
									fclose(loadedcfg);
									x = y = 0;
									old_menu = last_menu = FILE_MENU;
									menu = SMB_EDIT_MENU;
									break;
								case 1:
									fscanf(loadedcfg,"%d.%d.%d.%d %d.%d.%d.%d %d.%d.%d.%d", &ipa, &ipb, &ipc, &ipd, &neta, &netb, &netc, &netd, &gatea, &gateb, &gatec, &gated);
									fclose(loadedcfg);
									x = y = 0;
									old_menu = last_menu = FILE_MENU;
									menu = IP_EDIT_MENU;
									break;
							}
						}
						else{
							x = y = 0;
							old_menu = last_menu = FILE_MENU;
							menu = READ_ERROR;
						}
					}
					if(new_pad & PAD_CIRCLE) {		// if circle is pressed then it will go back to the previous menu.
						x = y = 0;
						mcport = 0;
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
						updateSMB(ipa, ipb, ipc, ipd, ipe, share, username, smbpassword, x);
					}
					if((new_pad & PAD_RIGHT) && x < 4) {
						x++;
						updateSMB(ipa, ipb, ipc, ipd, ipe, share, username, smbpassword, x);
					}
					if(new_pad & PAD_R1) {														// R1, L1, R2 and L2 are the buttons responsible for changing the IP values. When pressed, the math gets done and the UI gets updated with the new values.
						plusOne(&ipa, &ipb, &ipc, &ipd, &ipe, x);
						updateSMB(ipa, ipb, ipc, ipd, ipe, share, username, smbpassword, x);
					}
					if(new_pad & PAD_L1) {
						subsOne(&ipa, &ipb, &ipc, &ipd, &ipe, x);
						updateSMB(ipa, ipb, ipc, ipd, ipe, share, username, smbpassword, x);
					}
					if(new_pad & PAD_L2) {
						subsTen(&ipa, &ipb, &ipc, &ipd, &ipe, x);
						updateSMB(ipa, ipb, ipc, ipd, ipe, share, username, smbpassword, x);
					}
					if(new_pad & PAD_R2) {
						plusTen(&ipa, &ipb, &ipc, &ipd, &ipe, x);
						updateSMB(ipa, ipb, ipc, ipd, ipe, share, username, smbpassword, x);
					}
					if(new_pad & PAD_CIRCLE) {				// If circle is pressed the variables get reset and the previous menu gets loaded
						x = y = 0;
						ipa = ipb = ipc = ipd = ipe = 0;
						share[0] = '\0';
						username[0] = '\0';
						smbpassword[0] = '\0';
						path[0] = '\0';
						file_chosen[0] = '\0';
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
						updateIPCONF(ipa, ipb, ipc, ipd, ipe, neta, netb, netc, netd, gatea, gateb, gatec, gated, x, y);
					}
					if((new_pad & PAD_RIGHT) && x < 3) {
						x++;
						updateIPCONF(ipa, ipb, ipc, ipd, ipe, neta, netb, netc, netd, gatea, gateb, gatec, gated, x, y);
					}
					if((new_pad & PAD_UP) && y > 0) {
						y--;
						updateIPCONF(ipa, ipb, ipc, ipd, ipe, neta, netb, netc, netd, gatea, gateb, gatec, gated, x, y);
					}
					if((new_pad & PAD_DOWN) && y < 2) {
						y++;
						updateIPCONF(ipa, ipb, ipc, ipd, ipe, neta, netb, netc, netd, gatea, gateb, gatec, gated, x, y);
					}
					if(new_pad & PAD_R1) {												// R1, L1, R2 and L2 are the buttons responsible for changing the IP values. When pressed, the math gets done and the UI gets updated with the new values.
						switch(y){
							case 0:
								plusOne(&ipa, &ipb, &ipc, &ipd, &ipe, x);
								break;
							case 1:
								plusOne(&neta, &netb, &netc, &netd, &ipe, x);
								break;
							case 2:
								plusOne(&gatea, &gateb, &gatec, &gated, &ipe, x);
								break;
						}
						updateIPCONF(ipa, ipb, ipc, ipd, ipe, neta, netb, netc, netd, gatea, gateb, gatec, gated, x, y);
					}
					if(new_pad & PAD_L1) {
						switch(y){
							case 0:
								subsOne(&ipa, &ipb, &ipc, &ipd, &ipe, x);
								break;
							case 1:
								subsOne(&neta, &netb, &netc, &netd, &ipe, x);
								break;
							case 2:
								subsOne(&gatea, &gateb, &gatec, &gated, &ipe, x);
								break;
						}
						updateIPCONF(ipa, ipb, ipc, ipd, ipe, neta, netb, netc, netd, gatea, gateb, gatec, gated, x, y);
					}
					if(new_pad & PAD_L2) {
						switch(y){
							case 0:
								subsTen(&ipa, &ipb, &ipc, &ipd, &ipe, x);
								break;
							case 1:
								subsTen(&neta, &netb, &netc, &netd, &ipe, x);
								break;
							case 2:
								subsTen(&gatea, &gateb, &gatec, &gated, &ipe, x);
								break;
						}
						updateIPCONF(ipa, ipb, ipc, ipd, ipe, neta, netb, netc, netd, gatea, gateb, gatec, gated, x, y);
					}
					if(new_pad & PAD_R2) {
						switch(y){
							case 0:
								plusTen(&ipa, &ipb, &ipc, &ipd, &ipe, x);
								break;
							case 1:
								plusTen(&neta, &netb, &netc, &netd, &ipe, x);
								break;
							case 2:
								plusTen(&gatea, &gateb, &gatec, &gated, &ipe, x);
								break;
						}
						updateIPCONF(ipa, ipb, ipc, ipd, ipe, neta, netb, netc, netd, gatea, gateb, gatec, gated, x, y);
					}
					if(new_pad & PAD_CIRCLE) {					// If circle is pressed the variables get reset and the previous menu gets loaded
						x = y = 0;
						ipa = ipb = ipc = ipd = ipe = 0;
						neta = netb = netc = netd = 0;
						gatea = gateb = gatec = gated = 0;
						share[0] = '\0';
						username[0] = '\0';
						smbpassword[0] = '\0';
						path[0] = '\0';
						file_chosen[0] = '\0';
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
						FILE *loadedcfg;
						loadedcfg = fopen(path, "r+");
						if(loadedcfg != NULL){
							switch(last_menu){
								case 20:
									fprintf(loadedcfg,"%d.%d.%d.%d:%d %s\n%s\n%s", ipa, ipb, ipc, ipd, ipe, share, username, smbpassword);
									fclose(loadedcfg);
									old_menu = last_menu = WRITE_MENU;
									menu = SMB_EDIT_MENU;
									break;
								case 21:
									fprintf(loadedcfg,"%d.%d.%d.%d %d.%d.%d.%d %d.%d.%d.%d", ipa, ipb, ipc, ipd, neta, netb, netc, netd, gatea, gateb, gatec, gated);
									fclose(loadedcfg);
									old_menu = last_menu = WRITE_MENU;
									menu = IP_EDIT_MENU;
									break;
							}
						}
						else{								// If there is an issue writing the file then the error screen is shown
							x = y = 0;
							old_menu = last_menu = WRITE_MENU;
							menu = WRITE_ERROR;
						}
 					}
					if((new_pad & PAD_CIRCLE) || ((new_pad & PAD_CROSS) && y == 1)) {		// If either circle or the No option are chosen then it returns to the previous menu
						x = y = 0;
						switch(last_menu){
							case 20:
								old_menu = last_menu = WRITE_MENU;
								menu = SMB_EDIT_MENU;
								break;
							case 21:
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
						*file_chosen = "OPNPS2LD.ELF";
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
						*file_chosen = "BOOT.ELF or ULE.ELF";
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
					if((new_pad & PAD_CIRCLE) || ((new_pad & PAD_CROSS) && y == 1)) {		//If circle or No are selected then it goes back to the main menu
						x = y = 0;
						old_menu = last_menu = EXIT_MENU;
						menu = MAIN_MENU;
					}
					break;
				case READ_ERROR:			//Error file does not exist dialog.
					if((new_pad & PAD_CIRCLE) || (new_pad & PAD_CROSS)) {		// Circle or X take you back to the previous menu.
						path[0] = '\0';
						file_chosen[0] = '\0';
						old_menu = last_menu = READ_ERROR;
						menu = FILE_MENU;
					}
					break;
				case WRITE_ERROR:			// Error can't write file dialog.
					if((new_pad & PAD_CIRCLE) || (new_pad & PAD_CROSS)) {		// Circle or X take you back to the previous menu.
						switch(*file_chosen[0]){
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
