#include <stdlib.h>
#include <stdio.h>
#include "utils.h"
#include "parser.h"
#include "ODDefine.h"

list* OD_read_cfg(char * path) {
	return read_data_cfg(path);
}
char* OD_find_str(list* ls, char* s1, char* s2) {
	return option_find_str(ls, s1, s2);
}
network OD_parse_network_cfg_custom(char * cfg, int n) {
	return parse_network_cfg(cfg);
}
void OD_load_weights(network net, char* weights) {
	load_weights(&net, weights);
}
char ** OD_get_labels(char * s){
	return get_labels(s);
}
void OD_set_batch_network(network* net, int n){
	set_batch_network(net, n);
}
image OD_ipl_to_image(IplImage* src) {
	unsigned char *data = (unsigned char *)src->imageData;
	int h = src->height;
	int w = src->width;
	int c = src->nChannels;
	int step = src->widthStep;  // Size of aligned image row in bytes.
	image out = make_image(w, h, c);
	int i, j, k, count = 0;;

	for (k = 0; k < c; ++k) {
		for (i = 0; i < h; ++i) {
			for (j = 0; j < w; ++j) {
				out.data[count++] = data[i*step + j*c + k] / 255.; // Normalize
			}
		}
	}
	return out;
}
ObjectBoxs OD_detector(ODDATA * pODHandle, image im) {

	network net = pODHandle->net;

	char **names = pODHandle->names;
	clock_t time;
	char buff[256];
	char *input = buff;
	int j;
	float nms = .4;

	while (1) {
		printf("resize image...\n");
		image sized = resize_image(im, net.w, net.h);

		printf("net.n:%d \n", net.n);
		printf("layer l = net.layers[net.n - 1]...\n");
		layer l = net.layers[net.n - 1];
		printf("calloc(l.w*l.h*l.n, sizeof(box))...\n");

		box *boxes = calloc(l.w*l.h*l.n, sizeof(box));
		float **probs = calloc(l.w*l.h*l.n, sizeof(float *));
		for (j = 0; j < l.w*l.h*l.n; ++j) probs[j] = calloc(l.classes, sizeof(float *));

		float *X = sized.data;
		time = clock();
		printf("network_predict...\n");
		network_predict(net, X);
		printf("network_predict is Done.\n");
		printf("%s: Predicted in %f seconds.\n", input, sec(clock() - time));
		get_region_boxes(l, 1, 1, pODHandle->thresh, probs, boxes, 0, 0);
		if (nms) do_nms_sort(boxes, probs, l.w*l.h*l.n, l.classes, nms);

		//检测出目标总个数
		int nums = l.w*l.h*l.n;
		printf("OD_detector get Object nums: %d \n", nums);

		//计算可保留的检测目标
		int num_ObjectBox = 0;
		for (int i = 0; i < nums; ++i) {

			int class = max_index(probs[i], l.classes);
			float prob = probs[i][class];
			if (prob > pODHandle->thresh) {
				printf("^^^^^^^^^Get One Box , prob is %f , class is %d ^^^^^^^^\n", prob, class);
				num_ObjectBox++;
			}
		}
		//定义数组
		ObjectBoxs objectBoxs;
		objectBoxs.nums = num_ObjectBox;

		//如果目标数小于等于0，返回-1
		if (num_ObjectBox <= 0) {
			return objectBoxs;
		}


		//过滤小于阈值的目标
		int index = 0;
		for (int i = 0; i < nums; ++i) {

			int class = max_index(probs[i], l.classes);
			float prob = probs[i][class];
			if (prob > pODHandle->thresh) {
				int width = im.h * .012;

				box b = boxes[i];

				int left = (b.x - b.w / 2.)*im.w;
				int right = (b.x + b.w / 2.)*im.w;
				int top = (b.y - b.h / 2.)*im.h;
				int bot = (b.y + b.h / 2.)*im.h;


				if (left < 0) left = 0;
				if (right > im.w - 1) right = im.w - 1;
				if (top < 0) top = 0;
				if (bot > im.h - 1) bot = im.h - 1;

				int box_width = right - left;
				int box_height = bot - top;

				//printf("%s: %.0f (%d,%d,%d,%d)\n", names[class], prob * 100, left, top, box_width, box_height);

				ObjectBox oBox;
				oBox.left = left;
				oBox.top = top;
				oBox.width = box_width;
				oBox.height = box_height;
				oBox.name = names[class];
				oBox.prob = prob;
				oBox.nclass = class;
				objectBoxs.objectBoxArray[index++] = oBox;
				//printf("class:%d,name:%s\n",class,names[class]);
			}
		}
		printf("OD_detector get Object nums with thresh: %d \n", index);

		free_image(im);
		free_image(sized);
		free(boxes);
		free_ptrs((void **)probs, l.w*l.h*l.n);


		return objectBoxs;
	}
}
