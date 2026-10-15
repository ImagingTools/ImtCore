// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtclientgql/CGqlHeartbeatCheckerComp.h>


// ACF includes
#include <istd/CChangeNotifier.h>

// ImtCore includes
#include <imtgql/CGqlRequest.h>
#include <GeneratedFiles/imtappsdl/SDL/1.0/CPP/Application.h>


namespace imtclientgql
{


// public methods

// reimplemented (imtcom::IConnectionStatusProvider)

imtcom::IConnectionStatusProvider::ConnectionStatus CGqlHeartbeatCheckerComp::GetConnectionStatus() const
{
	return m_status;
}


// protected methods

// reimplemented (imod::CMultiModelDispatcherBase)

void CGqlHeartbeatCheckerComp::OnModelChanged(int /*modelId*/, const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	SetStatus(CS_UNKNOWN, QString());

	SendHeartbeat();
}


// reimplemented (icomp::CComponentBase)

void CGqlHeartbeatCheckerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	m_timer.setSingleShot(true);
	m_timer.setInterval(qRound(*m_checkIntervalAttrPtr * 1000));
	connect(&m_timer, &QTimer::timeout, this, &CGqlHeartbeatCheckerComp::SendHeartbeat);

	// Registering the model sends the first heartbeat
	if (m_serverConnectionModelCompPtr.IsValid()){
		BaseClass2::RegisterModel(m_serverConnectionModelCompPtr.GetPtr());
	}

	if (m_lastHeartbeatId == 0){
		SendHeartbeat();
	}
}


void CGqlHeartbeatCheckerComp::OnComponentDestroyed()
{
	BaseClass2::UnregisterAllModels();

	m_timer.stop();

	BaseClass::OnComponentDestroyed();
}


// private methods

void CGqlHeartbeatCheckerComp::SendHeartbeat()
{
	m_timer.stop();

	const int heartbeatId = ++m_lastHeartbeatId;

	if (!m_asyncClientCompPtr.IsValid()){
		return;
	}

	imtgql::CGqlRequest request;
	sdl::V1_0::imtapp::GetApplicationInfoRequestArguments arguments;
	arguments.input.emplace();
	if (!sdl::V1_0::imtapp::CGetApplicationInfoGqlRequest::SetupGqlRequest(request, arguments)){
		return;
	}

	m_asyncClientCompPtr->SendModelRequest<sdl::V1_0::imtapp::CApplicationInfo>(request).then(this, [this, heartbeatId](const GqlModelResult<sdl::V1_0::imtapp::CApplicationInfo>& result) {
		OnHeartbeatFinished(heartbeatId, result);
	});
}


void CGqlHeartbeatCheckerComp::OnHeartbeatFinished(int heartbeatId, const GqlModelResult<sdl::V1_0::imtapp::CApplicationInfo>& result)
{
	if (heartbeatId != m_lastHeartbeatId){
		return;
	}

	switch (result.errorCategory){
	case IAsyncGqlClient::EC_NONE:
		SetStatus(CS_CONNECTED, result.model.version.HasValue() ? *result.model.version : QString());
		break;
	// The server answered, only the answer is not usable
	case IAsyncGqlClient::EC_SERVER_ERROR:
	case IAsyncGqlClient::EC_INVALID_RESPONSE:
		SetStatus(CS_CONNECTED, QString());
		break;
	case IAsyncGqlClient::EC_NETWORK:
	case IAsyncGqlClient::EC_TIMEOUT:
	case IAsyncGqlClient::EC_INVALID_REQUEST:
	case IAsyncGqlClient::EC_INTERNAL:
		SetStatus(CS_DISCONNECTED, QString());
		break;
	}

	m_timer.start();
}


void CGqlHeartbeatCheckerComp::SetStatus(ConnectionStatus status, const QString& serverVersion)
{
	m_serverVersion.SetText(serverVersion);

	if (m_status != status){
		// Unknown is no transition, so it is announced without the connection change flags
		istd::IChangeable::ChangeSet changeSet;
		switch (status){
		case CS_CONNECTED:
			changeSet = istd::IChangeable::ChangeSet(CF_CONNECTED);
			break;
		case CS_DISCONNECTED:
			changeSet = istd::IChangeable::ChangeSet(CF_DISCONNECTED);
			break;
		case CS_UNKNOWN:
			changeSet = GetAnyChange();
			break;
		}
		istd::CChangeNotifier notifier(this, &changeSet);

		m_status = status;
	}
}


} // namespace imtclientgql


