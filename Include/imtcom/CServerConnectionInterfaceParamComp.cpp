#include <imtcom/CServerConnectionInterfaceParamComp.h>


namespace imtcom
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CServerConnectionInterfaceParamComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	BaseClass2::RegisterProtocol(PT_HTTP);
	BaseClass2::RegisterProtocol(PT_WEBSOCKET);

	BaseClass2::SetPort(PT_HTTP, *m_defaulHttpSocketPortAttrPtr);
	BaseClass2::SetPort(PT_WEBSOCKET, *m_defaulWebSocketPortAttrPtr);
	BaseClass2::SetHost(*m_defaultHostAttrPtr);
}


} // namespace imtcom


