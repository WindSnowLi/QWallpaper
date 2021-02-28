#ifndef REWAPPPAPER_H_
#define REWAPPPAPER_H_
#include <QtWidgets/QMainWindow>
#include "ui_rewallpaper.h"
#include <atlimage.h>
#include <QMouseEvent>
#include <QSystemTrayIcon>
#include <qmenu.h>
#include <memory>
class QWallpaper : public QMainWindow
{
	Q_OBJECT

public:
	QWallpaper(QWidget* parent = Q_NULLPTR);
	~QWallpaper();
private:
	Ui::ReWallpaperClass ui;
	//系统壁纸缓存区
	CImage systemWallpaper_Buff;
	//恢复原有系统壁纸
	void RestoresWallpaper();
	//加载程序所需要的内容,并做一些程序基本的初始化工作
	void ToLoadProgramItem();
	//初始化加载ui
	void ToLoadProgramUI();
	//初始化程序主要线程
	void InitializeThread();
	//初始化无边框界面
	void InitializeNoBorder();
	//初始化托盘
	void InitializeTray();
private slots:
	//选择视频文件
	void SelectFile();
	//结束播放
	void EndPlay();
	//水波纹开关
	void Ripple();
	//设置程序主界面壁纸
	//void SetMainWindowBackGround(QString picturePath);
	//设置音量
	void SetPlayerVolume(int volume);
	//设置滑动水波频率
	void SetSlidRippleHz(int hz);
	//设置水波滑动力度
	void SetSlidRippleStrength(int strength);
	//设置水波点击力度
	void SetClickRippleStrength(int strength);
	//清空历史播放记录
	void CleanPlayerTable();
	//关闭程序
	void closeEvent(QCloseEvent* event);
	//设置自启动
	void SetAutoStart();
	//托盘消息
	void ActiveTray(QSystemTrayIcon::ActivationReason reason);
	//循环播放
	void SetLoopPlay();
	//暂停播放
	void setPausePlay();
	//继续播放
	void setContinuePlay();
private:
	//原窗口背景
	QPixmap  m_pixmap;
	//设置拖动
	QPoint m_ptPress;
	//左键是否按下
	bool m_bLeftBtnPress = false;
	//托盘指针
	std::shared_ptr<QSystemTrayIcon> m_systemTray;
	//托盘菜单
	std::shared_ptr <QMenu> m_menu;
	//托盘退出
	std::shared_ptr<QAction> m_menu_exit;
	//托盘暂停
	std::shared_ptr<QAction> m_menu_pause;
	//托盘播放
	std::shared_ptr<QAction> m_menu_playing;
protected:
	void mouseMoveEvent(QMouseEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
};
#endif // !REWAPPPAPER_H_