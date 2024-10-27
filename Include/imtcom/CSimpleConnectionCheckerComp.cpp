#include <imtcom/CSimpleConnectionCheckerComp.h>


// ImtCore includes
#include <imtgql/CGqlRequest.h>


namespace imtcom
{


// public methods

// reimplemented (IConnectionStatusProvider)

IConnectionStatusProvider::ConnectionStatus CSimpleConnectionCheckerComp::GetConnectionStatus() const
{
	return IConnectionStatusProvider::ConnectionStatus::CS_DISCONNECTED;
}


} // namespace imtcom


