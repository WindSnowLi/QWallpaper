#ifndef SINGLEAPPLICATION_H
#define SINGLEAPPLICATION_H

#include <QObject>
#include <QApplication>

class QWidget;
class QLocalServer;

class SingleApplication : public QApplication
{
    Q_OBJECT
public:
    SingleApplication(int& argc, char** argv);
    bool isRunning();               // �Ƿ��Ѿ���ʵ��������
    QWidget* mainWindow;            // MainWindowָ��

private slots:
    // ��������ʱ����
    void newLocalConnection();

private:
    // ��ʼ����������
    void initLocalConnection();
    // ���������
    void newLocalServer();
    bool bRunning;                  // �Ƿ��Ѿ���ʵ��������
    QLocalServer* localServer;      // ����socket Server
    QString serverName;             // ��������
};

#endif // SINGLEAPPLICATION_H