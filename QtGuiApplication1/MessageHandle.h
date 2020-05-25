#pragma once

#include <QApplication>
#include <QtDebug>
#include <QFile>
#include <QTextStream>

static const QtMessageHandler QT_DEFAULT_MESSAGE_HANDLER = qInstallMessageHandler(0);

static void myMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString & msg)
{
    QString txt;
    switch (type) {
    case QtDebugMsg:
        txt = QString("Debug: %1").arg(msg);
        break;
    case QtWarningMsg:
        txt = QString("Warning: %1").arg(msg);
    break;
    case QtCriticalMsg:
        txt = QString("Critical: %1").arg(msg);
    break;
    case QtFatalMsg:
        txt = QString("Fatal: %1").arg(msg);
    break;
    }


  //  QFile outFile("log");
   // outFile.open(QIODevice::WriteOnly | QIODevice::Append); 
	//QTextStream ts(&outFile);    
	//ts << txt << endl;

	(*QT_DEFAULT_MESSAGE_HANDLER)(type, context, msg);
}

 //qInstallMessageHandler(myMessageHandler);   
