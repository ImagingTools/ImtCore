// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

#include <memory>


// ACF includes
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtbase/ISearchController.h>
#include <imtbase/IObjectCollection.h>
#include <imtbase/IObjectCollectionIterator.h>
#include <imtbase/IOperationContext.h>
#include <imtbase/IDocumentChangeGenerator.h>
#include <imtbase/IOperationContextController.h>
#include <imtbase/CComplexCollectionFilter.h>
#include <imtservergql/CPermissibleGqlRequestHandlerComp.h>
#include <imtgql/IGqlRequestExtractor.h>
#include <imtbase/IComplexCollectionFilter.h>
#include <imtbase/CMimeType.h>
#include <imtcol/ICollectionHeadersProvider.h>
#include <imtserverapp/CParamSetRepresentationController.h>
#include <imtserverapp/CComplexCollectionFilterRepresentationController.h>
#include <imtserverapp/CDocumentFilterRepresentationController.h>
#include <imtserverapp/CDocumentIdFilterRepresentationController.h>
#include <imtcol/CDocumentCollectionFilter.h>
#include <imtcol/CDocumentIdFilter.h>
#include <imtbase/CComplexCollectionFilter.h>
#include <imtauth/IUserActionManager.h>
#include <imtauth/CTenantFilterParam.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/ImtCollection_fwd.h>


#undef GetObject


namespace imtservergql
{


class CObjectCollectionControllerAttrCompBase: 
			virtual public imtgql::IGqlRequestExtractor,
			virtual public imtbase::ISearchController,
			public sdl::V1_0::imtbase::CImtCollectionGqlHandlerCompBase
{
public:
	typedef sdl::V1_0::imtbase::CImtCollectionGqlHandlerCompBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CObjectCollectionControllerAttrCompBase)
		I_REGISTER_INTERFACE(imtgql::IGqlRequestExtractor)
		I_REGISTER_INTERFACE(imtbase::ISearchController)
		I_ASSIGN(m_collectionIdAttrPtr, "CollectionId", "Collection-ID", true, "");
		I_ASSIGN(m_collectionNameAttrPtr, "CollectionName", "Collection Name", true, "");
	I_END_COMPONENT

protected:
	I_ATTR(QByteArray, m_collectionIdAttrPtr);
	I_ATTR(QString, m_collectionNameAttrPtr);
};


class CObjectCollectionControllerCompBase: public CObjectCollectionControllerAttrCompBase
{
public:
	typedef CObjectCollectionControllerAttrCompBase BaseClass;

	I_BEGIN_COMPONENT(CObjectCollectionControllerCompBase);
		I_ASSIGN_MULTI_0(m_replaceableFieldsAttrPtr, "ReplaceableFilterFields", "List of filter fields to be replaced", false);
		I_ASSIGN_MULTI_0(m_replacementFieldsAttrPtr, "ReplacementFilterFields", "List of filter fields to replace with", false);
		I_ASSIGN(m_objectCollectionCompPtr, "ObjectCollection", "Object collection", true, "ObjectCollection");
		I_ASSIGN(m_userActionManagerCompPtr, "UserActionManager", "User action manager", false, "UserActionManager");
		I_ASSIGN(m_headersProviderCompPtr, "HeadersProvider", "Collection headers provider", false, "HeadersProvider");
		I_ASSIGN(m_operationContextControllerCompPtr, "OperationContextController", "Operation context controller", false, "OperationContextController");
		I_ASSIGN(m_tenantFilterEnabledAttrPtr, "TenantFilterEnabled", "Enable tenant filter param injection", false, false);
		I_ASSIGN_MULTI_0(m_objectTypeIdAttrPtr, "ObjectTypeIds", "Object type IDs", false);
		I_ASSIGN_MULTI_0(m_objectIconPathsAttrPtr, "ObjectIconPaths", "List of item paths related to object type-IDs", false);
		I_ASSIGN_MULTI_0(m_objectFactCompPtr, "CollectionObjectFactory", "Collection object factories", false);
		I_ASSIGN_MULTI_0(m_mimeTypeAttrPtr, "MimeType", "Mime type for the import/export object", false);
		I_ASSIGN_MULTI_0(m_importExportObjectFactCompPtr, "ImportExportObjectFactory", "Object factory for the import/export object", false);
		I_ASSIGN_MULTI_0(m_filePersistenceCompPtr, "FilePersistence", "File persistence for the import/export object", false);
		I_ASSIGN(m_versionInfoCompPtr, "VersionInfo", "Version info", false, "VersionInfo");
	I_END_COMPONENT;

	enum OperationType
	{
		OT_UNKNOWN,
		OT_NEW,
		OT_GET,
		OT_UPDATE,
		OT_LIST
	};

	virtual QMap<int, QByteArray> GetSupportedCommandIds() const;

	// reimplemented (ISearchController)
	virtual QByteArray GetControllerId() const override;
	virtual QString GetControllerName() const override;
	virtual int GetMatchCount(const QString& text) const override;
	virtual const imtbase::ISearchResults* Search(const QString& text, int offset, int count) const override;

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
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
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

	// reimplemented (imtservergql::CGqlRequestHandlerCompBase)
	virtual QJsonObject CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;

	// reimplemented (imtgql::IGqlRequestExtractor)
	virtual istd::IChangeableUniquePtr ExtractObject(const imtgql::CGqlRequest& gqlRequest, QByteArray& newObjectId, QString& errorMessage) const override;

protected:
	void ReplaceComplexFilterFields(imtbase::IComplexCollectionFilter& filter) const;
	void UpdateFieldsInfoFromHeaders(imtbase::IComplexCollectionFilter& filter) const;

	virtual bool GetOperationFromRequest(const imtgql::CGqlRequest& gqlRequest, imtgql::CGqlParamObject& gqlObject, QString& errorMessage, int& operationType) const;
	virtual QByteArray GetObjectIdFromInputParams(const imtgql::CGqlParamObject &inputParams) const;
	virtual QByteArray GetObjectIdFromRequest(const imtgql::CGqlRequest& gqlRequest) const;
	virtual QJsonObject GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual QJsonObject InsertObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual QJsonObject UpdateObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual QJsonObject RenameObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual QJsonObject SetObjectDescription(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual QJsonObject ListObjects(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual QJsonObject GetElementsCount(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual QJsonObject DeleteObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	/// \todo rename to GetElementMetaInfo
	virtual QJsonObject GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual QJsonObject GetInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual QJsonObject GetDataMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual QJsonObject GetObjectTypeId(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual QJsonObject ImportObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual QJsonObject ExportObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;

	virtual bool ConvertObject(const istd::IChangeable& source, istd::IChangeable& target, QString& errorMessage) const;
	virtual int GetObjectTypeIdIndex(const QByteArray& typeId) const;
	virtual int GetMimeTypeIndex(const QString& mimeType) const;
	virtual QString GetExtensionFromMimeType(const imtbase::CMimeType& mimeType) const;
	virtual QString GetExportFileName(const QByteArray& objectId) const;
	virtual bool UpdateObjectFromRequest(const imtgql::CGqlRequest& gqlRequest, istd::IChangeable& object, QString& errorMessage) const;
	virtual QVariant GetInputArgumentFromRequest(const imtgql::CGqlRequest& gqlRequest, const QString& argumentKey) const;
	virtual imtbase::ICollectionInfo::Ids ExtractObjectIdsForRemoval(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;

	using GqlItemSetupContext = std::shared_ptr<const void>;
	virtual GqlItemSetupContext CreateGqlItemSetupContext(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual bool SetupGqlItemWithContext(
			const imtgql::CGqlRequest& gqlRequest,
			const GqlItemSetupContext& setupContext,
			QJsonObject& itemObj,
			const imtbase::IObjectCollectionIterator* objectCollectionIterator,
			QString& errorMessage) const;

	/**
		Setup a GraphQL item based on the information about an element in the object collection.
	*/
	virtual bool SetupGqlItem(
			const imtgql::CGqlRequest& gqlRequest,
			QJsonObject& itemObj,
			const QByteArray& collectionId,
			QString& errorMessage) const;

	virtual bool SetupGqlItem(
			const imtgql::CGqlRequest& gqlRequest,
			QJsonObject& itemObj,
			const imtbase::IObjectCollectionIterator* objectCollectionIterator,
			QString& errorMessage) const;

	/**
		Extract information-IDs from the GraphQL object.
	*/
	virtual QByteArrayList GetInformationIds(const imtgql::CGqlRequest& gqlRequest, const QByteArray& objectId) const;

	/**
		Get specific information about the the object in the collection.
	*/
	virtual QVariant GetObjectInformation(const QByteArray& informationId, const QByteArray& objectId) const;

	/**
		Get  object type-Id.
	*/
	virtual QByteArray GetObjectTypeIdFromRequest(const imtgql::CGqlRequest& gqlRequest) const;

	/**
		Create  representation from the data object.
	*/
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const QByteArray& objectTypeId,
				const imtgql::CGqlRequest& gqlRequest,
				QJsonObject& dataObj,
				QString& errorMessage) const;

	/**
		Create object from the GraphQL
	*/
	[[deprecated]] virtual istd::IChangeableUniquePtr CreateObjectFromInputParams(const QList<imtgql::CGqlParamObject>& inputParams, QByteArray &objectId, QString& errorMessage) const;

	/**
		Create object from the GraphQL
	*/
	virtual istd::IChangeableUniquePtr CreateObjectFromRequest(const imtgql::CGqlRequest& gqlRequest, QByteArray& newObjectId, QString& errorMessage) const;

	/**
		Prepare filters from the GraphQL
	*/
	virtual void PrepareFilters(const imtgql::CGqlRequest& gqlRequest, const imtgql::CGqlParamObject& viewParamsGql, iprm::CParamsSet& filterParams) const;

	/**
		Set multiple optional additional filters to initially get a more refined collection.
	*/
	virtual void SetAdditionalFilters(const imtgql::CGqlRequest& gqlRequest,const imtgql::CGqlParamObject& viewParamsGql, iprm::CParamsSet* filterParams) const;
	
	virtual void SetAdditionalFilters(const imtgql::CGqlRequest& gqlRequest, imtbase::CComplexCollectionFilter& complexFilter) const;

	/**
		Create an optional tenant filter param to be injected into filter params.
		Returns nullptr when tenant filtering is disabled.
		The returned pointer is owned by the caller (ParamsSet takes ownership).
	*/
	virtual imtauth::CTenantFilterParam* CreateTenantFilterParam(const imtgql::CGqlRequest& gqlRequest) const;

	virtual istd::IChangeableUniquePtr CreateObject(const QByteArray& typeId) const;
	virtual QString GetObjectNameFromRequest(const imtgql::CGqlRequest& gqlRequest) const;
	virtual bool CreateCollectionFilterFromViewParamsSdl(
				const sdl::V1_0::imtbase::CCollectionViewParams& viewParams,
				int& offset,
				int& count,
				iprm::CParamsSet& filterParams) const;
	virtual bool CreateCollectionFilterFromSdl(
				sdl::V1_0::imtbase::CComplexCollectionFilter& collectionFilter,
				iprm::CParamsSet& filterParams) const;
	virtual bool CreateDocumentFilterFromSdl(
				sdl::V1_0::imtbase::CDocumentCollectionFilter& documentFilter,
				iprm::CParamsSet& filterParams) const;
	virtual QString ConvertMetaInfoToString(int infoType, const QVariant& metaInfoValue) const;

protected:
	// Helper to build the OR-contains filter used by search (shared by count + paged item fetch).
	void BuildSearchComplexFilter(const QString& text, sdl::V1_0::imtbase::CComplexCollectionFilter& outFilter) const;
	bool GetParamsSetFromRepresentation(sdl::V1_0::imtbase::CParamsSet& representation, iprm::IParamsSet& paramsSet) const;
	bool RegisterFilterToSelectionParams(
				iser::ISerializable& filterParam,
				const imtserverapp::IJsonRepresentationController& representationController);

	// By returning False, you can prevent the deletion of elements.
	virtual bool OnBeforeRemoveElements(const QByteArrayList& elementIds, const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual void OnAfterRemoveElements(const QByteArrayList& elementIds, const imtgql::CGqlRequest& gqlRequest) const;

	virtual bool OnBeforeSetObjectName(const QByteArray& objectId, QString& newName, const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual void OnAfterSetObjectName(const QByteArray& objectId, const QString& oldName, const QString& newName, const imtgql::CGqlRequest& gqlRequest) const;

	virtual bool OnBeforeSetObjectDescription(
				const QByteArray& objectId,
				QString& description,
				const imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const;
	virtual void OnAfterSetObjectDescription(
				const QByteArray& objectId,
				const QString& description,
				const imtgql::CGqlRequest& gqlRequest) const;

	/**
		Create an optionally adapted copy of the collection object before it is serialized
		as payload of the \c GetObjectData request.
		The default implementation returns an invalid pointer, meaning the original
		collection object is serialized unchanged.
	*/
	virtual istd::IChangeableUniquePtr CreateAdaptedObjectData(
				const QByteArray& objectId,
				const istd::IChangeable& object,
				const imtgql::CGqlRequest& gqlRequest) const;

	QByteArray ExtractObjectIdFromGetObjectTypeIdGqlRequest(const imtgql::CGqlRequest& gqlRequest) const;
	QByteArray ExtractObjectIdFromGetObjectDataGqlRequest(const imtgql::CGqlRequest& gqlRequest) const;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	virtual bool DoUpdateObjectFromRequest(const imtgql::CGqlRequest& gqlRequest, istd::IChangeable& object, QByteArray& newObjectId, QString& errorMessage) const;
	bool SerializeObject(istd::IChangeable& object, QByteArray& objectData) const;
	bool DeSerializeObject(istd::IChangeable& object, const QByteArray& objectData) const;
	bool CreateUserActionLog(
				const QByteArray& objectId,
				const QByteArray& objectTypeId,
				const QByteArray& actionTypeId,
				const imtgql::CGqlRequest& gqlRequest) const;

protected:
	QMap<QByteArray, QByteArray> m_fieldReplacementMap;

private:
	iprm::CParamsSet m_selectionParams;
	imtcol::CDocumentCollectionFilter m_documentCollectionFilter;
	imtcol::CDocumentIdFilter m_documentIdFilter;
	imtbase::CComplexCollectionFilter m_complexCollectionFilter;
	imtserverapp::CParamSetRepresentationController m_paramSetRepresentationController;
	imtserverapp::CDocumentFilterRepresentationController m_documentFilterRepresentationController;
	imtserverapp::CComplexCollectionFilterRepresentationController m_complexCollectionFilterRepresentationController;
	imtserverapp::CDocumentIdFilterRepresentationController m_documentIdFilterRepresentationController;

protected:
	I_MULTIATTR(QByteArray, m_replaceableFieldsAttrPtr);
	I_MULTIATTR(QByteArray, m_replacementFieldsAttrPtr);
	I_REF(imtbase::IObjectCollection, m_objectCollectionCompPtr);
	I_REF(imtcol::ICollectionHeadersProvider, m_headersProviderCompPtr);
	I_REF(imtbase::IOperationContextController, m_operationContextControllerCompPtr);
	I_REF(imtauth::IUserActionManager, m_userActionManagerCompPtr);
	I_ATTR(bool, m_tenantFilterEnabledAttrPtr);

	I_MULTIATTR(QByteArray, m_objectTypeIdAttrPtr);
	I_MULTIATTR(QByteArray, m_objectIconPathsAttrPtr);
	I_MULTIFACT(istd::IChangeable, m_objectFactCompPtr);

	I_MULTIATTR(QString, m_mimeTypeAttrPtr);
	I_MULTIFACT(istd::IChangeable, m_importExportObjectFactCompPtr);
	I_MULTIREF(ifile::IFilePersistence, m_filePersistenceCompPtr);
	I_REF(iser::IVersionInfo, m_versionInfoCompPtr);
};


} // namespace imtservergql
