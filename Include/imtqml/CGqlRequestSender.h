#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtNetwork/QNetworkReply>


namespace imtqml
{


class CGqlRequestSender: public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString state READ State WRITE SetState NOTIFY stateChanged)

public:
	static void SetGlobalAccessToken(const QString& accessToken);

	Q_INVOKABLE inline bool setGqlQuery(QString query)
	{
		return SetGqlQuery(query);
	}

	bool SetGqlQuery(QString query);

public Q_SLOTS:
	virtual void OnReplyFinished(const QByteArray& data);
	void replyFinished();
	virtual void errorOccurred(QNetworkReply::NetworkError code);

Q_SIGNALS:
	void stateChanged(const QString& state);

protected:
	const QString& State() const;
	void SetState(const QString& newState);

protected:
	static QString s_accessToken;
	QString m_state;
};


} // namespace imtqml


