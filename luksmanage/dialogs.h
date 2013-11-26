#ifndef DIALOGS_H
#define DIALOGS_H
//make sure you have one max_len+1 bytes in passphrase buffer
int get_passwd(char * passphrase, int max_len, const char * message_img);
void show_bkground(const char * bkground_fname);
int get_yesno(int * perm, const char * bkground_fname);
#endif
