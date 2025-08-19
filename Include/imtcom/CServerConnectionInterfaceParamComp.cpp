#include <imtcom/CServerConnectionInterfaceParamComp.h>


namespace imtcom
{


// protected methods

// reimplemented (imtcom/CServerConnectionInterfaceParam)
int CServerConnectionInterfaceParamComp::GetConnectionFlags() const
{
	if (m_sslEnabledCompPtr.IsValid()){
		if (m_sslEnabledCompPtr->IsEnabled()){
			return CF_SECURE;
		}
		else{
			return CF_DEFAULT;
		}
	}
	return BaseClass2::GetConnectionFlags();
}

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


