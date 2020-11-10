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
static int DrawPoint(int fd, int x, int y, unsigned short color)
#else
static int DrawPoint(int fd, int x, int y, unsigned int color)
#endif
{
	int offset;
	struct fb_var_screeninfo vinfo;

	if(ioctl(fd, FBIOGET_VSCREENINFO, &vinfo) < 0) {
		perror("Frame Buffer Variable Info Read Error");
		exit(-1);
	}

#ifdef BPP16
	offset = (x + y*vinfo.xres)*2;
	lseek(fd, offset, SEEK_SET);
	write(fd, &color, 2);
#else
	offset = (x + y*vinfo.xres)*4;
	lseek(fd, offset, SEEK_SET);
	write(fd, &color, 4);
#endif
	return 0;
}

#ifdef BPP16
static int DrawLine(int fd, int start_x, int end_x, int y, unsigned short color)
#else
static int DrawLine(int fd, int start_x, int end_x, int y, unsigned int color)
#endif
{
	int x, offset;
	struct fb_var_screeninfo vinfo;

	if(ioctl(fd, FBIOGET_VSCREENINFO, &vinfo) < 0) {
		perror("Frame Buffer Variable Info Read Error");
		exit(-1);
	}

	for(x=start_x; x<end_x; x++) {
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

	return 0;
}

int main(int argc, char *argv[])
{
	int fbfd, status, offset;
	unsigned short pixel;

	fbfd = open("/dev/fb0", O_RDWR);
	if(fbfd < 0 ) {
		perror("Frame Buffer File Open Error");
		exit(-1);
	}
	printf("fb device open ok!!\n");

	DrawLine(fbfd, 0, 100, 200, makepixel(0, 0, 255));		
	DrawLine(fbfd, 0, 100, 300, makepixel(0, 0, 255));	
	DrawLine(fbfd, 0, 100, 400, makepixel(0, 0, 255));	
	DrawLine(fbfd, 200, 300, 400, makepixel(0, 0, 255));	
	DrawLine(fbfd, 400, 500, 400, makepixel(0, 0, 255));	
	DrawLine(fbfd, 600, 700, 400, makepixel(0, 0, 255));

	close(fbfd);

	return 0;
}
	

