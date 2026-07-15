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

private:
	static void EnsureNetworkAccessManager();

private:
	class NetworkOperation
	{
	public:
		NetworkOperation() = delete;
		NetworkOperation(int timeout);
		~NetworkOperation();

		QEventLoop connectionLoop;
		QTimer timer;
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


