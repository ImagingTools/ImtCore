// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtcom/CRequestSender.h>


// Qt includes
#include <QtCore/QMetaObject>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QThread>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QSslConfiguration>
#include <QtNetwork/QSslSocket>


namespace imtcom
{


// public methods


thread_local QNetworkAccessManager* CRequestSender::s_networkManagerPtr = nullptr;
static QNetworkAccessManager* s_mainThreadNetworkProbePtr = nullptr;
static QMutex s_networkBackendMutex;


QNetworkReply* CRequestSender::DoSyncGet(const QNetworkRequest& request, int timeout)
{
	EnsureNetworkAccessManager();

	NetworkOperation networkOperation(timeout);

	QNetworkReply* replyPtr = s_networkManagerPtr->get(request);
	if (replyPtr != nullptr){
		QObject::connect(replyPtr, &QNetworkReply::finished, &networkOperation.connectionLoop, &QEventLoop::quit);

		replyPtr->ignoreSslErrors();

		networkOperation.connectionLoop.exec(QEventLoop::ExcludeUserInputEvents);

		if (replyPtr->isRunning()){
			replyPtr->abort();
		}
	}

	return replyPtr;
}


QNetworkReply* CRequestSender::DoSyncPut(const QNetworkRequest& request, const QByteArray& data, int timeout)
{
	EnsureNetworkAccessManager();

	NetworkOperation networkOperation(timeout);

	QNetworkReply* replyPtr = s_networkManagerPtr->put(request, data);
	if (replyPtr != nullptr){
		QObject::connect(replyPtr, &QNetworkReply::finished, &networkOperation.connectionLoop, &QEventLoop::quit);

		networkOperation.connectionLoop.exec(QEventLoop::ExcludeUserInputEvents);

		if (replyPtr->isRunning()){
			replyPtr->abort();
		}
	}

	return replyPtr;
}


QNetworkReply* CRequestSender::DoSyncPost(const QNetworkRequest& request, const QByteArray& data, int timeout)
{
	EnsureNetworkAccessManager();

	NetworkOperation networkOperation(timeout);

	QNetworkReply* replyPtr = s_networkManagerPtr->post(request, data);
	if (replyPtr != nullptr){
		QObject::connect(replyPtr, &QNetworkReply::finished, &networkOperation.connectionLoop, &QEventLoop::quit);

		replyPtr->ignoreSslErrors();

		networkOperation.connectionLoop.exec(QEventLoop::ExcludeUserInputEvents);

		if (replyPtr->isRunning()){
			replyPtr->abort();
		}
	}

	return replyPtr;
}


QNetworkReply* CRequestSender::DoSyncCustomRequest(const QNetworkRequest& request, const QByteArray& verb, const QByteArray& data, int timeout)
{
	EnsureNetworkAccessManager();

	NetworkOperation networkOperation(timeout);

	QNetworkReply* replyPtr = s_networkManagerPtr->sendCustomRequest(request, verb, data);
	if (replyPtr != nullptr){
		QObject::connect(replyPtr, &QNetworkReply::finished, &networkOperation.connectionLoop, &QEventLoop::quit);

		networkOperation.connectionLoop.exec(QEventLoop::ExcludeUserInputEvents);

		if (replyPtr->isRunning()){
			replyPtr->abort();
		}
	}

	return replyPtr;
}


// private methods

void CRequestSender::InitializeNetworkBackend()
{
	EnsureNetworkBackendInitialized();
}


void CRequestSender::EnsureNetworkBackendInitialized()
{
	// Fast path without lock once primed.
	if (s_mainThreadNetworkProbePtr != nullptr){
		return;
	}

	QCoreApplication* appPtr = QCoreApplication::instance();
	if (appPtr == nullptr){
		return;
	}

	auto primeBackendOnAppThread = [appPtr]() {
		QMutexLocker lock(&s_networkBackendMutex);
		if (s_mainThreadNetworkProbePtr != nullptr){
			return;
		}

		// Keep one QNAM for process lifetime, parented to qApp on the app thread.
		// Destroying a temporary QNAM is not always enough — Qt may lazy-init more
		// globals on the first real use; a long-lived main-thread QNAM covers that.
		s_mainThreadNetworkProbePtr = new QNetworkAccessManager(appPtr);

		// SSL backend load (OpenSSL etc.) also creates process-wide QObjects;
		// do it here so a worker's first HTTPS/WSS never owns that init.
		Q_UNUSED(QSslSocket::supportsSsl());
		Q_UNUSED(QSslConfiguration::defaultConfiguration());
	};

	if (QThread::currentThread() == appPtr->thread()){
		primeBackendOnAppThread();
		return;
	}

	// Worker (or other secondary) path: hop to the app thread. Main is in exec()
	// while workers serve requests, so BlockingQueuedConnection is safe.
	QMetaObject::invokeMethod(appPtr, primeBackendOnAppThread, Qt::BlockingQueuedConnection);
}


void CRequestSender::EnsureNetworkAccessManager()
{
	EnsureNetworkBackendInitialized();

	if (s_networkManagerPtr == nullptr){
		// Per-thread QNAM (no parent): each worker that does sync HTTP gets its own.
		s_networkManagerPtr = new QNetworkAccessManager();
	}
}


// public methods of the embedded class NetworkOperation

CRequestSender::NetworkOperation::NetworkOperation(int timeout)
{
	// If the application will be finished, the internal event loop will be also finished:
	QObject::connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, &connectionLoop, &QEventLoop::quit);

	// Timeout: singleShot parented to the loop (same thread) — not a free QTimer
	// and not qApp, so no cross-thread child-of-QCoreApplication warning.
	if (timeout > 0){
		QTimer::singleShot(timeout, &connectionLoop, &QEventLoop::quit);
	}
}


CRequestSender::NetworkOperation::~NetworkOperation()
{
	QObject::disconnect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, &connectionLoop, &QEventLoop::quit);

	Q_ASSERT(!connectionLoop.isRunning());
}


} // namespace imtcom


