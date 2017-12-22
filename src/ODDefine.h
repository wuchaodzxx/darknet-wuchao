#ifndef ODD_DEFINE_H
#define ODD_DEFINE_H

#include "network.h"
#include "ObjectDetectAlgorithmDLL.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/legacy/legacy.hpp>
typedef struct ST_ODDATA
{
	network net;
	char **names;
	float thresh;
	IVA_OBJECT_DETECT_RULE rule;
}ODDATA, *P_ODDATA;
typedef struct {
	int left;
	int top;
	int width;
	int height;
	int nclass;
	float prob;
	char **name;
} ObjectBox;
typedef struct {
	ObjectBox objectBoxArray[1024];
	int nums;
} ObjectBoxs;

#ifdef __cplusplus  
extern "C" {
#endif  

	list* OD_read_cfg(char * path);
	char* OD_find_str(list* ls, char* s1, char* s2);
	network OD_parse_network_cfg_custom(char * cfg, int n);
	void OD_load_weights(network net, char* weights);
	ObjectBoxs OD_detector(ODDATA * pODHandle, image img);
	image OD_ipl_to_image(IplImage* src);
	char ** OD_get_labels(char * s);
	void OD_set_batch_network(network* net, int n);
#ifdef __cplusplus  
}
#endif  


#endif
