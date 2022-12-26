/* display.h */

void initDisplay();
void displayMenu(int menu, int mcport, int ipa, int ipb, int ipc, int ipd, int ipe, int neta, int netb, int netc, int netd, int gatea, int gateb, int gatec, int gated, char *share, char *username, char *smbpassword, char *file_chosen);
void updateMain(int y);
void updateSelectedFile(int y);
void updateSMB(int ipa, int ipb, int ipc, int ipd, int ipe, char *share, char *username, char *smbpassword, int x);
void updateIPCONF(int ipa, int ipb, int ipc, int ipd, int ipe, int neta, int netb, int netc, int netd, int gatea, int gateb, int gatec, int gated, int x, int y);
void updateYN(int y);
void updateExit(int y);
