// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


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
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/ImtCollection.h>


#undef GetObject


namespace imtservergql
{


class CObjectCollectionControllerAttrCompBase: 
			virtual public imtgql::IGqlRequestExtractor,
			virtual public imtbase::ISearchController,
			public sdl::imtbase::ImtCollection::CGraphQlHandlerCompBase
{
public:
	typedef sdl::imtbase::ImtCollection::CGraphQlHandlerCompBase BaseClass;

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
	virtual const imtbase::ISearchResults* Search(const QString& text) const override;

	// reimplemented (sdl::imtbase::ImtCollection::CGraphQlHandlerCompBase)
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
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
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

	// reimplemented (imtservergql::CGqlRequestHandlerCompBase)
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;

	// reimplemented (imtgql::IGqlRequestExtractor)
	virtual istd::IChangeableUniquePtr ExtractObject(const imtgql::CGqlRequest& gqlRequest, QByteArray& newObjectId, QString& errorMessage) const override;

protected:
	void ReplaceComplexFilterFields(imtbase::IComplexCollectionFilter& filter) const;
	void UpdateFieldsInfoFromHeaders(imtbase::IComplexCollectionFilter& filter) const;

	virtual bool GetOperationFromRequest(const imtgql::CGqlRequest& gqlRequest, imtgql::CGqlParamObject& gqlObject, QString& errorMessage, int& operationType) const;
	virtual QByteArray GetObjectIdFromInputParams(const imtgql::CGqlParamObject &inputParams) const;
	virtual QByteArray GetObjectIdFromRequest(const imtgql::CGqlRequest& gqlRequest) const;
	virtual imtbase::CTreeItemModel* GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual imtbase::CTreeItemModel* InsertObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual imtbase::CTreeItemModel* UpdateObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual imtbase::CTreeItemModel* RenameObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual imtbase::CTreeItemModel* SetObjectDescription(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual imtbase::CTreeItemModel* ListObjects(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual imtbase::CTreeItemModel* GetElementsCount(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual imtbase::CTreeItemModel* DeleteObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	/// \todo rename to GetElementMetaInfo
	virtual imtbase::CTreeItemModel* GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual imtbase::CTreeItemModel* GetInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual imtbase::CTreeItemModel* GetDataMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual imtbase::CTreeItemModel* GetObjectTypeId(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual imtbase::CTreeItemModel* ImportObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual imtbase::CTreeItemModel* ExportObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;

	virtual bool ConvertObject(const istd::IChangeable& source, istd::IChangeable& target) const;
	virtual int GetObjectTypeIdIndex(const QByteArray& typeId) const;
	virtual int GetMimeTypeIndex(const QString& mimeType) const;
	virtual QString GetExtensionFromMimeType(const imtbase::CMimeType& mimeType) const;
	virtual QString GetExportFileName(const QByteArray& objectId) const;
	virtual bool UpdateObjectFromRequest(const imtgql::CGqlRequest& gqlRequest, istd::IChangeable& object, QString& errorMessage) const;
	virtual QVariant GetInputArgumentFromRequest(const imtgql::CGqlRequest& gqlRequest, const QString& argumentKey) const;
	virtual imtbase::ICollectionInfo::Ids ExtractObjectIdsForRemoval(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;

	/**
		Setup a GraphQL item at the given position in the model based on the information about an element in the object collection.
	*/
	virtual bool SetupGqlItem(
			const imtgql::CGqlRequest& gqlRequest,
			imtbase::CTreeItemModel& model,
			int itemIndex,
			const QByteArray& collectionId,
			QString& errorMessage) const;

	virtual bool SetupGqlItem(
			const imtgql::CGqlRequest& gqlRequest,
			imtbase::CTreeItemModel& model,
			int itemIndex,
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
				imtbase::CTreeItemModel& dataModel,
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

	virtual istd::IChangeableUniquePtr CreateObject(const QByteArray& typeId) const;
	virtual QString GetObjectNameFromRequest(const imtgql::CGqlRequest& gqlRequest) const;
	virtual bool CreateCollectionFilterFromViewParamsSdl(
				const sdl::imtbase::ImtCollection::CCollectionViewParams::V1_0& viewParams,
				int& offset,
				int& count,
				iprm::CParamsSet& filterParams) const;
	virtual bool CreateCollectionFilterFromSdl(
				sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilter::V1_0& collectionFilter,
				iprm::CParamsSet& filterParams) const;
	virtual bool CreateDocumentFilterFromSdl(
				sdl::imtbase::DocumentCollectionFilter::CDocumentCollectionFilter::V1_0& documentFilter,
				iprm::CParamsSet& filterParams) const;
	virtual QString ConvertMetaInfoToString(int infoType, const QVariant& metaInfoValue) const;
	bool GetParamsSetFromRepresentation(sdl::imtbase::ImtBaseTypes::CParamsSet::V1_0& representation, iprm::IParamsSet& paramsSet) const;
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

	I_MULTIATTR(QByteArray, m_objectTypeIdAttrPtr);
	I_MULTIATTR(QByteArray, m_objectIconPathsAttrPtr);
	I_MULTIFACT(istd::IChangeable, m_objectFactCompPtr);

	I_MULTIATTR(QString, m_mimeTypeAttrPtr);
	I_MULTIFACT(istd::IChangeable, m_importExportObjectFactCompPtr);
	I_MULTIREF(ifile::IFilePersistence, m_filePersistenceCompPtr);
	I_REF(iser::IVersionInfo, m_versionInfoCompPtr);
};


} // namespace imtservergql


