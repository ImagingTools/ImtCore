// ImtCore includes
#include <graphqlclient/graphqlclient.h>

// Qt includes
#include <QtCore/QDebug>
#include <QtCore/QJsonObject>



class Subscriber: public QObject, virtual public graphqlclient::ISubscriber
{
	Q_OBJECT
	virtual void OnSubscriptionReceived(
		const QByteArray& subscriptionId,
		const QByteArray& subscriptionData) override
	{
		qDebug() << "OnSubscriptionReceived" << subscriptionId << subscriptionData;
	}

	virtual void OnSubscriptionStatusChanged(
		const QByteArray& subscriptionId,
		const SubscriptionStatus& status,
		const QString& message) override
	{
		qDebug() << "OnSubscriptionStatusChanged" << subscriptionId << status;
	}

};


