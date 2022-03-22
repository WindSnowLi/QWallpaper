﻿#include "qwallpaper.h"
#include <QtWidgets/QApplication>
#include <QSplashScreen>
#include <QElapsedTimer>
#include "SingleApplication.h"
#include <QMessageBox>
#include "CRippleWidget.h"
int main(int argc, char *argv[])
{
	SingleApplication app(argc, argv);
	CRippleWidget c;
	c.show();
	return app.exec();
    if (!app.isRunning())
    {
        QWallpaper w;
        //传入一个要激活程序的窗口，当多开时会激活已有进程的窗口，且多开失败
        app.mainWindow = &w;
        w.show();
		//如果有启动参数auto，说明为注册表自启动，启动后自动隐藏
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
