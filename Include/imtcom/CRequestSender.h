// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>
#include <QtNetwork/QNetworkAccessManager>


namespace imtcom
{


/**
*	Helper implementation for executing network requests.
*/
class CRequestSender: public QObject
{
public:
	static QNetworkReply* DoSyncGet(const QNetworkRequest& request, int timeout);
	static QNetworkReply* DoSyncPut(const QNetworkRequest& request, const QByteArray& data, int timeout);
	static QNetworkReply* DoSyncPost(const QNetworkRequest& request, const QByteArray& data, int timeout);
	static QNetworkReply* DoSyncCustomRequest(const QNetworkRequest& request, const QByteArray& verb, const QByteArray& data, int timeout);

	/**
		Prime Qt network/SSL global objects on the application thread.

		Qt installs several process-wide QObjects as children of QCoreApplication
		on the first QNetworkAccessManager / SSL touch. If that first touch is on
		a CWorkerThread, Qt logs once:
		  QObject: Cannot create children for a parent that is in a different thread
		  (Parent is QCoreApplication..., current thread is CWorkerThread...)
		Subsequent requests are quiet because globals already exist.

		Call from main-thread component setup (e.g. CWorkerManagerComp) before any
		worker runs. Safe to call repeatedly / from any thread.
	*/
	static void InitializeNetworkBackend();

private:
	static void EnsureNetworkAccessManager();
	static void EnsureNetworkBackendInitialized();

private:
	class NetworkOperation
	{
	public:
		NetworkOperation() = delete;
		NetworkOperation(int timeout);
		~NetworkOperation();

		// QEventLoop only — no QTimer member. A stack QTimer on a worker used to
		// contribute to affinity warnings when Qt's timer machinery first touched
		// process-wide objects under QCoreApplication. Timeout uses singleShot
		// with the loop as context (same thread as the loop).
		QEventLoop connectionLoop;
	};

	// Intentionally a raw, never-deleted pointer (not std::unique_ptr): a unique_ptr
	// here registers a dynamic TLS/atexit destructor that can run after Qt's own
	// thread/event infrastructure has already been torn down at process exit,
	// which crashes (c0000409) when ~QNetworkAccessManager touches it. Leaking one
	// instance per thread that ever sent a request is a harmless, one-time cost
	// reclaimed by the OS on process exit.
	static thread_local QNetworkAccessManager* s_networkManagerPtr;
};


} // namespace imtcom


