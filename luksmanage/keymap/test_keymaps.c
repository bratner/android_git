#include <stdio.h>
#include "keymap.h"
#define MAPNAME numbers
int main(int argc, char * argv[]){
	int i,j;
	int count=sizeof(MAPNAME)/sizeof(struct keymap);
	printf("We got %d records\n",count);
	for(i=0; i<count; i++){
		printf("Charachter %s mapped to (%d,%d)-(%d,%d)\n",MAPNAME[i].symbol,MAPNAME[i].x1,MAPNAME[i].y1,MAPNAME[i].x2,MAPNAME[i].y2);
	}
	return 0;	
}
