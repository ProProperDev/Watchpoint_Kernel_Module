IMAGE_INSTALL += " watchpoint_module"
inherit extrausers
EXTRA_USERS_PARAMS = "usermod -P 0000 root;"
