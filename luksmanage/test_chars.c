#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "chars.h"
int main(int argc, char * argv[]){
	int k;
	struct stat mst;
	char file[255];
	for(k=0;k<94;k++)
	{
	   int i=k+0x20;
	   printf("testing for %c(%d) %s -> ",(char)i,i,char_names[k]);
	   sprintf(file,"/home/bratner/android/gt-i9100/clockworkmod_galaxys2_initramfs/res/images/%s.png",char_names[k]);
	   if(stat(file,&mst) != 0)
	     printf("%s.png MISSING!!!\n",char_names[k]);
	   else
	     printf("%s.png OK\n",char_names[k]);
	
	}
	return 0;
}
