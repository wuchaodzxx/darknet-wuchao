/****************************************************************************
*   Ŀ��:  ��ȡ�����ļ�����Ϣ����map����ʽ����
*   Ҫ��:  �����ļ��ĸ�ʽ����#��Ϊ��ע�ͣ����õ���ʽ��key = value���м���пո�Ҳ��û�пո�
*   ��Դ��http://www.cnblogs.com/tzhangofseu/archive/2011/10/02/2197966.html
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
