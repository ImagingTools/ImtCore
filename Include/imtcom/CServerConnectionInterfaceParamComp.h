#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

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
		I_ASSIGN(m_defaultHostAttrPtr, "DefaultHost", "Default host name", true, "localhost");
		I_ASSIGN(m_defaulWebSocketPortAttrPtr, "DefaultWebSocketPort", "Default value for the WebSocket port", true, 9000);
		I_ASSIGN(m_defaulHttpSocketPortAttrPtr, "DefaultHttpPort", "Default value for the HTTP port", true, 9001);
		I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_ATTR(int, m_defaulWebSocketPortAttrPtr);
	I_ATTR(int, m_defaulHttpSocketPortAttrPtr);
	I_ATTR(QString, m_defaultHostAttrPtr);
};


} // namespace imtcom


