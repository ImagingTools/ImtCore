// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
	if (m_defaulHttpSocketPortAttrPtr.IsValid()){
		BaseClass2::RegisterProtocol(PT_HTTP);

		BaseClass2::SetPort(PT_HTTP, *m_defaulHttpSocketPortAttrPtr);
	}

	if (m_defaulWebSocketPortAttrPtr.IsValid()){
		BaseClass2::RegisterProtocol(PT_WEBSOCKET);

		BaseClass2::SetPort(PT_WEBSOCKET, *m_defaulWebSocketPortAttrPtr);
	}

	if (m_defaulGrpcSocketPortAttrPtr.IsValid()){
		BaseClass2::RegisterProtocol(PT_GRPC);

		BaseClass2::SetPort(PT_GRPC, *m_defaulGrpcSocketPortAttrPtr);
	}

	QString host = "localhost";
	if (m_defaultHostAttrPtr.IsValid()){
		host = *m_defaultHostAttrPtr;
	}

	BaseClass2::SetHost(host);
}


} // namespace imtcom


