#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <errno.h>

#include "minui/minui.h"
#include "input_handler.h"

#define SDCARD_DEVICE		"/dev/block/mmcblk0p11"
#define DATA_DEVICE		"/dev/block/mmcblk0p10"

#define SDCARD_MAPPER_NAME	"sdcard"
#define DATA_MAPPER_NAME	"data"

static int finito=0;

static void on_click(int x, int y)
{
   //do something on click
   printf("INFO: Pointer click recorded at %d:%d\n",x,y);
   if( x>300 && y>600 ){
	finito=1;
   }
}

static void on_touch(int x, int y, int touch){
	if(touch)
	printf("INFO: Pointer touched at %d:%d\n",x,y);
	else
	printf("INFO: Pointer released at %d:%d\n",x,y);
	
}
static void on_key(int keycode){
   printf("INFO: Keypress recorded with keycode %d\n",keycode);
}

static void start_events(){
   init_input(&on_key,&on_click, &on_touch);
}

static void stop_events(){
   kill_input();
}

static void main_init(){
	
}
int main(int argc, char **argv, char **envp) {
	main_init(); /* initialize stuff */
	start_events();
	while(finito==0){
	   usleep(10000);
	};
        stop_events();
	return 0;
}
