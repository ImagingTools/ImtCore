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

void CGqlHeartbeatCheckerComp::OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet)
{
	// Attaching a model notifies with CF_NO_UNDO only, the first heartbeat is sent after all models are registered
	if (changeSet.ContainsExplicit(istd::IChangeable::CF_NO_UNDO, true)){
		return;
	}

	// New settings invalidate the last result and while the check is disabled there is none.
	// A change of the related connection or enabling the check only asks for a fresh check
	if ((modelId == MI_SERVER_CONNECTION) || !IsCheckEnabled()){
		SetStatus(CS_UNKNOWN, QString());
	}

	SendHeartbeat();
}


// reimplemented (icomp::CComponentBase)

void CGqlHeartbeatCheckerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	m_timer.setSingleShot(true);
	connect(&m_timer, &QTimer::timeout, this, &CGqlHeartbeatCheckerComp::SendHeartbeat);

	if (m_serverConnectionModelCompPtr.IsValid()){
		BaseClass2::RegisterModel(m_serverConnectionModelCompPtr.GetPtr(), MI_SERVER_CONNECTION);
	}
	if (m_relatedConnectionStatusModelCompPtr.IsValid()){
		BaseClass2::RegisterModel(m_relatedConnectionStatusModelCompPtr.GetPtr(), MI_RELATED_CONNECTION_STATUS);
	}
	if (m_checkEnablerModelCompPtr.IsValid()){
		BaseClass2::RegisterModel(m_checkEnablerModelCompPtr.GetPtr(), MI_CHECK_ENABLER);
	}

	SendHeartbeat();
}


void CGqlHeartbeatCheckerComp::OnComponentDestroyed()
{
	BaseClass2::UnregisterAllModels();

	m_timer.stop();

	BaseClass::OnComponentDestroyed();
}


// private methods

bool CGqlHeartbeatCheckerComp::IsCheckEnabled() const
{
	return !m_checkEnablerCompPtr.IsValid() || m_checkEnablerCompPtr->IsEnabled();
}


void CGqlHeartbeatCheckerComp::SendHeartbeat()
{
	m_timer.stop();

	// Also drops the reply of a heartbeat still in flight when the check gets disabled
	const int heartbeatId = ++m_lastHeartbeatId;

	if (!IsCheckEnabled() || !m_asyncClientCompPtr.IsValid()){
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

	ScheduleNextHeartbeat();
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


void CGqlHeartbeatCheckerComp::ScheduleNextHeartbeat()
{
	const double interval = (m_status == CS_CONNECTED) ? *m_checkIntervalAttrPtr : *m_retryIntervalAttrPtr;
	if (interval > 0){
		m_timer.start(qRound(interval * 1000));
	}
}


} // namespace imtclientgql


