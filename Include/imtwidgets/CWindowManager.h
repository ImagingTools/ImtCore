#pragma once


// Qt includes
#include <QtCore/QString>
#include <QtCore/QMutex>


class CWindowManager
{
public:
	CWindowManager();

	qint64 FindProcessIdByTitle(const QString& title);
	void RaiseWindowByProcessId(qint64 processId);
	void RaiseWindowByTitle(const QString& title);

	Qt::HANDLE GetHandle() {return m_handle;}
	void SetHandle(Qt::HANDLE handle) {m_handle = handle;}
	QString GetTitle() {return m_title;}

protected:
	Qt::HANDLE FindHandleByProcessId(qint64 processId);
	Qt::HANDLE FindHandleByTitle(const QString& title);

private:
	Qt::HANDLE m_handle;
	QString m_title;
};


