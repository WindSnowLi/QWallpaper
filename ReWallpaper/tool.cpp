﻿#include "tool.h"
#include <QtCore\qdir.h>
#include <QSettings>
#include <QCoreApplication>
#include <qapplication.h>
#include <fstream>
#include <cstring>
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

std::wstring tool::QSTOCS(QString qs)
{
	return  qs.toStdWString();
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
	QString pathBase = QString("C:\\Users\\") + tool::GetSystemUserName() + "\\AppData\\Roaming\\Microsoft\\Windows\\Themes\\CachedFiles\\";
	QDir dir(pathBase);
	QStringList nameFilters;
	nameFilters << "*.jpg" << "*.JPG";
	return pathBase + dir.entryList(nameFilters)[0];
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
	// 程序路径
	QString application_path = QApplication::applicationFilePath();
	tool::PathConvert(application_path);

	application_path = "\"" + application_path + "\" auto";
	QSettings reg(
		"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",
		QSettings::NativeFormat);
	// 如果此键不存在，则返回的是空字符串
	QString val = reg.value(appName).toString();
	switch (operation) {
		//取消自启动
	case 0:
		//移除键
		reg.remove(appName);
		break;

	case 1:
		//设置自启动
		if (val != application_path) {
			//写入随意的字符串都能写入，但唯独自启动的程序路径写入自动成空，加入这个先添加空的值得解决了
			reg.setValue(appName, "\"\"");
			//设置键
			reg.setValue(appName, application_path);
		}
		break;

	case 2:
		//查询自启动
		if (val.compare(application_path) == 0) {
			reg.deleteLater();
			return true;
		}
		else {
			reg.deleteLater();
			return false;
		}
		break;
	default:
		reg.deleteLater();
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