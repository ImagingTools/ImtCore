#pragma once


// Qt includes
#include <QtWebSockets/QtWebSockets>

// ImtCore includes
#include <imtrest/ISender.h>


namespace imtrest
{


class CWebSocketSender: public QObject, virtual public ISender
{
	Q_OBJECT
public:
	CWebSocketSender(QWebSocket* webSocketPtr);
	const QWebSocket* GetSocket() const;

	// reimplemented (ISender)
	virtual bool SendResponse(ConstResponsePtr& response) const override;
	virtual bool SendRequest(ConstRequestPtr& reguest) const override;

public Q_SLOTS:
	void OnSendTextMessage(const QByteArray& data) const;

Q_SIGNALS:
	void SendTextMessage(const QByteArray& data) const;

private:
	QWebSocket* m_webSocketPtr;
};


} // namespace imtrest


