// Qt includes
#include <QtCore/QDebug>
#include <QtCore/QCoreApplication>
#include <QtCore/QJsonObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>


class Network: public QObject
{
	Q_OBJECT
public Q_SLOTS:
	void slotReadyRead()
	{
		QNetworkReply* reply = dynamic_cast<QNetworkReply*>(sender());
		if (reply != nullptr){
			QByteArray data = reply->readAll();
			qDebug() << "Reply data: " << data;
		}
	}

	void slotError(QNetworkReply::NetworkError error)
	{
		qDebug() << "Network error: " << error;
	}

	void slotSslErrors(const QList<QSslError> &errors)
	{
		for (const QSslError& sslError: errors){
			qDebug() << "Ssl error: " << sslError.errorString();
		}
	}

};


