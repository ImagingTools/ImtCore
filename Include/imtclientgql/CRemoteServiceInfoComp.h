// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QMutex>
#include <QtCore/QObject>
#include <QtCore/QTimer>

// ACF includes
#include <icomp/CComponentBase.h>
#include <imod/CMultiModelDispatcherBase.h>

// ImtCore includes
#include <imtclientgql/IAsyncGqlClient.h>
#include <imtclientgql/IServerCredentialProvider.h>
#include <imtcom/IConnectionStatusProvider.h>
#include <imtcom/IServiceInfoProvider.h>
#include <imtsdl/TElementList.h>
#include <GeneratedFiles/imtappsdl/SDL/1.0/CPP/Application_fwd.h>


namespace imtclientgql
{


/**
	Information about a remote service the application depends on, e.g. the authorization server of a product.

	Every time the \c ConnectionStatus reports a connection, e.g. the WebSocket to the service, the \c GetApplicationInfo query
	is sent once through the \c AsyncClient and the application name, version, version kind and build time of the answer are kept,
	together with the services the remote service depends on itself, so the information is a tree.
	The status of the service follows the connection: connected after the answer, disconnected while the connection is lost
	or the request fails, unknown before the connection status or the answer is known.
	A lost connection forgets everything but the application name, so a restarted service reports its new version.
	With a \c ServerCredentialProvider the query carries the token of this server: the answer tells the state of the token,
	which is checked again periodically, because the service knows about a revoked token only by the token itself.
	The information may be read from any thread.
*/
class CRemoteServiceInfoComp:
			public QObject,
			public icomp::CComponentBase,
			virtual public imtcom::IServiceInfoProvider,
			private imod::CMultiModelDispatcherBase
{
	Q_OBJECT
public:
	typedef icomp::CComponentBase BaseClass;
	typedef imod::CMultiModelDispatcherBase BaseClass2;

	I_BEGIN_COMPONENT(CRemoteServiceInfoComp);
		I_REGISTER_INTERFACE(imtcom::IServiceInfoProvider);
		I_ASSIGN(m_serviceIdAttrPtr, "ServiceId", "Role of the remote service for the application, e.g. AuthServer", true, "");
		I_ASSIGN(m_asyncClientCompPtr, "AsyncClient", "Asynchronous GraphQL client of the remote service", true, "AsyncClient");
		I_ASSIGN(m_connectionStatusCompPtr, "ConnectionStatus", "Status of the connection to the remote service, the information is requested on each connection", true, "ConnectionStatus");
		I_ASSIGN_TO(m_connectionStatusModelCompPtr, m_connectionStatusCompPtr, true);
		I_ASSIGN(m_serverCredentialCompPtr, "ServerCredentialProvider", "Token of this server at the remote service, its state is reported with the information", false, "ServerCredentialProvider");
	I_END_COMPONENT;

	// reimplemented (imtcom::IServiceInfoProvider)
	virtual ServiceInfo GetServiceInfo() const override;

protected:
	// reimplemented (imod::CMultiModelDispatcherBase)
	virtual void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	void UpdateServiceInfo();
	void SendInfoRequest(const QByteArray& serverToken);
	void OnRequestFinished(int requestId, bool isTokenUsed, const GqlModelResult<sdl::V1_0::imtapp::CApplicationInfo>& result);
	void SetCredentialStatus(CredentialStatus status);
	void SetServiceStatus(imtcom::IConnectionStatusProvider::ConnectionStatus status);
	void SetServiceInfo(const ServiceInfo& serviceInfo);
	static void ReadDependencies(
				const imtsdl::TElementList<sdl::V1_0::imtapp::CServiceInfo>& dependencyList,
				int depth,
				QList<ServiceInfo>& dependencies);

private:
	I_ATTR(QByteArray, m_serviceIdAttrPtr);
	I_REF(IAsyncGqlClient, m_asyncClientCompPtr);
	I_REF(imtcom::IConnectionStatusProvider, m_connectionStatusCompPtr);
	I_REF(imod::IModel, m_connectionStatusModelCompPtr);
	I_REF(IServerCredentialProvider, m_serverCredentialCompPtr);

	mutable QMutex m_serviceInfoMutex;
	ServiceInfo m_serviceInfo;
	QTimer m_credentialCheckTimer;

	// Only the answer to the last request is used, e.g. after a quick reconnection
	int m_lastRequestId = 0;
};


} // namespace imtclientgql


