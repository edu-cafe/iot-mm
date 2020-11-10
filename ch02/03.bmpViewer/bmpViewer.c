#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>                     /* USHRT_MAX 상수를 위해서 사용 */
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <linux/fb.h>

#include "bmpHeader.h"

#define FBDEVFILE            "/dev/fb0"
#define NO_OF_COLOR    24/8		// 24비트(true color) BMP 이미지

#define LIMIT_UINT(n) (n>UINT_MAX)?UINT_MAX:(n<0)?0:n
#define LIMIT_USHRT(n) (n>USHRT_MAX)?USHRT_MAX:(n<0)?0:n
#define LIMIT_UBYTE(n) (n>UCHAR_MAX)?UCHAR_MAX:(n<0)?0:n

extern int readBmp(char *filename, unsigned char **pData, int *cols, int *rows);

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

int main(int argc, char **argv)
{
    int cols, rows;
    int width, height;
    unsigned char *pData;
    unsigned char r, g, b;
#ifdef BPP16
    unsigned short *pBmpData, *pfbmap, pixel;
#else
	unsigned int *pBmpData, *pfbmap, pixel;
#endif
    struct fb_var_screeninfo vinfo;
    int fbfd;
    int x, y, k, t;

    if(argc != 2) {
        printf("\nUsage: ./%s xxx.bmp\n", argv[0]);
        return 0;
    }

    /* 프레임버퍼를 오픈한다. */
    fbfd = open(FBDEVFILE, O_RDWR);
    if(fbfd < 0) {
        perror("open( )");
        return -1;
    }

    /* 현재 프레임버퍼에 대한 고정된 화면 정보를 얻어온다. */
    if(ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) < 0) {
        perror("ioctl( ) : FBIOGET_VSCREENINFO");
        return -1;
    }

    cols = vinfo.xres;
    rows = vinfo.yres;

    /* BMP 출력을 위한 변수의 메모리 할당 */
	pData = (unsigned char *)malloc(cols * rows * sizeof(unsigned char) * NO_OF_COLOR);

    /* 프레임버퍼에 대한 메모리 맵을 수행한다. */
#ifdef BPP16
	pfbmap = (unsigned short *)mmap(0, cols*rows*2, 
#else
    pfbmap = (unsigned int *)mmap(0, cols*rows*4, 
#endif
                PROT_READ|PROT_WRITE,  MAP_SHARED, fbfd, 0);
    if((unsigned)pfbmap == (unsigned)-1) {
        perror("mmap( )");
        return -1;
    }

    /* BMP 파일에서 헤더 정보를 가져온다. */
    if(readBmp(argv[1], &pData, &width, &height) < 0) {
        perror("readBMP( )");
        return -1;
    }

    printf("\nWidth : %d, Height : %d\n", width, height);

    /* 24 비트의 BMP 이미지 데이터를 16or32비트 이미지 데이터로 변경 */
    for(y = 0; y < height; y++) {
        k = (height-y-1)*width*NO_OF_COLOR;
        t = y*cols;
 
        for(x = 0; x < width; x++) {
            b = LIMIT_UBYTE(pData[k+x*NO_OF_COLOR+0]);
            g = LIMIT_UBYTE(pData[k+x*NO_OF_COLOR+1]);
            r = LIMIT_UBYTE(pData[k+x*NO_OF_COLOR+2]);
#ifdef BPP16
            pixel = LIMIT_USHRT(makepixel(r, g, b));
#else
			pixel = LIMIT_UINT(makepixel(r, g, b));
#endif
            pfbmap[t+x] = pixel; 
        };
    };

#ifdef BPP16
    munmap(pfbmap, cols*rows*2);
#else
	munmap(pfbmap, cols*rows*4);
#endif
	
    free(pData);

    close(fbfd);

    return 0;
}

