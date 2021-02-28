#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QObject>
#include <QtCore\qmutex.h>

class MyThread : public QObject
{
	Q_OBJECT

public:
	MyThread() {};
	~MyThread() {};
	//跳出线程的while循环
	virtual	void closeThread();
signals:
public slots:
	//线程主要功能
	virtual void startThreadSlot() {};
	//暂停线程，加锁
	virtual void PauseThread();
	//继续线程，解锁
	virtual void ContinueThread();
	//尝试加锁，成功返回true
	virtual bool toTryLock();
protected:
	QMutex m_mutex;
	volatile bool isStop = true;
};


class LoopPlayback :public MyThread {
	Q_OBJECT
public:
	LoopPlayback();
	~LoopPlayback() {};
signals:
public slots:
	void startThreadSlot();
};

class Ripple :public MyThread {
	Q_OBJECT
public:
	Ripple();
	~Ripple() {};
signals:
public slots:
	void startThreadSlot();
};
//都继承自Qt基类，会自动释放
//循环播放线程对象声明
extern LoopPlayback* looplaybackobject;
//循环播放线程容器声明
extern QThread* looplayback;
//水波纹线程对象声明
extern Ripple* rippleobject;
//水波纹线程容器声明
extern QThread* ripple;
#endif // !MYTHREAD_H