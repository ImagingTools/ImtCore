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
	class NetworkOperation
	{
	public:
		NetworkOperation() = delete;
		NetworkOperation(int timeout);
		~NetworkOperation();

		QNetworkAccessManager* networkManagerPtr;
		QEventLoop connectionLoop;
		QTimer timer;
	};
};


} // namespace imtcom


