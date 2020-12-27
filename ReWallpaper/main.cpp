#include "qwallpaper.h"
#include <QtWidgets/QApplication>
#include <QSplashScreen>
#include <QElapsedTimer>
#include "SingleApplication.h"
#include <QMessageBox>
int main(int argc, char *argv[])
{
	SingleApplication app(argc, argv);
    if (!app.isRunning())
    {
        QWallpaper w;
        //����һ��Ҫ�������Ĵ��ڣ����࿪ʱ�ἤ�����н��̵Ĵ��ڣ��Ҷ࿪ʧ��
        app.mainWindow = &w;
        w.show();
		//�������������auto��˵��Ϊע������������������Զ�����
		if (argc >= 2)
		{
			QString tempargv;
			if (argv[1] != NULL) {
				tempargv = argv[1];
				tempargv.replace(" ", "");
			}

			if (strcmp(tempargv.toStdString().c_str(), "auto") == 0)
			{
				w.hide();
			}

		}

        return app.exec();
    }
	return 0;
}
