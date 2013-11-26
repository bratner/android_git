#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <errno.h>
#include <signal.h>
#include "minui/minui.h"

#include "input_handler.h"


static struct input_event keyqueue[2048];
static pthread_t t;
static pthread_mutex_t keymutex;
static pthread_mutex_t exitmutex;
static int stop_flag = 0;
static int sp = 0;
static void (*key_handler)(int keycode);
static void (*click_handler)(int x, int y);
static void (*touch_handler)(int x, int y, int touch);
static void *input_thread(); 

static void handle_key(struct input_event ev){
   if (key_handler != NULL)
   	key_handler((int)(ev.code));
}

static void handle_click(struct input_event ev){
   if(click_handler != NULL)
     click_handler((int)(ev.code),(int)(ev.value));
}
static void handle_touch(int x, int y, int touch){ //if touch is zero the it was released
   if(touch_handler != NULL)
	touch_handler(x,y,touch);
}
int init_input(void (*on_key)(int keycode), void (*on_click)(int x, int y), void (*on_touch)(int x, int y, int touch)){
	ev_init();
	key_handler = on_key;
	click_handler = on_click;
	touch_handler = on_touch;
	stop_flag = 0;
	pthread_mutex_init(&keymutex, NULL);
	pthread_mutex_init(&exitmutex, NULL);


        if(pthread_create(&t, NULL, input_thread, NULL) != 0){
                printf("ERROR: unable to create input thread\n");
		ev_exit();
		return -1;
        }else{
                printf("INFO: created the input thread\n");
	}

	return 0;
}	


int kill_input(){
//	pthread_cancel(t);
//	pthread_kill(t, SIGKILL);
        pthread_mutex_lock(&exitmutex);
        stop_flag=1;
	fprintf(stderr,"Stop flag is 1\n");
	pthread_mutex_unlock(&exitmutex);	
	//pthread_join(t,NULL);
	ev_exit();
	return 0;
}
static void *input_thread() {
        int rel_sum = 0;
        int p_x,p_y,p_active=0,p_click=0;
	struct input_event ev;
	int key_up=0;
	int local_exit=0;

/*
ABS_MT_TOUCH_MAJOR      0x30
ABS_MT_TOUCH_MINOR      0x31 
ABS_MT_WIDTH_MAJOR      0x32  
 ABS_MT_WIDTH_MINOR      0x33 
ABS_MT_ORIENTATION      0x34  
ABS_MT_POSITION_X       0x35  
ABS_MT_POSITION_Y       0x36   
ABS_MT_TOOL_TYPE        0x37    
ABS_MT_BLOB_ID          0x38    
ABS_MT_TRACKING_ID      0x39   
ABS_MT_PRESSURE         0x3a    
*/
        for(;local_exit==0;) {
                p_active=0; p_click=0;
                do {
			// usleep(10000);
                        ev_get(&ev, 0);
                        /* printf("INFO input thread: got event 0x%x code 0x%x value 0x%x\n",ev.type, ev.code, ev.value); */

                        switch(ev.type) {
                                case EV_SYN:
                                        continue;
                                case EV_ABS:
                                        if(ev.code == 0x35 )
                                           p_x = ev.value;
                                        if(ev.code == 0x36 )
                                           p_y = ev.value;
                                        if(ev.code == 0x30){
                                                pthread_mutex_lock(&keymutex);
                                                handle_touch(p_x,p_y,ev.value);
                                                pthread_mutex_unlock(&keymutex);
                                           if(p_active == 0 && ev.value > 0){
                                                p_active=1;
                                           }else if ( p_active == 1 && ev.value == 0 ){
                                                p_click=1;
                                                p_active=0;
                                                ev.type=0x1337;
                                                ev.code=p_x;
                                                ev.value=p_y;
                                            }
					}
                                        //printf("INFO: pointer %d,%d active %d, click %d \n", p_x,p_y,p_active,p_click); 
                                        break;

                                default:
                                        rel_sum = 0;
                        }

	        	pthread_mutex_lock(&exitmutex);
			local_exit=stop_flag;
			pthread_mutex_unlock(&exitmutex);

                } while( (ev.type != EV_KEY || ev.code > KEY_MAX) && p_click == 0 && local_exit == 0);

                rel_sum = 0;

                // going to do some logic make it singular
                pthread_mutex_lock(&keymutex);
		switch(ev.type) {
			case(EV_KEY):
				if(key_up == 1) {
					key_up = 0;
					break;
				}
				key_up = 1;
			case(EV_REL):
				handle_key(ev);
				break;
			case(0x1337):
				// printf("INFO: Got a pointer tap at %d,%d\n",event.code,event.value);
				handle_click(ev);
				break;
			case(EV_SYN):
				break;
		}
		pthread_mutex_unlock(&keymutex);
	
        } //for
  fprintf(stderr,"INFO: input thread finished\n");
  return 0;
}//input_thread


