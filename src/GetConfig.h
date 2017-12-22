/****************************************************************************
*   目的:  读取配置文件的信息，以map的形式存入
*   要求:  配置文件的格式，以#作为行注释，配置的形式是key = value，中间可有空格，也可没有空格
*   来源：http://www.cnblogs.com/tzhangofseu/archive/2011/10/02/2197966.html
*****************************************************************************/
#ifndef _GET_CONFIG_H_
#define _GET_CONFIG_H_

#include <string>
#include <map>
using namespace std;

#define COMMENT_CHAR '#'

bool ReadConfig(const string & filename, map<string, string> & m);
void PrintConfig(const map<string, string> & m);

#endif
