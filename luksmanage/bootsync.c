#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include "bootsync.h"

void bootsync(){
   char buf[20];
   int fd;
   if((fd=open(BOOTSYNC,O_RDONLY))<0)
	return;
   read(fd,buf,sizeof(buf));
   close(fd);
}
