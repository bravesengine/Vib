//#include <vld.h>


#include "MessageHandle.h"
#include "QtHapticMain.h"
#include <QtWidgets/QApplication>
#include "singleton.h"
#include "HapticModifFunc.h"
#include "HapticPlotKeyAction.h"

#include "Device/HapticDeviceManager.h"
#include "Device/HapticClipResouce.h"
#include "TableManager.h"
#include <dbghelp.h>


#include "FileSystem/HFS_Manager.h"

#ifdef WIN32

#include <windows.h>
#include <tchar.h>

LONG WINAPI unhandled_handler(struct _EXCEPTION_POINTERS* apExceptionInfo);

typedef BOOL(WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType, CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam, CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam, CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam);

void create_minidump(struct _EXCEPTION_POINTERS* apExceptionInfo)
{
	HMODULE mhLib = ::LoadLibrary(_T("dbghelp.dll"));
	MINIDUMPWRITEDUMP pDump = (MINIDUMPWRITEDUMP)::GetProcAddress(mhLib, "MiniDumpWriteDump");

	QDate Date = QDate::currentDate();
	QTime Time = QTime::currentTime();

	QString DumpFileName = QString("%1_%2_%3_%4_%5_%6_HapticEditor.dmp").arg(Date.year()).arg(Date.month()).arg(Date.day()).
		arg(Time.hour()).arg(Time.minute()).arg(Time.second());

	HANDLE  hFile = ::CreateFile( DumpFileName.toStdWString().c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);

	//HANDLE  hFile = ::CreateFile(_T("core.dmp"), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
	//	FILE_ATTRIBUTE_NORMAL, NULL);

	_MINIDUMP_EXCEPTION_INFORMATION ExInfo;
	ExInfo.ThreadId = ::GetCurrentThreadId();
	ExInfo.ExceptionPointers = apExceptionInfo;
	ExInfo.ClientPointers = FALSE;

	pDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, NULL, NULL);
	::CloseHandle(hFile);
}

LONG WINAPI unhandled_handler(struct _EXCEPTION_POINTERS* apExceptionInfo)
{
	create_minidump(apExceptionInfo);
	return EXCEPTION_CONTINUE_SEARCH;
}

#endif // WIN32

using namespace HT;

int main(int argc, char *argv[])
{
#ifdef WIN32
	SetUnhandledExceptionFilter(unhandled_handler);
#endif

	qInstallMessageHandler(myMessageHandler);

	QApplication a(argc, argv);
	QtHapticMain w;
	w.show();

	TableManager::instance()->LoadTable();

	int ret =  a.exec();

	//Singleton<HapticModifFunc>::DestroyInstance();
	Singleton<HapticPlotKeyAction>::DestroyInstance();
	Singleton<HapticClipResouce>::DestroyInstance();
	Singleton<HapticDeviceManager>::DestroyInstance();
	Singleton<TableManager>::DestroyInstance();
	
	Singleton<HFS_Manager>::DestroyInstance();

	//Singleton<BVF_Exporter>::DestroyInstance();
	//Singleton<BVF_Importer>::DestroyInstance();
		
	return 0;// ret;
}
