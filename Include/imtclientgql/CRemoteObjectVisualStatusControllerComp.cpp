#include <imtclientgql/CRemoteObjectVisualStatusControllerComp.h>


namespace imtclientgql
{


// protected methods

// reimplemented (sdl::imtbase::DocumentRevision::CGraphQlHandlerCompBase)

sdl::imtbase::ImtCollection::CVisualStatus CRemoteObjectVisualStatusControllerComp::OnGetObjectVisualStatus(
			const sdl::imtbase::ImtCollection::CGetObjectVisualStatusGqlRequest& /*getObjectVisualStatusRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtbase::ImtCollection::CVisualStatus::V1_0 response;
	if (!SendModelRequest<sdl::imtbase::ImtCollection::CVisualStatus::V1_0, sdl::imtbase::ImtCollection::CVisualStatus>(gqlRequest, response)){
		errorMessage = QString("Unable to get remote object visual status. Error: Sending request is failed");

		return sdl::imtbase::ImtCollection::CVisualStatus();
	}

	sdl::imtbase::ImtCollection::CVisualStatus retVal;
	retVal.Version_1_0 = std::make_optional(response);

	return retVal;
}


// reimplemented (imtgql::IGqlRequestHandler)

bool CRemoteObjectVisualStatusControllerComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	bool isSupported = BaseClass::IsRequestSupported(gqlRequest);
	if (!isSupported){
		return false;
	}

	sdl::imtbase::ImtCollection::CGetObjectVisualStatusGqlRequest getVisualStatusRequest(gqlRequest, false);
	if (getVisualStatusRequest.IsValid()){
		sdl::imtbase::ImtCollection::GetObjectVisualStatusRequestArguments arguments = getVisualStatusRequest.GetRequestedArguments();
		if (!arguments.input.Version_1_0.has_value()){
			return false;
		}
		
		QByteArray typeId;
		if (arguments.input.Version_1_0->typeId){
			typeId = *getVisualStatusRequest.GetRequestedArguments().input.Version_1_0->typeId;
		}

		return m_typeIdsAttrPtr.FindValue(typeId) >= 0;
	}

	return true;
}


} // namespace imtclientgql


