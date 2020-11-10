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

	DrawCircle(fbfd, 200, 200, 100, makepixel(255, 0, 255));

	return 0;
}
	

