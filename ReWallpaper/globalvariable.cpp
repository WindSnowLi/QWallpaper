
#include "globalvariable.h"
namespace global {
	//��Ƶ·��
	std::vector <std::string> videoDirectory;
	//��굱ǰ����λ��
	CPoint mousePoint;
	//��ǰ����µĴ�������
	char mouseDownWindowTitle[100] = { 0 };

	/************************/
	//��������Ϊ��ʼ�����ݣ��Գ�ʼ�����ݽ������ó��������״̬�����Զ�����һ������Ϊʵ������
	/************************/
	//ˮ����������     ��ʼ����ʵ��Ϊ*20
	int rippleSlidStrength = 0;
	//ˮ������Ƶ��,,�����Ϊ1000/ʵ��Ƶ��    ����
	int rippleSlidHz = 1;
	//ˮ���������	��ʼ����ʵ��Ϊ*20
	int rippleClickStrength = 0;
	//��ǰ��Ƶ����Ŀ¼������
	int lineNumber = 0;
	//����������
	int playerVolume = 0;
	//�Ƿ�ѭ������	1Ϊѭ�����ţ�0Ϊ�б�ѭ��
	int loopPlay = 1;
	std::string nowFilePath;
	/***********************/
}