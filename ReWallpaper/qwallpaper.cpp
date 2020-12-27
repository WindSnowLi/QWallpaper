#pragma execution_character_set("UTF-8")
#include "qwallpaper.h"
#include "videoplayer.h"
#include "globalvariable.h"
#include "ReadWriteini.hpp"
#include "tool.h"
#include <Qfile>
#include <QFileDialog>
#include <WinUser.h>
#include <fstream>
#include <QtWinExtras/qwinfunctions.h>
#include <CRipple.h>
#include "mythread.h"
#include <qthread.h>
#include <atlimage.h>
#include <qpalette.h>
#include <QSettings>
#include <QCoreApplication>
#include <unordered_map>
#include <memory>
#include <iostream>
#include <QMessageBox>
#pragma comment (lib, "CRIPPLE.lib")
#pragma comment (lib, "Qt5WinExtrasd.lib")


/*
*┌────────────────────────────────────────────────┐
*│　描    述：初始胡播放器目标窗口句柄
*│　作    者：windSnowLi
*│　版    本：0.0.1.1
*│　创建时间：2020/05/312020/05/13
*└────────────────────────────────────────────────┘
*┌────────────────────────────────────────────────┐
*│　命名空间：无
*│　类    名：无
*│　参    数：(HWND, LPARAM)
*└────────────────────────────────────────────────┘
*/

HWND videoPlayerWorkerw;
BOOL CALLBACK EnumwindowProcFindDesktopwindow(HWND hwnd, LPARAM lparam) {
	HWND p = ::FindWindowEx(hwnd, NULL, L"SHELLDLL_DefView", NULL);
	if (p == NULL)return 1;
	videoPlayerWorkerw = ::FindWindowEx(NULL, hwnd, L"Workerw", NULL);
	return 0;
}

/*
*┌────────────────────────────────────────────────┐
*│　描    述：窗口初始化
*│　作    者：windSnowLi
*│　版    本：0.0.1
*│　创建时间：2020/05/31
*└────────────────────────────────────────────────┘
*┌────────────────────────────────────────────────┐
*│　命名空间：QWallpaper
*│　类    名：QWallpaper
*│　参    数：(QWidget *)
*└────────────────────────────────────────────────┘
*/

QWallpaper::QWallpaper(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	//需要靠前才生效
	InitializeNoBorder();
	//首先加载程序所需要的内容,并做一些程序基本的初始化工作
	ToLoadProgramItem();
	//初始化主要线程
	InitializeThread();
	//初始化加载UI
	ToLoadProgramUI();
	//初始化托盘
	InitializeTray();
	//一经启动，暂定为继续播放上次的视频
	looplaybackobject->toTryLock();
	QString tempPath = tool::GetValidFilePath(global::videoDirectory, global::lineNumber).c_str();
	std::string tempPathStr = tempPath.toStdString();
	if (tool::JudgeFile((char*)tempPathStr.c_str())) {
		videoPlayer->loadPlayer(tempPathStr.c_str());
		looplaybackobject->ContinueThread();
		global::nowFilePath = tempPathStr;
		if (ui.loopplayer->isChecked()) {
			//因为获取路径会加一
			global::lineNumber--;
			ui.filename->setText(tempPath);
		}
		else
		{
			ui.filename->setText("自动播放");
		}
	}
}

/*
*┌────────────────────────────────────────────────┐
*│　描    述：主窗口析构函数
*│　作    者：windSnowLi
*│　版    本：0.0.1
*│　创建时间：2020/05/31
*└────────────────────────────────────────────────┘
*┌────────────────────────────────────────────────┐
*│　命名空间：QWallpaper
*│　类    名：QWallpaper
*│　参    数：无
*└────────────────────────────────────────────────┘
*/
QWallpaper::~QWallpaper()
{
	//退出程序时执行一次壁纸还原指令
	QWallpaper::EndPlay();
	//清理视频循环播放线程
	if (looplayback->isRunning()) {
		looplaybackobject->closeThread();
		looplaybackobject->toTryLock();
		looplaybackobject->ContinueThread();
		looplayback->quit();
		looplayback->wait();
	}
	//删除播放器
	if (NULL != videoPlayer) {
		delete videoPlayer;
		videoPlayer = NULL;
	}
	//清理水波纹循环线程
	if (ripple->isRunning()) {
		rippleobject->closeThread();
		rippleobject->toTryLock();
		rippleobject->ContinueThread();
		ripple->quit();
		ripple->wait();
	}
}

/*
*┌────────────────────────────────────────────────┐
*│　描    述：恢复系统原有壁纸
*│　作    者：windSnowLi
*│　版    本：0.0.1
*│　创建时间：2020/05/31
*└────────────────────────────────────────────────┘
*┌────────────────────────────────────────────────┐
*│　命名空间：QWallpaper
*│　类    名：QWallpaper
*│　参    数：无
*└────────────────────────────────────────────────┘
*/

void QWallpaper::RestoresWallpaper()
{
	CopyFile(tool::QSTOCS(tool::GetSystemWallpaperPath()), L"./picture/systemwallpaper/systemwallpaper.jpg", FALSE);
	QString tempWallpaperPathQStr = tool::GetProgramPath() + "\\picture\\systemwallpaper\\systemwallpaper.jpg";
	std::string tempWallpaperPathStr = tempWallpaperPathQStr.toStdString();
	char tempWallpaperPathchar[500];
	sprintf(tempWallpaperPathchar, tempWallpaperPathStr.c_str());
	SystemParametersInfoA(0x0014, 0, tempWallpaperPathchar, SPIF_UPDATEINIFILE);
}

/*
*┌────────────────────────────────────────────────┐
*│　描    述：选择视频，过滤掉非视频文件，vlc支持的视频格式非常全
*│　作    者：windSnowLi
*│　版    本：0.0.1
*│　创建时间：2020/05/31
*└────────────────────────────────────────────────┘
*┌────────────────────────────────────────────────┐
*│　命名空间：QWallpaper
*│　类    名：QWallpaper
*│　参    数：无
*└────────────────────────────────────────────────┘
*/

void QWallpaper::SelectFile() {
	QString filename;
	//限定只能打开*.txt文件
	filename = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr(
		"*.asf;*.avi;*.divx;*.dv;*.flv;*.gxf;*.m1v;"
		"*.m2v;*.m2ts;*.m4v;*.mkv;*.mov;*.mp2;*.mp4;"
		"*.mpeg;*.mpeg1;*.mpeg2;*.mpeg4;*.mpg;*.mts;"
		"*.mxf;*.ogg;*.ogm;*.ps;*.ts;*.vob;*.wmv;*.a52;"
		"*.aac;*.ac3;*.dts;*.flac;*.m4a;*.m4p;*.mka;"
		"*.mod;*.mp1;*.mp2;*.mp3;*.ogg;;"
		"所有 (*.*)"));
	if (!filename.isNull()) {
		//暂停循环播放线程
		looplaybackobject->toTryLock();
		QFile file(filename);
		tool::PathConvert(filename);
		std::string filepath = filename.toStdString();
		videoPlayer->loadPlayer(filepath.c_str());
		global::videoDirectory.push_back(filepath);
		//继续循环播放线程
		looplaybackobject->ContinueThread();
	}
}
/*
*┌────────────────────────────────────────────────┐
*│　描    述：结束播放时进行一些善后操作
*│　作    者：windSnowLi
*│　版    本：0.0.1
*│　创建时间：2020/05/31
*└────────────────────────────────────────────────┘
*┌────────────────────────────────────────────────┐
*│　命名空间：QWallpaper
*│　类    名：QWallpaper
*│　参    数：无
*└────────────────────────────────────────────────┘
*/

void QWallpaper::EndPlay()
{
	//尝试终止循环播放线程
	looplaybackobject->toTryLock();
	//释放播放器
	videoPlayer->releasePlayer();
	//还原壁纸
	QWallpaper::RestoresWallpaper();
}

/*
*┌────────────────────────────────────────────────┐
*│　描    述：水波纹设置按钮
*│　作    者：windSnowLi
*│　版    本：0.0.1
*│　创建时间：2020/05/31
*└────────────────────────────────────────────────┘
*┌────────────────────────────────────────────────┐
*│　命名空间：QWallpaper
*│　类    名：QWallpaper
*│　参    数：无
*└────────────────────────────────────────────────┘
*/

void QWallpaper::Ripple()
{
	if (ui.ripple->isChecked()) {
		rippleobject->toTryLock();
		rippleobject->ContinueThread();
		g_Ripple->startTimer();
	}
	else
	{
		g_Ripple->cancelTimer();
		rippleobject->PauseThread();
		QWallpaper::RestoresWallpaper();
	}
}
/*
*┌────────────────────────────────────────────────┐
*│　描    述：设置主程序背景
*│　作    者：windSnowLi
*│　版    本：0.0.1
*│　创建时间：2020/05/31
*└────────────────────────────────────────────────┘
*┌────────────────────────────────────────────────┐
*│　命名空间：QWallpaper
*│　类    名：QWallpaper
*│　参    数：(QString)
*└────────────────────────────────────────────────┘
*/
/*
void QWallpaper::SetMainWindowBackGround(QString picturePath)
{
	QPixmap pixmap = QPixmap(picturePath).scaled(this->size());
	QPalette palette(this->palette());
	palette.setBrush(QPalette::Background, QBrush(pixmap));
	this->setPalette(palette);
}
*/
/*
*┌────────────────────────────────────────────────┐
*│　描    述：设置播放器音量
*│　作    者：windSnowLi
*│　版    本：0.0.1
*│　创建时间：2020/05/31
*└────────────────────────────────────────────────┘
*┌────────────────────────────────────────────────┐
*│　命名空间：QWallpaper
*│　类    名：QWallpaper
*│　参    数：(int)
*└────────────────────────────────────────────────┘
*/

void QWallpaper::SetPlayerVolume(int volume)
{
	//设置音量
	videoPlayer->setVolume(volume);
	//设置显示音量示数
	ui.showVolume->setText(QString::number(volume, 10));
	//传递给播放器全局变量
	global::playerVolume = volume;
}
/*
*┌────────────────────────────────────────────────┐
*│　描    述：设置滑动水波频率
*│　作    者：windSnowLi
*│　版    本：0.0.1
*│　创建时间：2020/05/31
*└────────────────────────────────────────────────┘
*┌────────────────────────────────────────────────┐
*│　命名空间：QWallpaper
*│　类    名：QWallpaper
*│　参    数：(int)
*└────────────────────────────────────────────────┘
*/

void QWallpaper::SetSlidRippleHz(int hz)
{
	//设置显示水波纹滑动频率示数
	ui.showSlidHz->setText(QString::number(hz, 10));
	//传递给频率全局变量
	global::rippleSlidHz = 1000 / hz;
}
/*
*┌────────────────────────────────────────────────┐
*│　描    述：设置水波滑动力度
*│　作    者：windSnowLi
*│　版    本：0.0.1
*│　创建时间：2020/05/31
*└────────────────────────────────────────────────┘
*┌────────────────────────────────────────────────┐
*│　命名空间：QWallpaper
*│　类    名：QWallpaper
*│　参    数：(int)
*└────────────────────────────────────────────────┘
*/
void QWallpaper::SetSlidRippleStrength(int strength)
{
	//设置显示水波纹滑动力度示数
	ui.showSlidStrength->setText(QString::number(strength, 10));
	//传递给水波力度全局变量
	global::rippleSlidStrength = strength * 20;
}

/*
*┌────────────────────────────────────────────────┐
*│　描    述：设置点击水波力度
*│　作    者：windSnowLi
*│　版    本：0.0.1
*│　创建时间：2020/05/31
*└────────────────────────────────────────────────┘
*┌────────────────────────────────────────────────┐
*│　命名空间：QWallpaper
*│　类    名：QWallpaper
*│　参    数：(int)
*└────────────────────────────────────────────────┘
*/

void QWallpaper::SetClickRippleStrength(int strength)
{
	//设置显示水波纹点击力度示数
	ui.showClickStrength->setText(QString::number(strength, 10));
	//传递给水波力度全局变量
	global::rippleClickStrength = strength * 20;
}

/*
*┌────────────────────────────────────────────────┐
*│　描    述：清空播放器列表
*│　作    者：windSnowLi
*│　版    本：0.0.1
*│　创建时间：2020/05/31
*└────────────────────────────────────────────────┘
*┌────────────────────────────────────────────────┐
*│　命名空间：QWallpaper
*│　类    名：QWallpaper
*│　参    数：无
*└────────────────────────────────────────────────┘
*/

void QWallpaper::CleanPlayerTable()
{
	global::videoDirectory.clear();
	global::lineNumber = 0;
}

/*
*┌────────────────────────────────────────────────┐
*│　描    述：加载获得程序所需项
*│　作    者：windSnowLi
*│　版    本：0.0.1
*│　创建时间：2020/05/31
*└────────────────────────────────────────────────┘
*┌────────────────────────────────────────────────┐
*│　命名空间：QWallpaper
*│　类    名：QWallpaper
*│　参    数：无
*└────────────────────────────────────────────────┘
*/
void QWallpaper::ToLoadProgramItem()
{
	HWND hwnd_progman = ::FindWindow(L"Progman", NULL);
	if (hwnd_progman == NULL) {
		QMessageBox message(QMessageBox::NoIcon, "错误", "初始化失败");
		message.exec();
		exit(0);
	}
	DWORD_PTR result = 0;
	SendMessageTimeout(hwnd_progman, 0x052c, NULL, NULL, SMTO_NORMAL, 1000, &result);
	//寻找播放器目标窗口句柄
	EnumWindows(EnumwindowProcFindDesktopwindow, NULL);
	//将目标窗口句柄传递给播放器
	videoPlayer->workerw = videoPlayerWorkerw;
	//将目标窗口句柄传递给水波纹对象
	g_Ripple->workerw = videoPlayerWorkerw;
	//加载系统原有壁纸至缓存区   在这没有使用 QPixmap自带的QtWin::toHBITMAP(systemWallpaper_Buff);函数，因为使用CImage更省内存
	systemWallpaper_Buff.Load(tool::QSTOCS(tool::GetSystemWallpaperPath()));
	//转化系统壁纸格式并传给水波纹对象
	//系统壁纸HBITMAP缓存
	HBITMAP systemWallpaper_hbmp = (HBITMAP)systemWallpaper_Buff.operator HBITMAP();
	int tempWindowHwnd = QWidget::winId();
	g_Ripple->InitRipple((HWND)tempWindowHwnd, systemWallpaper_hbmp, 10);
	DeleteObject(systemWallpaper_hbmp);
	//拼接配置文件路径
	QString videoDirectory_config_file_path = tool::GetProgramPath() + "\\config\\videoDirectory.xml";
	//读取历史播放记录
	tool::BoostRead(videoDirectory_config_file_path, global::videoDirectory);
	//清理无效播放路径
	tool::CleanVoidFilelist(global::videoDirectory);

	//加载程序上次的的既定选择
	QString program_config_file_path = tool::GetProgramPath() + "\\config";


	QDir confif_dir(program_config_file_path);
	if (!confif_dir.exists())
	{
		if (!confif_dir.mkdir(program_config_file_path)) {
			exit(0);
		}
	}
	program_config_file_path += "\\config.ini";
	std::shared_ptr<rwini::ReadWriteini> RWini = std::make_shared<rwini::ReadWriteini>(program_config_file_path.toStdString().c_str());
	auto ini_table = std::make_shared<std::vector<std::string>>();
	ini_table->push_back("SlidStrength");
	ini_table->push_back("SlidHz");
	ini_table->push_back("ClickStrength");
	ini_table->push_back("LineNumber");
	ini_table->push_back("VideoPlayerVolume");
	ini_table->push_back("LoopPlay");

	int* p_initable[6];
	//水波滑动力度
	p_initable[0] = &global::rippleSlidStrength;
	//水波滑动频率
	p_initable[1] = &global::rippleSlidHz;
	//水波点击力度
	p_initable[2] = &global::rippleClickStrength;
	//当前视频所在目录的行数
	p_initable[3] = &global::lineNumber;
	//播放器音量
	p_initable[4] = &global::playerVolume;
	//是否循环播放	1为循环播放，0为列表循环
	p_initable[5] = &global::loopPlay;

	int initable_list = 0;

	for (auto&& i : *ini_table) {

		try
		{
			std::string&& temp_buff = RWini->FindValue("program", i);
			*p_initable[initable_list] = atoi(temp_buff.c_str());
		}
		catch (const rwini::RwiniException& e)
		{
			if (e.GetExceptionType() == rwini::RwExceptionType::NotFoundKey ||
				e.GetExceptionType() == rwini::RwExceptionType::NotFoundSection)
			{
				RWini->AutoInsertKey("program", i, 0);
				*p_initable[initable_list] = 0;
			}
		}
		initable_list++;
		if (initable_list > 5)
		{
			break;
		}

	}
}

/*
*┌────────────────────────────────────────────────┐
*│　描    述：UI基本初始化
*│　作    者：windSnowLi
*│　版    本：0.0.1
*│　创建时间：2020/05/31
*└────────────────────────────────────────────────┘
*┌────────────────────────────────────────────────┐
*│　命名空间：QWallpaper
*│　类    名：QWallpaper
*│　参    数：无
*└────────────────────────────────────────────────┘
*/
void QWallpaper::ToLoadProgramUI()
{
	//设置任务栏图标
	setWindowIcon(QIcon(":/ico/Resources/DynamicWallpaper.ico"));
	//设置音量图标
	QPixmap* pixmap = new QPixmap(tool::GetProgramPath() + "\\picture\\sound.ico");
	pixmap->scaled(ui.volume->size(), Qt::KeepAspectRatio);
	ui.volume->setScaledContents(true);
	ui.volume->setPixmap(*pixmap);
	//设置初始频率滑块位置
	if (global::rippleSlidHz == 0) {
		SetSlidRippleHz(1);
	}
	else {
		ui.rippleSlidHz->setValue(global::rippleSlidHz);
	}
	//设置初始滑动力度滑块位置
	if (global::rippleSlidStrength == 0) {
		SetSlidRippleStrength(global::rippleSlidStrength);
	}
	else {
		ui.rippleSlidStrength->setValue(global::rippleSlidStrength);
	}
	//设置初始点击力度滑块位置
	if (global::rippleClickStrength == 0) {
		SetClickRippleStrength(global::rippleClickStrength);
	}
	else {
		ui.rippleClickStrength->setValue(global::rippleClickStrength);
	}
	//检测是否自启动
	if (tool::OperationAutoStart(2)) {
		ui.SetAutoStart->setChecked(Qt::Checked);
	}
	ui.loopplayer->setChecked(global::loopPlay);
	//设置音量
	if (global::playerVolume == 0) {
		SetPlayerVolume(global::playerVolume);
	}
	else {
		ui.volumeSlider->setValue(global::playerVolume);
	}
	//设置是否循环播放
	ui.loopplayer->setChecked(global::loopPlay);
}
/*
*┌────────────────────────────────────────────────┐
*│　描    述：初始化程序的主要线程
*│　作    者：windSnowLi
*│　版    本：0.0.1
*│　创建时间：2020/05/31
*└────────────────────────────────────────────────┘
*┌────────────────────────────────────────────────┐
*│　命名空间：QWallpaper
*│　类    名：QWallpaper
*│　参    数：无
*└────────────────────────────────────────────────┘
*/
void QWallpaper::InitializeThread()
{
	//为创建视频循环播放做准备
	looplaybackobject->moveToThread(looplayback);
	//终止线程时要调用deleteLater槽函数
	connect(looplayback, SIGNAL(finished()), looplaybackobject, SLOT(deleteLater()));
	//开启线程槽函数
	connect(looplayback, SIGNAL(started()), looplaybackobject, SLOT(startThreadSlot()));
	connect(looplayback, SIGNAL(finished()), this, SLOT(finishedThreadSlot()));
	looplayback->start();
	looplaybackobject->PauseThread();

	//初始化水波纹线程
	rippleobject->moveToThread(ripple);
	//终止线程时要调用deleteLater槽函数
	connect(ripple, SIGNAL(finished()), rippleobject, SLOT(deleteLater()));
	//开启线程槽函数
	connect(ripple, SIGNAL(started()), rippleobject, SLOT(startThreadSlot()));
	connect(ripple, SIGNAL(finished()), this, SLOT(finishedThreadSlot()));
	ripple->start();
	rippleobject->PauseThread();
}

/*
*┌────────────────────────────────────────────────┐
*│　描    述：初始化无边框窗口
*│　作    者：windSnowLi
*│　版    本：0.0.1
*│　创建时间：2020/06/1
*└────────────────────────────────────────────────┘
*┌────────────────────────────────────────────────┐
*│　命名空间：QWallpaper
*│　类    名：QWallpaper
*│　参    数：无
*└────────────────────────────────────────────────┘
*/

void QWallpaper::InitializeNoBorder()
{
	setObjectName("QtWallpaper");
	//设置程序主窗口背景
	//SetMainWindowBackGround(GetProgramPath() + "\\picture\\background.jpg");
	//去掉标题栏
	setWindowFlags(Qt::FramelessWindowHint);
	//加载背景图
	m_pixmap.load(":/png/background/Resources/no_border_background.png");
	resize(m_pixmap.size());
	//设置窗口大小
	//this->resize(800, 600);
	setAttribute(Qt::WA_TranslucentBackground);
	QString temp_borderbackgroung_path = tool::GetProgramPath() + "\\picture\\no_border_background.png";
	temp_borderbackgroung_path.replace("\\", "/");
	temp_borderbackgroung_path = ".QWidget{image: url(" + temp_borderbackgroung_path + ");}";
	//路径不这么转换无法加载
	setStyleSheet(temp_borderbackgroung_path);
}

/*
*┌────────────────────────────────────────────────┐
*│　描    述：初始化托盘
*│　作    者：windSnowLi
*│　版    本：0.0.1
*│　创建时间：
*└────────────────────────────────────────────────┘
*┌────────────────────────────────────────────────┐
*│　命名空间：QWallpaper
*│　类    名：QWallpaper
*│　参    数：无
*└────────────────────────────────────────────────┘
*/

void QWallpaper::InitializeTray()
{
	m_systemTray = std::make_shared <QSystemTrayIcon>(this);
	m_systemTray->setIcon(QIcon(":/ico/Resources/DynamicWallpaper.ico"));
	m_systemTray->setToolTip("壁纸");
	m_systemTray->show();

	//点击托盘，执行相应的动作
	connect(m_systemTray.get(), &QSystemTrayIcon::activated, this, &QWallpaper::ActiveTray);
	//创建菜单
	m_menu = std::make_shared<QMenu>(this);
	//创建退出action
	m_menu_exit = std::make_shared<QAction>(m_menu.get());
	//设置退出action图标
	m_menu_exit->setIcon(QIcon(":/ico/Resources/menu_exit.ico"));
	//设置退出action文本
	m_menu_exit->setText("退出");
	//添加到菜单
	m_menu->addAction(m_menu_exit.get());
	//连接功能
	connect(m_menu_exit.get(), &QAction::triggered, this, &QApplication::quit);

	m_menu_pause = std::make_shared<QAction>(m_menu.get());
	m_menu_pause->setIcon(QIcon(":/ico/Resources/pause.png"));
	m_menu_pause->setText("暂停播放");
	m_menu->addAction(m_menu_pause.get());

	connect(m_menu_pause.get(), &QAction::triggered, this, &QWallpaper::setPausePlay);
	m_menu_playing = std::make_shared<QAction>(m_menu.get());
	m_menu_playing->setIcon(QIcon(":/ico/Resources/playing.png"));
	m_menu_playing->setText("继续播放");
	m_menu->addAction(m_menu_playing.get());
	connect(m_menu_playing.get(), &QAction::triggered, this, &QWallpaper::setContinuePlay);
	//设置托盘
	m_systemTray->setContextMenu(m_menu.get());
}




void QWallpaper::ActiveTray(QSystemTrayIcon::ActivationReason reason)
{
	switch (reason)
	{
	case QSystemTrayIcon::Trigger:
		if (this->isVisible()) {
			this->hide();
		}
		else {
			this->show();
		}
		break;
	}
}

/*
*┌────────────────────────────────────────────────┐
*│　描    述：设置循环播放,1为循环播放，0为列表循环
*│　作    者：windSnowLi
*│　版    本：1.0
*│　创建时间：2020/06/02
*└────────────────────────────────────────────────┘
*┌────────────────────────────────────────────────┐
*│　命名空间：QWallpaper
*│　类    名：QWallpaper
*│　参    数：无
*└────────────────────────────────────────────────┘
*/

void QWallpaper::SetLoopPlay()
{
	if (ui.loopplayer->isChecked()) {
		global::loopPlay = 1;
		ui.filename->setText(global::nowFilePath.c_str());
	}
	else {
		global::loopPlay = 0;
		ui.filename->setText("自动播放");
	}
}

void QWallpaper::setPausePlay()
{
	if (videoPlayer->getPlayingStatus()) {
		//尝试暂停循环播放线程
		looplaybackobject->toTryLock();
		videoPlayer->set_stop();
	}
}

void QWallpaper::setContinuePlay()
{
	if (videoPlayer->getVideoStatus() && !videoPlayer->getPlayingStatus()) {
		looplaybackobject->ContinueThread();
		videoPlayer->set_play();
	}
}

/*
*┌────────────────────────────────────────────────┐
*│　描    述：关闭程序做的一些善后操作
*│　作    者：windSnowLi
*│　版    本：0.0.1
*│　创建时间：2020/05/31
*└────────────────────────────────────────────────┘
*┌────────────────────────────────────────────────┐
*│　命名空间：QWallpaper
*│　类    名：QWallpaper
*│　参    数：(QCloseEvent*)
*└────────────────────────────────────────────────┘
*/
void QWallpaper::closeEvent(QCloseEvent* event)
{
	//拼接配置文件路径
	QString videoDirectory_config_file_path = tool::GetProgramPath() + "\\config\\videoDirectory.xml";
	//将播放历史记录输出到videoDirectory.xml配置文件
	tool::BoostWrite(videoDirectory_config_file_path, global::videoDirectory);
	//将当前设置的数据写入配置文件
	QString program_config_file_path = tool::GetProgramPath() + "\\config\\config.ini";
	std::shared_ptr<rwini::ReadWriteini> RWini = std::make_shared<rwini::ReadWriteini>(program_config_file_path.toStdString().c_str());
	std::string temp_section = "program";
	auto ini_table = std::make_shared<std::unordered_map<std::string, int>>();
	ini_table->insert(std::make_pair("VideoPlayerVolume", global::playerVolume));
	ini_table->insert(std::make_pair("SlidHz", (int)(1 / (global::rippleSlidHz * 1.0 / 1000))));
	ini_table->insert(std::make_pair("SlidStrength", global::rippleSlidStrength / 20));
	ini_table->insert(std::make_pair("ClickStrength", global::rippleClickStrength / 20));
	ini_table->insert(std::make_pair("LoopPlay", global::loopPlay));
	ini_table->insert(std::make_pair("LineNumber", global::lineNumber));
	for (auto&& i : *ini_table) {
		try
		{
			RWini->SetValue(temp_section, i.first, i.second);
		}
		catch (const rwini::RwiniException& e)
		{
			if (e.GetExceptionType() == rwini::RwExceptionType::NotFoundKey ||
				e.GetExceptionType() == rwini::RwExceptionType::NotFoundSection)
			{
				RWini->AutoInsertKey(temp_section, i.first, i.second);
			}
		}
	}
	RWini->Writeini();
	//恢复壁纸
	QWallpaper::RestoresWallpaper();
	Q_UNUSED(event);
}

/*
*┌────────────────────────────────────────────────┐
*│　描    述：设置自启动
*│　作    者：windSnowLi
*│　版    本：0.0.1
*│　创建时间：2020/05/31
*└────────────────────────────────────────────────┘
*┌────────────────────────────────────────────────┐
*│　命名空间：QWallpaper
*│　类    名：QWallpaper
*│　参    数：无
*└────────────────────────────────────────────────┘
*/
void QWallpaper::SetAutoStart()
{
	if (ui.SetAutoStart->isChecked()) {
		tool::OperationAutoStart(1);
	}
	else
	{
		tool::OperationAutoStart(0);
	}
}

/*
*┌────────────────────────────────────────────────┐
*│　描    述：记录按下位置,为无标题栏拖动做准备
*│　作    者：windSnowLi
*│　版    本：0.0.1
*│　创建时间：2020/06/1
*└────────────────────────────────────────────────┘
*┌────────────────────────────────────────────────┐
*│　命名空间：QWallpaper
*│　类    名：QWallpaper
*│　参    数：(QMouseEvent*)
*└────────────────────────────────────────────────┘
*/

void QWallpaper::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_ptPress = event->globalPos();
		m_bLeftBtnPress = true;
	}
	if (m_bLeftBtnPress && event->button() == Qt::RightButton) {
		this->close();
	}
	if ((!m_bLeftBtnPress) && (event->button() == Qt::RightButton)) {
		//窗口最小化
		//setWindowState(Qt::WindowMinimized);
		//窗口隐藏

		this->hide();
	}
	event->ignore();
}

/*
*┌────────────────────────────────────────────────┐
*│　描    述：移动窗口
*│　作    者：windSnowLi
*│　版    本：0.0.1
*│　创建时间：2020/06/1
*└────────────────────────────────────────────────┘
*┌────────────────────────────────────────────────┐
*│　命名空间：QWallpaper
*│　类    名：QWallpaper
*│　参    数：(QMouseEvent*)
*└────────────────────────────────────────────────┘
*/
void QWallpaper::mouseMoveEvent(QMouseEvent* event)
{
	if (m_bLeftBtnPress)
	{
		move(pos() + event->globalPos() - m_ptPress);
		m_ptPress = event->globalPos();
	}
	event->ignore();
}

/*
*┌────────────────────────────────────────────────┐
*│　描    述：鼠标移出操作
*│　作    者：windSnowLi
*│　版    本：0.0.1
*│　创建时间：2020/06/1
*└────────────────────────────────────────────────┘
*┌────────────────────────────────────────────────┐
*│　命名空间：QWallpaper
*│　类    名：QWallpaper
*│　参    数：(QMouseEvent*)
*└────────────────────────────────────────────────┘
*/
void QWallpaper::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
		m_bLeftBtnPress = false;
	event->ignore();
}