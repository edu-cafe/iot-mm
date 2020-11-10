#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/fb.h>

unsigned int makepixel(unsigned char r, unsigned char g, unsigned char b)
{
	return (unsigned int)( (r<<16) | (g<<8) | (b) );
}

static int DrawPoint(int fd, int x, int y, unsigned int color)
{
	int offset;
	struct fb_var_screeninfo vinfo;

	if(ioctl(fd, FBIOGET_VSCREENINFO, &vinfo) < 0) {
		perror("Frame Buffer Variable Info Read Error");
		exit(-1);
	}

	offset = (x + y*vinfo.xres)*4;
	lseek(fd, offset, SEEK_SET);
	write(fd, &color, 4);
	return 0;
}

static int DrawLine(int fd, int start_x, int end_x, int y, unsigned int color)
{
	int x, offset;
	struct fb_var_screeninfo vinfo;

	if(ioctl(fd, FBIOGET_VSCREENINFO, &vinfo) < 0) {
		perror("Frame Buffer Variable Info Read Error");
		exit(-1);
	}

	for(x=start_x; x<end_x; x++) {
		offset = (x + y*vinfo.xres)*4;
		lseek(fd, offset, SEEK_SET);
		write(fd, &color, 4);
	}

	return 0;
}

static void DrawCircle(int fd, int center_x, int center_y, int radius, unsigned int color)
{
	int x=radius, y=0;
	int radiusError = 1-x;

	while(x >= y) {
		DrawPoint(fd, x + center_x, y + center_y, color);
		DrawPoint(fd, y + center_x, x + center_y, color);
		DrawPoint(fd, -x + center_x, y + center_y, color);
		DrawPoint(fd, -y + center_x, x + center_y, color);
		DrawPoint(fd, -x + center_x, -y + center_y, color);
		DrawPoint(fd, -y + center_x, -x + center_y, color);
		DrawPoint(fd, x + center_x, -y + center_y, color);
		DrawPoint(fd, y + center_x, -x + center_y, color);

		y++;	

		if(radiusError < 0) {
			radiusError += 4 * y + 1;
		} else {
			x--;
			radiusError += 4 * (y - x + 1);
		}
	}
}

static int DrawFace(int fd, int start_x, int start_y, int end_x, int end_y, unsigned int color)
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
			offset = (x + y*vinfo.xres)*4;
			lseek(fd, offset, SEEK_SET);
			write(fd, &color, 4);
		}
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

	DrawFace(fbfd, 0, 0, 0, 0, makepixel(255, 255, 0));	//Yellow

	DrawPoint(fbfd, 100, 350, makepixel(255, 0, 0));
	DrawPoint(fbfd, 100, 351, makepixel(255, 0, 0));
	DrawPoint(fbfd, 100, 352, makepixel(255, 0, 0));
	DrawPoint(fbfd, 200, 350, makepixel(0, 255, 0));
	DrawPoint(fbfd, 200, 351, makepixel(0, 255, 0));
	DrawPoint(fbfd, 200, 352, makepixel(0, 255, 0));
	DrawPoint(fbfd, 300, 350, makepixel(0, 0, 255));
	DrawPoint(fbfd, 300, 351, makepixel(0, 0, 255));
	DrawPoint(fbfd, 300, 352, makepixel(0, 0, 255));

	DrawLine(fbfd, 0, 100, 200, makepixel(0, 255, 255)); //Cyan

	DrawCircle(fbfd, 200, 200, 100, makepixel(255, 0, 255)); //Magenta

	close(fbfd);

	return 0;
}
	

