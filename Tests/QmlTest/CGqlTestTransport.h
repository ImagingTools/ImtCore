// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtCore/QVariantMap>
#include <QtQml/QQmlNetworkAccessManagerFactory>


namespace imtqmltest
{


/**
	Replaces the GraphQL server for QML tests: records the body of every POST sent by the QML engine
	and answers it with the configured response.
*/
class CGqlTestTransport: public QObject
{
	Q_OBJECT
	Q_PROPERTY(int requestCount READ GetRequestCount NOTIFY requestReceived)
	Q_PROPERTY(QString lastRequestBody READ GetLastRequestBody NOTIFY requestReceived)
	// header names are lower case
	Q_PROPERTY(QVariantMap lastRequestHeaders READ GetLastRequestHeaders NOTIFY requestReceived)
	Q_PROPERTY(QString responseBody READ GetResponseBody WRITE SetResponseBody NOTIFY responseChanged)
	Q_PROPERTY(int responseStatus READ GetResponseStatus WRITE SetResponseStatus NOTIFY responseChanged)

public:
	explicit CGqlTestTransport(QObject* parentPtr = nullptr);

	int GetRequestCount() const;
	QString GetLastRequestBody() const;
	QVariantMap GetLastRequestHeaders() const;
	QString GetResponseBody() const;
	void SetResponseBody(const QString& responseBody);
	int GetResponseStatus() const;
	void SetResponseStatus(int responseStatus);

	/**
		Register a request sent by the client.
	*/
	void RegisterRequest(const QByteArray& requestBody, const QVariantMap& requestHeaders);

	Q_INVOKABLE void reset();

	/**
		Parse a request body with the server-side GraphQL parser.
		\return {parsed, requestType, commandId, fields, params}; fields are nested maps with \c true for leaf fields.
	*/
	Q_INVOKABLE QVariantMap parseRequest(const QString& requestBody) const;
	Q_INVOKABLE QVariantMap parseLastRequest() const;

Q_SIGNALS:
	void requestReceived();
	void responseChanged();

private:
	int m_requestCount;
	QByteArray m_lastRequestBody;
	QVariantMap m_lastRequestHeaders;
	QByteArray m_responseBody;
	int m_responseStatus;
};


class CGqlTestNetworkAccessManagerFactory: public QQmlNetworkAccessManagerFactory
{
public:
	explicit CGqlTestNetworkAccessManagerFactory(CGqlTestTransport& transport);

	// reimplemented (QQmlNetworkAccessManagerFactory)
	virtual QNetworkAccessManager* create(QObject* parentPtr) override;

private:
	CGqlTestTransport& m_transport;
};


} // namespace imtqmltest


