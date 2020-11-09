#include "mythread.h"
#include "globalvariable.h"
#include <qthread.h>
#include <Windows.h>
#include <qcursor.h>
#include <qdebug.h>
#include "videoplayer.h"
#include "tool.h"
//ѭ�������̶߳�����
LoopPlayback* looplaybackobject = new LoopPlayback();
//ѭ�������߳���������
QThread* looplayback = new  QThread();
//ˮ�����̶߳�����
Ripple* rippleobject = new Ripple();
//ˮ�����߳���������
QThread* ripple = new QThread();
void MyThread::closeThread()
{
	isStop = true;
}

void MyThread::PauseThread()
{
	m_mutex.lock();
}

void MyThread::ContinueThread()
{
	m_mutex.unlock();
}

bool MyThread::toTryLock()
{
	return m_mutex.tryLock();
}

LoopPlayback::LoopPlayback()
{
	isStop = false;
}

void LoopPlayback::startThreadSlot()
{
	float temp;
	QThread::msleep(2700);
	while (1) {
		QThread::msleep(300);
		temp = videoPlayer->get_position();
		if (global::loopPlay && temp >= 0.9) {
			videoPlayer->set_position(0.1);
		}
		else if (!global::loopPlay && temp >= 0.9) {
			std::string nextFilePath = tool::GetValidFilePath(global::videoDirectory, global::lineNumber);
			videoPlayer->loadPlayer(nextFilePath.c_str());
			global::nowFilePath = nextFilePath;
		}

		m_mutex.lock();
		m_mutex.unlock();
		if (isStop) {
			break;
		}
	}
}

Ripple::Ripple()
{
	isStop = false;
}
void Ripple::startThreadSlot()
{
	CPoint tempMousePosition = { 0 };
	while (1) {
		Sleep(global::rippleSlidHz);
		// ��ȡ��굱ǰλ��
		GetCursorPos(&global::mousePoint);
		// ��ȡ���ڱ���
		::GetWindowTextA(WindowFromPoint(global::mousePoint), global::mouseDownWindowTitle, sizeof(global::mouseDownWindowTitle));
		if ((strcmp(global::mouseDownWindowTitle, "FolderView") == 0) && tempMousePosition != global::mousePoint) {
			tempMousePosition = global::mousePoint;
			g_Ripple->DropStone(global::mousePoint.x, global::mousePoint.y, 2, global::rippleSlidStrength);
		}
		if ((strcmp(global::mouseDownWindowTitle, "FolderView") == 0) && (GetKeyState(1) & 0x8000 || GetAsyncKeyState(2) & 0x8000)) {
			g_Ripple->DropStone(global::mousePoint.x, global::mousePoint.y, 2, global::rippleClickStrength);
		}
		m_mutex.lock();
		m_mutex.unlock();
		if (isStop) {
			break;
		}
	}
}