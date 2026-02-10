// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtclientgql/CRemoteObjectCollectionControllerComp.h>


namespace imtclientgql
{


// protected methods

// reimplemented (sdl::imtbase::DocumentRevision::CGraphQlHandlerCompBase)

sdl::imtbase::ImtCollection::CDuplicateElementsPayload CRemoteObjectCollectionControllerComp::OnDuplicateElements(
			const sdl::imtbase::ImtCollection::CDuplicateElementsGqlRequest& /*duplicateElementsRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::ImtCollection::CDuplicateElementsPayload>(gqlRequest, errorMessage);
}


sdl::imtbase::ImtCollection::CVisualStatus CRemoteObjectCollectionControllerComp::OnGetObjectVisualStatus(
			const sdl::imtbase::ImtCollection::CGetObjectVisualStatusGqlRequest& /*getObjectVisualStatusRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::ImtCollection::CVisualStatus>(gqlRequest, errorMessage);
}


sdl::imtbase::ImtCollection::CRemoveElementsPayload CRemoteObjectCollectionControllerComp::OnRemoveElements(
			const sdl::imtbase::ImtCollection::CRemoveElementsGqlRequest& /*removeElementsRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::ImtCollection::CRemoveElementsPayload>(gqlRequest, errorMessage);
}


sdl::imtbase::ImtCollection::CRemoveElementSetPayload CRemoteObjectCollectionControllerComp::OnRemoveElementSet(
			const sdl::imtbase::ImtCollection::CRemoveElementSetGqlRequest& /*removeElementSetRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::ImtCollection::CRemoveElementSetPayload>(gqlRequest, errorMessage);
}


sdl::imtbase::ImtCollection::CRestoreObjectsPayload CRemoteObjectCollectionControllerComp::OnRestoreObjects(
			const sdl::imtbase::ImtCollection::CRestoreObjectsGqlRequest& /*restoreObjectsRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::ImtCollection::CRestoreObjectsPayload>(gqlRequest, errorMessage);
}


sdl::imtbase::ImtCollection::CRestoreObjectSetPayload CRemoteObjectCollectionControllerComp::OnRestoreObjectSet(
			const sdl::imtbase::ImtCollection::CRestoreObjectSetGqlRequest& /*restoreObjectSetRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::ImtCollection::CRestoreObjectSetPayload>(gqlRequest, errorMessage);
}


sdl::imtbase::ImtCollection::CSetObjectNamePayload CRemoteObjectCollectionControllerComp::OnSetObjectName(
			const sdl::imtbase::ImtCollection::CSetObjectNameGqlRequest& /*setObjectNameRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::ImtCollection::CSetObjectNamePayload>(gqlRequest, errorMessage);
}


sdl::imtbase::ImtCollection::CSetObjectDescriptionPayload CRemoteObjectCollectionControllerComp::OnSetObjectDescription(
			const sdl::imtbase::ImtCollection::CSetObjectDescriptionGqlRequest& /*setObjectDescriptionRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::ImtCollection::CSetObjectDescriptionPayload>(gqlRequest, errorMessage);
}


sdl::imtbase::ImtCollection::CExportObjectPayload CRemoteObjectCollectionControllerComp::OnExportObject(
			const sdl::imtbase::ImtCollection::CExportObjectGqlRequest& /*exportObjectRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::ImtCollection::CExportObjectPayload>(gqlRequest, errorMessage);
}


sdl::imtbase::ImtCollection::CImportObjectPayload CRemoteObjectCollectionControllerComp::OnImportObject(
			const sdl::imtbase::ImtCollection::CImportObjectGqlRequest& /*importObjectRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::ImtCollection::CImportObjectPayload>(gqlRequest, errorMessage);
}


sdl::imtbase::ImtCollection::CGetObjectTypeIdPayload CRemoteObjectCollectionControllerComp::OnGetObjectTypeId(
			const sdl::imtbase::ImtCollection::CGetObjectTypeIdGqlRequest& /*getObjectTypeIdRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::ImtCollection::CGetObjectTypeIdPayload>(gqlRequest, errorMessage);
}


sdl::imtbase::ImtCollection::CGetCollectionHeadersPayload CRemoteObjectCollectionControllerComp::OnGetCollectionHeaders(
			const sdl::imtbase::ImtCollection::CGetCollectionHeadersGqlRequest& /*getCollectionHeadersRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::ImtCollection::CGetCollectionHeadersPayload>(gqlRequest, errorMessage);
}


sdl::imtbase::ImtCollection::CGetElementsCountPayload CRemoteObjectCollectionControllerComp::OnGetElementsCount(
			const sdl::imtbase::ImtCollection::CGetElementsCountGqlRequest& /*getElementsCountRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::ImtCollection::CGetElementsCountPayload>(gqlRequest, errorMessage);
}


sdl::imtbase::ImtCollection::CGetElementIdsPayload CRemoteObjectCollectionControllerComp::OnGetElementIds(
			const sdl::imtbase::ImtCollection::CGetElementIdsGqlRequest& /*getElementIdsRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::ImtCollection::CGetElementIdsPayload>(gqlRequest, errorMessage);
}


sdl::imtbase::ImtCollection::CInsertNewObjectPayload CRemoteObjectCollectionControllerComp::OnInsertNewObject(
			const sdl::imtbase::ImtCollection::CInsertNewObjectGqlRequest& /*insertNewObjectRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::ImtCollection::CInsertNewObjectPayload>(gqlRequest, errorMessage);
}


sdl::imtbase::ImtCollection::CSetObjectDataPayload CRemoteObjectCollectionControllerComp::OnSetObjectData(
			const sdl::imtbase::ImtCollection::CSetObjectDataGqlRequest& /*setObjectDataRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::ImtCollection::CSetObjectDataPayload>(gqlRequest, errorMessage);
}


sdl::imtbase::ImtCollection::CGetObjectDataPayload CRemoteObjectCollectionControllerComp::OnGetObjectData(
			const sdl::imtbase::ImtCollection::CGetObjectDataGqlRequest& /*getObjectDataRequest*/,
			const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::ImtCollection::CGetObjectDataPayload>(gqlRequest, errorMessage);
}


sdl::imtbase::ImtCollection::CGetDataMetaInfoPayload CRemoteObjectCollectionControllerComp::OnGetDataMetaInfo(
			const sdl::imtbase::ImtCollection::CGetDataMetaInfoGqlRequest& /*getDataMetaInfoRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::ImtCollection::CGetDataMetaInfoPayload>(gqlRequest, errorMessage);
}


sdl::imtbase::ImtCollection::CGetElementInfoPayload CRemoteObjectCollectionControllerComp::OnGetElementInfo(
			const sdl::imtbase::ImtCollection::CGetElementInfoGqlRequest& /*getElementInfoRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::ImtCollection::CGetElementInfoPayload>(gqlRequest, errorMessage);
}


sdl::imtbase::ImtCollection::CGetElementMetaInfoPayload CRemoteObjectCollectionControllerComp::OnGetElementMetaInfo(
			const sdl::imtbase::ImtCollection::CGetElementMetaInfoGqlRequest& /*getElementMetaInfoRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::ImtCollection::CGetElementMetaInfoPayload>(gqlRequest, errorMessage);
}


sdl::imtbase::ImtCollection::CCreateSubCollectionPayload CRemoteObjectCollectionControllerComp::OnCreateSubCollection(
			const sdl::imtbase::ImtCollection::CCreateSubCollectionGqlRequest& /*createSubCollectionRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::ImtCollection::CCreateSubCollectionPayload>(gqlRequest, errorMessage);
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


