#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <errno.h>

#include "minui/minui.h"
#include "chars.h"

#define CRYPTSETUP		"/system/bin/cryptsetup"

#define SDCARD_DEVICE		"/dev/block/mmcblk0p11"
#define DATA_DEVICE		"/dev/block/mmcblk0p10"

#define SDCARD_MAPPER_NAME	"encrypted-sdcard"
#define DATA_MAPPER_NAME	"encrypted-data"

#define BUTTON_WIDTH		44
#define BUTTON_HEIGHT		77
#define CHAR_WIDTH		10
#define CHAR_HEIGHT		18


#define CHAR_START		0x20
#define CHAR_END		0x7E

struct keymap {
	unsigned char key;
	int xpos;
	int ypos;
	int selected;
};

struct keymap keys[CHAR_END - CHAR_START];
struct input_event keyqueue[2048];

char passphrase[1024];

pthread_mutex_t keymutex;
unsigned int sp = 0;

gr_surface background;
gr_surface buttons[CHAR_END - CHAR_START];

int res, current = 0;

char *escape_input(char *str) {
	size_t i, j = 0;
	char *new = malloc(sizeof(char) * (strlen(str) * 2 + 1));

	for(i = 0; i < strlen(str); i++) {
		if(!(((str[i] >= 'A') && (str[i] <= 'Z')) ||
		((str[i] >= 'a') && (str[i] <= 'z')) ||
		((str[i] >= '0') && (str[i] <= '9')) )) {
			new[j] = '\\';
			j++;
		}
		new[j] = str[i];
		j++;
	}
	new[j] = '\0';

	return new;
}

void draw_keymap() {
	size_t i;
	char keybuf[2];

	for(i = 0; i < (CHAR_END - CHAR_START); i++) {
		sprintf(keybuf, "%c", keys[i].key);

		/* if(keys[i].selected == 1) {
			gr_color(255, 0, 0, 255);
			gr_fill(keys[i].xpos, keys[i].ypos, keys[i].xpos + BUTTON_WIDTH, keys[i].ypos);
			gr_color(255, 255, 255, 255);
		}
		else
	 		gr_color(128, 128, 255, 255);
		}*/
		gr_blit(buttons[i], 0, 0, BUTTON_WIDTH, BUTTON_HEIGHT, keys[i].xpos, keys[i].ypos);
		if(keys[i].selected == 1) {
                        gr_color(255, 255, 255, 128);
                        gr_fill(keys[i].xpos, keys[i].ypos, keys[i].xpos + BUTTON_WIDTH, keys[i].ypos + BUTTON_HEIGHT);
			gr_color(0,0,0,255);
		}
		//gr_color(0,0,0,0);
		//gr_fill(keys[10].xpos, keys[10].ypos, keys[10].xpos + BUTTON_WIDTH, keys[10].ypos);
		/* gr_text(keys[i].xpos, keys[i].ypos, keybuf); */
	}
}

static void *input_thread() {
	int rel_sum = 0;
	int p_x,p_y,p_active=0,p_click=0;
		
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
	for(;;) {
		struct  input_event ev;
		p_active=0; p_click=0;
		do {
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
					if(ev.code == 0x30)
					   if(p_active == 0 && ev.value > 0)
						p_active=1;
					   else if ( p_active == 1 && ev.value == 0 ){
						p_click=1;
						p_active=0;
						ev.type=0x1337;
						ev.code=p_x;
						ev.value=p_y;
				            }
					//printf("INFO: pointer %d,%d active %d, click %d \n", p_x,p_y,p_active,p_click); 
					break;

				default:
					rel_sum = 0;
			}

			/* if(rel_sum > 4 || rel_sum < -4)
				break; */
		/* change this to act on mouse up or down */
		} while( (ev.type != EV_KEY || ev.code > KEY_MAX) && p_click == 0);

		rel_sum = 0;

		// Add the key to the fifo
		pthread_mutex_lock(&keymutex);
		if(sp < (sizeof(keyqueue) / sizeof(struct input_event)))
			sp++;

		keyqueue[sp] = ev;
		pthread_mutex_unlock(&keymutex);
	}

	return 0;
}

void ui_init(void) {
	int i,ret;
	gr_init();
	ev_init();

	// Generate bitmap from /system/res/padlock.png ( you can change the path in minui/resources.c)
	// res_create_surface("padlock", &background);
	for(i=0;i<(CHAR_END - CHAR_START);i++){
	      	ret=res_create_surface(char_names[i], &buttons[i]);
		printf("INFO: Create surface returned %d for %c\n", ret, (char)(i+32));
	}
}

void draw_screen() {
	// This probably only looks good in HTC Wildfire resolution
	int bgwidth, bgheight, bgxpos, bgypos, i, cols;

	gr_color(0, 0, 0, 255);
	gr_fill(0, 0, gr_fb_width(), gr_fb_height());

	//bgwidth = gr_get_width(background);
	//bgheight = gr_get_height(background);
	//bgxpos = (gr_fb_width() - gr_get_width(background)) / 2;
	//bgypos = (gr_fb_height() - gr_get_height(background)) / 2;

	//gr_blit(background, 0, 0, bgwidth, bgheight, bgxpos, bgypos);

	gr_color(128,128,255,255);
	gr_text(0, CHAR_HEIGHT, "Enter unlock phrase: ");

	cols = gr_fb_width() / CHAR_WIDTH;

	for(i = 0; i < (int) strlen(passphrase); i++) 
		gr_text(i * CHAR_WIDTH, CHAR_HEIGHT * 2, "*");

	for(; i < cols - 1; i++)
		gr_text(i * CHAR_WIDTH, CHAR_HEIGHT * 2, "_");

	//gr_text(0, gr_fb_height() - CHAR_HEIGHT, "Press Volup to unlock");
	//gr_text(0, gr_fb_height(), "Press Voldown to erase");

	draw_keymap();
	gr_flip();
}

void generate_keymap() {
	int xpos, ypos;
	char key;
	int i;

	xpos = 0;
	ypos = CHAR_HEIGHT * 2;

	for(i = 0, key = CHAR_START; key < CHAR_END; key++, i++, xpos += BUTTON_WIDTH) {
		if(xpos >= gr_fb_width() - BUTTON_WIDTH) {
			ypos += BUTTON_HEIGHT;

			xpos = 0;
		}

		keys[i].key = key;
		keys[i].xpos = xpos;
		keys[i].ypos = ypos;
		keys[i].selected = 0;
	}

	keys[current].selected = 1;
}

void unlock() {
	char buffer[2048];
	int fd, failed = 0;

	gr_color(0, 0, 0, 255);
	gr_fill(0, 0, gr_fb_width(), gr_fb_height());
	gr_color(255, 255, 255, 255);

	gr_text((gr_fb_width() / 2) - ((strlen("Unlocking...") / 2) * CHAR_WIDTH), gr_fb_height() / 2, "Unlocking...");
	gr_flip();

	snprintf(buffer, sizeof(buffer) - 1, "echo %s | %s luksOpen %s %s", escape_input(passphrase), CRYPTSETUP, SDCARD_DEVICE, SDCARD_MAPPER_NAME);
	/* system(buffer);

	snprintf(buffer, sizeof(buffer) - 1, "echo %s | %s luksOpen %s %s", escape_input(passphrase), CRYPTSETUP, DATA_DEVICE, DATA_MAPPER_NAME);
	system(buffer);

	snprintf(buffer, sizeof(buffer) - 1, "/dev/mapper/%s", SDCARD_MAPPER_NAME);
	fd = open(buffer, 0);
	if(fd < 0)
		failed = 1;

	snprintf(buffer, sizeof(buffer) - 1, "/dev/mapper/%s", DATA_MAPPER_NAME);
	fd = open(buffer, 0);
	if(fd < 0)
		failed = 1;

	if(!failed) {
		gr_text((gr_fb_width() / 2) - ((strlen("Success!") / 2) * CHAR_WIDTH), gr_fb_height() / 2 + CHAR_HEIGHT, "Success!");
		gr_flip();
		exit(0);
	}

	gr_text((gr_fb_width() / 2) - ((strlen("Failed!") / 2) * CHAR_WIDTH), gr_fb_height() / 2 + CHAR_HEIGHT, "Failed!");
	gr_flip();

	sleep(2);
	passphrase[0] = '\0';*/
	fd=open("/bootsync", O_RDONLY );
	read(fd,(void *)buffer,20);
	close(fd);
	gr_exit();
	exit(0);	
}

void handle_pointer(struct input_event event){
	int p_x,p_y;
	int b_x,b_y;
	int button;
	p_x=event.code;
	p_y=event.value - keys[0].ypos;
	b_x = p_x / BUTTON_WIDTH;
	b_y = p_y / BUTTON_HEIGHT;
	button = b_y*( gr_fb_width() / BUTTON_WIDTH )+b_x;
	keys[current].selected = 0;
	current = button;
	keys[current].selected = 1;
	
	draw_screen();	
}
void handle_key(struct input_event event) {
	int cols;

	cols = gr_fb_width() / (CHAR_WIDTH * 3);
	keys[current].selected = 0;
/*struct input_absinfo {
        __s32 value;
        __s32 minimum;
        __s32 maximum;
        __s32 fuzz;
        __s32 flat;
        __s32 resolution;
};
*/
/*	printf("INFO: got REL event with code %d, value %d, */
	// Joystick down or up
	if(event.type == EV_REL && event.code == 1) {
		if(event.value > 0) {
			if(current + cols < (CHAR_END - CHAR_START))
				current += cols;
		} else if(event.value < 0) {
			if(current - cols > 0)
				current -= cols;
		}
	}

	// Joystick left or right
	if(event.type == EV_REL && event.code == 0) {
		if(event.value > 0 && current < (CHAR_END - CHAR_START) - 1)
				current++;
		else if(event.value < 0 && current > 0)
				current--;
	}

	keys[current].selected = 1;

	// Pressed joystick
	if(event.type == EV_KEY && event.value == 0 && event.code == BTN_MOUSE) {
		snprintf(passphrase, sizeof(passphrase) - 1, "%s%c", passphrase, keys[current].key);
	}

	// Pressed vol down
	if(event.type == EV_KEY && event.code == KEY_VOLUMEDOWN)
		passphrase[strlen(passphrase) - 1] = '\0';

	// Pressed vol up
	if(event.type == 1 && event.code == KEY_VOLUMEUP) {
		unlock();
	}

	draw_screen();
}

int main(int argc, char **argv, char **envp) {
	struct input_event event;
	pthread_t t;
	unsigned int i, key_up = 0;
	int fd;
	ui_init();
	generate_keymap();
	printf("INFO: Generation keymap\n");
	draw_screen();
	printf("INFO: screen should be drawn");

	if(pthread_create(&t, NULL, input_thread, NULL) != 0)
		printf("ERROR: unable to create input thread\n");
	else
		printf("INFO: created the input thread\n");

	pthread_mutex_init(&keymutex, NULL);

	for(;;) {
		pthread_mutex_lock(&keymutex);

		if(sp > 0) {
			for(i = 0; i < sp; i++)
				keyqueue[i] = keyqueue[i + 1];

			event = keyqueue[0];
			sp--;

			pthread_mutex_unlock(&keymutex);
		} else {
			pthread_mutex_unlock(&keymutex);
			continue;
		}

		switch(event.type) {
			case(EV_KEY):
				if(key_up == 1) {
					key_up = 0;
					break;
				}
				key_up = 1;
			case(EV_REL):
				handle_key(event);
				break;
			case(0x1337):
				printf("INFO: Got a pointer tap at %d,%d\n",event.code,event.value);
				handle_pointer(event);
				break;
			case(EV_SYN):
				break;
		}
	}

	return 0;
}
