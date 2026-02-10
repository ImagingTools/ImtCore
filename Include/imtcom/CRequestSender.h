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

	static thread_local std::unique_ptr<QNetworkAccessManager> s_networkManagerPtr;
};


} // namespace imtcom


