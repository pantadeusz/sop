
#include <deco.h>
#include <stdio.h>
#include <string.h>

void deco_frame_topbottom(char *txt) {
	int i;
	printf("+-");
	for (i = 0; i < strlen(txt); i++)
	printf("-");
	printf("-\n");
}

void deco_inframe(char *txt) {
	deco_frame_topbottom(txt);
	printf("| %s |\n", txt);
	deco_frame_topbottom(txt);
}
