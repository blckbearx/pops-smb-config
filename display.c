#include <debug.h>
#include <unistd.h>
#include <string.h>
#include "menus.h"
#include "version.h"
#include "config.h"

//80x28 characters in the screen
/* List of menus (see menus.h for enum definitions):
 * MAIN_MENU - MC Selection
 * FILE_MENU - File selection menu
 * INFO_MENU - Info Screen
 * WRITE_MENU - Write confirmation
 * EXIT_MENU - Exit confirmation
 * SMB_EDIT_MENU - SMBCONFIG editing
 * IP_EDIT_MENU - IPCONFIG editing
 * READ_ERROR - File does not exist/cannot read
 * READ_CORR_ERROR - File is corrupt or incomplete
 * WRITE_ERROR - Can't save/file cannot be accessed for writing
 * EXIT_ERROR - Can't find executable
 */

/* Menus are just text based, they get drawn whenever a new menu is chosen.
 * The update functions just check for the position of the cursor or the state of the variables and update the content accordingly.
 */

/* defining the colours for the PS buttons and UI */
#define CIRCLE_RED 0x312cf5
#define CROSS_BLUE 0xc88a6b
#define TRIANGLE_GREEN 0x95dc2a
#define SQUARE_PINK 0xc38edb
#define WHITE 0xffffff
#define DARK_GREY 0x404040
#define GREY 0x909090
#define RED 0x0000ff

// Keyboard layout and dimensions
#define KB_COLS 13
#define KB_LOWER_LEN  36
#define KB_UPPER_LEN  53
#define KB_LOWER_ROWS  3
#define KB_UPPER_ROWS  4

static const char kb_lower[] =
    "abcdefghijklm"
    "nopqrstuvwxyz"
    "0123456789   ";

static const char kb_upper[] =
    "ABCDEFGHIJKLM"
    "NOPQRSTUVWXYZ"
    "!@#$%^&*()-_="
    "+[]{}|;:'\",./";

void initDisplay(){
	init_scr();			// initialize scr for text output.
	scr_setCursor(0);	// Disable the cursor to prevent it from sticking.
	scr_clear();		// clear the screen just to be safe and then draw the first menu.
}

void displayMenu(int menu, int mcport, const smb_config_t *smb, const ip_config_t *ipconf, const char *file_chosen){
	scr_setXY(0, 0);
	scr_clear();
	scr_setfontcolor(GREY);
	scr_printf("\n  POPSTARTER SMB CONFIG TOOL %s\n", VERSION);
	scr_setfontcolor(DARK_GREY);
	scr_printf("________________________________________________________________________________");
	scr_setXY(0, 22);
	scr_printf("________________________________________________________________________________\n");
	scr_setfontcolor(WHITE);
	scr_setXY(0, 3);
	switch(menu){
		case MAIN_MENU:
			scr_printf("\n\n  Choose Memory Card to work on:\n\n\n");
			scr_printf("    > Memory Card on Slot 1 <\n\n");
			scr_printf("      Memory Card on Slot 2\n");
			scr_setXY(0, 24);
			scr_printf("  Use the D-PAD to navigate                              Press SELECT for info");
			scr_setXY(0, 26);
			scr_printf("  Press ");
			scr_setfontcolor(CROSS_BLUE);
			scr_printf("X");
			scr_setfontcolor(WHITE);
			scr_printf(" to select an option                              Press START to exit");
			break;
		case FILE_MENU:
			scr_printf("\n\n  Choose the file you want to edit from Slot %d:\n\n\n", mcport+1);
			scr_printf("    > SMBCONFIG.DAT <\n\n");
			scr_printf("      IPCONFIG.DAT\n");
			scr_setXY(0, 24);
			scr_printf("  Use the D-PAD to navigate");
			scr_setXY(0, 26);
			scr_printf("  Press ");
			scr_setfontcolor(CROSS_BLUE);
			scr_printf("X");
			scr_setfontcolor(WHITE);
			scr_printf(" to select an option                               Press ");
			scr_setfontcolor(CIRCLE_RED);
			scr_printf("O");
			scr_setfontcolor(WHITE);
			scr_printf(" to go back");
			break;
		case INFO_MENU:
			scr_printf("\n\n  In order to use this software you need to already have the\n");
			scr_printf("  POPSTARTER files on your Memory Card as the purpose of this software\n");
			scr_printf("  is to be able to update the configuration without the use of a PC.\n\n");
			scr_printf("  Due to the lack of text input, this software only allows you to change IP\n");
			scr_printf("  addresses (the SMB Share Name, User and Password must be already set).\n\n");
			scr_printf("  Thanks to pukko for the great pad.c example.");
			scr_setXY((80-(8+strlen(VERSION)+2)), 19);
			scr_printf("Version %s", VERSION);
			scr_setXY(0, 24);
			scr_printf("  Press ");
			scr_setfontcolor(CIRCLE_RED);
			scr_printf("O");
			scr_setfontcolor(WHITE);
			scr_printf(" to go back");
			break;
		case SMB_EDIT_MENU:
            scr_printf("\n\n  Editing SMBCONFIG.DAT on Slot %d:\n\n\n", mcport+1);
            scr_printf("    > IP Address: %03d.%03d.%03d.%03d:%03d <\n", smb->ip[0], smb->ip[1], smb->ip[2], smb->ip[3], smb->port);
            scr_printf("                   ^\n");
            scr_printf("      Share:    %s\n\n", smb->share);
            scr_printf("      User:     %s\n\n", smb->username);
            scr_printf("      Password: %s\n\n\n", smb->password);
            scr_printf("      Save\n");
            scr_setXY(0, 24);
            scr_printf("  Use the D-PAD to navigate                                  Press ");
			scr_setfontcolor(CROSS_BLUE);
            scr_printf("X");
            scr_setfontcolor(WHITE);
            scr_printf(" to choose");
			scr_setXY(0, 26);
            scr_printf("  R1 = +1 / L1 = -1 / R2 = +10 / L2 = -10                   Press ");
			scr_setfontcolor(CIRCLE_RED);
            scr_printf("O");
            scr_setfontcolor(WHITE);
            scr_printf(" to go back");
            break;
		case IP_EDIT_MENU:
			scr_printf("\n\n  Editing IPCONFIG.DAT on Slot %d:\n\n\n", mcport+1);
			scr_printf("    > IP Address: %03d.%03d.%03d.%03d <\n",
			           ipconf->ip[0], ipconf->ip[1], ipconf->ip[2], ipconf->ip[3]);
			scr_printf("                   ^\n");
			scr_printf("      Netmask:    %03d.%03d.%03d.%03d\n",
			           ipconf->netmask[0], ipconf->netmask[1], ipconf->netmask[2], ipconf->netmask[3]);
			scr_printf("\n");
			scr_printf("      Gateway:    %03d.%03d.%03d.%03d\n",
			           ipconf->gateway[0], ipconf->gateway[1], ipconf->gateway[2], ipconf->gateway[3]);
			scr_setXY(0, 24);
			scr_printf("  Use the D-PAD to move the cursor                      Press ");
			scr_setfontcolor(CROSS_BLUE);
			scr_printf("X");
			scr_setfontcolor(WHITE);
			scr_printf(" to save config");
			scr_setXY(0, 26);
			scr_printf("  R1 = +1 / L1 = -1 / R2 = +10 / L2 = -10                   Press ");
			scr_setfontcolor(CIRCLE_RED);
			scr_printf("O");
			scr_setfontcolor(WHITE);
			scr_printf(" to go back");
			break;
		case WRITE_MENU:
			scr_printf("\n\n  Do you want to save %s to the Memory Card on Slot %d?\n\n\n", file_chosen, mcport+1);
			scr_printf("    > Yes <\n\n      No\n");
			scr_setXY(0, 24);
			scr_printf("  Use the D-PAD to navigate");
			scr_setXY(0, 26);
			scr_printf("  Press ");
			scr_setfontcolor(CROSS_BLUE);
			scr_printf("X");
			scr_setfontcolor(WHITE);
			scr_printf(" to select an option                                 Press ");
			scr_setfontcolor(CIRCLE_RED);
			scr_printf("O");
			scr_setfontcolor(WHITE);
			scr_printf(" to go back");
			break;
		case EXIT_MENU:
			scr_printf("\n\n  Choose the software you want to exit to:\n\n\n");
			scr_printf("    > Browser <\n\n      OpenPS2Loader  \n\n      wLaunchELF  \n");
			scr_setXY(0, 24);
			scr_printf("  Use the D-PAD to navigate");
			scr_setXY(0, 26);
			scr_printf("  Press ");
			scr_setfontcolor(CROSS_BLUE);
			scr_printf("X");
			scr_setfontcolor(WHITE);
			scr_printf(" to select an option                               Press ");
			scr_setfontcolor(CIRCLE_RED);
			scr_printf("O");
			scr_setfontcolor(WHITE);
			scr_printf(" to go back");
			break;
		case READ_ERROR:
			scr_setfontcolor(RED);
			scr_printf("\n\n  Error:");
			scr_setfontcolor(WHITE);
			scr_printf(" Can't load %s from Memory Card on Slot %d.\n         The file does not exist or Memory Card is not present.\n\n\n", file_chosen, mcport+1);
			scr_printf("    > OK <\n");
			scr_setXY(0, 24);
			scr_printf("  Press ");
			scr_setfontcolor(CROSS_BLUE);
			scr_printf("X");
			scr_setfontcolor(WHITE);
			scr_printf(" to confirm");
			scr_setXY(0, 26);
			scr_printf("  Press ");
			scr_setfontcolor(CIRCLE_RED);
			scr_printf("O");
			scr_setfontcolor(WHITE);
			scr_printf(" to go back");
			break;
		case READ_CORR_ERROR:
			scr_setfontcolor(RED);
			scr_printf("\n\n  Error:");
			scr_setfontcolor(WHITE);
			scr_printf(" Can't load %s from Memory Card on Slot %d.\n         The file is corrupted or incomplete.\n\n\n", file_chosen, mcport+1);
			scr_printf("    > OK <\n");
			scr_setXY(0, 24);
			scr_printf("  Press ");
			scr_setfontcolor(CROSS_BLUE);
			scr_printf("X");
			scr_setfontcolor(WHITE);
			scr_printf(" to confirm");
			scr_setXY(0, 26);
			scr_printf("  Press ");
			scr_setfontcolor(CIRCLE_RED);
			scr_printf("O");
			scr_setfontcolor(WHITE);
			scr_printf(" to go back");
			break;
		case WRITE_ERROR:
			scr_setfontcolor(RED);
			scr_printf("\n\n  Error:");
			scr_setfontcolor(WHITE);
			scr_printf(" Can't write %s to Memory Card on Slot %d.\n         The file cannot be accessed.\n\n\n", file_chosen, mcport+1);
			scr_printf("    > OK <\n");
			scr_setXY(0, 24);
			scr_printf("  Press ");
			scr_setfontcolor(CROSS_BLUE);
			scr_printf("X");
			scr_setfontcolor(WHITE);
			scr_printf(" to confirm");
			scr_setXY(0, 26);
			scr_printf("  Press ");
			scr_setfontcolor(CIRCLE_RED);
			scr_printf("O");
			scr_setfontcolor(WHITE);
			scr_printf(" to go back");
			break;
		case EXIT_ERROR:
			scr_setfontcolor(RED);
			scr_printf("\n\n  Error:");
			scr_setfontcolor(WHITE);
			scr_printf(" Can't load %s from either Memory Card.\n         The file does not exist or there are no Memory Cards present.\n\n\n", file_chosen);
			scr_printf("    > OK <\n");
			scr_setXY(0, 24);
			scr_printf("  Press ");
			scr_setfontcolor(CROSS_BLUE);
			scr_printf("X");
			scr_setfontcolor(WHITE);
			scr_printf(" to confirm");
			scr_setXY(0, 26);
			scr_printf("  Press ");
			scr_setfontcolor(CIRCLE_RED);
			scr_printf("O");
			scr_setfontcolor(WHITE);
			scr_printf(" to go back");
			break;
	}
}

void updateMain(int y){
	scr_setXY(0, 8);
	switch (y){
		case 0:
			scr_printf("    > Memory Card on Slot 1 <\n\n");
			scr_printf("      Memory Card on Slot 2  \n");
			break;
		case 1:
			scr_printf("      Memory Card on Slot 1  \n\n");
			scr_printf("    > Memory Card on Slot 2 <\n");
			break;
	}
}

void updateSelectedFile(int y){
	scr_setXY(0, 8);
	switch (y){
		case 0:
			scr_printf("    > SMBCONFIG.DAT <\n\n");
			scr_printf("      IPCONFIG.DAT  \n");
			break;
		case 1:
			scr_printf("      SMBCONFIG.DAT  \n\n");
			scr_printf("    > IPCONFIG.DAT <\n");
			break;
	}
}

void updateSMBEdit(const smb_config_t *smb, int x, int y){
    scr_setXY(0, 8);
    if(y == 0){
        scr_printf("    > IP Address: %03d.%03d.%03d.%03d:%03d <\n", smb->ip[0], smb->ip[1], smb->ip[2], smb->ip[3], smb->port);
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
    } else {
        scr_printf("      IP Address: %03d.%03d.%03d.%03d:%03d  \n", smb->ip[0], smb->ip[1], smb->ip[2], smb->ip[3], smb->port);
        scr_printf("                                    ");
    }

    scr_setXY(0, 10);
    if(y == 1)
        scr_printf("    > Share:    %.66s <\n", smb->share);
    else
        scr_printf("      Share:    %-68.68s\n", smb->share);

    scr_setXY(0, 12);
    if(y == 2)
        scr_printf("    > User:     %.66s <\n", smb->username);
    else
        scr_printf("      User:     %-68.68s\n", smb->username);

    scr_setXY(0, 14);
    if(y == 3)
        scr_printf("    > Password: %.66s <\n", smb->password);
    else
        scr_printf("      Password: %-68.68s\n", smb->password);

    scr_setXY(0, 17);
    if(y == 4)
        scr_printf("    > Save <\n");
    else
        scr_printf("      Save  \n");
}

void displayKeyboard(const char *field_name, const char *buf, int kb_x, int kb_y, int upper_mode)
{
    scr_setXY(0, 0);
    scr_clear();

    scr_setfontcolor(GREY);
    scr_printf("\n  POPSTARTER SMB CONFIG TOOL %s\n", VERSION);
    scr_setfontcolor(DARK_GREY);
    scr_printf("________________________________________________________________________________\n");

    scr_setXY(0, 4);
    scr_setfontcolor(WHITE);
    scr_printf("  Editing %s:\n\n", field_name);
    scr_printf("  > %-74s\n", buf);
    scr_printf("  ______________________________________________________________________________\n");

    scr_setfontcolor(GREY);
    if(upper_mode){
        scr_printf("  [ UPPERCASE / SYMBOLS ]\n\n");
	}
	else{
        scr_printf("  [ lowercase / numbers ]\n\n");
	}
	scr_setfontcolor(WHITE);

    {
        const char *layout = upper_mode ? kb_upper : kb_lower;
        int rows = upper_mode ? KB_UPPER_ROWS : KB_LOWER_ROWS;
        int usable = upper_mode ? KB_UPPER_LEN : KB_LOWER_LEN;
        int row, col;

        for(row = 0; row < rows; row++){
            scr_printf("  ");
            for(col = 0; col < KB_COLS; col++){
                int idx = row * KB_COLS + col;
                char ch = (idx < usable) ? layout[idx] : ' ';
                if(row == kb_y && col == kb_x)
                    scr_printf("[%c]", ch);
                else
                    scr_printf(" %c ", ch);
            }
            scr_printf("\n");
        }
    }

    scr_setXY(0, 22);
    scr_setfontcolor(DARK_GREY);
    scr_printf("________________________________________________________________________________");
	scr_setXY(0, 24);
    scr_setfontcolor(WHITE);
    scr_printf("  D-PAD: Move cursor              SELECT: Switch case              START: Save");
	scr_setXY(0, 26);
	scr_printf("  ");
    scr_setfontcolor(CROSS_BLUE);
    scr_printf("X");
    scr_setfontcolor(WHITE);
    scr_printf(": Type            ");
    scr_setfontcolor(SQUARE_PINK);
    scr_printf("[]");
    scr_setfontcolor(WHITE);
    scr_printf(": Backspace             ");
    scr_setfontcolor(TRIANGLE_GREEN);
    scr_printf("Tri");
    scr_setfontcolor(WHITE);
    scr_printf(": Space            ");
    scr_setfontcolor(CIRCLE_RED);
    scr_printf("O");
    scr_setfontcolor(WHITE);
    scr_printf(": Cancel");
}

void updateKeyboard(const char *buf, int kb_x, int kb_y, int upper_mode)
{
    scr_setXY(0, 6);
    scr_setfontcolor(WHITE);
    scr_printf("  > %-74s\n", buf);
    scr_printf("  ______________________________________________________________________________\n");

    scr_setXY(0, 9);
    scr_setfontcolor(GREY);
    if(upper_mode){
        scr_printf("  [ UPPERCASE / SYMBOLS ]\n\n");
	}
	else{
        scr_printf("  [ lowercase / numbers ]\n\n");
	}
	scr_setfontcolor(WHITE);

    {
        const char *layout = upper_mode ? kb_upper : kb_lower;
        int rows = upper_mode ? KB_UPPER_ROWS : KB_LOWER_ROWS;
        int usable = upper_mode ? KB_UPPER_LEN : KB_LOWER_LEN;
        int row, col;

        scr_setXY(0, 11);
        for(row = 0; row < rows; row++){
            scr_printf("  ");
            for(col = 0; col < KB_COLS; col++){
                int idx = row * KB_COLS + col;
                char ch = (idx < usable) ? layout[idx] : ' ';
                if(row == kb_y && col == kb_x)
                    scr_printf("[%c]", ch);
                else
                    scr_printf(" %c ", ch);
            }
            scr_printf("\n");
        }
    }
}

void updateIPCONF(const ip_config_t *ipconf, int x, int y){
	scr_setXY(0, 8);
	if(y == 0){
		scr_printf("    > IP Address: %03d.%03d.%03d.%03d <\n",
		           ipconf->ip[0], ipconf->ip[1], ipconf->ip[2], ipconf->ip[3]);
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
		scr_printf("      IP Address: %03d.%03d.%03d.%03d  \n",
		           ipconf->ip[0], ipconf->ip[1], ipconf->ip[2], ipconf->ip[3]);
		scr_printf("\n");
	}
	if(y == 1){
		scr_printf("    > Netmask:    %03d.%03d.%03d.%03d <\n",
		           ipconf->netmask[0], ipconf->netmask[1], ipconf->netmask[2], ipconf->netmask[3]);
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
		scr_printf("      Netmask:    %03d.%03d.%03d.%03d\n",
		           ipconf->netmask[0], ipconf->netmask[1], ipconf->netmask[2], ipconf->netmask[3]);
		scr_printf("\n");
	}
	if(y == 2){
		scr_printf("    > Gateway:    %03d.%03d.%03d.%03d <\n",
		           ipconf->gateway[0], ipconf->gateway[1], ipconf->gateway[2], ipconf->gateway[3]);
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
		scr_printf("      Gateway:    %03d.%03d.%03d.%03d\n",
		           ipconf->gateway[0], ipconf->gateway[1], ipconf->gateway[2], ipconf->gateway[3]);
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

void updateExit(int y){
	scr_setXY(0, 8);
	switch (y){
		case 0:
			scr_printf("    > Browser <\n\n      OpenPS2Loader  \n\n      wLaunchELF\n");
			break;
		case 1:
			scr_printf("      Browser  \n\n    > OpenPS2Loader <\n\n      wLaunchELF\n");
			break;
		case 2:
			scr_printf("      Browser  \n\n      OpenPS2Loader  \n\n    > wLaunchELF <\n");
			break;
	}
}
