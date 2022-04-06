#ifndef USERS_H
#define USERS_H

int read_users(FILE *fp);
int switch_user(void);

extern int CurrentUser;
extern char *UserName;

extern int sram_fd;
#define sram_setuser(x) ioctl(sram_fd, SR_SETUSER, x)
#define sram_getuser() ioctl(sram_fd, SR_GETUSER)
#define sram_setuserlist(x) ioctl(sram_fd, SR_SETUSERLIST, x)
#define sram_getuserlist(x) ioctl(sram_fd, SR_GETUSERLIST, x)
void update_user(void);

#endif
