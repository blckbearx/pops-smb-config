/* display.h */

#include "config.h"

void initDisplay();
void displayMenu(int menu, int mcport, const smb_config_t *smb, const ip_config_t *ipconf, const char *file_chosen);
void updateMain(int y);
void updateSelectedFile(int y);
void updateSMB(const smb_config_t *smb, int x);
void updateIPCONF(const ip_config_t *ipconf, int x, int y);
void updateYN(int y);
void updateExit(int y);
