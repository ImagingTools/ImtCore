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
	if (!SendModelRequest<sdl::imtbase::ImtCollection::CVisualStatus::V1_0, sdl::imtbase::ImtCollection::CVisualStatus>(gqlRequest, response, errorMessage)){
		errorMessage = QString("Unable to get remote object visual status. Error: %1").arg(errorMessage);

		return sdl::imtbase::ImtCollection::CVisualStatus();
	}

	sdl::imtbase::ImtCollection::CVisualStatus retVal;
	retVal.Version_1_0 = std::move(response);

	return retVal;
}


sdl::imtbase::ImtCollection::CRemoveElementSetPayload CRemoteObjectVisualStatusControllerComp::OnRemoveElementSet(
			const sdl::imtbase::ImtCollection::CRemoveElementSetGqlRequest& /*removeElementSetRequest*/,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	return sdl::imtbase::ImtCollection::CRemoveElementSetPayload();
}


sdl::imtbase::ImtCollection::CRestoreObjectsPayload CRemoteObjectVisualStatusControllerComp::OnRestoreObjects(
			const sdl::imtbase::ImtCollection::CRestoreObjectsGqlRequest& /*restoreObjectsRequest*/,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	return sdl::imtbase::ImtCollection::CRestoreObjectsPayload();
}


sdl::imtbase::ImtCollection::CRestoreObjectSetPayload CRemoteObjectVisualStatusControllerComp::OnRestoreObjectSet(
			const sdl::imtbase::ImtCollection::CRestoreObjectSetGqlRequest& /*restoreObjectSetRequest*/,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	return sdl::imtbase::ImtCollection::CRestoreObjectSetPayload();
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

	return false;
}


} // namespace imtclientgql


