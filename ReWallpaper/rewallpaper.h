#ifndef REWAPPPAPER_H_
#define REWAPPPAPER_H_
#include <QtWidgets/QMainWindow>
#include "ui_rewallpaper.h"
#include <atlimage.h>
#include <QMouseEvent>
#include <QSystemTrayIcon>
#include <qmenu.h>
class ReWallpaper : public QMainWindow
{
	Q_OBJECT

public:
	ReWallpaper(QWidget* parent = Q_NULLPTR);
	~ReWallpaper();
private:
	Ui::ReWallpaperClass ui;
	//ϵͳ��ֽ������
	CImage systemWallpaper_Buff;
	//�ָ�ԭ��ϵͳ��ֽ
	void RestoresWallpaper();
	//���س�������Ҫ������,����һЩ��������ĳ�ʼ������
	void ToLoadProgramItem();
	//��ʼ������ui
	void ToLoadProgramUI();
	//��ʼ��������Ҫ�߳�
	void InitializeThread();
	//��ʼ���ޱ߿����
	void InitializeNoBorder();
	//��ʼ������
	void InitializeTray();
private slots:
	//ѡ����Ƶ�ļ�
	void SelectFile();
	//��������
	void EndPlay();
	//ˮ���ƿ���
	void Ripple();
	//���ó����������ֽ
	//void SetMainWindowBackGround(QString picturePath);
	//��������
	void SetPlayerVolume(int volume);
	//���û���ˮ��Ƶ��
	void SetSlidRippleHz(int hz);
	//����ˮ����������
	void SetSlidRippleStrength(int strength);
	//����ˮ���������
	void SetClickRippleStrength(int strength);
	//�����ʷ���ż�¼
	void CleanPlayerTable();
	//�رճ���
	void closeEvent(QCloseEvent* event);
	//����������
	void SetAutoStart();
	//������Ϣ
	void ActiveTray(QSystemTrayIcon::ActivationReason reason);
	//ѭ������
	void SetLoopPlay();
private:
	//ԭ���ڱ���
	QPixmap  m_pixmap;
	//�����϶�
	QPoint m_ptPress;
	//����Ƿ���
	bool m_bLeftBtnPress = false;
	//����ָ��
	QSystemTrayIcon* m_systemTray;
	//���̲˵�
	QMenu* m_menu;
	//���̹�����
	QAction* m_action1;
protected:
	void mouseMoveEvent(QMouseEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
};
#endif // !REWAPPPAPER_H_