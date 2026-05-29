// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtclientgql/CRemoteObjectCollectionControllerComp.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/ImtCollection.h>


namespace imtclientgql
{


// protected methods

// reimplemented (sdl::V1_0::imtbase::CGraphQlHandlerCompBase)

sdl::V1_0::imtbase::CDuplicateElementsPayload CRemoteObjectCollectionControllerComp::OnDuplicateElements(
			const sdl::V1_0::imtbase::CDuplicateElementsGqlRequest& /*duplicateElementsRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtbase::CDuplicateElementsPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtbase::CVisualStatus CRemoteObjectCollectionControllerComp::OnGetObjectVisualStatus(
			const sdl::V1_0::imtbase::CGetObjectVisualStatusGqlRequest& /*getObjectVisualStatusRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtbase::CVisualStatus>(gqlRequest, errorMessage);
}


sdl::V1_0::imtbase::CRemoveElementsPayload CRemoteObjectCollectionControllerComp::OnRemoveElements(
			const sdl::V1_0::imtbase::CRemoveElementsGqlRequest& /*removeElementsRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtbase::CRemoveElementsPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtbase::CRemoveElementSetPayload CRemoteObjectCollectionControllerComp::OnRemoveElementSet(
			const sdl::V1_0::imtbase::CRemoveElementSetGqlRequest& /*removeElementSetRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtbase::CRemoveElementSetPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtbase::CRestoreObjectsPayload CRemoteObjectCollectionControllerComp::OnRestoreObjects(
			const sdl::V1_0::imtbase::CRestoreObjectsGqlRequest& /*restoreObjectsRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtbase::CRestoreObjectsPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtbase::CRestoreObjectSetPayload CRemoteObjectCollectionControllerComp::OnRestoreObjectSet(
			const sdl::V1_0::imtbase::CRestoreObjectSetGqlRequest& /*restoreObjectSetRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtbase::CRestoreObjectSetPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtbase::CSetObjectNamePayload CRemoteObjectCollectionControllerComp::OnSetObjectName(
			const sdl::V1_0::imtbase::CSetObjectNameGqlRequest& /*setObjectNameRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtbase::CSetObjectNamePayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtbase::CSetObjectDescriptionPayload CRemoteObjectCollectionControllerComp::OnSetObjectDescription(
			const sdl::V1_0::imtbase::CSetObjectDescriptionGqlRequest& /*setObjectDescriptionRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtbase::CSetObjectDescriptionPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtbase::CExportObjectPayload CRemoteObjectCollectionControllerComp::OnExportObject(
			const sdl::V1_0::imtbase::CExportObjectGqlRequest& /*exportObjectRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtbase::CExportObjectPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtbase::CImportObjectPayload CRemoteObjectCollectionControllerComp::OnImportObject(
			const sdl::V1_0::imtbase::CImportObjectGqlRequest& /*importObjectRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtbase::CImportObjectPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtbase::CGetObjectTypeIdPayload CRemoteObjectCollectionControllerComp::OnGetObjectTypeId(
			const sdl::V1_0::imtbase::CGetObjectTypeIdGqlRequest& /*getObjectTypeIdRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtbase::CGetObjectTypeIdPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtbase::CGetCollectionHeadersPayload CRemoteObjectCollectionControllerComp::OnGetCollectionHeaders(
			const sdl::V1_0::imtbase::CGetCollectionHeadersGqlRequest& /*getCollectionHeadersRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtbase::CGetCollectionHeadersPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtbase::CGetElementsCountPayload CRemoteObjectCollectionControllerComp::OnGetElementsCount(
			const sdl::V1_0::imtbase::CGetElementsCountGqlRequest& /*getElementsCountRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtbase::CGetElementsCountPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtbase::CGetElementIdsPayload CRemoteObjectCollectionControllerComp::OnGetElementIds(
			const sdl::V1_0::imtbase::CGetElementIdsGqlRequest& /*getElementIdsRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtbase::CGetElementIdsPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtbase::CInsertNewObjectPayload CRemoteObjectCollectionControllerComp::OnInsertNewObject(
			const sdl::V1_0::imtbase::CInsertNewObjectGqlRequest& /*insertNewObjectRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtbase::CInsertNewObjectPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtbase::CSetObjectDataPayload CRemoteObjectCollectionControllerComp::OnSetObjectData(
			const sdl::V1_0::imtbase::CSetObjectDataGqlRequest& /*setObjectDataRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtbase::CSetObjectDataPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtbase::CGetObjectDataPayload CRemoteObjectCollectionControllerComp::OnGetObjectData(
			const sdl::V1_0::imtbase::CGetObjectDataGqlRequest& /*getObjectDataRequest*/,
			const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtbase::CGetObjectDataPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtbase::CGetDataMetaInfoPayload CRemoteObjectCollectionControllerComp::OnGetDataMetaInfo(
			const sdl::V1_0::imtbase::CGetDataMetaInfoGqlRequest& /*getDataMetaInfoRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtbase::CGetDataMetaInfoPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtbase::CGetElementInfoPayload CRemoteObjectCollectionControllerComp::OnGetElementInfo(
			const sdl::V1_0::imtbase::CGetElementInfoGqlRequest& /*getElementInfoRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtbase::CGetElementInfoPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtbase::CGetElementMetaInfoPayload CRemoteObjectCollectionControllerComp::OnGetElementMetaInfo(
			const sdl::V1_0::imtbase::CGetElementMetaInfoGqlRequest& /*getElementMetaInfoRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtbase::CGetElementMetaInfoPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtbase::CCreateSubCollectionPayload CRemoteObjectCollectionControllerComp::OnCreateSubCollection(
			const sdl::V1_0::imtbase::CCreateSubCollectionGqlRequest& /*createSubCollectionRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtbase::CCreateSubCollectionPayload>(gqlRequest, errorMessage);
}


// reimplemented (imtgql::IGqlRequestHandler)

bool CRemoteObjectCollectionControllerComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	bool isSupported = BaseClass::IsRequestSupported(gqlRequest);
	if (isSupported){
		const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
		if (inputParamPtr == nullptr){
			return false;
		}

		QByteArray collectionId = inputParamPtr->GetParamArgumentValue("collectionId").toByteArray();
		if (!collectionId.isEmpty()){
			return m_collectionIdsAttrPtr.FindValue(collectionId) >= 0;
		}
	}

	return false;
}


} // namespace imtclientgql


