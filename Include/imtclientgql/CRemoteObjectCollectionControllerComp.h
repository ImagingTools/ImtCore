// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtclientgql/TClientRequestManagerCompWrap.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/ImtCollection.h>


namespace imtclientgql
{


class CRemoteObjectCollectionControllerComp: public TClientRequestManagerCompWrap<sdl::V1_0::imtbase::CImtCollectionGqlHandlerCompBase>
{
public:
	typedef TClientRequestManagerCompWrap<sdl::V1_0::imtbase::CImtCollectionGqlHandlerCompBase> BaseClass;

	I_BEGIN_COMPONENT(CRemoteObjectCollectionControllerComp);
		I_ASSIGN_MULTI_0(m_collectionIdsAttrPtr, "CollectionIds", "Collection-IDs", false);
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::V1_0::imtbase::CImtCollectionGqlHandlerCompBase)
	virtual sdl::V1_0::imtbase::CDuplicateElementsPayload OnDuplicateElements(
				const sdl::V1_0::imtbase::CDuplicateElementsGqlRequest& duplicateElementsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CVisualStatus OnGetObjectVisualStatus(
				const sdl::V1_0::imtbase::CGetObjectVisualStatusGqlRequest& getObjectVisualStatusRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CRemoveElementsPayload OnRemoveElements(
				const sdl::V1_0::imtbase::CRemoveElementsGqlRequest& removeElementsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CRemoveElementSetPayload OnRemoveElementSet(
				const sdl::V1_0::imtbase::CRemoveElementSetGqlRequest& removeElementSetRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CRestoreObjectsPayload OnRestoreObjects(
				const sdl::V1_0::imtbase::CRestoreObjectsGqlRequest& restoreObjectsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CRestoreObjectSetPayload OnRestoreObjectSet(
				const sdl::V1_0::imtbase::CRestoreObjectSetGqlRequest& restoreObjectSetRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CSetObjectNamePayload OnSetObjectName(
				const sdl::V1_0::imtbase::CSetObjectNameGqlRequest& setObjectNameRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CSetObjectDescriptionPayload OnSetObjectDescription(
				const sdl::V1_0::imtbase::CSetObjectDescriptionGqlRequest& setObjectDescriptionRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CExportObjectPayload OnExportObject(
				const sdl::V1_0::imtbase::CExportObjectGqlRequest& exportObjectRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CImportObjectPayload OnImportObject(
				const sdl::V1_0::imtbase::CImportObjectGqlRequest& importObjectRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CGetObjectTypeIdPayload OnGetObjectTypeId(
				const sdl::V1_0::imtbase::CGetObjectTypeIdGqlRequest& getObjectTypeIdRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CGetCollectionHeadersPayload OnGetCollectionHeaders(
				const sdl::V1_0::imtbase::CGetCollectionHeadersGqlRequest& getCollectionHeadersRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CGetElementsCountPayload OnGetElementsCount(
				const sdl::V1_0::imtbase::CGetElementsCountGqlRequest& getElementsCountRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CGetElementIdsPayload OnGetElementIds(
				const sdl::V1_0::imtbase::CGetElementIdsGqlRequest& getElementIdsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CInsertNewObjectPayload OnInsertNewObject(
				const sdl::V1_0::imtbase::CInsertNewObjectGqlRequest& insertNewObjectRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CSetObjectDataPayload OnSetObjectData(
				const sdl::V1_0::imtbase::CSetObjectDataGqlRequest& setObjectDataRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CGetObjectDataPayload OnGetObjectData(
				const sdl::V1_0::imtbase::CGetObjectDataGqlRequest& getObjectDataRequest,
				const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CGetDataMetaInfoPayload OnGetDataMetaInfo(
				const sdl::V1_0::imtbase::CGetDataMetaInfoGqlRequest& getDataMetaInfoRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CGetElementInfoPayload OnGetElementInfo(
				const sdl::V1_0::imtbase::CGetElementInfoGqlRequest& getElementInfoRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CGetElementMetaInfoPayload OnGetElementMetaInfo(
				const sdl::V1_0::imtbase::CGetElementMetaInfoGqlRequest& getElementMetaInfoRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CCreateSubCollectionPayload OnCreateSubCollection(
				const sdl::V1_0::imtbase::CCreateSubCollectionGqlRequest& createSubCollectionRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

	// reimplemented (imtgql::IGqlRequestHandler)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;

protected:
	I_MULTIATTR(QByteArray, m_collectionIdsAttrPtr);
};


} // namespace imtclientgql


