#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QCoreApplication::setApplicationName("Vibration Forge");
	QCoreApplication::setOrganizationName("Braves");
	QCoreApplication::setApplicationVersion(QT_VERSION_STR);
	//QCommandLineParser parser;
	//parser.setApplicationDescription("Vibration Forge");
	//parser.addHelpOption();
	//parser.addVersionOption();
	//parser.addPositionalArgument("file", "The file to open.");
	//parser.process(app);
	MainWindow mainWin;
	mainWin.show();
	return app.exec();
}
