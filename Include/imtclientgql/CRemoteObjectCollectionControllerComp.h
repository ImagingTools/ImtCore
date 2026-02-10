// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtclientgql/TClientRequestManagerCompWrap.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/ImtCollection.h>


namespace imtclientgql
{


class CRemoteObjectCollectionControllerComp: public TClientRequestManagerCompWrap<sdl::imtbase::ImtCollection::CGraphQlHandlerCompBase>
{
public:
	typedef TClientRequestManagerCompWrap<sdl::imtbase::ImtCollection::CGraphQlHandlerCompBase> BaseClass;

	I_BEGIN_COMPONENT(CRemoteObjectCollectionControllerComp);
		I_ASSIGN_MULTI_0(m_collectionIdsAttrPtr, "CollectionIds", "Collection-IDs", false);
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtbase::DocumentRevision::CGraphQlHandlerCompBase)
	virtual sdl::imtbase::ImtCollection::CDuplicateElementsPayload OnDuplicateElements(
				const sdl::imtbase::ImtCollection::CDuplicateElementsGqlRequest& duplicateElementsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::ImtCollection::CVisualStatus OnGetObjectVisualStatus(
				const sdl::imtbase::ImtCollection::CGetObjectVisualStatusGqlRequest& getObjectVisualStatusRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::ImtCollection::CRemoveElementsPayload OnRemoveElements(
				const sdl::imtbase::ImtCollection::CRemoveElementsGqlRequest& removeElementsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::ImtCollection::CRemoveElementSetPayload OnRemoveElementSet(
				const sdl::imtbase::ImtCollection::CRemoveElementSetGqlRequest& removeElementSetRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::ImtCollection::CRestoreObjectsPayload OnRestoreObjects(
				const sdl::imtbase::ImtCollection::CRestoreObjectsGqlRequest& restoreObjectsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::ImtCollection::CRestoreObjectSetPayload OnRestoreObjectSet(
				const sdl::imtbase::ImtCollection::CRestoreObjectSetGqlRequest& restoreObjectSetRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::ImtCollection::CSetObjectNamePayload OnSetObjectName(
				const sdl::imtbase::ImtCollection::CSetObjectNameGqlRequest& setObjectNameRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::ImtCollection::CSetObjectDescriptionPayload OnSetObjectDescription(
				const sdl::imtbase::ImtCollection::CSetObjectDescriptionGqlRequest& setObjectDescriptionRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::ImtCollection::CExportObjectPayload OnExportObject(
				const sdl::imtbase::ImtCollection::CExportObjectGqlRequest& exportObjectRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::ImtCollection::CImportObjectPayload OnImportObject(
				const sdl::imtbase::ImtCollection::CImportObjectGqlRequest& importObjectRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::ImtCollection::CGetObjectTypeIdPayload OnGetObjectTypeId(
				const sdl::imtbase::ImtCollection::CGetObjectTypeIdGqlRequest& getObjectTypeIdRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::ImtCollection::CGetCollectionHeadersPayload OnGetCollectionHeaders(
				const sdl::imtbase::ImtCollection::CGetCollectionHeadersGqlRequest& getCollectionHeadersRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::ImtCollection::CGetElementsCountPayload OnGetElementsCount(
				const sdl::imtbase::ImtCollection::CGetElementsCountGqlRequest& getElementsCountRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::ImtCollection::CGetElementIdsPayload OnGetElementIds(
				const sdl::imtbase::ImtCollection::CGetElementIdsGqlRequest& getElementIdsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::ImtCollection::CInsertNewObjectPayload OnInsertNewObject(
				const sdl::imtbase::ImtCollection::CInsertNewObjectGqlRequest& insertNewObjectRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::ImtCollection::CSetObjectDataPayload OnSetObjectData(
				const sdl::imtbase::ImtCollection::CSetObjectDataGqlRequest& setObjectDataRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::ImtCollection::CGetObjectDataPayload OnGetObjectData(
				const sdl::imtbase::ImtCollection::CGetObjectDataGqlRequest& getObjectDataRequest,
				const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual sdl::imtbase::ImtCollection::CGetDataMetaInfoPayload OnGetDataMetaInfo(
				const sdl::imtbase::ImtCollection::CGetDataMetaInfoGqlRequest& getDataMetaInfoRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::ImtCollection::CGetElementInfoPayload OnGetElementInfo(
				const sdl::imtbase::ImtCollection::CGetElementInfoGqlRequest& getElementInfoRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::ImtCollection::CGetElementMetaInfoPayload OnGetElementMetaInfo(
				const sdl::imtbase::ImtCollection::CGetElementMetaInfoGqlRequest& getElementMetaInfoRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::ImtCollection::CCreateSubCollectionPayload OnCreateSubCollection(
				const sdl::imtbase::ImtCollection::CCreateSubCollectionGqlRequest& createSubCollectionRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

	// reimplemented (imtgql::IGqlRequestHandler)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;

protected:
	I_MULTIATTR(QByteArray, m_collectionIdsAttrPtr);
};


} // namespace imtclientgql


