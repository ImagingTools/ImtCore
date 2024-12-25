#include <imtclientgql/CRemoteObjectVisualStatusControllerComp.h>


namespace imtclientgql
{


// protected methods

// reimplemented (sdl::imtbase::DocumentRevision::V1_0::CGraphQlHandlerCompBase)

sdl::imtbase::ImtCollection::CVisualStatus::V1_0 CRemoteObjectVisualStatusControllerComp::OnGetObjectVisualStatus(
			const sdl::imtbase::ImtCollection::V1_0::CGetObjectVisualStatusGqlRequest& /*getObjectVisualStatusRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtbase::ImtCollection::CVisualStatus::V1_0 response;
	if (!SendModelRequest<sdl::imtbase::ImtCollection::CVisualStatus::V1_0, sdl::imtbase::ImtCollection::CVisualStatus>(gqlRequest, response)){
		errorMessage = QString("Unable to get remote object visual status. Error: Sending request is failed");
		return response;
	}

	return response;
}


// reimplemented (imtgql::IGqlRequestHandler)

bool CRemoteObjectVisualStatusControllerComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	bool isSupported = BaseClass::IsRequestSupported(gqlRequest);
	if (!isSupported){
		return false;
	}

	sdl::imtbase::ImtCollection::V1_0::CGetObjectVisualStatusGqlRequest getVisualStatusRequest(gqlRequest, false);
	if (getVisualStatusRequest.IsValid()){
		QByteArray typeId = *getVisualStatusRequest.GetRequestedArguments().input.TypeId;

		return m_typeIdsAttrPtr.FindValue(typeId) >= 0;
	}

	return true;
}


} // namespace imtclientgql


