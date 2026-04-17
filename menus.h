/* menus.h */

enum menu_codes {
    MAIN_MENU,
    FILE_MENU,
    INFO_MENU,
    WRITE_MENU,
    EXIT_MENU,
    SMB_EDIT_MENU,
    IP_EDIT_MENU,
    KEYBOARD_MENU,
    READ_ERROR,
    READ_CORR_ERROR,
    WRITE_ERROR,
    EXIT_ERROR
};

enum smb_field {
    FIELD_IP_ADDRESS,
    FIELD_SHARE,
    FIELD_USERNAME,
    FIELD_PASSWORD
};

enum ipconf_field {
    FIELD_IPCONF_IP = 100,
    FIELD_IPCONF_NETMASK,
    FIELD_IPCONF_GATEWAY
};
