#include "tool.h"
#include <QtCore\qdir.h>
#include <QSettings>
#include <QCoreApplication>
#include <qapplication.h>
#include <fstream>

/*
*┌────────────────────────────────────────────────┐
*│　描    述：QString转CString
*│　作    者：windSnowLi
*│　版    本：0.0.1
*│　创建时间：2020/05/31
*└────────────────────────────────────────────────┘
*┌────────────────────────────────────────────────┐
*│　命名空间：tool
*│　类    名：tool
*│　参    数：(QString)
*└────────────────────────────────────────────────┘
*/

CString  tool::QSTOCS(QString qs)
{
	return  qs.toStdString().c_str();
}

/*
*┌────────────────────────────────────────────────┐
*│　描    述：或许程序当前所属路径
*│　作    者：windSnowLi
*│　版    本：0.0.1
*│　创建时间：2020/05/31
*└────────────────────────────────────────────────┘
*┌────────────────────────────────────────────────┐
*│　命名空间：tool
*│　类    名：tool
*│　参    数：无
*└────────────────────────────────────────────────┘
*/

QString tool::GetSystemUserName()
{
	return QDir::home().dirName();
}

/*
*┌────────────────────────────────────────────────┐
*│　描    述：获取当前系统壁纸路径
*│　作    者：windSnowLi
*│　版    本：0.0.1
*│　创建时间：2020/05/31
*└────────────────────────────────────────────────┘
*┌────────────────────────────────────────────────┐
*│　命名空间：tool
*│　类    名：tool
*│　参    数：无
*└────────────────────────────────────────────────┘
*/

QString tool::GetSystemWallpaperPath()
{
	return "C:\\Users\\" + tool::GetSystemUserName() + "\\AppData\\Roaming\\Microsoft\\Windows\\Themes\\CachedFiles\\CachedImage_1920_1080_POS4.jpg";
}

/*
*┌────────────────────────────────────────────────┐
*│　描    述：清理视频列表中无效的视频路径
*│　作    者：windSnowLi
*│　版    本：0.0.1
*│　创建时间：2020/05/31
*└────────────────────────────────────────────────┘
*┌────────────────────────────────────────────────┐
*│　命名空间：tool
*│　类    名：tool
*│　参    数：(std::vector<std::string>&)
*└────────────────────────────────────────────────┘
*/

void tool::CleanVoidFilelist(std::vector<std::string>& filelist)
{
	std::vector <std::string>::iterator iter;
	iter = filelist.begin();
	std::string tempile;
	while (iter != filelist.end()) {
		tempile = *iter;
		if (!JudgeFile((char*)tempile.c_str())) {
			filelist.erase(iter);
			iter = filelist.begin();
		}
		else
		{
			iter++;
		}
	}
	if (filelist.size() > 50) {
		filelist.erase(filelist.begin(), filelist.end() - 50);
	}
}

/*
*┌────────────────────────────────────────────────┐
*│　描    述：判断文件是否存在
*│　作    者：windSnowLi
*│　版    本：0.0.1
*│　创建时间：2020/05/31
*└────────────────────────────────────────────────┘
*┌────────────────────────────────────────────────┐
*│　命名空间：tool
*│　类    名：tool
*│　参    数：(char*)
*└────────────────────────────────────────────────┘
*/
bool tool::JudgeFile(char* path)
{
	QFile file(path);
	if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		file.close();
		return true;
	}
	file.close();
	return false;
}

/*
*┌────────────────────────────────────────────────┐
*│　描    述：将/替换成‘\\\\’
*│　作    者：windSnowLi
*│　版    本：0.0.1
*│　创建时间：2020/05/31
*└────────────────────────────────────────────────┘
*┌────────────────────────────────────────────────┐
*│　命名空间：tool
*│　类    名：tool
*│　参    数：(QString &)
*└────────────────────────────────────────────────┘
*/

void tool::PathConvert(QString& ch) {
	ch.replace("/", "\\");
}

/*
*┌────────────────────────────────────────────────┐
*│　描    述：自启动设置
*│　作    者：windSnowLi
*│　版    本：0.0.1
*│　创建时间：2020/05/31
*└────────────────────────────────────────────────┘
*┌────────────────────────────────────────────────┐
*│　命名空间：tool
*│　类    名：tool
*│　参    数：(int) 参数int 0为取消自启动，1为设置自启动，2为查询自启动，自启动返回true,不是自启动返回false
*└────────────────────────────────────────────────┘
*/
bool tool::OperationAutoStart(int operation)
{
	//程序名称
	QString appName = QApplication::applicationName();
	//QString appName = "Wallpaper";
	// 程序路径
	QString appPath = GetProgramPath();
	tool::PathConvert(appPath);
	appPath = "\"" + appPath + "\\" + appName + ".exe\" auto";
	QSettings* reg = new QSettings(
		"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
		QSettings::NativeFormat);
	// 如果此键不存在，则返回的是空字符串
	QString val = reg->value(appName).toString();
	switch (operation) {
		//取消自启动
	case 0:
		//移除键
		reg->remove(appName);
		break;
		//设置自启动
	case 1:
		if (val != appPath) {
			//设置键
			reg->setValue(appName, appPath);
		}
		break;
		//查询自启动
	case 2:
		if (val.compare(appPath) == 0) {
			reg->deleteLater();
			return true;
		}
		else {
			reg->deleteLater();
			return false;
		}
		break;
	default:
		reg->deleteLater();
		return false;
	}
	return false;
}

/*
*┌────────────────────────────────────────────────┐
*│　描    述：获取程序路径
*│　作    者：windSnowLi
*│　版    本：0.0.1
*│　创建时间：2020/05/31
*└────────────────────────────────────────────────┘
*┌────────────────────────────────────────────────┐
*│　命名空间：tool
*│　类    名：tool
*│　参    数：无
*└────────────────────────────────────────────────┘
*/

QString tool::GetProgramPath()
{
	return QCoreApplication::applicationDirPath();
}

/*
*┌────────────────────────────────────────────────┐
*│　描    述：查找一个历史播放记录里一个存在的视频文件
*│　作    者：windSnowLi
*│　版    本：0.0.1
*│　创建时间：2020/05/31
*└────────────────────────────────────────────────┘
*┌────────────────────────────────────────────────┐
*│　命名空间：tool
*│　类    名：tool
*│　参    数：(std::vector <std::string>&,int &)
*└────────────────────────────────────────────────┘
*/
std::string tool::GetValidFilePath(std::vector <std::string>& filelist, int& linenumber)
{
	//判断当前行数是否达到总行数
	if (linenumber >= filelist.size()) {
		linenumber = 0;
	}
	std::vector <std::string>::iterator iter;
	//偏移到第lineNumber个
	iter = filelist.begin() + linenumber;
	while (iter != filelist.end())
	{
		if (linenumber >= filelist.size()) {
			linenumber = 0;
		}
		linenumber++;
		QFile file(iter->c_str());
		if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
			return *iter;
		}
	}
	return " ";
}