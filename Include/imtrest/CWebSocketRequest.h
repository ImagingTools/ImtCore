// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QUrl>
#include <QtNetwork/QHostAddress>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtrest/IRequest.h>
#include <imtrest/IRequestServlet.h>


namespace imtrest
{


class IRequestEventHandler: public istd::IPolymorphic
{
public:
	virtual void OnRequestDestroyed(IRequest* request) = 0;
};


class CWebSocketRequest: public QObject, virtual public IRequest
{
	Q_OBJECT
public:
	enum MethodType
	{
		MT_UNKNOWN = 0,
		MT_CONNECTION_INIT,
		MT_CONNECTION_ACK,
		MT_KEEP_ALIVE,
		MT_KEEP_ALIVE_ACK,
		MT_START,
		MT_SUBSCRIBE,
		MT_START_ACK,
		MT_ERROR,
		MT_DATA,
		MT_STOP,
		MT_COMPLETE,
		MT_QUERY,
		MT_QUERY_DATA
	};

	CWebSocketRequest(const IProtocolEngine& engine);
	CWebSocketRequest(const IRequestServlet& requestHandler, const IProtocolEngine& engine);
	~CWebSocketRequest();

	QByteArrayList GetHeaders() const;
	void SetHeader(const QByteArray &headerId, const QByteArray &headerData);
	QByteArray GetHeaderValue(const QByteArray& headerType) const;
	QUrl GetUrl() const;
	QByteArray GetClientId() const;
	QHostAddress GetRemoteAddress() const;
	void SetBody(const QByteArray& body);
	void SetRequestHandler(const IRequestServlet* requestHandlerPtr);
	MethodType GetMethodType() const;
	void SetMethodType(MethodType methodType);
	QByteArray GetQueryId() const;
	void RegisterRequestEventHandler(IRequestEventHandler* requestEventHandler);
	void SetCommandId(const QByteArray& commandId);

	// reimplemented (IRequest)
	virtual RequestState GetState() const override;
	virtual QByteArray GetCommandId() const override;
	virtual CommandParams GetCommandParams() const override;
	virtual QByteArray GetBody() const override;
	virtual QByteArray GetRequestId() const override;
	virtual bool ParseDeviceData(QIODevice& device) override;

	// reimplemented (INetworkObject)
	virtual const IProtocolEngine& GetProtocolEngine() const override;

	// reimplemented (istd::IChangeable)
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QHostAddress m_remoteAddress;
	QUrl m_url;
	RequestState m_state;
	QByteArray m_body;
	QByteArray m_clientId;
	QByteArray m_queryId;
	MethodType m_type;
	QByteArray m_commandId;

	const IRequestServlet* m_requestHandlerPtr;
	const IProtocolEngine& m_engine;

	typedef QMap<QByteArray, QByteArray> HeaderMap;
	HeaderMap m_headers;

	QByteArray m_lastHeader;
	QList<IRequestEventHandler*> m_requestEventHandlers;
	QByteArray m_requestId;
};


} // namespace imtrest


