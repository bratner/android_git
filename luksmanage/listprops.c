#include <stdio.h>
#include <cutils/properties.h>

void propfn(const char *key, const char *value, void *cookie){
	printf("Got %s : %s \n", key, value);
}
int main(int argc, char * argv[] ){
	int ret;
	
	printf("Listing system properties\n");
        ret=property_list(&propfn, NULL);
	return 0;
}
