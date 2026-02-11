// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CWebSocketRequest.h>


// Qt includes
#include <QtCore/QUrlQuery>
#include <QtCore/QUuid>
#if QT_CONFIG(ssl)
#include <QtNetwork/QSslSocket>
#endif
#include <QtWebSockets/QWebSocket>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>


namespace imtrest
{


// public methods

CWebSocketRequest::CWebSocketRequest(const IProtocolEngine& engine)
	: m_requestHandlerPtr(nullptr), m_engine(engine)
{
	m_requestId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
}


CWebSocketRequest::CWebSocketRequest(const IRequestServlet& handler, const IProtocolEngine& engine)
			:m_state(RS_NON_STARTED),
			m_requestHandlerPtr(&handler),
			m_engine(engine)
{
	m_requestId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
}


CWebSocketRequest::~CWebSocketRequest()
{
	for (int index = 0; index < m_requestEventHandlers.count(); index++){
		m_requestEventHandlers[index]->OnRequestDestroyed(this);
	}
}


QByteArrayList CWebSocketRequest::GetHeaders() const
{
	return m_headers.keys();
}


void CWebSocketRequest::SetHeader(const QByteArray& headerId, const QByteArray& headerData)
{
	m_headers.insert(headerId, headerData);
}

QByteArray CWebSocketRequest::GetHeaderValue(const QByteArray& headerType) const
{
	if (m_headers.contains(headerType)){
		return m_headers[headerType];
	}

	return QByteArray();
}


QUrl CWebSocketRequest::GetUrl() const
{
	return m_url;
}


QByteArray CWebSocketRequest::GetClientId() const{
	return m_clientId;
}


QByteArray CWebSocketRequest::GetBody() const
{
	return m_body;
}


QByteArray CWebSocketRequest::GetRequestId() const
{
	return m_requestId;
}


bool CWebSocketRequest::ParseDeviceData(QIODevice& /*device*/)
{
	return false;
}

QHostAddress CWebSocketRequest::GetRemoteAddress() const
{
	return m_remoteAddress;
}


void CWebSocketRequest::SetBody(const QByteArray &body)
{
	this->m_body = body;
	m_type = MT_UNKNOWN;
	QJsonDocument document = QJsonDocument::fromJson(body);
	QJsonObject object = document.object();
	if (object.value("type") == "connection_init"){
		m_type = MT_CONNECTION_INIT;
	}
	if (object.value("type") == "connection_ack"){
		m_type = MT_CONNECTION_ACK;
	}
	if (object.value("type") == "start"){
		m_type = MT_START;
	}
	if (object.value("type") == "subscribe"){
		m_type = MT_START;
	}
	if (object.value("type") == "start_ack"){
		m_type = MT_START_ACK;
	}
	if (object.value("type") == "error"){
		m_type = MT_ERROR;
	}
	if (object.value("type") == "connection_error"){
		m_type = MT_ERROR;
	}
	if (object.value("type") == "stop"){
		m_type = MT_STOP;
	}
	if (object.value("type") == "unsubscribe"){
		m_type = MT_STOP;
	}
	if (object.value("type") == "complete"){
		m_type = MT_STOP;
	}
	if (object.value("type") == "data"){
		m_type = MT_DATA;
	}
	if (object.value("type") == "next"){
		m_type = MT_DATA;
	}
	if (object.value("type") == "query"){
		m_type = MT_QUERY;
	}
	if (object.value("type") == "query_data"){
		m_type = MT_QUERY_DATA;
	}
	if (object.value("type") == "keep_alive"){
		m_type = MT_KEEP_ALIVE;
	}
	if (object.value("type") == "ping"){
		m_type = MT_KEEP_ALIVE;
	}
	if (object.value("type") == "pong"){
		m_type = MT_KEEP_ALIVE_ACK;
	}

	m_queryId = m_requestId = object.value("id").toString().toUtf8();
	m_clientId = object.value("clientid").toString().toUtf8();
}


void CWebSocketRequest::SetRequestHandler(const IRequestServlet* requestHandlerPtr)
{
	if (m_requestHandlerPtr != requestHandlerPtr){
		m_requestHandlerPtr = requestHandlerPtr;
	}
}


CWebSocketRequest::MethodType CWebSocketRequest::GetMethodType() const
{
	return m_type;
}

void CWebSocketRequest::SetMethodType(MethodType methodType)
{
	m_type = methodType;
}


QByteArray CWebSocketRequest::GetQueryId() const
{
	return m_requestId;
}


void CWebSocketRequest::RegisterRequestEventHandler(IRequestEventHandler* requestEventHandler)
{
	Q_ASSERT(requestEventHandler != nullptr);

	m_requestEventHandlers.append(requestEventHandler);
}


void CWebSocketRequest::SetCommandId(const QByteArray& commandId)
{
	m_commandId = commandId;
}


// reimplemented (IRequest)

IRequest::RequestState CWebSocketRequest::GetState() const
{
	return m_state;
}


QByteArray CWebSocketRequest::GetCommandId() const
{
	return m_commandId;
}


IRequest::CommandParams CWebSocketRequest::GetCommandParams() const
{
	if (m_url.hasQuery()){
		QUrlQuery query(m_url);

		CommandParams params;

		QList<QPair<QString, QString>> items = query.queryItems();
		for (const QPair<QString, QString>& item : items){
			params[item.first.toUtf8()] = item.second.toUtf8();
		}

		return params;
	}

	return CommandParams();
}


// reimplemented (INetworkObject)

const IProtocolEngine& CWebSocketRequest::GetProtocolEngine() const
{
	return m_engine;
}


// reimplemented (istd::IChangeable)

bool CWebSocketRequest::ResetData(CompatibilityMode /*mode*/)
{
	m_body.clear();
	m_headers.clear();
	m_lastHeader.clear();
	m_url.clear();

	m_state = RS_NON_STARTED;
	m_requestHandlerPtr = nullptr;

	return true;
}


} // namespace imtrest


