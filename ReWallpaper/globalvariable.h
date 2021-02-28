#ifndef GLOBALVARIABLE_H_
#define GLOBALVARIABLE_H_ 
#include <vector>
#include <string>
#include "videoplayer.h"
#include <CRipple.h>
#include <atlimage.h>
#include <QtGui\qwindowdefs.h>
#include <memory>
namespace global {
	//������ʷ��¼
	extern std::vector<std::string>videoDirectory;
	//��굱ǰ����λ��
	extern CPoint mousePoint;
	//��ǰ����µĴ�������
	extern char mouseDownWindowTitle[100];
	//ˮ����������
	extern int rippleSlidStrength;
	//ˮ������Ƶ��
	extern int rippleSlidHz;
	//ˮ���������
	extern int rippleClickStrength;
	//��ǰ��Ƶ����Ŀ¼������
	extern int lineNumber;
	//����������
	extern int playerVolume;
	//�Ƿ�ѭ������	1Ϊѭ�����ţ�2Ϊ�б�ѭ��
	extern int loopPlay;
	//��ǰ���ŵ��ļ���·��
	extern std::string nowFilePath;
	//ȫ��ˮ���ƶ���
	extern std::shared_ptr<CRipple> g_Ripple;
}
#endif // !GLOBALVARIABLE_H_