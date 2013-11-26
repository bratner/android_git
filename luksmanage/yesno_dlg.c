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
#include "keymap/keymap.h"
#include "vibrator.h"

#define KBD_YESNO 3
/* debugging */
#define DEBUG 1
#define CHECK_X 55
#define CHECK_Y 283

static int dlg_exit=0;
static const char * keyboard_file;
static int checked=0;
static int keyboard=KBD_YESNO; /* current keyboard */
static gr_surface keyboards; /* not an array here :-) keyboards - full screen images */
static gr_surface checkmark;
static int buttons_count[4]; /* file-global array holds amount of buttons in each keyboard */
static struct keymap *buttons[4]={lowcase,capitals,numbers,yesno};
static void draw_full_screen();
static void draw_check();

static char * getkbdsym(int x, int y, struct keymap ** outmap){

	char * sym=NULL;
	struct keymap * map;
	int i;
	if(DEBUG)
	   printf("Keyboard is %d\n",keyboard);
	if(DEBUG)
	   printf("Buttons count is %d\n", buttons_count[keyboard] );
	
	for(i=0; i<buttons_count[keyboard]; i++){
		if( buttons[keyboard][i].x1 <= x && buttons[keyboard][i].y1 <=y && 
		    buttons[keyboard][i].x2 >= x &&  buttons[keyboard][i].y2 >= y){
			sym=buttons[keyboard][i].symbol;
			if(outmap != NULL)
				*outmap=&buttons[keyboard][i];
			break; // from for
		}
	}
	if(DEBUG)
		printf("DEBUG: returning from getkbdsym\n");
	return sym;
}
static void on_click(int x, int y)
{
   char * sym;
   static char * prevsym = NULL;
   //do something on click
   printf("INFO: Pointer click recorded at %d:%d\n",x,y);
   sym=getkbdsym(x,y,NULL);
   if (sym!=NULL){
	if (strncmp(sym, "$yes", sizeof("$yes"))==0){
		dlg_exit=1;
	}
	else if (strncmp(sym, "$no", sizeof("$no"))==0){
			dlg_exit=2;
	}
	else {
		checked=checked?0:1;	
		draw_check();
		gr_flip();
		
	}
   }
   prevsym=sym;
}

static void on_touch(int x, int y, int touch){
	char * symbol;
	struct keymap * map=NULL;
	static struct keymap * prevmap=NULL;
	static char * prevsym = NULL;
        
	if(touch){
		  printf("INFO: Pointer touched at %d:%d\n",x,y);
		  
		  symbol=getkbdsym(x,y,&map);
		  if( prevsym!=symbol && prevmap!=NULL){
			 gr_blit(keyboards, prevmap->x1,prevmap->y1,prevmap->x2 - prevmap->x1, prevmap->y2 - prevmap->y1,prevmap->x1,prevmap->y1);
			 draw_check();
			 gr_flip();
			 prevmap=NULL;
		  }
	}else{
  	  printf("INFO: Pointer released at %d:%d\n",x,y); 
	  prevsym=symbol=NULL;
	  if(prevmap){
	      gr_blit(keyboards, prevmap->x1,prevmap->y1,prevmap->x2 - prevmap->x1, prevmap->y2 - prevmap->y1 ,prevmap->x1,prevmap->y1);
	      draw_check();
	      gr_flip();
	      printf("INFO: blitting %d,%d %d,%d \n",  prevmap->x1,prevmap->y1,prevmap->x2 , prevmap->y2 );
	      prevmap = NULL;
	  }
	}

	if(symbol != NULL && touch && prevsym!=symbol){
		vib_vibrate(20); 
		gr_color(255,255,255,128);
		draw_check();
		gr_fill(map->x1,map->y1,map->x2, map->y2);    
		gr_flip();
		prevsym=symbol;
		prevmap=map;
	}   
	printf("INFO: you touched %s\n",symbol);
}
static void on_key(int keycode){
   printf("INFO: Keypress recorded with keycode %d\n",keycode);
}

static void start_events(){
   init_input(&on_key,&on_click, &on_touch);
}

static void stop_events(){
	printf("INFO: stop_events()\n");
   	kill_input();
}

static void load_pngs(){
	int i,ret;
	ret=res_create_surface(keyboard_file, &keyboards);
	if(DEBUG)
           	printf("INFO: Create surface %s returned %d\n",keyboard_file, ret);
	
	ret=res_create_surface("checkmark", &checkmark);
	if(DEBUG)
		printf("INFO: Create surface 'checkmark' returned %d\n", ret);
}
static void draw_check(){
	gr_blit(keyboards,CHECK_X,CHECK_Y,gr_get_width(checkmark),gr_get_height(checkmark), CHECK_X, CHECK_Y);
	if(checked){
		gr_blit(checkmark,0,0,gr_get_width(checkmark),gr_get_height(checkmark), CHECK_X, CHECK_Y );
	}
}
static void draw_full_screen(){
	gr_blit(keyboards, 0,0,480,800,0,0);
	draw_check();
	gr_flip();
	printf("INFO: drawing keyboard %s\n", keyboard_file);	
}
static void init_dlg(){
	buttons_count[KBD_YESNO]=yesno_count;
	dlg_exit=0;
	vib_open();
	start_events();
	load_pngs();
	draw_full_screen();	
}

static void deinit_dlg(){
	int i,ret;
	stop_events();
	vib_close();
        res_free_surface(keyboards);
        if(DEBUG)
          printf("INFO: Free surface %s\n",keyboard_file);
	res_free_surface(checkmark);	
}
int get_yesno(int * perm, const char * image_file) {
	checked=*perm;
	if(!image_file)
		return -1;
	keyboard_file = image_file; /* we think of our yes-no dialog as 3 button keyboard */
	init_dlg();
	while(!dlg_exit){
		usleep(10000);
	};

        deinit_dlg();
	*perm=checked;
	return dlg_exit;
}
