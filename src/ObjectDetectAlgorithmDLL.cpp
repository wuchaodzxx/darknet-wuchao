#include <stdlib.h>
#include <stdio.h>
#include "detection_layer.h"
#include "cost_layer.h"
#include "utils.h"
#include "parser.h"
#include "box.h"
#include "demo.h"
#include "ODDefine.h"
#include "ODUtils.h"
#include "GetConfig.h"
#include <time.h> 

using namespace cv;
using namespace std;


ObjectDetect_SetLogCBFun g_logCBfun = NULL;  //������־�ص�ȫ�ֱ���
void* g_User = NULL;  //�����û����ݵ�ȫ�ֱ���

void quickSort(ObjectBox array[], int X, int Y); //X,Y�ֱ����������ʼ�±�

//	��ǩ���� 			����		Ӣ����
//	û����۵ĵ��� 			1061		crane
//	��۵ĵ��� 			1062		workingcrane
//	ˮ��ó� 			1063		cementpump
//	��׮�� 				1064		pile
//	�󷭶����������ؿ� 		1065		largetrunck
//	���� 				1066		towercrane
//	�ھ�� 				1067		digr
//	�� 				1068		smoke
//	�� 				1069		fire
//

//��ǩ����
//int labelCode[1024] = {1061,1062,1063,1064,1065,1066,1067,1068,1069};
int labelCode[1024] = { 1066, 1063, 1062, 1067, 1069, 1064, 1068, 1065, 1061 };

//
string labelNamesEn[1024] = {};

// {"towercrane","cementpump","workingcrane","digr","fire","pile","smoke","largetrunck","crane"};

//��ӡ��־
void LogObjectDetect(int nLogLevel, const char *szMessage)
{
	if (NULL != g_logCBfun)
	{
		(*g_logCBfun)(nLogLevel, (char*)szMessage, g_User);
	}
}


/**
* @brief	ģ����־�ص����ú�����
* @param	pLogFunc ��־�ص�������ַ��
* @param	pUserData �û��������ݡ�
* @return	��
* @note
*/
void ObjectDetect_SetLogCallBack(ObjectDetect_SetLogCBFun pLogFunc, void* pUserData) {
	g_logCBfun = pLogFunc;
	g_User = pUserData;
}

/**
* @brief	����Ŀ���⹫�������Ľṹ��ָ��
* @param	pODRule��Ŀ������������Ľṹ��ָ��
* @return	�ɹ�������voidָ�룻ʧ�ܣ�����NULL
* @note
*/
void* CreateObjectDetectDLLHandle(IVA_OBJECT_DETECT_RULE* pODRule) {
	map<string, string> m;
	string sname_list;
	string scfg;
	string sweights;
	string sthresh;
	setlocale(LC_ALL, "Chinese-simplified");
	if (ReadConfig(pODRule->pIniPath, m))
	{
		map<string, string>::const_iterator mite = m.begin();
		for (; mite != m.end(); ++mite)
		{
			cout << mite->first << "=" << mite->second << endl;

			if (mite->first.compare("names") == 0)
			{
				sname_list = mite->second;
			}
			else if (mite->first.compare("cfg") == 0)
			{
				scfg = mite->second;
			}
			else if (mite->first.compare("weights") == 0)
			{
				sweights = mite->second;
			}
			else if (mite->first.compare("thresh") == 0)
			{
				sthresh = mite->second;
			}
		}
	}
	else
	{
		LogObjectDetect(LOG_LEVEL_ERROR, "[ObjectDetectAlgorithm] read config failed!\n");
		return NULL;
	}
	setlocale(LC_ALL, "C");
	cout << "name_list=" << sname_list << ",cfg=" << scfg << ",weights=" << sweights << ",thresh=" << sthresh << endl;

	/////////////////////////////////////
	char **names = OD_get_labels((char *)sname_list.c_str());

	char *cfg = (char *)scfg.c_str();
	char *weights = (char *)sweights.c_str();
	float thresh = atof((char *)sthresh.c_str());
	if (strcmp(cfg, "None") == 0 || strcmp(weights, "None") == 0) {
		LogObjectDetect(LOG_LEVEL_ERROR, "[ObjectDetectAlgorithm] cfg or weights not defined in config file \n");
		printf("cfg or weights not defined in config file \n");
		return NULL;
	}


	network net = OD_parse_network_cfg_custom(cfg, 1);
	if (weights) {
		OD_load_weights(net, weights);
	}
	OD_set_batch_network(&net, 1);
	srand(2222222);

	ODDATA* oDDATA = new ODDATA;
	oDDATA->net = net;
	oDDATA->names = names;
	oDDATA->thresh = thresh;
	oDDATA->rule = *pODRule;
	return oDDATA;
}

/**
* @brief	Ŀ����������
* @param	pODHandle��Ŀ����ṹ��ָ��
* @param	pByte����Ƶ֡����
* @param	nSize����Ƶ֡���ݴ�С
* @param	nWidth����Ƶ֡���
* @param	nHeight����Ƶ֡�߶�
* @param	nFrameType����Ƶ֡���ͣ�RGB24:1, YV12:2, YUV420:3, BGR24:4
* @param	pODInfo��Ŀ������������Ϣ���������Ŀ�����ԭͼ�����ͼ
* @return	����-1������ʧ�ܣ�����0������ɹ�
* @note
*/
int ObjectDetectAlgorithmDLLExe(void* pODHandle, unsigned char* pByte, int nSize, int nWidth, int nHeight, int nFrameType, IVA_OBJECT_DETECT_INFO* pODInfo) {
	LogObjectDetect(LOG_LEVEL_INFO, "[ObjectDetectAlgorithm] ObjectDetectAlgorithmDLLExe is running......!\n");
	//ԭʼͼ��ṹ��
	ST_IVA_IMAGE_INFO sT_IVA_IMAGE_INFO;
	sT_IVA_IMAGE_INFO.pImage = pByte;
	sT_IVA_IMAGE_INFO.ulSize = nSize;
	sT_IVA_IMAGE_INFO.unHeight = nHeight;
	sT_IVA_IMAGE_INFO.unWidth = nWidth;
	//������ṹ��
	IVA_LABEL_INFO* iVA_LABEL_INFO_array = pODInfo->pLabelInfo;

	//��ʱ��ʼ
	clock_t start, ends;
	start = clock();

	unsigned char* pRGB = NULL;
	pRGB = (unsigned char*)malloc(nWidth*nHeight * 3);

	if (pRGB)
	{

		IplImage *im = cvCreateImageHeader(cvSize(nWidth, nHeight), IPL_DEPTH_8U, 3);
		if (!CV_IS_IMAGE_HDR(im))
		{
			LogObjectDetect(LOG_LEVEL_ERROR, "[ObjectDetectAlgorithm] cvCreateImageHeader error!\n");
			free(pRGB);
			return -1;
		}

		if (nFrameType == 1)  //1��ʾ��RGB24����
		{
			memcpy(pRGB, pByte, nWidth*nHeight * 3);
			cvSetData(im, pRGB, nWidth * 3);
			cvCvtColor(im, im, CV_RGB2BGR);   //opencvĬ�ϵ���BGR��ʽ  YUV12_to_RGB24ת����RGB���������·�ת��
			//cvShowImage("1222", im);
			//cvWaitKey(0);
		}
		else if (nFrameType == 2)  //2��ʾ��YV12���ݸ�ʽ
		{
			if (YUV12_to_RGB24((unsigned char*)pByte, nSize, pRGB, nWidth, nHeight))
			{
				cvSetData(im, pRGB, nWidth * 3);
				cvFlip(im, im, 0);   //note:zhangpei 2014/12/09  �ⲿ����ͼ�������·�ת�ģ������ڲ���Ҫ�ٴη�תΪͼ��������
				cvCvtColor(im, im, CV_RGB2BGR);   //opencvĬ�ϵ���BGR��ʽ  YUV12_to_RGB24ת����RGB���������·�ת��
			}
			else
			{
				LogObjectDetect(LOG_LEVEL_ERROR, "[ObjectDetectAlgorithm]YUV12_to_RGB24 error !\n");
			}
		}
		else if (nFrameType == 3)   //2��ʾ��YUV420p���ݸ�ʽ
		{
			if (YUV420P_To_BGR24((unsigned char*)pByte, pRGB, nWidth, nHeight))
			{
				cvSetData(im, pRGB, nWidth * 3);
			}
			else
			{
				LogObjectDetect(LOG_LEVEL_ERROR, "[ObjectDetectAlgorithm] YUV420P_To_BGR24 error !\n");
			}
		}
		else if (nFrameType == 4)  //4��ʾBGR24���ݸ�ʽ
		{
			memcpy(pRGB, pByte, nWidth*nHeight * 3);
			cvSetData(im, pRGB, nWidth * 3);
		}
		else
		{
			LogObjectDetect(LOG_LEVEL_ERROR, "[ObjectDetectAlgorithm] don't support the {nFrameType} !\n");
		}
		//cvShowImage("exeimage", im);
		//cvWaitKey(0);


		////ת����darknet��image�ṹ��
		image img2 = OD_ipl_to_image(im);
		
		
		ObjectBoxs oBoxs = OD_detector((ODDATA *)pODHandle, img2);
		//����⵽�Ŀ������������
		if (oBoxs.nums > 0) {
			quickSort(oBoxs.objectBoxArray, 0, oBoxs.nums - 1);
		}

		//��ʱ����
		ends = clock();
		//cout << "OD_detector is Done.using time:" << ends - start << endl;

		//��ȡָ������
		int top = 1;
		int left = 1;
		int right = left + nWidth - 2;
		int buttom = top + nHeight - 2;
		if (((ODDATA *)pODHandle)->rule.nDetectRect>0){
			IVA_RECT iVA_RECT;
			iVA_RECT = *((ODDATA *)pODHandle)->rule.pDetectRect;
			top = iVA_RECT.y;
			left = iVA_RECT.x;
			right = left + iVA_RECT.width - 1;
			buttom = top + iVA_RECT.height - 1;
		}
		printf("top=%d,left=%d,right=%d,buttom=%d \n", top, left, right, buttom);

		int box_sum = 0;
		if (oBoxs.nums > 0) {
			int i1;
			for (i1 = 0; i1 < oBoxs.nums; i1++) {
				ObjectBox oBox = oBoxs.objectBoxArray[i1];
				std::cout << "oBox.nclass:" << oBox.nclass << std::endl;
				//cout << "NAME:" << ((ODDATA *)pODHandle)->names[oBox.nclass] << endl;;
				//���ɶ�Ӧ�Ľṹ��
				IVA_LABEL_INFO iVA_LABEL_INFO;
				IVA_RECT iVA_RECT;

				int box_top = oBox.top;
				int box_left = oBox.left;
				int box_right = oBox.left + oBox.width - 1;
				int box_buttom = oBox.top + oBox.height - 1;
				printf("box_top=%d,box_left=%d,box_right=%d,box_buttom=%d \n", box_top, box_left, box_right, box_buttom);
				//���nDetectRect��ʾ�������ͼƬ
				if (((ODDATA *)pODHandle)->rule.nDetectRect == 0) {

					iVA_RECT.x = oBox.left;
					iVA_RECT.y = oBox.top;
					iVA_RECT.height = oBox.height;
					iVA_RECT.width = oBox.width;
					
					iVA_LABEL_INFO.nLabelCode = labelCode[oBox.nclass];
					iVA_LABEL_INFO.labelRect = iVA_RECT;

					iVA_LABEL_INFO_array[box_sum++] = iVA_LABEL_INFO;
				}
				else if (oBox.left <= right && box_right >= left && oBox.top <= buttom && box_buttom >= top) {
					//�����غ�����
					int left_com = max(left, oBox.left);
					int top_com = max(top, oBox.top);
					int right_com = min(right, oBox.left + oBox.width);
					int buttom_com = min(buttom, oBox.top + oBox.height);


					iVA_RECT.x = left_com;
					iVA_RECT.y = top_com;
					iVA_RECT.height = buttom_com - top_com + 1;
					iVA_RECT.width = right_com - left_com + 1;

					iVA_LABEL_INFO.nLabelCode = labelCode[oBox.nclass];
					iVA_LABEL_INFO.labelRect = iVA_RECT;

					iVA_LABEL_INFO_array[box_sum++] = iVA_LABEL_INFO;

					printf("Oroginal  %s,lableCode:%d  : %f (%d,%d,%d,%d)\n", oBox.name, iVA_LABEL_INFO.nLabelCode, oBox.prob, oBox.left, oBox.top, oBox.width, oBox.height);
					printf("Now       %s,lableCode:%d  : %f (%d,%d,%d,%d)\n", oBox.name, iVA_LABEL_INFO.nLabelCode, oBox.prob, iVA_LABEL_INFO.labelRect.x, iVA_LABEL_INFO.labelRect.y, iVA_LABEL_INFO.labelRect.width, iVA_LABEL_INFO.labelRect.height);

				}

				// ��ʽ������ӡ�������ݵ�buffer
				char  buffer[200], s[] = "[ObjectDetectAlgorithm] ";	
				int j;
				j = sprintf(buffer, "%s ", s);
				j += sprintf(buffer + j, "lableCode : %d ,", labelCode[oBox.nclass]);
				j += sprintf(buffer + j, "prob : %f \n", oBox.prob);
				printf(buffer);
				LogObjectDetect(LOG_LEVEL_INFO, buffer);	
			}
		}
		else {
			printf("no object detected! \n");
			pODInfo->nLabelRect = 0;
		}
		printf("Object num is %d now;\n", box_sum);

		//���ͼ���������
		labelNamesEn[1066 - 1061] = "towercrane";
		labelNamesEn[1063 - 1061] = "cementpump";
		labelNamesEn[1062 - 1061] = "workingcrane";
		labelNamesEn[1067 - 1061] = "digr";
		labelNamesEn[1069 - 1061] = "fire";
		labelNamesEn[1064 - 1061] = "pile";
		labelNamesEn[1068 - 1061] = "smoke";
		labelNamesEn[1065 - 1061] = "largetrunck";
		labelNamesEn[1061 - 1061] = "crane";


		//��ȡ��ǰʱ��
		time_t tt = time(NULL);//��䷵�ص�ֻ��һ��ʱ��cuo
		tm* t = localtime(&tt);
		char  buffer_time_origin[400], s2[] = "originImage_";
		int j2;
		j2 = sprintf(buffer_time_origin, "%s", s2);
		j2 += sprintf(buffer_time_origin + j2, "%d%02d%02d%02d%02d%d%s", t->tm_year + 1900,
			t->tm_mon + 1,
			t->tm_mday,
			t->tm_hour,
			t->tm_min,
			t->tm_sec,
			".bmp");
		//����ԭʼͼ������
		cvSaveImage(buffer_time_origin, im);
		//cvSaveImage("originImage.bmp", im);


		IplImage *src = im;
		IplImage *dst;
		dst = cvCloneImage(src);

		if (box_sum>0) {
			int i2;
			for (i2 = 0; i2 < box_sum; i2++) {
				int nLabelCode = iVA_LABEL_INFO_array[i2].nLabelCode;
				IVA_RECT iVA_RECT = iVA_LABEL_INFO_array[i2].labelRect;

				string labelName = labelNamesEn[nLabelCode - 1061];

				std::cout << "nLabelCode:" << nLabelCode << std::endl;
				CvFont font;
				cvInitFont(&font, CV_FONT_HERSHEY_PLAIN, 1.0f, 1.0f, 0, 1, CV_AA);//������ʾ������  

				CvPoint P1, P2;
				P1.x = iVA_RECT.x;
				P1.y = iVA_RECT.y;
				P2.x = iVA_RECT.x + iVA_RECT.width - 1;
				P2.y = iVA_RECT.y + iVA_RECT.height - 1;
				const char *strID = labelName.c_str();
				cvRectangle(dst, P1, P2, CV_RGB(0, 255, 0), 2); //��ɫ����  
				cvPutText(dst, strID, cvPoint(P1.x + 10, P1.y + 20), &font, CV_RGB(255, 0, 0));//��ɫ����ע��  

			}
		}
		cvCvtColor(dst, dst, CV_BGR2RGB);
		//cvShowImage("outimage", dst);
		//cvWaitKey(0);



		//��ȡ��ǰʱ�䣨�룩
		time_t timep;
		time(&timep);

		//���÷��ؽ���ṹ��

		int nDetectRect = ((ODDATA *)pODHandle)->rule.nDetectRect;
		if (nDetectRect<box_sum && nDetectRect>0){
			pODInfo->nLabelRect = nDetectRect;//�����Ŀ
		}
		else{
			pODInfo->nLabelRect = box_sum;//�����Ŀ
		}
		pODInfo->ulTime = timep;//�㷨����ʱ��
		pODInfo->pLabelInfo = iVA_LABEL_INFO_array;//�����Ϣ
		if (nSize <= (pODInfo->originImage.nBuffSize)){
			printf("copy image for return \n");
			memcpy(pODInfo->originImage.pImage, sT_IVA_IMAGE_INFO.pImage, nSize);
			pODInfo->originImage.ulSize = nSize;
			pODInfo->originImage.unHeight = nHeight;
			pODInfo->originImage.unWidth = nWidth;
		}
		else{
			LogObjectDetect(LOG_LEVEL_ERROR, "[ObjectDetectAlgorithm] pODInfo->originImage.nBuffSize is less than imageSize \n");
		}
		if (dst->imageSize <= (pODInfo->resultImage.nBuffSize)){
			memcpy(pODInfo->resultImage.pImage, dst->imageData, dst->imageSize);
			pODInfo->resultImage.ulSize = dst->imageSize;
			pODInfo->resultImage.unHeight = nHeight;
			pODInfo->resultImage.unWidth = nWidth;
		}
		else{
			LogObjectDetect(LOG_LEVEL_ERROR, "[ObjectDetectAlgorithm] pODInfo->resultImage.nBuffSize is less than imageSize \n");
		}

		//������ͼ������
		//cvCvtColor(dst, dst, CV_RGB2BGR);
		char  buffer_time_result[400], s3[] = "resultImage_";
		int j3;
		j3 = sprintf(buffer_time_result, "%s", s3);
		j3 += sprintf(buffer_time_result + j3, "%d%02d%02d%02d%02d%d%s", t->tm_year + 1900,
			t->tm_mon + 1,
			t->tm_mday,
			t->tm_hour,
			t->tm_min,
			t->tm_sec,
			".bmp");
		cvSaveImage(buffer_time_result, dst);
		//"/home/test001/soft/VAS/123/"
		//cvSaveImage("resultImage.bmp", dst);

		cvReleaseImageHeader(&im);
		cvReleaseImageHeader(&dst);
		free(pRGB);
		LogObjectDetect(LOG_LEVEL_INFO, "[ObjectDetectAlgorithm] ObjectDetectAlgorithmDLLExe is done!\n");
	}
	return 0;
}

/**
* @brief	�ͷ�Ŀ���⹫�������Ľṹ��ָ��
* @param	pODHandle��Ŀ�������ݽṹ��ָ��
* @return	void
* @note
*/
void DestoryObjectDetectDLLHandle(void* pODHandle) {

	pODHandle = NULL;

}


//�����㷨
void quickSort(ObjectBox array[], int X, int Y)
{
	int z, y, i;
	ObjectBox k;
	if (X<Y)
	{
		z = X;
		y = Y;
		k = array[z];
		do {
			while ((z<y) && (array[y].prob <= k.prob))
				y--;
			if (z<y)
			{
				array[z] = array[y];
				z++;
			}
			while ((z<y) && array[z].prob >= k.prob)
				z++;
			if (z<y)
			{
				array[y] = array[z];
			}

		} while (z != y);
		array[z] = k;
		quickSort(array, X, z - 1);
		quickSort(array, z + 1, Y);
	}
}
