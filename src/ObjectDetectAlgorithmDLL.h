#pragma once
//目标检测算法定义的接口
//zhangpei  2016/09/19
//zhangpei  2017/10/11
//zhangpei  2017/12/14

#ifndef OBJECT_DETECT_ALGORITHM_DLL_H
#define OBJECT_DETECT_ALGORITHM_DLL_H
#ifdef __cplusplus
extern "C"{
#endif
	//日志状态
#ifndef TYPEDEF_ERROR_LOG_LEVEL
#define TYPEDEF_ERROR_LOG_LEVEL

	typedef   unsigned   char   BYTE;
	typedef enum E_ERROR_LOG_LEVEL
	{
		LOG_LEVEL_ERROR = 1,
		LOG_LEVEL_WARN,
		LOG_LEVEL_INFO,
	}ERROR_LOG_LEVEL;
#endif //TYPEDEF_ERROR_LOG_LEVEL

	//矩形框
	typedef struct ST_IVA_RECT
	{
		int x;      //目标框左上角点的x坐标
		int y;      //目标框左上角点的y坐标
		int width;  //目标框的宽度
		int height; //目标框的高度
	}IVA_RECT, *P_IVA_RECT;

	//输入规则
	typedef struct ST_IVA_OBJECT_DETECT_RULE
	{
		int nDetectRect;        //输入检测框的数目
		IVA_RECT* pDetectRect;  //输入检测框的具体坐标信息
		const char*  pIniPath;  //ODConfig.ini文件的路径，目标检测算法用到的相关模型文件路径写入ODConfig.ini文件
	}IVA_OBJECT_DETECT_RULE, *P_IVA_OBJECT_DETECT_RULE;

	//类别结构体
	typedef struct ST_IVA_LABEL_INFO
	{
		int nLabelCode;               //类别编码,详细见《标签内容-平台3.0-v2-20171214.xlsx》
		IVA_RECT labelRect;           //类别对应的区域
	}IVA_LABEL_INFO, *P_IVA_LABEL_INFO;

	//图像信息结构体
	typedef struct ST_IVA_IMAGE_INFO
	{
		unsigned char* pImage;        //图像数据,JPEG压缩格式
		unsigned int   nBuffSize;	  //图片分配缓存
		unsigned long  ulSize;        //每个图像的大小  
		unsigned int   unWidth;       //每个图像的宽度  
		unsigned int   unHeight;      //每个图像的高度  
	}IVA_IMAGE_INFO, *P_IVA_IMAGE_INFO;

	//目标检测结果结构体
	typedef struct ST_IVA_OBJECT_DETECT_INFO
	{
		int nLabelRect;   //类别及对应框的数目
		IVA_LABEL_INFO* pLabelInfo; //类别信息  //改为指针
		IVA_IMAGE_INFO  originImage;    //原图
		IVA_IMAGE_INFO  resultImage;    //结果图
		unsigned long   ulTime;   //算法处理完当前帧的时间，时间为从1970-1-1 00:00:00开始直到算法处理完经过的时间(s)
	}IVA_OBJECT_DETECT_INFO, *P_IVA_OBJECT_DETECT_INFO;

	/**
	* @brief	目标检测算法模块日志回调设置函数。
	* @param	nLogLevel 日志级别，见ERROR_LOG_LEVEL定义
	* @param	szMessage 日志内容。
	* @param	pUserData 用户参数。
	* @return	无
	* @note
	*/
	typedef void(*ObjectDetect_SetLogCBFun)(int nLogLevel, const char *szMessage, void* pUserData);


	/**
	* @brief	模块日志回调设置函数。
	* @param	pLogFunc 日志回调函数地址。
	* @param	pUserData 用户附加数据。
	* @return	无
	* @note
	*/
	void ObjectDetect_SetLogCallBack(ObjectDetect_SetLogCBFun pLogFunc, void* pUserData);

	/**
	* @brief	创建目标检测公共变量的结构体指针
	* @param	pODRule：目标检测输入参数的结构体指针
	* @return	成功，返回void指针；失败，返回NULL
	* @note
	*/
	void* CreateObjectDetectDLLHandle(IVA_OBJECT_DETECT_RULE* pODRule);

	/**
	* @brief	目标检测主函数
	* @param	pODHandle：目标检测结构体指针
	* @param	pByte：视频帧数据
	* @param	nSize：视频帧数据大小
	* @param	nWidth：视频帧宽度
	* @param	nHeight：视频帧高度
	* @param	nFrameType：视频帧类型，RGB24:1, YV12:2, YUV420:3, BGR24:4
	* @param	pODInfo：目标检测输出检测信息，包含类别、目标检测框、原图、结果图
	* @return	返回-1，处理失败；返回0，处理成功
	* @note
	*/
	int ObjectDetectAlgorithmDLLExe(void* pODHandle, unsigned char* pByte, int nSize, int nWidth, int nHeight, int nFrameType, IVA_OBJECT_DETECT_INFO* pODInfo);

	/**
	* @brief	释放目标检测公共变量的结构体指针
	* @param	pODHandle：目标检测数据结构体指针
	* @return	void
	* @note
	*/
	void DestoryObjectDetectDLLHandle(void* pODHandle);
#ifdef __cplusplus
}
#endif
#endif //OBJECT_DETECT_ALGORITHM_DLL_H
