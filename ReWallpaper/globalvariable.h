#ifndef GLOBALVARIABLE_H_
#define GLOBALVARIABLE_H_ 
#include <vector>
#include <string>
#include "videoplayer.h"
#include <CRipple.h>
#include <atlimage.h>
#include <QtGui\qwindowdefs.h>
namespace global {
	//播放历史记录
	extern std::vector<std::string>videoDirectory;
	//鼠标当前所在位置
	extern CPoint mousePoint;
	//当前鼠标下的窗口名称
	extern char mouseDownWindowTitle[100];
	//水波滑动力度
	extern int rippleSlidStrength;
	//水波滑动频率
	extern int rippleSlidHz;
	//水波点击力度
	extern int rippleClickStrength;
	//当前视频所在目录的行数
	extern int lineNumber;
	//播放器音量
	extern int playerVolume;
	//是否循环播放	1为循环播放，2为列表循环
	extern int loopPlay;
	//当前播放的文件的路径
	extern std::string nowFilePath;
}
#endif // !GLOBALVARIABLE_H_