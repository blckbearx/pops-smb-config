/* config.h */

#ifndef CONFIG_H
#define CONFIG_H

// Structure for SMB configuration
typedef struct {
    int ip[4];           // IP address octets (ipa, ipb, ipc, ipd)
    int port;            // Port number (ipe)
    char share[81];      // SMB share name
    char username[256];  // SMB username
    char password[256];  // SMB password
} smb_config_t;

// Structure for IP configuration
typedef struct {
    int ip[4];           // IP address octets
    int netmask[4];      // Netmask octets
    int gateway[4];      // Gateway octets
} ip_config_t;

// Structure to hold all application state
typedef struct {
    smb_config_t smb;
    ip_config_t ipconf;
    int mcport;          // Memory card port (0 or 1)
    char file_chosen[20];
    char path[260];
} app_state_t;

// Helper functions for config management
void init_smb_config(smb_config_t *config);
void init_ip_config(ip_config_t *config);
void init_app_state(app_state_t *state);

// File I/O functions
int read_smb_config(const char *path, smb_config_t *config);
int read_ip_config(const char *path, ip_config_t *config);
int write_smb_config(const char *path, const smb_config_t *config);
int write_ip_config(const char *path, const ip_config_t *config);

#endif /* CONFIG_H */
