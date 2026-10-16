// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CHttpServerComp.h>


#ifdef IMTREST_HTTP_SERVER_AVAILABLE


// Qt includes
#include <QtNetwork/QSslConfiguration>
#include <QtNetwork/QTcpSocket>

// ACF includes
#include <iprm/TParamsPtr.h>
#include <iprm/IEnableableParam.h>

// ImtCore includes
#include <imtrest/IRequest.h>
#include <imtrest/IResponse.h>


namespace imtrest
{


// public methods

CHttpServerComp::CHttpServerComp()
{
}


CHttpServerComp::~CHttpServerComp()
{
	if (m_transportPtr){
		m_transportPtr->Shutdown();

		m_transportPtr.reset();
	}
}


// reimplemented (imtrest::IResponseDispatcher)

bool CHttpServerComp::SendResponse(const QByteArray& requestId, ConstResponsePtr& response) const
{
	if (!m_transportPtr || !response.IsValid()){
		return false;
	}

	IHttpTransportHandler::ResponseData responseData;

	int protocolStatusCode = 500;
	QByteArray statusLiteral;
	if (!response->GetProtocolEngine().GetProtocolStatusCode(response->GetStatusCode(), protocolStatusCode, statusLiteral)){
		protocolStatusCode = 500;
	}

	responseData.statusCode = protocolStatusCode;
	responseData.body = response->GetData();

	bool hasContentType = false;
	const IResponse::Headers headers = response->GetHeaders();
	for (IResponse::Headers::ConstIterator headerIter = headers.constBegin(); headerIter != headers.constEnd(); ++headerIter){
		if (headerIter.key().compare(QByteArrayLiteral("content-type"), Qt::CaseInsensitive) == 0){
			hasContentType = true;
		}

		responseData.headers.append(qMakePair(headerIter.key(), headerIter.value()));
	}

	const QByteArray dataTypeId = response->GetDataTypeId();
	if (!hasContentType && !dataTypeId.isEmpty()){
		responseData.headers.append(qMakePair(QByteArrayLiteral("Content-Type"), dataTypeId));
	}

	return m_transportPtr->SendResponse(requestId, responseData);
}


bool CHttpServerComp::SendRequest(const QByteArray& /*requestId*/, ConstRequestPtr& /*request*/) const
{
	// Server-initiated requests are not supported by HTTP
	return false;
}


// reimplemented (imtrest::IServer)

bool CHttpServerComp::StartServer()
{
	return EnsureServerStarted();
}


bool CHttpServerComp::StopServer()
{
	if (m_transportPtr){
		m_transportPtr->StopListening();
	}

	return true;
}


IServer::ServerStatus CHttpServerComp::GetServerStatus() const
{
	if (m_transportPtr && m_transportPtr->IsListening()){
		return SS_LISTENING;
	}

	return SS_NOT_STARTED;
}


// protected methods

// reimplemented (imtrest::IHttpTransportHandler)

bool CHttpServerComp::OnHttpRequest(const RequestData& request)
{
	// Called in an I/O thread: the servlets are processed in the thread of the component, the I/O thread is not blocked.
	return QMetaObject::invokeMethod(this, [this, request](){ ProcessRequest(request); }, Qt::QueuedConnection);
}


void CHttpServerComp::OnHttpRequestAborted(const QByteArray& /*requestId*/)
{
	// Nothing to do: a later response for the request is rejected by the transport
}


bool CHttpServerComp::OnWebSocketHandshake(QTcpSocket* socketPtr)
{
	if (!m_webSocketUpgradeHandlerCompPtr.IsValid()){
		return false;
	}

	// The ownership of the socket is transferred to the handler in any case:
	m_webSocketUpgradeHandlerCompPtr->HandleWebSocketHandshake(socketPtr);

	return true;
}


// reimplemented (imod::CMultiModelDispatcherBase)

void CHttpServerComp::OnModelChanged(int /*modelId*/, const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	Q_ASSERT_X(m_sslConfigurationCompPtr.IsValid() && m_sslConfigurationManagerCompPtr.IsValid(), "Update server's SSL configuration", "SSL configuration or manager is not set!");

	if (m_startServerOnCreateAttrPtr.IsValid() && *m_startServerOnCreateAttrPtr && m_isInitialized){
		EnsureServerStarted();
	}
}


// reimplemented (ibase::TRuntimeStatusHanderCompWrap)

void CHttpServerComp::OnSystemShutdown()
{
	BaseClass2::UnregisterAllModels();

	if (m_transportPtr){
		m_transportPtr->StopListening();
	}
}


// reimplemented (icomp::CComponentBase)

void CHttpServerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	CHttpTransport::Configuration configuration;
	configuration.ioThreadCount = qMax(0, *m_ioThreadCountAttrPtr);
	configuration.requestHeaderTimeout = qMax(0, *m_requestHeaderTimeoutAttrPtr);
	configuration.keepAliveTimeout = qMax(0, *m_keepAliveTimeoutAttrPtr);
	configuration.isWebSocketEnabled = m_webSocketUpgradeHandlerCompPtr.IsValid();

	m_transportPtr.reset(new CHttpTransport(*this));
	m_transportPtr->SetConfiguration(configuration);

	if (m_sslConfigurationModelCompPtr.IsValid() && m_sslConfigurationManagerCompPtr.IsValid()){
		BaseClass2::RegisterModel(m_sslConfigurationModelCompPtr.GetPtr());
	}

	if (m_startServerOnCreateAttrPtr.IsValid() && *m_startServerOnCreateAttrPtr){
		EnsureServerStarted();
	}

	m_isInitialized = true;
}


void CHttpServerComp::OnComponentDestroyed()
{
	m_isInitialized = false;

	BaseClass2::UnregisterAllModels();

	if (m_transportPtr){
		// Closes all connections and stops the I/O threads:
		m_transportPtr->Shutdown();
	}

	BaseClass::OnComponentDestroyed();
}


// private methods

bool CHttpServerComp::EnsureServerStarted()
{
	if (!m_transportPtr){
		return false;
	}

	// If the server is running, stop it. The established connections are kept:
	m_transportPtr->StopListening();

	QSslConfiguration sslConfiguration;
	bool isSecureConnection = false;

	iprm::TParamsPtr<iprm::IEnableableParam> sslEnableParamPtr(m_sslConfigurationCompPtr.GetPtr(), imtcom::ISslConfigurationManager::ParamKeys::s_enableSslModeParamKey);
	if (sslEnableParamPtr.IsValid() && sslEnableParamPtr->IsEnabled() && m_sslConfigurationManagerCompPtr.IsValid()){
		if (m_sslConfigurationManagerCompPtr->CreateSslConfiguration(*m_sslConfigurationCompPtr, sslConfiguration)){
			isSecureConnection = true;

			SendInfoMessage(0, QStringLiteral("Secure connection (SSL) enabled on HTTP server"));
		}
		else{
			SendErrorMessage(0, QStringLiteral("Could not enable secure connection (SSL) on HTTP server"));
		}
	}

	quint16 port = 0;
	if (m_serverConnnectionInterfaceCompPtr.IsValid()){
		port = quint16(m_serverConnnectionInterfaceCompPtr->GetPort(imtcom::IServerConnectionInterface::PT_HTTP));
	}

	if (!m_transportPtr->Listen(QHostAddress::Any, port, isSecureConnection ? &sslConfiguration : nullptr)){
		SendErrorMessage(0, QStringLiteral("HTTP server could not be started on port %1. Error: %2").arg(QString::number(port), m_transportPtr->GetErrorString()));

		return false;
	}

	SendInfoMessage(
				0,
				QStringLiteral("HTTP server successfully started on port %1 (I/O threads: %2, WebSocket upgrade: %3)")
							.arg(m_transportPtr->GetServerPort())
							.arg(m_transportPtr->GetIoThreadCount())
							.arg(m_webSocketUpgradeHandlerCompPtr.IsValid() ? QStringLiteral("enabled") : QStringLiteral("disabled")));

	return true;
}


void CHttpServerComp::ProcessRequest(const RequestData& requestData)
{
	if (!m_transportPtr){
		return;
	}

	IHttpTransportHandler::ResponseData errorResponse;
	errorResponse.statusCode = 500;

	if (!m_requestHandlerCompPtr.IsValid() || !m_protocolEngineCompPtr.IsValid()){
		m_transportPtr->SendResponse(requestData.requestId, errorResponse);

		return;
	}

	const CHttpRequest::MethodType methodType = GetMethodType(requestData.method);
	if (methodType == CHttpRequest::MT_UNKNOWN){
		IHttpTransportHandler::ResponseData notImplementedResponse;
		notImplementedResponse.statusCode = 501;

		m_transportPtr->SendResponse(requestData.requestId, notImplementedResponse);

		return;
	}

	IRequestUniquePtr requestPtr = m_protocolEngineCompPtr->CreateRequest(*m_requestHandlerCompPtr);
	CHttpRequest* httpRequestPtr = dynamic_cast<CHttpRequest*>(requestPtr.GetPtr());
	if (httpRequestPtr == nullptr){
		SendErrorMessage(0, QStringLiteral("Protocol engine of the HTTP server does not create HTTP requests"));

		m_transportPtr->SendResponse(requestData.requestId, errorResponse);

		return;
	}

	httpRequestPtr->SetRequestId(requestData.requestId);
	httpRequestPtr->SetMethodType(methodType);
	httpRequestPtr->SetUrl(requestData.url);
	httpRequestPtr->SetRemoteAddress(requestData.remoteAddress);
	httpRequestPtr->SetBody(requestData.body);

	for (const QPair<QByteArray, QByteArray>& header : requestData.headers){
		const QByteArray headerId = header.first.toLower();
		const QByteArray existingValue = httpRequestPtr->GetHeaderValue(headerId);
		if (existingValue.isEmpty()){
			httpRequestPtr->SetHeader(headerId, header.second);

			continue;
		}

		// Repeated header fields are combined (RFC 9110, 5.3), cookies are separated by a semicolon (RFC 6265, 5.4):
		const QByteArray separator = (headerId == QByteArrayLiteral("cookie")) ? QByteArrayLiteral("; ") : QByteArrayLiteral(", ");
		httpRequestPtr->SetHeader(headerId, existingValue + separator + header.second);
	}

	httpRequestPtr->SetState(IRequest::RS_MESSAGE_COMPLETE);

	const IRequest& request = *requestPtr.PopPtr();

	ConstResponsePtr responsePtr = m_requestHandlerCompPtr->ProcessRequest(request);
	if (!responsePtr.IsValid()){
		// Asynchronous processing (e.g. CWorkerManagerComp): the handler owns the request and sends the response over IResponseDispatcher
		return;
	}

	// Synchronous processing:
	SendResponse(request.GetRequestId(), responsePtr);

	delete &request;
}


CHttpRequest::MethodType CHttpServerComp::GetMethodType(const QByteArray& method)
{
	if (method == QByteArrayLiteral("GET")){
		return CHttpRequest::MT_GET;
	}

	if (method == QByteArrayLiteral("POST")){
		return CHttpRequest::MT_POST;
	}

	if (method == QByteArrayLiteral("PUT")){
		return CHttpRequest::MT_PUT;
	}

	if (method == QByteArrayLiteral("DELETE")){
		return CHttpRequest::MT_DELETE;
	}

	if (method == QByteArrayLiteral("PATCH")){
		return CHttpRequest::MT_PATCH;
	}

	if (method == QByteArrayLiteral("HEAD")){
		return CHttpRequest::MT_HEAD;
	}

	if (method == QByteArrayLiteral("OPTIONS")){
		return CHttpRequest::MT_OPTIONS;
	}

	return CHttpRequest::MT_UNKNOWN;
}


} // namespace imtrest


#endif // IMTREST_HTTP_SERVER_AVAILABLE


