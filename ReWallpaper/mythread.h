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
	//�����̵߳�whileѭ��
	virtual	void closeThread();
signals:
public slots:
	//�߳���Ҫ����
	virtual void startThreadSlot() {};
	//��ͣ�̣߳�����
	virtual void PauseThread();
	//�����̣߳�����
	virtual void ContinueThread();
	//���Լ������ɹ�����true
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
//ѭ�������̶߳�������
extern LoopPlayback* looplaybackobject;
//ѭ�������߳���������
extern QThread* looplayback;
//ˮ�����̶߳�������
extern Ripple* rippleobject;
//ˮ�����߳���������
extern QThread* ripple;
#endif // !MYTHREAD_H