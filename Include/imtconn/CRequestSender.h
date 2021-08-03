#pragma once


// Qt includes
#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>
#include <QtNetwork/QNetworkAccessManager>


namespace imtconn
{


class CRequestSender: public QObject
{
public:
	static QNetworkReply* Get(const QNetworkRequest& request, int timeout);
	static QNetworkReply* Put(const QNetworkRequest& request, const QByteArray& data, int timeout);
	static QNetworkReply* Post(const QNetworkRequest& request, const QByteArray& data, int timeout);
	static QNetworkReply* CustomRequest(const QNetworkRequest& request, const QByteArray& verb, const QByteArray& data, int timeout);

private:
	class WorkingObjects
	{
	public:
		WorkingObjects() = delete;
		WorkingObjects(int timeout);
		~WorkingObjects();

		QNetworkAccessManager* networkManagerPtr;
		QEventLoop connectionLoop;
		QTimer timer;
	};
};


} // namespace imtconn


