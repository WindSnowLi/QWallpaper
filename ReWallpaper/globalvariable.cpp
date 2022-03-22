
#include "globalvariable.h"
#include <memory>
namespace global {
	//视频路径
	std::vector <std::string> videoDirectory;
	//鼠标当前所在位置
	CPoint mousePoint;
	//当前鼠标下的窗口名称
	char mouseDownWindowTitle[100] = { 0 };

	/************************/
	//以下数据为初始化数据，以初始化数据进行设置程序初运行状态，会自动设置一下数据为实际数据
	/************************/
	//水波滑动力度     初始化后实际为*20
	int rippleSlidStrength = 0;
	//水波滑动频率,,本结果为1000/实际频率    所得
	int rippleSlidHz = 1;
	//水波点击力度	初始化后实际为*20
	int rippleClickStrength = 0;
	//当前视频所在目录的行数
	int lineNumber = 0;
	//播放器音量
	int playerVolume = 0;
	//是否循环播放	1为循环播放，0为列表循环
	int loopPlay = 1;
	std::string nowFilePath;
	//全局水波纹对象
	std::shared_ptr<CRipple> g_Ripple = std::make_shared<CRipple>();
	/***********************/
}