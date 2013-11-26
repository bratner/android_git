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

#define KBD_LOWC 0
#define KBD_CAPS 1
#define KBD_NUM 2
/* debugging */
#define DEBUG 1
#define MAXPASSWD 15
#define STARS_X 57
#define STARS_Y 270
#define STARS_W 24
#define STARS_H 21
#define MSG_X 21 
#define MSG_Y 36

static int dlg_exit=0;

static char * passwd; /* global password buffer */
static const char * msg_image=NULL; /* message image file name  */
static int passwd_max_len; /* maximum allowed length of password */
static int passwd_count=0; /* number of charachters in the password */
static int keyboard=KBD_LOWC; /* current keyboard */
static gr_surface keyboards[3]; /* keyboards - full screen images */
static gr_surface stars;
static gr_surface message;
static char * keyboard_files[3]={"lowcase","capitals","numbers"}; /* keyboard image filenames */
static struct keymap *buttons[3]={lowcase,capitals,numbers}; /* array[keyboard][button] */
static int buttons_count[3]; /* file-global array holds amount of buttons in each keyboard */

static void draw_full_screen();

static void lol_talf(){
	int i;
	gr_surface talf;
	res_create_surface("talf",&talf);
 	gr_blit(talf,0,0,480,800,0,0);
	gr_flip();	
	sleep(5);
	res_free_surface(talf);
	for(i=0;i<passwd_count;i++)
	  passwd[i]=0;
	passwd_count=0;
	draw_full_screen();
}
static char * getkbdsym(int x, int y, struct keymap ** outmap){
	char * sym=NULL;
	struct keymap * map;
	int i;
	
	for(i=0; i<buttons_count[keyboard]; i++){
		if( buttons[keyboard][i].x1 <= x && buttons[keyboard][i].y1 <=y && 
		    buttons[keyboard][i].x2 >= x &&  buttons[keyboard][i].y2 >= y){
			sym=buttons[keyboard][i].symbol;
			if(outmap != NULL)
				*outmap=&buttons[keyboard][i];
			break; // from for
		}
	}
	return sym;
}
static void passwd_add_char(char c){
   int passwd_count_draw;
   /* business logic of add a charachter to the buffer */
   if(c!=0){
       if(passwd_count < passwd_max_len){
		passwd[passwd_count++]=c;
       }
   }	

   /* graphics display of dots */
   passwd_count_draw = passwd_count;
   if(passwd_count > MAXPASSWD )
	passwd_count_draw = MAXPASSWD;

   gr_blit(keyboards[keyboard],STARS_X,STARS_Y,STARS_W*(MAXPASSWD+1),STARS_H,STARS_X,STARS_Y); //clear password text
   gr_blit(stars, 0, 0, STARS_W*passwd_count, STARS_H, STARS_X, STARS_Y);
   gr_flip();
   if(strstr(passwd,"talf")!=NULL)
	lol_talf();
   
}
static void on_click(int x, int y)
{
   char * sym;
   static char * prevsym = NULL;
   //do something on click
   printf("INFO: Pointer click recorded at %d:%d\n",x,y);
   sym=getkbdsym(x,y,NULL);
   if (sym!=NULL){
	if (strncmp(sym, "$go", sizeof("$go"))==0){
		dlg_exit=1;
	}
	else if (strncmp(sym, "$bksp", sizeof("$bksp"))==0){
		if(strlen(passwd)>0){
			passwd[strlen(passwd)-1]=0;
			passwd_count--;
			//redraw password stars
			passwd_add_char(0);
		}		
	}
	else if (strncmp(sym, "$caps", sizeof("$caps"))==0){
		keyboard=KBD_CAPS;
		draw_full_screen();
        }
	else if (strncmp(sym, "$lowc", sizeof("$lowc"))==0){
		keyboard=KBD_LOWC;
		draw_full_screen();
        }
	else if (strncmp(sym, "$num", sizeof("$num"))==0){
                keyboard=KBD_NUM;
                draw_full_screen();
 
	}
        else if (strncmp(sym, "$abc", sizeof("$abc"))==0){
                keyboard=KBD_LOWC;
                draw_full_screen();
        }
	else if (strncmp(sym,"$space", sizeof("$space"))==0){
		passwd_add_char(' ');
	
	}else{
		passwd_add_char(sym[0]);
	}

   }
   prevsym=sym;
}

static void on_touch(int x, int y, int touch){
	char * symbol;
	struct keymap * map;
	static struct keymap * prevmap=NULL;
	static char * prevsym = NULL;
        
	if(touch){
		  printf("INFO: Pointer touched at %d:%d\n",x,y);
		  symbol=getkbdsym(x,y,&map);
		  if( prevsym!=symbol && prevmap!=NULL){
			 gr_blit(keyboards[keyboard], prevmap->x1,prevmap->y1,prevmap->x2 - prevmap->x1, prevmap->y2 - prevmap->y1,prevmap->x1,prevmap->y1);
			 gr_flip();
			 prevmap=NULL;
		  }
	}else{
  	  printf("INFO: Pointer released at %d:%d\n",x,y); 
	  prevsym=symbol=NULL;
	  if(prevmap){
	      gr_blit(keyboards[keyboard], prevmap->x1,prevmap->y1,prevmap->x2 - prevmap->x1, prevmap->y2 - prevmap->y1 ,prevmap->x1,prevmap->y1);
	      gr_flip();
	      printf("INFO: blitting %d,%d %d,%d \n",  prevmap->x1,prevmap->y1,prevmap->x2 , prevmap->y2 );
	      prevmap = NULL;
	  }
	}

	if(symbol != NULL && touch && prevsym!=symbol){
		vib_vibrate(20); 
		gr_color(255,255,255,128);
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
	for(i=KBD_LOWC;i<=KBD_NUM;i++){
		ret=res_create_surface(keyboard_files[i], &keyboards[i]);
		if(DEBUG)
                  	printf("INFO: Create surface %s returned %d\n",keyboard_files[i], ret);
	}
	ret=res_create_surface("dots", &stars);
	if(DEBUG)
		printf("INFO: Create surface 'stars' returned %d\n", ret);
	if(msg_image){
		ret=res_create_surface(msg_image, &message);
		if(DEBUG)
			printf("INFO: Create surface %s returned %d\n",msg_image, ret);
	}	
}
static void draw_message(){
	gr_blit(message,0,0,gr_get_width(message),gr_get_height(message),MSG_X,MSG_Y);
	gr_flip;
}
static void draw_full_screen(){
	gr_blit(keyboards[keyboard], 0,0,480,800,0,0);
	draw_message();
	passwd_add_char(0);
	gr_flip();
	printf("INFO: drawing keyboard %s\n", keyboard_files[keyboard]);	
}
static void init_dlg(){
	buttons_count[KBD_LOWC]=lowcase_count;
	buttons_count[KBD_CAPS]=capitals_count;
	buttons_count[KBD_NUM]=numbers_count;
	passwd[passwd_max_len]=0; //make sure the string has an end
	passwd[0]=0;
	passwd_count=0;
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
        for(i=KBD_LOWC;i<=KBD_NUM;i++){
                res_free_surface(keyboards[i]);
                if(DEBUG)
                        printf("INFO: Free surface %s\n",keyboard_files[i]);
        }
	res_free_surface(stars);	
	if(message){
		res_free_surface(message);
	}
}
int get_passwd(char * passphrase, int max_len, const char * message_file) {
	int i;

	passwd = passphrase;
	msg_image = message_file;
	passwd_max_len = max_len;

	for(i=0;i<passwd_count;i++)
          passwd[i]=0;

	init_dlg();
	while(!dlg_exit){
		usleep(10000);
	};
        deinit_dlg();
	return 0;
}
