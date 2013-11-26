#include <stdio.h>
#include <unistd.h>
#define GETRETCODE(x) ((((short int)(x)) & 0xff00) >> 8)

int main(int argc, char * argv[]){
	uid_t uid,euid;
	gid_t gid,egid;
	int ret_slot0, ret_slot1;
	int old_slot, new_slot;
	char * CS="/sbin/cryptsetup";
	char * DATA_DEV="/dev/block/mmcblk0p10";
	char cmd[1024]={0};

	close(STDERR_FILENO);
	dup2(STDOUT_FILENO,STDERR_FILENO);		

	euid = geteuid();
        if( euid != 0){
	    printf("ERROR: Installation error. Contact your favorite IT person.\n");
            return 1;
         }else{
	    printf("DEBUG: Setting uid to 0 -> %d\n", setuid(0));
	    printf("INFO: EUID %d. Working.\n",(int)euid);
	    printf("DEBUG: orig passwd: %s \n ", argv[1]);
	    printf("DEBUG: new passwd: %s \n ", argv[2]);
	    printf("INFO: Figuring out which key slot is empty\n");

	    //sprintf(cmd,"echo 0 | %s -q -S0 luksAddKey %s ",CS,DATA_DEV);
	    sprintf(cmd,"/sbin/sh -c 'echo 0 | /sbin/cryptsetup -q -S0 luksAddKey /dev/block/mmcblk0p10'");
            ret_slot0=system(cmd);
            ret_slot0=GETRETCODE(ret_slot0); 
	    //printf("DEBUG: For slot 0 return code %d\n",ret_slot0);

            //sprintf(cmd,"echo 0 | %s -q -S1 luksAddKey %s ",CS,DATA_DEV);
	    sprintf(cmd,"/sbin/sh -c 'echo 0 | /sbin/cryptsetup -q -S1 luksAddKey /dev/block/mmcblk0p10'");
	
            ret_slot1=system(cmd);
            ret_slot1=GETRETCODE(ret_slot1);
            //printf("DEBUG: For slot 1 return code %d\n",ret_slot1);
		
	    if(ret_slot0==1 && ret_slot1==2){
	    	printf("DEBUG: Slot 1 is free\n");
		old_slot=0; new_slot=1;
	    }else if (ret_slot0==2 && ret_slot1==1){
		printf("Debug: Slot 0 is free\n");
		old_slot=1; new_slot=0;
	    }else{
		printf("ERROR: Something went wrong. Nothing serious. Just tell your least favorite IT person.\n");
		return 2;
	    }
		printf("DEBUG: Creating a new key in slot %d\n",new_slot);
		snprintf(cmd,sizeof(cmd),"/sbin/sh -c 'echo -e \"%s\\n%s\" | /sbin/cryptsetup -S%d luksAddKey /dev/block/mmcblk0p10'",argv[1],argv[2],new_slot);
		printf("DEBUG: %s\n",cmd);
                ret_slot0=system(cmd);
		//ret_slot0=0;
                ret_slot0=GETRETCODE(ret_slot0);
		printf("DEBUG: Creating new key returned %d\n",ret_slot0);
		if(ret_slot0 != 0 ){
		    printf("ERROR: Error creating new key in slot %d. Contact any IT person.\n",new_slot);
		    return 3;
		} else {
		    printf("DEBUG: Created a new key in slot %d\n", new_slot);
		}
 
		printf("DEBUG: Killing the old key in slot %d\n",old_slot);
		snprintf(cmd,sizeof(cmd),"/sbin/sh -c 'echo \"%s\" | /sbin/cryptsetup -q luksKillSlot /dev/block/mmcblk0p10 %d'",argv[2],old_slot);
		printf("DEBUG: %s\n",cmd);
                ret_slot0=system(cmd);
		//ret_slot0=0;
                ret_slot0=GETRETCODE(ret_slot0);
		printf("DEBUG: Clearing old key returned %d\n",ret_slot0);
		if(ret_slot0 != 0 ){
                    printf("ERROR: Error removing old key from slot %d. Contact any IT person.\n",old_slot);
                    return 4;
                } else {
                    printf("DEBUG: Removed old key from slot %d\n", new_slot);
                }
 
		
	 }
	return 0;
}
