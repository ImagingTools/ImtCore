#pragma once


// Qt includes
#include <QtCore/QString>


namespace imtwidgets
{


class CWindowSystem
{
public:
	static qint64 FindProcessIdByTitle(const QString& title);
	static bool RaiseWindowByProcessId(qint64 processId);
	static bool RaiseWindowByTitle(const QString& title);
	static bool RaiseWindowByProcessTitle(const QString& title);

protected:
	static Qt::HANDLE FindHandleByProcessId(qint64 processId);
	static Qt::HANDLE FindHandleByTitle(const QString& title);
};


} // namespace imtwidgets


