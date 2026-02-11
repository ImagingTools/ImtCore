// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <iprm/IEnableableParam.h>

// ImtCore includes
#include <imtcom/CServerConnectionInterfaceParam.h>


namespace imtcom
{


class CServerConnectionInterfaceParamComp : public CServerConnectionInterfaceParam, public icomp::CComponentBase
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef imtcom::CServerConnectionInterfaceParam BaseClass2;

	I_BEGIN_COMPONENT(CServerConnectionInterfaceParamComp);
		I_REGISTER_INTERFACE(IServerConnectionInterface);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_ASSIGN(m_defaultHostAttrPtr, "DefaultHost", "Default host name", false, "localhost");
		I_ASSIGN(m_defaulWebSocketPortAttrPtr, "DefaultWebSocketPort", "Default value for the WebSocket port", false, 9000);
		I_ASSIGN(m_defaulHttpSocketPortAttrPtr, "DefaultHttpPort", "Default value for the HTTP port", false, 9001)
		I_ASSIGN(m_defaulGrpcSocketPortAttrPtr, "DefaultgRPCPort", "Default value for the gRPC port", false, 50101);
		I_ASSIGN(m_sslEnabledCompPtr, "ExternalEnableSSL", "External SSL configuration", false, "ExternalEnableSSL");
		I_END_COMPONENT;

protected:
	// reimplemented (imtcom/CServerConnectionInterfaceParam)
	virtual int GetConnectionFlags() const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_ATTR(int, m_defaulWebSocketPortAttrPtr);
	I_ATTR(int, m_defaulHttpSocketPortAttrPtr);
	I_ATTR(int, m_defaulGrpcSocketPortAttrPtr);
	I_ATTR(QString, m_defaultHostAttrPtr);
	I_REF(iprm::IEnableableParam, m_sslEnabledCompPtr);
};


} // namespace imtcom


