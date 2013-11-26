#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define VIBFILE "/sys/devices/virtual/timed_output/vibrator/enable"
static int vibfd;
void vib_open(){
	vibfd=open(VIBFILE, O_WRONLY);
}
void vib_vibrate(int msec){
	char buffer[100];
	sprintf(buffer,"%d",msec);
	if(vibfd > 0)
  	  write(vibfd,buffer,strlen(buffer));
}
void vib_close(){
	close(vibfd);
}
