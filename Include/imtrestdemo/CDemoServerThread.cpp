// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrestdemo/CDemoServerThread.h>


// Qt includes
#include <QtCore/QMetaObject>
#include <QtCore/QObject>
#include <QtCore/QThread>

// ImtCore includes
#include <imtcom/CRequestSender.h>


namespace imtrestdemo
{


CDemoServerThread::CDemoServerThread(const DemoCompositionFactory& factory)
	:m_factory(factory)
{
}


CDemoServerThread::~CDemoServerThread()
{
	Stop();
}


bool CDemoServerThread::Start(const CDemoServerSettings& settings)
{
	Stop();

	// The manager of the workers primes the network of Qt in the thread of the application if it was not done yet: it waits for
	// that thread, which waits here. So it is done before the thread of the server starts.
	imtcom::CRequestSender::InitializeNetworkBackend();

	m_threadPtr.reset(new QThread);
	m_threadPtr->setObjectName(QStringLiteral("ImtRestDemoServerThread"));
	m_threadPtr->start();

	m_contextPtr.reset(new QObject);
	m_contextPtr->moveToThread(m_threadPtr.get());

	m_serverPtr.reset(new CDemoServer(m_factory));

	bool isStarted = false;

	// The server is created and works in the thread, its objects belong to it
	QMetaObject::invokeMethod(m_contextPtr.get(), [this, &settings, &isStarted](){
		isStarted = m_serverPtr->Start(settings);
		m_errorString = m_serverPtr->GetErrorString();
		m_port = m_serverPtr->GetPort();
		m_filesDirectory = m_serverPtr->GetFilesDirectory();
		m_uploadDirectory = m_serverPtr->GetUploadDirectory();
		m_info = m_serverPtr->GetInfo();
	}, Qt::BlockingQueuedConnection);

	if (!isStarted){
		Stop();
	}

	return isStarted;
}


void CDemoServerThread::Stop()
{
	if (m_threadPtr == nullptr){
		return;
	}

	if (m_serverPtr != nullptr){
		QMetaObject::invokeMethod(m_contextPtr.get(), [this](){
			m_serverPtr->Stop();
		}, Qt::BlockingQueuedConnection);
	}

	m_threadPtr->quit();
	m_threadPtr->wait();

	m_serverPtr.reset();
	m_contextPtr.reset();
	m_threadPtr.reset();
}


bool CDemoServerThread::IsRunning() const
{
	return m_threadPtr != nullptr && m_serverPtr != nullptr;
}


int CDemoServerThread::GetPort() const
{
	return m_port;
}


QString CDemoServerThread::GetErrorString() const
{
	return m_errorString;
}


QString CDemoServerThread::GetFilesDirectory() const
{
	return m_filesDirectory;
}


QString CDemoServerThread::GetUploadDirectory() const
{
	return m_uploadDirectory;
}


QJsonObject CDemoServerThread::GetInfo() const
{
	return m_info;
}


} // namespace imtrestdemo
