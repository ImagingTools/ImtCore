#pragma once

// QtCore includes
#include <graphqlserver/graphqlserver.h>

// Qt includes
#include <QtCore/QSharedPointer>
#include <QtCore/QUrl>


namespace graphqlclient {


struct ClientSettings
{
	QUrl serverUrl;
	QUrl webSocketServerUrl;
	quint16 webSocketPort;
	QUrl pathToLog;
	QByteArray ProductName;
};

/**
	Interface for the subscription client.
*/
class ISubscriber
{
public:
	enum SubscriptionStatus
	{
		SS_UNKNOWN = 0,
		SS_UNREGISTERED,
		SS_IN_REGISTRATION,
		SS_REGISTERED
	};

	/**
		Handling the incomming data for a given subscription
		\param subscriptionId	ID of the subscription the client is listening
		\param subscriptionData	Related data
	*/
	virtual void OnSubscriptionReceived(
		const QByteArray& subscriptionId,
		const QByteArray& subscriptionData) = 0;

	/**
		Handling the incomming notification about the change of the subscription status.
		\param subscriptionId	ID of the subscription the client is listening
		\param status	New subscription status
		\param message
	*/
	virtual void OnSubscriptionStatusChanged(
		const QByteArray& subscriptionId,
		const SubscriptionStatus& status,
		const QString& message) = 0;
};


QByteArray RegisterSubscribtion(const graphqlserver::IGqlRequest& subscriptionRequest, ISubscriber& subscriber, QString& errorMessage);
bool UnregisterSubscription(const QByteArray& subscriptionId, QString& errorMessage);


} //namespace graphqlserver
