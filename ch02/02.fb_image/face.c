#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/fb.h>

#ifdef BPP16
unsigned short makepixel(unsigned char r, unsigned char g, unsigned char b)
{
	return (unsigned short)( ((r>>3)<<11) | ((g>>2)<<5) | (b>>3) );
}
#else
unsigned int makepixel(unsigned char r, unsigned char g, unsigned char b)
{
	return (unsigned int)( (r<<16) | (g<<8) | (b) );
}
#endif

#ifdef BPP16
static int DrawFace(int fd, int start_x, int start_y, int end_x, int end_y, unsigned short color)
#else
static int DrawFace(int fd, int start_x, int start_y, int end_x, int end_y, unsigned int color)
#endif
{
	int x, y, offset;
	struct fb_var_screeninfo vinfo;

	if(ioctl(fd, FBIOGET_VSCREENINFO, &vinfo) < 0) {
		perror("Frame Buffer Variable Info Read Error");
		exit(-1);
	}
	
	if(end_x == 0) end_x = vinfo.xres;
	if(end_y == 0) end_y = vinfo.yres;

	for(x=start_x; x<end_x; x++) {
		for(y=start_y; y<end_y; y++) {
#ifdef BPP16
			offset = (x + y*vinfo.xres)*2;
			lseek(fd, offset, SEEK_SET);
			write(fd, &color, 2);
#else
			offset = (x + y*vinfo.xres)*4;
			lseek(fd, offset, SEEK_SET);
			write(fd, &color, 4);
#endif
		}
	}

	return 0;
}

int main(int argc, char *argv[])
{
	int fbfd, status, offset;
	unsigned short pixel;
	int x2=0, y2=0;
	if(argc>1) {
		x2 = atoi(argv[1]);
		y2 = atoi(argv[2]);
	}

	fbfd = open("/dev/fb0", O_RDWR);
	if(fbfd < 0 ) {
		perror("Frame Buffer File Open Error");
		exit(-1);
	}
	printf("fb device open ok!!\n");

	DrawFace(fbfd, 0, 0, x2, y2, makepixel(255, 255, 0));	//Yellow

	close(fbfd);

	return 0;
}
	

