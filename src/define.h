#ifndef DEFINE_H
#define DEFINE_H

#include "network.h"
/*
�㷨���ö���ͷ�ļ�
by �ⳬ
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