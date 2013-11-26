#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <errno.h>

#include "minui/minui.h"

/* debugging */
#define DEBUG 1
/* sleep period */
#define SLEEP 0

static const char * bkground_file;
static gr_surface bkground;

static void draw_full_screen();


static void load_pngs(){
	int ret;
	ret=res_create_surface(bkground_file, &bkground);
	if(DEBUG)
		printf("INFO: Create surface %s returned %d\n",bkground_file, ret);
}
static void draw_full_screen(){
	gr_blit(bkground, 0,0,480,800,0,0);
	gr_flip();
}
static void init_dlg(){
	load_pngs();
	draw_full_screen();	
}

static void deinit_dlg(){
	res_free_surface(bkground);	
}
int show_bkground(const char * bkground_fname) {
	if(!bkground_fname){
		bkground_file="bkground";
	} else {
		bkground_file=bkground_fname;
	}
	init_dlg();
/*	usleep(SLEEP * 1000);*/
        deinit_dlg();
	return 0;
}
