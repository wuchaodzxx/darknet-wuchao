#ifndef ODD_UTILS_H
#define ODD_UTILS_H

#include <cstring>
#include <cstdlib>
#include <string.h>
#include <fstream>


typedef   unsigned   char   BYTE;
//extern "C"
//{
//#include "jpeglib.h"
//}


//#ifdef _DEBUG
//FILE _iob[3] = { __iob_func()[0], __iob_func()[1], __iob_func()[2] };
//#pragma comment(lib, "libjpegd.lib") 
//#else
//#pragma comment(lib, "libjpeg.lib") 
//#endif
using std::string;


void YUV12toRGB(unsigned char *src0, unsigned char *src1, unsigned char *src2
	, unsigned char *rgb24, int width, int height)
{
	int R, G, B, Y, U, V;
	int x, y;
	int nWidth = width >> 1; //色度信号宽度
	for (y = 0; y < height; ++y)
	{
		for (x = 0; x < width; ++x)
		{
			Y = *(src0 + (y * width) + x);
			V = *(src1 + ((y >> 1) * nWidth) + (x >> 1));
			U = *(src2 + ((y >> 1) * nWidth) + (x >> 1));
			R = (int)(Y + (1.402 * (V - 128)));
			G = (int)(Y - (0.34414 * (U - 128)) - (0.71414 * (V - 128)));
			B = (int)(Y + (1.772 * (U - 128)));
			//防止越界 
			if (R > 255) { R = 255; }
			if (R < 0) { R = 0; }
			if (G > 255) { G = 255; }
			if (G < 0) { G = 0; }
			if (B > 255) { B = 255; }
			if (B < 0) { B = 0; }
			*(rgb24 + (((height - y - 1) * width) + x) * 3) = B;
			*(rgb24 + (((height - y - 1) * width) + x) * 3 + 1) = G;
			*(rgb24 + (((height - y - 1) * width) + x) * 3 + 2) = R;
		}
	}
}

bool  YUV12_to_RGB24(unsigned char* pYV12, int nLen, unsigned char* pRGB24, int iWidth, int iHeight)
{
	bool bRet = false;
	int nY = iWidth * iHeight;
	int nBR = nY / 4;
	unsigned char* Y = (unsigned char *)malloc(nY);
	if (Y)
	{
		unsigned char* Cb = (unsigned char *)malloc(nBR);
		if (Cb)
		{
			unsigned char* Cr = (unsigned char *)malloc(nBR);
			if (Cr)
			{
				if (nY <= nLen)
				{
					memcpy(Y, &pYV12[0], nY);
				}
				if ((nY + nBR) <= nLen)
				{
					memcpy(Cr, &pYV12[nY], nBR);
				}
				if ((nY + 2 * nBR) <= nLen)
				{
					memcpy(Cb, &pYV12[nY + nBR], nBR);
				}
				YUV12toRGB(Y, Cb, Cr, pRGB24, iWidth, iHeight);
				free(Cr);
				bRet = true;
			}
			free(Cb);
		}
		free(Y);
	}
	return bRet;
}


bool YUV420_To_BGR24(unsigned char *puc_y, unsigned char *puc_u, unsigned char *puc_v, unsigned char *puc_rgb, int width_y, int height_y)
{
	if (!puc_y || !puc_u || !puc_v || !puc_rgb)
	{
		return false;
	}

	//初始化变量
	int baseSize = width_y * height_y;
	int rgbSize = baseSize * 3;
	BYTE* rgbData = new BYTE[rgbSize];
	memset(rgbData, 0, rgbSize);

	/* 变量声明 */
	int temp = 0;
	BYTE* rData = rgbData;                  //r分量地址
	BYTE* gData = rgbData + baseSize;       //g分量地址
	BYTE* bData = gData + baseSize;       //b分量地址
	int uvIndex = 0, yIndex = 0;

	//YUV->RGB 的转换矩阵
	//double  Yuv2Rgb[3][3] = {1, 0, 1.4022,
	//    1, -0.3456, -0.7145,
	//    1, 1.771,   0};
	for (int y = 0; y < height_y; y++)
	{
		for (int x = 0; x < width_y; x++)
		{
			uvIndex = (y >> 1) * (width_y >> 1) + (x >> 1);
			yIndex = y * width_y + x;

			/* r分量 */
			temp = (int)(puc_y[yIndex] + (puc_v[uvIndex] - 128) * 1.4022);
			rData[yIndex] = temp<0 ? 0 : (temp > 255 ? 255 : temp);

			/* g分量 */
			temp = (int)(puc_y[yIndex] + (puc_u[uvIndex] - 128) * (-0.3456) +
				(puc_v[uvIndex] - 128) * (-0.7145));
			gData[yIndex] = temp < 0 ? 0 : (temp > 255 ? 255 : temp);

			/* b分量 */
			temp = (int)(puc_y[yIndex] + (puc_u[uvIndex] - 128) * 1.771);
			bData[yIndex] = temp < 0 ? 0 : (temp > 255 ? 255 : temp);
		}
	}

	//将R,G,B三个分量赋给img_data
	int widthStep = width_y * 3;
	for (int y = 0; y < height_y; y++)
	{
		for (int x = 0; x < width_y; x++)
		{
			puc_rgb[y * widthStep + x * 3 + 2] = rData[y * width_y + x];   //R
			puc_rgb[y * widthStep + x * 3 + 1] = gData[y * width_y + x];   //G
			puc_rgb[y * widthStep + x * 3 + 0] = bData[y * width_y + x];   //B
		}
	}

	if (!puc_rgb)
	{
		return false;
	}

	delete[] rgbData;
	rgbData = NULL;
	return true;
}

bool YUV420P_To_BGR24(unsigned char* pYUV420, unsigned char* pBGR24, int width, int height)
{
	if (!pYUV420 || !pBGR24)
	{
		return false;
	}

	//初始化变量
	int baseSize = width*height;
	/* 变量声明 */
	//int temp = 0;
	BYTE* yData = pYUV420;                  //y分量地址
	BYTE* uData = pYUV420 + baseSize;       //u分量地址
	BYTE* vData = uData + (baseSize >> 2);   //v分量地址

	if (!YUV420_To_BGR24(yData, uData, vData, pBGR24, width, height))
	{
		return false;
	}

	return true;
}

#endif

