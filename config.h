/* config.h */

#include "menus.h"

#ifndef CONFIG_H
#define CONFIG_H

// Structure for SMB configuration
typedef struct {
    char ip_address[22]; // SMB IP address and port string (e.g. 192.168.0.1:445)
    char share[81];      // SMB share name
    char username[256];  // SMB username
    char password[256];  // SMB password
} smb_config_t;

// Structure for IP configuration
typedef struct {
    char ip_address[16]; // IP address string (e.g. 192.168.0.1)
    char netmask[16];    // Netmask string (e.g. 255.255.255.0)
    char gateway[16];    // Gateway string (e.g. 192.168.0.254)
} ip_config_t;

// Structure to hold all application state
typedef struct {
    smb_config_t smb;
    ip_config_t ipconf;
    int mcport;          // Memory card port (0 or 1)
    char file_chosen[20];
    char path[260];
    enum smb_field editing_field;
    enum ipconf_field editing_ip_field;
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
