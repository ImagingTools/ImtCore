#include <imtcom/CSimpleConnectionCheckerComp.h>


// ImtCore includes
#include <imtgql/CGqlRequest.h>


namespace imtcom
{


// public methods

// reimplemented (IConnectionStatusProvider)

IConnectionStatusProvider::ConnectionStatus CSimpleConnectionCheckerComp::GetConnectionStatus() const
{
	imtgql::CGqlRequest request(imtgql::CGqlRequest::RT_QUERY, *m_commandIdAttrPtr);

	imtbase::CTreeItemModel responseModel;
	bool retVal = SendModelRequest(request, responseModel);
	if (!retVal){
		return IConnectionStatusProvider::ConnectionStatus::CS_DISCONNECTED;
	}

	return IConnectionStatusProvider::ConnectionStatus::CS_CONNECTED;
}


} // namespace imtcom


