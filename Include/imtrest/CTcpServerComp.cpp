// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CTcpServerComp.h>


// Qt includes
#include <QtCore/QFileInfo>
#include <QtNetwork/QSslConfiguration>

// ACF includes
#include <iprm/TParamsPtr.h>
#include <iprm/IEnableableParam.h>

// ImtCore includes
#include <imtrest/IRequest.h>
#include <imtrest/IResponse.h>
#include <imtrest/ITransport.h>
#include <imtrest/CMultiThreadServer.h>


namespace imtrest
{


// public methods

CTcpServerComp::CTcpServerComp()
	:m_serverPtr(new CMultiThreadServer(this))
{
}


CTcpServerComp::~CTcpServerComp()
{
	// The threads are stopped in OnComponentDestroyed already, unless the component has not been created
	m_serverPtr.Reset();
}


imtrest::IRequestServlet* CTcpServerComp::GetRequestServlet()
{
	if (!m_requestHandlerCompPtr.IsValid()){
		return nullptr;
	}

	return m_requestHandlerCompPtr.GetPtr();
}


imtrest::IProtocolEngine* CTcpServerComp::GetProtocolEngine()
{
	if (!m_protocolEngineCompPtr.IsValid()){
		return nullptr;
	}

	return m_protocolEngineCompPtr.GetPtr();
}


int CTcpServerComp::GetThreadsLimit()
{
	return *m_threadsLimitAttrPtr;
}


CConnectionTimeoutConfig CTcpServerComp::GetConnectionTimeoutConfig() const
{
	CConnectionTimeoutConfig retVal;

	if (m_requestInactivityTimeoutAttrPtr.IsValid()){
		retVal.requestInactivityMs = *m_requestInactivityTimeoutAttrPtr;
	}

	if (m_keepAliveTimeoutAttrPtr.IsValid()){
		retVal.keepAliveMs = *m_keepAliveTimeoutAttrPtr;
	}

	if (m_requestProcessingTimeoutAttrPtr.IsValid()){
		retVal.requestProcessingMs = *m_requestProcessingTimeoutAttrPtr;
	}

	return retVal;
}


qint64 CTcpServerComp::GetWriteHighWatermark() const
{
	// A non-positive value is replaced by the default value of the streamer
	return m_writeHighWatermarkAttrPtr.IsValid() ? qint64(*m_writeHighWatermarkAttrPtr) : qint64(0);
}


// protected methods

void CTcpServerComp::OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	if (!m_isInitialized){
		return;
	}

	// The server is changed in its own thread: the settings can be changed in the thread of a request
	if (modelId == MI_SERVER_CONNECTION){
		QMetaObject::invokeMethod(this, [this](){ MoveToConfiguredPort(); }, Qt::AutoConnection);
	}
	else{
		QMetaObject::invokeMethod(this, [this](){ RestartForSslConfiguration(); }, Qt::AutoConnection);
	}
}


// reimplemented (ibase::TRuntimeStatusHanderCompWrap)

void CTcpServerComp::OnSystemShutdown()
{
	BaseClass2::UnregisterAllModels();
}


// reimplemented (icomp::CComponentBase)

void CTcpServerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	m_serverPtr->SetLogPtr(GetLogPtr());

	// Once: the server object stays the same when it listens on another port
	connect(m_serverPtr.GetPtr(), &CMultiThreadServer::NewThreadConnection, this, &CTcpServerComp::OnNewThreadConnection);

	if (m_sslConfigurationModelCompPtr.IsValid() && m_sslConfigurationManagerCompPtr.IsValid()){
		BaseClass2::RegisterModel(m_sslConfigurationModelCompPtr.GetPtr(), MI_SSL_CONFIGURATION);
	}

	if (m_serverConnectionInterfaceModelCompPtr.IsValid()){
		BaseClass2::RegisterModel(m_serverConnectionInterfaceModelCompPtr.GetPtr(), MI_SERVER_CONNECTION);
	}

	if (m_startServerOnCreateAttrPtr.IsValid() && *m_startServerOnCreateAttrPtr){
		m_isStartRequested = true;

		EnsureServerStarted();
	}

	m_isInitialized = true;
}


void CTcpServerComp::OnComponentDestroyed()
{
	// First: a notification from another thread does not start the server again while the threads are stopped
	BaseClass2::UnregisterAllModels();

	// The threads of the connections use the references and the attributes of the component: they are stopped here, while the
	// component has them, not later in the destructor. A queued change of the settings does nothing then
	m_isStartRequested = false;

	m_serverPtr->close();
	m_serverPtr->ShutdownServer();

	BaseClass::OnComponentDestroyed();
}


bool CTcpServerComp::SendResponse(const QByteArray& requestId, ConstResponsePtr& response) const
{
	return m_serverPtr->SendResponse(requestId, response);
}


bool CTcpServerComp::SendRequest(const QByteArray& requestId, ConstRequestPtr& request) const
{
	return m_serverPtr->SendRequest(requestId, request);
}


// reimplemented (imtrest::IServer)

bool CTcpServerComp::StartServer()
{
	m_isStartRequested = true;

	return EnsureServerStarted();
}

bool CTcpServerComp::StopServer()
{
	m_isStartRequested = false;

	m_serverPtr->close();

	return true;
}

IServer::ServerStatus CTcpServerComp::GetServerStatus() const
{
	if (m_serverPtr->isListening()){
		return SS_LISTENING;
	}

	return SS_NOT_STARTED;
}


// private methods

bool CTcpServerComp::StartListening(const QHostAddress &address, quint16 port)
{
	if (m_serverPtr->listen(address, port)){
		QString message = QStringLiteral("Tcp server successfully started on port %1").arg(port);

		SendInfoMessage(0, message);

		return true;
	}
	else{
		QString errorMessage = QStringLiteral("Tcp server could not be started on port %1. Error: %2").arg(QString::number(port), m_serverPtr->errorString());
		qDebug() << errorMessage;

		SendErrorMessage(0, errorMessage);
	}

	return false;
}


bool CTcpServerComp::EnsureServerStarted()
{
	if (!m_serverPtr.IsValid()){
		return false;
	}

	// If the server is running, stop it:
	if (m_serverPtr->isListening()){
		m_serverPtr->close();
	}

	Q_ASSERT(!m_serverPtr->isListening());
	if (m_serverPtr->isListening()){
		return false;
	}

	if (!ApplySecureConnection()){
		return false;
	}

	if (m_serverConnnectionInterfaceCompPtr.IsValid()){
		int port = m_serverConnnectionInterfaceCompPtr->GetPort(imtcom::IServerConnectionInterface::PT_HTTP);

		m_requestedPort = port;

		return StartListening(QHostAddress::Any, port);
	}
	else{
		return StartListening();
	}

	return false;
}


bool CTcpServerComp::GetSecureConnection(bool& isSecure, QSslConfiguration& sslConfiguration) const
{
	iprm::TParamsPtr<iprm::IEnableableParam> sslEnableParamPtr(m_sslConfigurationCompPtr.GetPtr(), imtcom::ISslConfigurationManager::ParamKeys::s_enableSslModeParamKey);

	isSecure = sslEnableParamPtr.IsValid() && sslEnableParamPtr->IsEnabled();
	if (!isSecure){
		return true;
	}

	return m_sslConfigurationManagerCompPtr.IsValid() && m_sslConfigurationManagerCompPtr->CreateSslConfiguration(*m_sslConfigurationCompPtr, sslConfiguration);
}


bool CTcpServerComp::ApplySecureConnection()
{
	bool isSecure = false;
	QSslConfiguration sslConfiguration;
	if (!GetSecureConnection(isSecure, sslConfiguration)){
		// The server that is asked to be secure must not work without the encryption: the clients would send the data as plain text
		SendErrorMessage(0, QStringLiteral("Could not enable secure connection (SSL) on Tcp server, the server is not started"));

		return false;
	}

	if (isSecure){
		m_serverPtr->EnableSecureConnection();

		m_serverPtr->SetSslConfiguration(sslConfiguration);

		SendInfoMessage(0, QStringLiteral("Secure connection (SSL) enabled on Tcp server"));
	}
	else{
		m_serverPtr->EnableSecureConnection(false);
	}

	return true;
}


void CTcpServerComp::MoveToConfiguredPort()
{
	// Checked when the queued call runs: the server can be stopped meanwhile
	if (!m_isStartRequested || !m_serverConnnectionInterfaceCompPtr.IsValid()){
		return;
	}

	// The notification can be of the host, of another protocol or of the same settings loaded again: only another port moves the server.
	// The port of the system (0) would be another one each time, a protocol without a port has -1
	const int port = m_serverConnnectionInterfaceCompPtr->GetPort(imtcom::IServerConnectionInterface::PT_HTTP);
	if (port <= 0){
		return;
	}

	const bool isListening = m_serverPtr->isListening();
	if (isListening && (m_serverPtr->serverPort() == port)){
		m_requestedPort = port;

		return;
	}

	// The port that could not be listened on is tried again only when it is set again after another one
	if (port == m_requestedPort){
		return;
	}

	m_requestedPort = port;

	const quint16 previousPort = isListening ? m_serverPtr->serverPort() : 0;

	m_serverPtr->close();

	// The server that cannot be secure as it is asked does not listen at all, also not on the previous port
	if (!ApplySecureConnection()){
		return;
	}

	if (StartListening(QHostAddress::Any, quint16(port)) || (previousPort == 0)){
		return;
	}

	if (StartListening(QHostAddress::Any, previousPort)){
		SendErrorMessage(0, QStringLiteral("Tcp server stays on port %1").arg(previousPort));
	}
	else{
		SendErrorMessage(0, QStringLiteral("Tcp server could not return to port %1, it does not listen").arg(previousPort));
	}
}


void CTcpServerComp::RestartForSslConfiguration()
{
	// The server that is stopped (StopServer) stays stopped. Checked when the queued call runs: the server can be stopped meanwhile
	if (!m_isStartRequested){
		return;
	}

	bool isSecure = false;
	QSslConfiguration sslConfiguration;
	if (		m_serverPtr->isListening() &&
				GetSecureConnection(isSecure, sslConfiguration) &&
				(isSecure == m_serverPtr->IsSecureConnection()) &&
				(!isSecure || (sslConfiguration == m_serverPtr->GetSslConfiguration()))){
		return;
	}

	EnsureServerStarted();
}


void CTcpServerComp::OnNewThreadConnection(const IRequest* request, const QByteArray& subCommandId)
{
	if (m_requestHandlerCompPtr.IsValid()){
		m_requestHandlerCompPtr->ProcessRequest(*request, subCommandId);
	}
}


} // namespace imtrest


