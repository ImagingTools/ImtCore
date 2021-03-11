#pragma once


// Qt includes
#include <QtCore/QString>


class CWindowSystem
{
public:
	static qint64 FindProcessIdByTitle(const QString& title);
	static void RaiseWindowByProcessId(qint64 processId);
	static void RaiseWindowByTitle(const QString& title);

protected:
	static Qt::HANDLE FindHandleByProcessId(qint64 processId);
	static Qt::HANDLE FindHandleByTitle(const QString& title);
};


