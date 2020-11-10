#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/fb.h>

int main(int argc, char *argv[])
{
	int fbfd = -1;
	struct fb_var_screeninfo vinfo, n_vinfo;
	struct fb_fix_screeninfo finfo;

	int xres=0, yres=0;
	if(argc > 1) {
		xres = atoi(argv[1]);
		yres = atoi(argv[2]);
	}

	fbfd = open("/dev/fb0", O_RDWR);
	if(fbfd < 0 ) {
		perror("Frame Buffer File Open Error");
		exit(-1);
	}
	printf("fb device open ok!!\n");
	
	if(ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) < 0) {
		perror("Frame Buffer Fixed Info Read Error");
		exit(-1);
	}

	if(ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) < 0) {
		perror("Frame Buffer Variable Info Read Error");
		exit(-1);
	}

	printf("Resolution: %d x %d, %d bpp\n", vinfo.xres, vinfo.yres,
				vinfo.bits_per_pixel);
	printf("Virtual Resolution: %d x %d\n", vinfo.xres_virtual, vinfo.yres_virtual);
	printf("Length of frame buffer memory: %d\n", finfo.smem_len);

#ifdef TEST
	//origin:800x480
	if(xres==0) vinfo.xres = 800;
	else vinfo.xres = xres;
	if(yres==0) vinfo.yres = 480;
	else vinfo.yres = yres;
	if(ioctl(fbfd, FBIOPUT_VSCREENINFO, &vinfo) < 0) {
		perror("Frame Buffer Variable Info Set Error");
		exit(-1);
	}

	if(ioctl(fbfd, FBIOGET_VSCREENINFO, &n_vinfo) < 0) {
		perror("Frame Buffer Variable Info Read Error");
		exit(-1);
	}
	printf("\n==>New Resolution: %d x %d, %d bpp\n", n_vinfo.xres, n_vinfo.yres,
				n_vinfo.bits_per_pixel);
#endif
	close(fbfd);

	return 0;
}



