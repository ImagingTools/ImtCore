#pragma once


// QtCore includes
#include <graphqlserver/graphqlserver.h>

// Qt includes
#include <QtCore/QSharedPointer>
#include <QtCore/QUrl>
#include <QtCore/QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>


namespace graphqlclient
{


enum ClientStatus
{
	CS_UNKNOWN,
	CS_NOT_CONNECTED,
	CS_CONNECTED
};


struct ConnectionSettings
{
	QUrl serverUrl;
	QUrl webSocketServerUrl;
	QByteArray productId;
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


class GraphQlRequestManager: public QObject
{
	Q_OBJECT
public:
	GraphQlRequestManager(const ConnectionSettings& connectionSettings);
	void SendRequest(
				const QByteArray& commandId,
				const graphqlserver::RequestType requestType,
				const QJsonObject& inputParams,
				const graphqlserver::ResultKeys& resultKeys,
				QString& errorMessage);

protected:
	/**
		Abstract method for handling the answers from server.
	*/
	virtual void OnResponse(const QByteArray& data) = 0;

	ConnectionSettings m_connectionSettings;
	QNetworkAccessManager m_manager;

private Q_SLOTS:
	virtual void OnDataReceived();
	virtual void OnNetworkError(QNetworkReply::NetworkError error);
	virtual void OnSslErrors(const QList<QSslError> &errors);
};


bool ConnectToServer(const ConnectionSettings& connectionSettings, QString& errorMessage);
bool DisconnectFromServer(QString& errorMessage);
ClientStatus GetConnectionStatus();

QByteArray RegisterSubscription(const QByteArray& commantId, const graphqlserver::ResultKeys& resultKeys, ISubscriber& subscriber, QString& errorMessage);
bool UnregisterSubscription(const QByteArray& subscriptionId, QString& errorMessage);


} //namespace graphqlclient
