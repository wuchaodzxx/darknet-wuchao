#include "ObjectDetectAlgorithmDLL.h"
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/legacy/legacy.hpp>
#include "ODDefine.h"
using namespace std;

using namespace cv;
int main(int argc, char **argv)
{

	IVA_OBJECT_DETECT_RULE* pODRule = new IVA_OBJECT_DETECT_RULE;
	ST_IVA_RECT * pST_IVA_RECT = new ST_IVA_RECT;
	pST_IVA_RECT->x = 27;
	pST_IVA_RECT->y = 278;
	pST_IVA_RECT->height = 237;
	pST_IVA_RECT->width = 590;
	pODRule->nDetectRect = 0;
	pODRule->pDetectRect = pST_IVA_RECT;
	pODRule->pIniPath = "./Config.ini";
	
	void * oDDATA  = CreateObjectDetectDLLHandle(pODRule);

	char *filename = argv[2];
	cout << "load image : " << filename << endl;

	IplImage* originImage = cvLoadImage(filename, -1);
	int width = originImage->width;
	int height = originImage->height;



	//判断宽高是否为偶数
	bool bResize = false;
	if (width % 2 != 0)
	{
		width = width + 1;
		bResize = true;
	}
	if (height % 2 != 0)
	{
		height = height + 1;
		bResize = true;
	}

	IplImage* image = cvCreateImage(cvSize(width, height), originImage->depth, originImage->nChannels);
	if (bResize == true)
	{
		cvResize(originImage, image);
	}
	else
	{
		cvCopy(originImage, image);
	}


	int nWidth = image->width;
	int nHeight = image->height;

	cout << "width:" << width << ",height:" << height << endl;

	IVA_OBJECT_DETECT_INFO  sIVA_OBJECT_DETECT_INFOp;
	sIVA_OBJECT_DETECT_INFOp.pLabelInfo = new IVA_LABEL_INFO[1024];
	sIVA_OBJECT_DETECT_INFOp.originImage.nBuffSize = 100 * 1024 * 1024;
	sIVA_OBJECT_DETECT_INFOp.originImage.pImage = (unsigned char *)malloc(sizeof(char)* 100 * 1024 * 1024);
	sIVA_OBJECT_DETECT_INFOp.resultImage.nBuffSize = 100 * 1024 * 1024;
	sIVA_OBJECT_DETECT_INFOp.resultImage.pImage = (unsigned char *)malloc(sizeof(char)* 100 * 1024 * 1024);
	cout << "run ObjectDetectAlgorithmDLLExe......." << endl;
	ObjectDetectAlgorithmDLLExe(oDDATA, (unsigned char *)image->imageData, image->imageSize, nWidth, nHeight, 1, &sIVA_OBJECT_DETECT_INFOp);
	cout << "result:" << sIVA_OBJECT_DETECT_INFOp.nLabelRect << endl;
	if (sIVA_OBJECT_DETECT_INFOp.nLabelRect>0){
		printf("labelRect.x:%d ,labelRect.y:%d ", sIVA_OBJECT_DETECT_INFOp.pLabelInfo[0].labelRect.x, sIVA_OBJECT_DETECT_INFOp.pLabelInfo[0].labelRect.y);
	}

	//原图显示
	unsigned char* poRGB = NULL;
	int oWidth = sIVA_OBJECT_DETECT_INFOp.originImage.unWidth;
	int oHeight = sIVA_OBJECT_DETECT_INFOp.originImage.unHeight;
	poRGB = (unsigned char*)malloc(oWidth*oHeight * 3);
	IplImage *oriIm = cvCreateImageHeader(cvSize(oWidth, oHeight), IPL_DEPTH_8U, 3);
	memcpy(poRGB, sIVA_OBJECT_DETECT_INFOp.originImage.pImage, oWidth*oHeight * 3);
	cvSetData(oriIm, poRGB, oWidth * 3);
	cvCvtColor(oriIm, oriIm, CV_BGR2RGB);
	cvShowImage("originImage", oriIm);
	cvWaitKey(0);
	//结果图显示
	unsigned char* pRGB = NULL;
	int rWidth = sIVA_OBJECT_DETECT_INFOp.resultImage.unWidth;
	int rHeight = sIVA_OBJECT_DETECT_INFOp.resultImage.unHeight;
	pRGB = (unsigned char*)malloc(rWidth*rHeight * 3);
	IplImage *outIm = cvCreateImageHeader(cvSize(rWidth, rHeight), IPL_DEPTH_8U, 3);
	memcpy(pRGB, sIVA_OBJECT_DETECT_INFOp.resultImage.pImage, rWidth*rHeight * 3);
	cvSetData(outIm, pRGB, rWidth * 3);
	cvCvtColor(outIm, outIm, CV_BGR2RGB);
	cvShowImage("resultImage", outIm);
	cvWaitKey(0);

	return 0;
}

