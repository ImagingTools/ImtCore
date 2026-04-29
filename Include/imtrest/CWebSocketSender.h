// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QPointer>
#include <QtWebSockets/QtWebSockets>

// ImtCore includes
#include <imtrest/ITransport.h>


namespace imtrest
{


class CWebSocketSender: public QObject, virtual public ITransport
{
	Q_OBJECT
public:
	CWebSocketSender(QWebSocket* webSocketPtr);
	const QWebSocket* GetSocket() const;

	// reimplemented (ITransport)
	virtual bool SendResponse(ConstResponsePtr& response) const override;
	virtual bool SendRequest(ConstRequestPtr& request) const override;

public Q_SLOTS:
	void OnSendTextMessage(const QByteArray& data) const;

Q_SIGNALS:
	void SendTextMessage(const QByteArray& data) const;

private:
	// QPointer auto-clears when the QWebSocket is destroyed (e.g. by deleteLater()
	// in CWebSocketServerComp::OnSocketDisconnected). A raw pointer here would
	// dangle after destruction and lead to crashes deep inside QWebSocket::qt_metacall
	// when OnSendTextMessage (queued to the main thread) eventually invokes
	// sendTextMessage on the destroyed socket.
	QPointer<QWebSocket> m_webSocketPtr;
};


} // namespace imtrest


