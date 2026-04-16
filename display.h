/* display.h */

#include "config.h"

#define KB_COLS       13
#define KB_LOWER_ROWS  3
#define KB_UPPER_ROWS  4
#define KB_LOWER_LEN  36
#define KB_UPPER_LEN  53

void initDisplay();
void displayMenu(int menu, int mcport, const smb_config_t *smb, const ip_config_t *ipconf, const char *file_chosen);
void updateMain(int y);
void updateSelectedFile(int y);
void updateSMB(const smb_config_t *smb, int x);
void updateIPCONF(const ip_config_t *ipconf, int x, int y);
void updateYN(int y);
void updateExit(int y);
void updateSMBEdit(const smb_config_t *smb, int x, int y);
void displayKeyboard(const char *field_name, const char *buf, int kb_x, int kb_y, int upper_mode);
void updateKeyboard(const char *buf, int kb_x, int kb_y, int upper_mode);
