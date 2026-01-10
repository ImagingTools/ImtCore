#include <imtcom/CSimpleConnectionCheckerComp.h>


namespace imtcom
{


// public methods
CSimpleConnectionCheckerComp::CSimpleConnectionCheckerComp() : m_status(CS_UNKNOWN)
{
}

// reimplemented (IConnectionStatusProvider)

IConnectionStatusProvider::ConnectionStatus CSimpleConnectionCheckerComp::GetConnectionStatus() const
{
	return IConnectionStatusProvider::ConnectionStatus::CS_DISCONNECTED;
}


} // namespace imtcom


