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

	// reimplemented (ISender)
	virtual bool SendResponse(ConstResponsePtr& response) const override;
	virtual bool SendRequest(ConstRequestPtr& reguest) const override;

private:
	QWebSocket* m_webSocketPtr;
};


} // namespace imtrest


