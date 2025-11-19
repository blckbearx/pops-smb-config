#include <stdio.h>
#include <string.h>
#include "config.h"

void init_smb_config(smb_config_t *config) {
    int i;
    for (i = 0; i < 4; i++) {
        config->ip[i] = 0;
    }
    config->port = 0;
    config->share[0] = '\0';
    config->username[0] = '\0';
    config->password[0] = '\0';
}

void init_ip_config(ip_config_t *config) {
    int i;
    for (i = 0; i < 4; i++) {
        config->ip[i] = 0;
        config->netmask[i] = 0;
        config->gateway[i] = 0;
    }
}

void init_app_state(app_state_t *state) {
    init_smb_config(&state->smb);
    init_ip_config(&state->ipconf);
    state->mcport = 0;
    state->file_chosen[0] = '\0';
    state->path[0] = '\0';
}

int read_smb_config(const char *path, smb_config_t *config) {
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        return -1;  // File not found
    }

    // Read with field width limits and check return value
    if (fscanf(file, "%d.%d.%d.%d:%d %80s\n%255s\n%255s",
               &config->ip[0], &config->ip[1], &config->ip[2], &config->ip[3],
               &config->port, config->share, config->username, config->password) != 8) {
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

    if (fscanf(file, "%d.%d.%d.%d %d.%d.%d.%d %d.%d.%d.%d",
               &config->ip[0], &config->ip[1], &config->ip[2], &config->ip[3],
               &config->netmask[0], &config->netmask[1], &config->netmask[2], &config->netmask[3],
               &config->gateway[0], &config->gateway[1], &config->gateway[2], &config->gateway[3]) != 12) {
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

    fprintf(file, "%d.%d.%d.%d:%d %s\n%s\n%s",
            config->ip[0], config->ip[1], config->ip[2], config->ip[3],
            config->port, config->share, config->username, config->password);

    fclose(file);
    return 0;  // Success
}

int write_ip_config(const char *path, const ip_config_t *config) {
    FILE *file = fopen(path, "w");
    if (file == NULL) {
        return -1;  // Cannot write
    }

    fprintf(file, "%d.%d.%d.%d %d.%d.%d.%d %d.%d.%d.%d",
            config->ip[0], config->ip[1], config->ip[2], config->ip[3],
            config->netmask[0], config->netmask[1], config->netmask[2], config->netmask[3],
            config->gateway[0], config->gateway[1], config->gateway[2], config->gateway[3]);

    fclose(file);
    return 0;  // Success
}
