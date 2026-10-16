// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtCore/QTimer>

// ACF includes
#include <icomp/CComponentBase.h>
#include <imod/CMultiModelDispatcherBase.h>
#include <imod/TModelWrap.h>
#include <iprm/CTextParam.h>
#include <iprm/IEnableableParam.h>

// ImtCore includes
#include <imtclientgql/IAsyncGqlClient.h>
#include <imtcom/IConnectionStatusProvider.h>
#include <imtcom/IServerConnectionInterface.h>
#include <GeneratedFiles/imtappsdl/SDL/1.0/CPP/Application_fwd.h>


namespace imtclientgql
{


/**
	Periodically sends the \c GetApplicationInfo query through an asynchronous GraphQL client
	and reports whether the server answers.

	While the server is not reachable the check is repeated after \c RetryInterval, while it is reachable after \c CheckInterval.

	Any answer of the server counts as connected, also a GraphQL error. Network errors and timeouts count as disconnected.
	The server version from the answer is provided by the \c ServerVersion sub-element.
	The status is reset and the check is repeated immediately when the server connection settings change.
	A change of the \c RelatedConnectionStatus, e.g. the WebSocket connection to the same server, repeats the check immediately and keeps the last status until the answer.
	While the optional \c CheckEnabler is disabled no heartbeats are sent and the status is unknown, enabling it checks immediately.

	The \c AsyncClient must have a finite timeout, otherwise a server that never answers keeps the last status.
*/
class CGqlHeartbeatCheckerComp:
			public QObject,
			public icomp::CComponentBase,
			virtual public imtcom::IConnectionStatusProvider,
			private imod::CMultiModelDispatcherBase
{
	Q_OBJECT
public:
	typedef icomp::CComponentBase BaseClass;
	typedef imod::CMultiModelDispatcherBase BaseClass2;

	I_BEGIN_COMPONENT(CGqlHeartbeatCheckerComp);
		I_REGISTER_INTERFACE(imtcom::IConnectionStatusProvider);
		I_REGISTER_SUBELEMENT(ServerVersion);
		I_REGISTER_SUBELEMENT_INTERFACE_T(ServerVersion, iprm::ITextParam, ExtractServerVersion);
		I_REGISTER_SUBELEMENT_INTERFACE_T(ServerVersion, istd::IChangeable, ExtractServerVersion);
		I_REGISTER_SUBELEMENT_INTERFACE_T(ServerVersion, imod::IModel, ExtractServerVersion);
		I_ASSIGN(m_asyncClientCompPtr, "AsyncClient", "Asynchronous GraphQL client used to send the heartbeat, it must have a finite timeout", true, "AsyncClient");
		I_ASSIGN(m_serverConnectionCompPtr, "ServerConnectionInterface", "Server connection settings used by the client, the check restarts when they change", false, "ServerConnectionInterface");
		I_ASSIGN_TO(m_serverConnectionModelCompPtr, m_serverConnectionCompPtr, false);
		I_ASSIGN(m_relatedConnectionStatusCompPtr, "RelatedConnectionStatus", "Status of another connection to the same server, e.g. the WebSocket, each change sends a heartbeat at once", false, "RelatedConnectionStatus");
		I_ASSIGN_TO(m_relatedConnectionStatusModelCompPtr, m_relatedConnectionStatusCompPtr, false);
		I_ASSIGN(m_checkEnablerCompPtr, "CheckEnabler", "Turns the heartbeats on and off, e.g. only while the status is shown. Without it they are always on", false, "CheckEnabler");
		I_ASSIGN_TO(m_checkEnablerModelCompPtr, m_checkEnablerCompPtr, false);
		I_ASSIGN(m_retryIntervalAttrPtr, "RetryInterval", "Seconds until the next heartbeat while the server is not reachable", true, 5.0);
		I_ASSIGN(m_checkIntervalAttrPtr, "CheckInterval", "Seconds until the next heartbeat while the server is reachable, 0 sends heartbeats only on settings changes", true, 30.0);
	I_END_COMPONENT;

	// reimplemented (imtcom::IConnectionStatusProvider)
	virtual ConnectionStatus GetConnectionStatus() const override;

protected:
	// reimplemented (imod::CMultiModelDispatcherBase)
	virtual void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	enum ModelId
	{
		MI_SERVER_CONNECTION = 0,
		MI_RELATED_CONNECTION_STATUS,
		MI_CHECK_ENABLER
	};

	bool IsCheckEnabled() const;

	void SendHeartbeat();
	void OnHeartbeatFinished(int heartbeatId, const GqlModelResult<sdl::V1_0::imtapp::CApplicationInfo>& result);
	void SetStatus(ConnectionStatus status, const QString& serverVersion);
	void ScheduleNextHeartbeat();

	template <typename InterfaceType>
	static InterfaceType* ExtractServerVersion(CGqlHeartbeatCheckerComp& component)
	{
		return &component.m_serverVersion;
	}

private:
	I_REF(IAsyncGqlClient, m_asyncClientCompPtr);
	I_REF(imtcom::IServerConnectionInterface, m_serverConnectionCompPtr);
	I_REF(imod::IModel, m_serverConnectionModelCompPtr);
	I_REF(imtcom::IConnectionStatusProvider, m_relatedConnectionStatusCompPtr);
	I_REF(imod::IModel, m_relatedConnectionStatusModelCompPtr);
	I_REF(iprm::IEnableableParam, m_checkEnablerCompPtr);
	I_REF(imod::IModel, m_checkEnablerModelCompPtr);
	I_ATTR(double, m_retryIntervalAttrPtr);
	I_ATTR(double, m_checkIntervalAttrPtr);

	ConnectionStatus m_status = CS_UNKNOWN;
	imod::TModelWrap<iprm::CTextParam> m_serverVersion;
	QTimer m_timer;

	// Only the reply to the last heartbeat sent is used, older replies are ignored
	int m_lastHeartbeatId = 0;
};


} // namespace imtclientgql


