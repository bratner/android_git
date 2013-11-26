#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <linux/input.h>
#include <errno.h>
#include "dialogs.h"
#include "minui/minui.h"
#include "bootsync.h"
#define SDCARD_DEVICE		"/dev/block/mmcblk0p11"
#define SDCARD_MOUNTPOINT	"/sdcard"
#define DATA_DEVICE		"/dev/block/mmcblk0p10"
//#define DATA_DEVICE           "/dev/block/loop0"
//#define DATA_MOUNTPOINT	      "/data/testdata"
#define DATA_MOUNTPOINT             "/data"

#define SDCARD_MAPPER_NAME	"sdcard"
#define DATA_MAPPER_NAME	"data"

#define CRYPTSETUP		"/sbin/cryptsetup"
#define DONTBOTHER		"/data/.dontbotherme"
#define GETRETCODE(x) ((((short int)(x)) & 0xff00) >> 8)
#define MINPASSWDLEN 12
#define MAXPASSWDLEN 32
#define BADRETRIES 3

static void main_init(){
  gr_init();
}

static void main_close(){
 gr_exit();
}

int main(int argc, char **argv, char **envp) {
	char passphrase[2048]={0};
	char passver[2048]={0};
	char buffer[1024];
	int persist;
	int ret, i, j;
	struct stat filestat;
  	main_init();      
	show_bkground(NULL);
        /* mount data */
	snprintf(buffer, sizeof(buffer)-1, "/sbin/sh -c '/sbin/mount -t ext4 -o nosuid,nodev,noatime,barrier=1,data=ordered,noauto_da_alloc,discard %s %s'",
 		 DATA_DEVICE, DATA_MOUNTPOINT);
        ret=system(buffer);
	printf("Mount returned %x\n",ret);
	ret=GETRETCODE(ret);
	//ret=((short int)ret && 0xff00) >> 8;
	printf("Mount returned %x\n",ret);
        if(ret==0){ /* /data mounted successfully */
		/* test for file-flag */
		if(stat(DONTBOTHER,&filestat)!=0){
		/* offer to encrypt */
		     ret=get_yesno(&persist,"encryptyn");
		     if(ret==1){
			j=0; /* start retry count */
                        do{ /* password verification loop */
				i=0;
				do{ /* password length loop */
					   get_passwd(passphrase, MAXPASSWDLEN, "enterpasswd"); 
					   i++;
					   if(strlen(passphrase) < MINPASSWDLEN){
						show_bkground("eshortpasswd");
                                                sleep(3);
					   }
				} while ( strlen(passphrase) < MINPASSWDLEN && i<BADRETRIES );

				if (i>=BADRETRIES && strlen(passphrase) < MINPASSWDLEN )
					  system("/sbin/reboot"); 

				   
				
				   get_passwd(passver, MAXPASSWDLEN, "verifypasswd");
				   j++;
			           if (strncmp(passver,passphrase,strlen(passphrase))!=0 ){
                                        show_bkground("enomatchpasswd");
                                        sleep(3);

				   }
			} while(strncmp(passver,passphrase,strlen(passphrase))!=0 && j<BADRETRIES);
			
                        if (i>=BADRETRIES && strncmp(passver,passphrase,strlen(passphrase))!=0 )
                           system("/sbin/reboot");

			show_bkground("pleasewait");
		     /* backup */
                        snprintf(buffer, sizeof(buffer)-1, "/sbin/sh -c '/sbin/backup_data.sh'");
                        ret=system(buffer);
                        ret=GETRETCODE(ret);
			
			if(ret==1){
				show_bkground("ebackup1");
				sleep(15);
				system("/sbin/reboot");
			}else if( ret == 2 ){
			 	show_bkground("ebackup1");
				sleep(15);
				system("/sbin/reboot");
			}else if(ret == 3 ){
				show_bkground("ebackup1");
				sleep(15);
				system("/sbin/reboot");
			} else if (ret != 0 ) {
				for(ret=0;ret<10;ret++){
				   show_bkground("ebackup1");
                                }
				system("/sbin/reboot");
			}

		     /* luksFormat */
                        snprintf(buffer, sizeof(buffer)-1, "echo %s | /sbin/sh -c '%s -c'aes-cbc-essiv:sha256' -hsha256 luksFormat %s '",
                                passphrase, CRYPTSETUP, DATA_DEVICE);
			ret=system(buffer);
		        ret=GETRETCODE(ret);
		     /* handle luksFormat errors */
		     /* luksOpen */
	                snprintf(buffer, sizeof(buffer)-1, "echo %s | /sbin/sh -c '%s luksOpen %s %s'", passphrase, 
				CRYPTSETUP, DATA_DEVICE, DATA_MAPPER_NAME);
        	        ret=system(buffer);
                	ret=GETRETCODE(ret);
		     /* handle luksOpen */
		     /* format_restore.sh */
                        snprintf(buffer, sizeof(buffer)-1, "/sbin/sh -c '/sbin/restore_data.sh'");
                        ret=system(buffer);
                        ret=GETRETCODE(ret);
	
		     /* luksOpen data */
		     /* mkfs data*/
		     /* mount data*/
		     /* restore data*/
		     /* unmount sdcard */
                     }else if(ret==2 && persist==1){
			close(open(DONTBOTHER, O_WRONLY | O_TRUNC | O_CREAT, 00666 ));
	             }
			
		}
	} else { /* couldn't mount /data , the partition is probably encrypted (or just crap) */
		get_passwd(passphrase, MAXPASSWDLEN, "enterpasswd");
		/* attempt to luksOpen and mount /data */
		snprintf(buffer, sizeof(buffer)-1, "echo %s | /sbin/sh -c '%s luksOpen %s %s'", passphrase, CRYPTSETUP, DATA_DEVICE, DATA_MAPPER_NAME);
		ret=system(buffer);
		ret=GETRETCODE(ret);
		printf("INFO: luksOpen returned %d\n", ret);
		if(ret!=0)
		   system("/sbin/reboot");
		snprintf(buffer, sizeof(buffer)-1, "/sbin/sh -c '/sbin/mount -t ext4 -o nosuid,nodev,noatime,barrier=1,data=ordered,noauto_da_alloc,discard /dev/mapper/%s %s'", DATA_MAPPER_NAME, DATA_MOUNTPOINT );
                ret=system(buffer);
                ret=GETRETCODE(ret);
		printf("INFO: mount returned %d\n", ret);
		if(ret !=0 )
			system("/sbin/reboot");
	}
	show_bkground("booting");
	main_close();
	bootsync();	
	return 0;
}
