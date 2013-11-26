#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <errno.h>
#include "dialogs.h"
#include "minui/minui.h"
#include "bootsync.h"
#define SDCARD_DEVICE		"/dev/block/mmcblk0p11"
#define DATA_DEVICE		"/dev/block/mmcblk0p10"
//#define DATA_DEVICE           "/dev/block/loop0"
//#define DATA_MOUNTPOINT	      "/data/testdata"
#define DATA_MOUNTPOINT             "/data"

#define SDCARD_MAPPER_NAME	"sdcard"
#define DATA_MAPPER_NAME	"data"

#define CRYPTSETUP		"/sbin/cryptsetup"

static void main_init(){
 //init graphics
  gr_init();
	
}
static void main_close(){
 gr_exit();
}
int main(int argc, char **argv, char **envp) {
	char passphrase[2048]={0};
	char buffer[1024];
	int isLuks=0;
	int ret=0;
	/* check for luks /data */
		/* check for luks file on sdcard */
		main_init(); /* initialize stuff */
		show_bkground(NULL);
		sleep(2);
		ret=get_yesno(&isLuks,"encryptyn");
		printf("Yesno said: %d with persistence %d\n",ret,isLuks);
		get_passwd(passphrase, 200, "enterpasswd");
		get_passwd(passphrase, 200, "verifypasswd");
		show_bkground("booting");
		sleep(2);
		show_bkground("enomatchpasswd");
		sleep(2);
                show_bkground("eshortpasswd");
                sleep(2);
                show_bkground("pleasewait");
                sleep(2);
		

		main_close();
		if(strlen(passphrase)>0)
		    printf("Here is your secret: %s\n",passphrase);
			
		/* attempt to luksOpen and mount /data */
	return 0;
}
