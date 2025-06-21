#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtcom/IServerConnectionInterface.h>
#include <imtclientgql/IClientProtocolEngine.h>


namespace imtclientgql
{


class CGqlClientEngineComp:
		public icomp::CComponentBase,
		virtual public imtclientgql::IClientProtocolEngine
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CGqlClientEngineComp);
		I_REGISTER_INTERFACE(imtclientgql::IClientProtocolEngine);
		I_ASSIGN(m_serverConnectionCompPtr, "ServerConnectionInterface", "Server connnection settings", true, "ServerConnectionInterface");
		I_ASSIGN(m_prefixServerAttrPtr, "ServerPrefix", "Prefix Server", false, "/");
	I_END_COMPONENT;

	CGqlClientEngineComp();

	// reimplemented (imtclientgql::IClientProtocolEngine)
	virtual QNetworkRequest* CreateNetworkRequest(const imtgql::IGqlRequest& request, imtbase::IUrlParam* = nullptr) const override;

private:
	I_ATTR(QByteArray, m_prefixServerAttrPtr);
	I_REF(imtcom::IServerConnectionInterface, m_serverConnectionCompPtr);
};


} // namespace imtclientgql


