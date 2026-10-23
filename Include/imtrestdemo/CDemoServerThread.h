// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <memory>

// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QString>

// ImtCore includes
#include <imtrestdemo/CDemoServer.h>


class QThread;
class QObject;


namespace imtrestdemo
{


/**
	Demo server that works in its own thread with its own event loop. The thread that starts it (for example the thread of a test)
	can wait for the answers of the server without blocking it, so a test can use blocking sockets and can start a program
	such as curl and wait for it.
*/
class CDemoServerThread
{
public:
	/**
		\param	factory		Creates the composition of the server, see CreateDemoCompositionFactory().
	*/
	explicit CDemoServerThread(const DemoCompositionFactory& factory);
	~CDemoServerThread();

	/**
		Start the thread and the server in it, the call returns when the server listens or has failed.
		\return \c false if the server is not started, see GetErrorString().
	*/
	bool Start(const CDemoServerSettings& settings);

	/**
		Stop the server and the thread, the call returns when they are stopped.
	*/
	void Stop();

	bool IsRunning() const;
	int GetPort() const;
	QString GetErrorString() const;
	QString GetFilesDirectory() const;
	QString GetUploadDirectory() const;
	QJsonObject GetInfo() const;

private:
	DemoCompositionFactory m_factory;
	std::unique_ptr<QThread> m_threadPtr;
	std::unique_ptr<QObject> m_contextPtr;
	std::unique_ptr<CDemoServer> m_serverPtr;
	QString m_errorString;
	int m_port = 0;
	QString m_filesDirectory;
	QString m_uploadDirectory;
	QJsonObject m_info;
};


} // namespace imtrestdemo


