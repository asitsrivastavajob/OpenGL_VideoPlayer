// IDCC-VVCVideoPlayer.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
/**
IDCC-VCCVideoPlayer
**/

#include <stdio.h>

#include "glew.h"
#include "glut.h"

#include <stdlib.h>
#include <malloc.h>
#include <string.h>

//set '1' to choose a type of file to play
#define LOAD_RGB24   1
#define LOAD_BGR24   0
#define LOAD_BGRA    0
#define LOAD_YUV420P 0
#pragma warning(disable : 4996)
int screen_w = 640, screen_h = 360;
const int pixel_w = 320, pixel_h = 180;

//Bit per Pixel
#if LOAD_BGRA
const int bpp = 32;
#elif LOAD_RGB24|LOAD_BGR24
const int bpp = 24;
#elif LOAD_YUV420P
const int bpp = 12;
#endif

//YUV file
FILE *fp = NULL;
unsigned char buffer[pixel_w*pixel_h*bpp / 8];
unsigned char buffer_convert[pixel_w*pixel_h * 3];

inline unsigned char CONVERT_ADJUST(double tmp)
{
	return (unsigned char)((tmp >= 0 && tmp <= 255) ? tmp : (tmp < 0 ? 0 : 255));
}

//YUV420P to RGB24
void CONVERT_YUV420PtoRGB24(unsigned char* yuv_src, unsigned char* rgb_dst, int nWidth, int nHeight)
{
	unsigned char *tmpbuf = (unsigned char *)malloc(nWidth*nHeight * 3);
	unsigned char Y, U, V, R, G, B;
	unsigned char* y_planar, *u_planar, *v_planar;
	int rgb_width, u_width;
	rgb_width = nWidth * 3;
	u_width = (nWidth >> 1);
	int ypSize = nWidth * nHeight;
	int upSize = (ypSize >> 2);
	int offSet = 0;

	y_planar = yuv_src;
	u_planar = yuv_src + ypSize;
	v_planar = u_planar + upSize;

	for (int i = 0; i < nHeight; i++)
	{
		for (int j = 0; j < nWidth; j++)
		{
			// Get the Y value from the y planar
			Y = *(y_planar + nWidth * i + j);

			// Get the V value from the u planar
			offSet = (i >> 1) * (u_width)+(j >> 1);
			V = *(u_planar + offSet);

			// Get the U value from the v plan
			U = *(v_planar + offSet);

			// Cacular the R,G,B values
			R = CONVERT_ADJUST((Y + (1.4075 * (V - 128))));
			G = CONVERT_ADJUST((Y - (0.3455 * (U - 128) - 0.7169 * (V - 128))));
			B = CONVERT_ADJUST((Y + (1.7790 * (U - 128))));

			offSet = rgb_width * i + j * 3;

			rgb_dst[offSet] = B;
			rgb_dst[offSet + 1] = G;
			rgb_dst[offSet + 2] = R;
		}
	}
	free(tmpbuf);
}

void display(void)
{
	if (fread(buffer, 1, pixel_w*pixel_h*bpp / 8, fp) != pixel_w*pixel_h*bpp / 8) 
	{
		// Loop
		fseek(fp, 0, SEEK_SET);
		fread(buffer, 1, pixel_w*pixel_h*bpp / 8, fp);
	}

	glRasterPos3f(-1.0f, 1.0f, 0);

	glPixelZoom((float)screen_w / (float)pixel_w, -(float)screen_h / (float)pixel_h);
#if LOAD_BGRA
	glDrawPixels(pixel_w, pixel_h, GL_BGRA, GL_UNSIGNED_BYTE, buffer);
#elif LOAD_RGB24
	glDrawPixels(pixel_w, pixel_h, GL_RGB, GL_UNSIGNED_BYTE, buffer);
#elif LOAD_BGR24
	glDrawPixels(pixel_w, pixel_h, GL_BGR_EXT, GL_UNSIGNED_BYTE, buffer);
#elif LOAD_YUV420P
	CONVERT_YUV420PtoRGB24(buffer, buffer_convert, pixel_w, pixel_h);
	glDrawPixels(pixel_w, pixel_h, GL_RGB, GL_UNSIGNED_BYTE, buffer_convert);
#endif

	glutSwapBuffers();

}

void timeFunc(int value)
{
	display();
	// Present frame every 40 ms
	glutTimerFunc(40, timeFunc, 0);
}



int main(int argc, char* argv[])
{

#if LOAD_BGRA
	fp = fopen("test_bgra_320x180.rgb", "rb+");
#elif LOAD_RGB24
	fp = fopen("test_rgb24_320x180.rgb", "rb+");
#elif LOAD_BGR24
	fp = fopen("test_bgr24_320x180.rgb", "rb+");
#elif LOAD_YUV420P
	fp = fopen("test_yuv420p_320x180.yuv", "rb+");
#endif
	if (fp == NULL)
	{
		printf("Cannot open this file.\n");
		return -1;
	}

	// GLUT init
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

	glutInitWindowPosition(100, 100);
	glutInitWindowSize(screen_w, screen_h);
	glutCreateWindow("IDCC-VVCVideoPlayer");

	glutDisplayFunc(&display);
	glutTimerFunc(30, timeFunc, 0);

	glutMainLoop();
	return 0;
}






