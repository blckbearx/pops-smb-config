#include <stdio.h>
#include <string.h>
#include "config.h"

void init_smb_config(smb_config_t *config) {
    config->ip_address[0] = '\0';
    config->share[0] = '\0';
    config->username[0] = '\0';
    config->password[0] = '\0';
}

void init_ip_config(ip_config_t *config) {
    config->ip_address[0] = '\0';
    config->netmask[0] = '\0';
    config->gateway[0] = '\0';
}

void init_app_state(app_state_t *state) {
    init_smb_config(&state->smb);
    init_ip_config(&state->ipconf);
    state->mcport = 0;
    state->file_chosen[0] = '\0';
    state->path[0] = '\0';
    state->editing_ip_field = FIELD_IPCONF_IP;
}

int read_smb_config(const char *path, smb_config_t *config) {
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        return -1;  // File not found
    }

    // Read with field width limits and check return value
    if (fscanf(file, "%21s %80s\n%255s\n%255s",
               config->ip_address,
               config->share, config->username, config->password) != 4) {
        fclose(file);
        return -2;  // Corrupt file
    }

    fclose(file);
    return 0;  // Success
}

int read_ip_config(const char *path, ip_config_t *config) {
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        return -1;  // File not found
    }

    if (fscanf(file, "%15s %15s %15s",
               config->ip_address,
               config->netmask,
               config->gateway) != 3) {
        fclose(file);
        return -2;  // Corrupt file
    }

    fclose(file);
    return 0;  // Success
}

int write_smb_config(const char *path, const smb_config_t *config) {
    FILE *file = fopen(path, "w");
    if (file == NULL) {
        return -1;  // Cannot write
    }

    fprintf(file, "%s %s\n%s\n%s",
            config->ip_address, config->share, config->username, config->password);

    fclose(file);
    return 0;  // Success
}

int write_ip_config(const char *path, const ip_config_t *config) {
    FILE *file = fopen(path, "w");
    if (file == NULL) {
        return -1;  // Cannot write
    }

    fprintf(file, "%s %s %s",
            config->ip_address, config->netmask, config->gateway);

    fclose(file);
    return 0;  // Success
}
