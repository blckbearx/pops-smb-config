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
#include "menus.h"
#include "fileio.h"
#include "config.h"

/* Keyboard layouts and constants */
static const char kb_lower[] =
    "abcdefghijklm"
    "nopqrstuvwxyz"
    "0123456789   ";

static const char kb_upper[] =
    "ABCDEFGHIJKLM"
    "NOPQRSTUVWXYZ"
    "!@#$%^&*()-_="
    "+[]{}|;:'\",./";

#define KB_MAX_IPADDR   21
#define KB_MAX_IPFIELD  15
#define KB_MAX_SHARE    80
#define KB_MAX_USERNAME 255
#define KB_MAX_PASSWORD 255

/* Helper functions for keyboard menu */
static const char *field_name(int f){
    switch(f){
        case FIELD_IP_ADDRESS:
            return "IP Address";
        case FIELD_SHARE:
            return "Share";
        case FIELD_USERNAME:
            return "User";
        case FIELD_PASSWORD:
            return "Password";
        case FIELD_IPCONF_IP:
            return "IP Address";
        case FIELD_IPCONF_NETMASK:
            return "Netmask";
        case FIELD_IPCONF_GATEWAY:
            return "Gateway";
        default:
            return "?";
    }
}

static int field_max(int f){
    switch(f){
        case FIELD_IP_ADDRESS:
            return KB_MAX_IPADDR;
        case FIELD_SHARE:
            return KB_MAX_SHARE;
        case FIELD_USERNAME:
            return KB_MAX_USERNAME;
        case FIELD_PASSWORD:
            return KB_MAX_PASSWORD;
        case FIELD_IPCONF_IP:
        case FIELD_IPCONF_NETMASK:
        case FIELD_IPCONF_GATEWAY:
            return KB_MAX_IPFIELD;
        default:
            return 0;
    }
}

static char *field_buf(smb_config_t *smb, enum smb_field f){
    switch(f){
        case FIELD_IP_ADDRESS:
            return smb->ip_address;
        case FIELD_SHARE:
            return smb->share;
        case FIELD_USERNAME:
            return smb->username;
        case FIELD_PASSWORD:
            return smb->password;
        default:
            return smb->share;
    }
}

static char *field_buf_ip(ip_config_t *ipconf, enum ipconf_field f){
    switch(f){
        case FIELD_IPCONF_IP:
            return ipconf->ip_address;
        case FIELD_IPCONF_NETMASK:
            return ipconf->netmask;
        case FIELD_IPCONF_GATEWAY:
            return ipconf->gateway;
        default:
            return ipconf->ip_address;
    }
}

static void kb_clamp(int *kb_x, int *kb_y, int upper_mode){
    int rows   = upper_mode ? KB_UPPER_ROWS : KB_LOWER_ROWS;
    int usable = upper_mode ? KB_UPPER_LEN  : KB_LOWER_LEN;
    if(*kb_y >= rows) *kb_y = rows - 1;
    while(*kb_y * KB_COLS + *kb_x >= usable && *kb_x > 0)
        (*kb_x)--;
}

int main(){
	int ret;
	int port, slot;					// controller port and slot.
	int menu, old_menu, last_menu, x, y;		// current menu, previous menu and cursor position (x, y).
	int kb_x, kb_y, kb_upper_mode;
    char kb_buf[256];
	struct padButtonStatus buttons;
	u32 paddata;
	u32 old_pad = 0;
	u32 new_pad;
	app_state_t state;				// Single structure containing all application state

	x = y = 0;							// Initialize some variables...
	kb_x = kb_y = kb_upper_mode = 0;
    kb_buf[0] = '\0';
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

			if(menu != KEYBOARD_MENU && old_menu != menu){				// only draw the menu when it should change, if menu doen't change then it doesn't get drawn again.
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
				case SMB_EDIT_MENU:
                    if((new_pad & PAD_UP) && y > 0){
                        y--;
                        updateSMBEdit(&state.smb, x, y);
                    }
                    if((new_pad & PAD_DOWN) && y < 4){
                        y++;
                        updateSMBEdit(&state.smb, x, y);
                    }

                    if(new_pad & PAD_CROSS){
                        if(y == 0){
                            state.editing_field = FIELD_IP_ADDRESS;
                        } else if(y == 1){
                            state.editing_field = FIELD_SHARE;
                        } else if(y == 2){
                            state.editing_field = FIELD_USERNAME;
                        } else if(y == 3){
                            state.editing_field = FIELD_PASSWORD;
                        } else if(y == 4){
                            x = y = 0;
                            old_menu = last_menu = SMB_EDIT_MENU;
                            menu = WRITE_MENU;
                        }

                        if(menu == SMB_EDIT_MENU && y >= 0 && y <= 3){
                            strcpy(kb_buf, field_buf(&state.smb, state.editing_field));
                            kb_x = kb_y = kb_upper_mode = 0;
                            old_menu = last_menu = SMB_EDIT_MENU;
                            menu = KEYBOARD_MENU;
                            displayKeyboard(field_name(state.editing_ip_field),
                                            kb_buf, kb_x, kb_y, kb_upper_mode);
                            old_menu = KEYBOARD_MENU;
                        }
                    }
                    if(new_pad & PAD_CIRCLE){
                        x = y = 0;
                        init_smb_config(&state.smb);
                        state.path[0] = '\0';
                        state.file_chosen[0] = '\0';
                        old_menu = last_menu = SMB_EDIT_MENU;
                        menu = FILE_MENU;
                    }
                    break;
				case KEYBOARD_MENU:
                {
                    int rows   = kb_upper_mode ? KB_UPPER_ROWS : KB_LOWER_ROWS;
                    int usable = kb_upper_mode ? KB_UPPER_LEN  : KB_LOWER_LEN;
                    int changed = 0;

                    if(new_pad & PAD_LEFT){
                        if(kb_x > 0){ kb_x--; changed = 1; }
                    }
                    if(new_pad & PAD_RIGHT){
                        int next = kb_y * KB_COLS + (kb_x + 1);
                        if(kb_x < KB_COLS - 1 && next < usable){ kb_x++; changed = 1; }
                    }
                    if(new_pad & PAD_UP){
                        if(kb_y > 0){ kb_y--; changed = 1; }
                    }
                    if(new_pad & PAD_DOWN){
                        if(kb_y < rows - 1){
                            int next = (kb_y + 1) * KB_COLS + kb_x;
                            if(next < usable){ kb_y++; changed = 1; }
                        }
                    }

                    if(new_pad & PAD_SELECT){
                        kb_upper_mode = !kb_upper_mode;
                        kb_clamp(&kb_x, &kb_y, kb_upper_mode);
                        changed = 1;
                    }

                    {
                        int current_field = (last_menu == IP_EDIT_MENU) ? state.editing_ip_field : state.editing_field;

                        if(new_pad & PAD_CROSS){
                            int len = strlen(kb_buf);
                            if(len < field_max(current_field)){
                                const char *layout = kb_upper_mode ? kb_upper : kb_lower;
                                char ch = layout[kb_y * KB_COLS + kb_x];
                                if(ch != ' '){
                                    kb_buf[len]     = ch;
                                    kb_buf[len + 1] = '\0';
                                    changed = 1;
                                }
                            }
                        }

                        if(new_pad & PAD_TRIANGLE){
                            int len = strlen(kb_buf);
                            if(len < field_max(current_field)){
                                kb_buf[len]     = ' ';
                                kb_buf[len + 1] = '\0';
                                changed = 1;
                            }
                        }

                        if(new_pad & PAD_SQUARE){
                            int len = strlen(kb_buf);
                            if(len > 0){
                                kb_buf[len - 1] = '\0';
                                changed = 1;
                            }
                        }
                    }

                    if(new_pad & PAD_START){
                        if(last_menu == SMB_EDIT_MENU){
                            strcpy(field_buf(&state.smb, state.editing_field), kb_buf);
                        } else if(last_menu == IP_EDIT_MENU){
                            strcpy(field_buf_ip(&state.ipconf, state.editing_ip_field), kb_buf);
                        }
                        x = y = 0;
                        menu = last_menu;
                        old_menu = KEYBOARD_MENU;
                    }

                    if(new_pad & PAD_CIRCLE){
                        x = y = 0;
                        menu = last_menu;
                    }

                    if(changed && menu == KEYBOARD_MENU){
                        updateKeyboard(kb_buf, kb_x, kb_y, kb_upper_mode);
                        old_menu = KEYBOARD_MENU;
                    }
                    break;
                }
				case IP_EDIT_MENU:            // IPCONFIG editing menu.
					if((new_pad & PAD_UP) && y > 0) {
						y--;
						updateIPCONF(&state.ipconf, x, y);
					}
					if((new_pad & PAD_DOWN) && y < 3) {
						y++;
						updateIPCONF(&state.ipconf, x, y);
					}
					if(new_pad & PAD_CROSS) {
						switch(y){
						case 0:
							state.editing_ip_field = FIELD_IPCONF_IP;
							break;
						case 1:
							state.editing_ip_field = FIELD_IPCONF_NETMASK;
							break;
						case 2:
							state.editing_ip_field = FIELD_IPCONF_GATEWAY;
							break;
						case 3:
							x = y = 0;
							old_menu = last_menu = IP_EDIT_MENU;
							menu = WRITE_MENU;
							break;
					}
						if(menu == IP_EDIT_MENU && y >= 0 && y <= 2){
							strcpy(kb_buf, field_buf_ip(&state.ipconf, state.editing_ip_field));
							kb_x = kb_y = kb_upper_mode = 0;
							old_menu = last_menu = IP_EDIT_MENU;
							menu = KEYBOARD_MENU;
							displayKeyboard(field_name(state.editing_ip_field),
									kb_buf, kb_x, kb_y, kb_upper_mode);
							old_menu = KEYBOARD_MENU;
						}
					}
					if(new_pad & PAD_CIRCLE) {                    // If circle is pressed the variables get reset and the previous menu gets loaded
						x = y = 0;
						init_ip_config(&state.ipconf);
						state.path[0] = '\0';
						state.file_chosen[0] = '\0';
						old_menu = last_menu = IP_EDIT_MENU;
						menu = FILE_MENU;
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
						if(checkFile("mc0:/APP_OPL/OPNPS2LD.ELF")){
							padEnd();
							ExitElf("mc0:/APP_OPL/OPNPS2LD.ELF", "mc0:/APP_OPL/");
						}
						else if(checkFile("mc1:/APP_OPL/OPNPS2LD.ELF")){
							padEnd();
							ExitElf("mc1:/APP_OPL/OPNPS2LD.ELF", "mc1:/APP_OPL/");
						}
						else if(checkFile("mc0:/OPL/OPNPS2LD.ELF")){
							padEnd();
							ExitElf("mc0:/OPL/OPNPS2LD.ELF", "mc0:/OPL/");
						}
						else if(checkFile("mc1:/OPL/OPNPS2LD.ELF")){
							padEnd();
							ExitElf("mc1:/OPL/OPNPS2LD.ELF", "mc1:/OPL/");
						}
						else{
							x = y = 0;
							old_menu = last_menu = EXIT_MENU;
							menu = EXIT_ERROR;
						}
					}
					if((new_pad & PAD_CROSS) && y == 2) {		// If wLE is selected and X is pressed then console launches wLaunchELF.
						strcpy(state.file_chosen, "BOOT.ELF");
						if(checkFile("mc0:/BOOT/BOOT.ELF")){
							padEnd();
							ExitElf("mc0:/BOOT/BOOT.ELF", "mc0:/BOOT/");
						}
						else if(checkFile("mc1:/BOOT/BOOT.ELF")){
							padEnd();
							ExitElf("mc1:/BOOT/BOOT.ELF", "mc1:/BOOT/");
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
