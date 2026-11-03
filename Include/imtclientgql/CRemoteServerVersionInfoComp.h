// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QMutex>
#include <QtCore/QObject>

// ACF includes
#include <icomp/CComponentBase.h>
#include <imod/CMultiModelDispatcherBase.h>

// ImtCore includes
#include <imtclientgql/IAsyncGqlClient.h>
#include <imtcom/IConnectionStatusProvider.h>
#include <imtcom/IServerVersionInfoProvider.h>
#include <GeneratedFiles/imtappsdl/SDL/1.0/CPP/Application_fwd.h>


namespace imtclientgql
{


/**
	Version of a remote server, e.g. the authorization server of a product.

	Every time the \c ConnectionStatus reports a connection, e.g. the WebSocket to the server, the \c GetApplicationInfo query
	is sent once through the \c AsyncClient and the version, version kind and build time of the answer are kept.
	A lost connection forgets them, so a restarted server reports its new version.
	The version may be read from any thread.
*/
class CRemoteServerVersionInfoComp:
			public QObject,
			public icomp::CComponentBase,
			virtual public imtcom::IServerVersionInfoProvider,
			private imod::CMultiModelDispatcherBase
{
	Q_OBJECT
public:
	typedef icomp::CComponentBase BaseClass;
	typedef imod::CMultiModelDispatcherBase BaseClass2;

	I_BEGIN_COMPONENT(CRemoteServerVersionInfoComp);
		I_REGISTER_INTERFACE(imtcom::IServerVersionInfoProvider);
		I_ASSIGN(m_asyncClientCompPtr, "AsyncClient", "Asynchronous GraphQL client of the remote server", true, "AsyncClient");
		I_ASSIGN(m_connectionStatusCompPtr, "ConnectionStatus", "Status of the connection to the remote server, the version is requested on each connection", true, "ConnectionStatus");
		I_ASSIGN_TO(m_connectionStatusModelCompPtr, m_connectionStatusCompPtr, true);
	I_END_COMPONENT;

	// reimplemented (imtcom::IServerVersionInfoProvider)
	virtual bool GetServerVersionInfo(ServerVersionInfo& versionInfo) const override;

protected:
	// reimplemented (imod::CMultiModelDispatcherBase)
	virtual void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	void UpdateVersionInfo();
	void OnRequestFinished(int requestId, const GqlModelResult<sdl::V1_0::imtapp::CApplicationInfo>& result);
	void SetVersionInfo(const ServerVersionInfo* versionInfoPtr);

private:
	I_REF(IAsyncGqlClient, m_asyncClientCompPtr);
	I_REF(imtcom::IConnectionStatusProvider, m_connectionStatusCompPtr);
	I_REF(imod::IModel, m_connectionStatusModelCompPtr);

	mutable QMutex m_versionInfoMutex;
	ServerVersionInfo m_versionInfo;
	bool m_hasVersionInfo = false;

	// Only the answer to the last request is used, e.g. after a quick reconnection
	int m_lastRequestId = 0;
};


} // namespace imtclientgql


