#ifndef DEFINE_H
#define DEFINE_H

#include "network.h"
/*
算法调用定义头文件
by 吴超
*/
typedef struct {
	int left;
	int top;
	int width;
	int height;
	float prob;
	char **name;
} ObjectBox;
typedef struct {
	ObjectBox objectBoxArray[1024];
	int nums;
} ObjectBoxs;
//
typedef struct {
	char **names;
	network net;
} NetworkHandle;
#endif