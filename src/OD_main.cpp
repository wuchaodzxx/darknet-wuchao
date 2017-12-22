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
	//cvShowImage("image", image);
	//cvWaitKey(0);

	int nWidth = image->width;
	int nHeight = image->height;

	cout << "width:" << width << ",height:" << height << endl;

	IVA_OBJECT_DETECT_INFO  sIVA_OBJECT_DETECT_INFOp;
	sIVA_OBJECT_DETECT_INFOp.pLabelInfo = new IVA_LABEL_INFO[1024];
	cout << "run ObjectDetectAlgorithmDLLExe......." << endl;
	ObjectDetectAlgorithmDLLExe(oDDATA, (unsigned char *)originImage->imageData, 20, nWidth, nHeight, 1, &sIVA_OBJECT_DETECT_INFOp);
	cout << "result:" << sIVA_OBJECT_DETECT_INFOp.nLabelRect << endl;
	return 0;
}

