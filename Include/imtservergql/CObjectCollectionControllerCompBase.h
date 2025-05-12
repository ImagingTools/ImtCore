#pragma once


// ACF includes
#include <iprm/CParamsSet.h>

// ImtCore includes
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
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/ImtCollection.h>


#undef GetObject


namespace imtservergql
{


class CObjectCollectionControllerCompBase:
			public imtgql::IGqlRequestExtractor,
			virtual public sdl::imtbase::ImtCollection::CGraphQlHandlerCompBase
{
public:
	typedef sdl::imtbase::ImtCollection::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CObjectCollectionControllerCompBase);
		I_REGISTER_INTERFACE(imtgql::IGqlRequestExtractor)
		I_ASSIGN_MULTI_0(m_replaceableFieldsAttrPtr, "ReplaceableFilterFields", "List of filter fields to be replaced", false);
		I_ASSIGN_MULTI_0(m_replacementFieldsAttrPtr, "ReplacementFilterFields", "List of filter fields to replace with", false);
		I_ASSIGN(m_objectCollectionCompPtr, "ObjectCollection", "Object collection", true, "ObjectCollection");
		I_ASSIGN(m_headersProviderCompPtr, "HeadersProvider", "Headers provider", false, "HeadersProvider");
		I_ASSIGN(m_translationManagerCompPtr, "TranslationManager", "Translation manager", false, "TranslationManager");
		I_ASSIGN(m_operationContextControllerCompPtr, "OperationContextController", "Operation context controller", false, "OperationContextController");
		I_ASSIGN_MULTI_0(m_objectTypeIdAttrPtr, "ObjectTypeIds", "Object type IDs", false);
		I_ASSIGN_MULTI_0(m_objectIconPathsAttrPtr, "ObjectIconPaths", "List of item paths related to object type-IDs", false);
		I_ASSIGN_MULTI_0(m_objectFactCompPtr, "CollectionObjectFactory", "Collection object factories", false);
		I_ASSIGN_MULTI_0(m_mimeTypeAttrPtr, "MimeType", "Mime type for the import/export object", false);
		I_ASSIGN_MULTI_0(m_importExportObjectFactCompPtr, "ImportExportObjectFactory", "Object factory for the import/export object", false);
		I_ASSIGN_MULTI_0(m_filePersistenceCompPtr, "FilePersistence", "File persistence for the import/export object", false);
	I_END_COMPONENT;

	enum OperationType
	{
		OT_UNKNOWN,
		OT_NEW,
		OT_GET,
		OT_UPDATE,
		OT_UPDATE_COLLECTION,
		OT_DELETE,
		OT_RENAME,
		OT_SET_DESCRIPTION,
		OT_LIST,
		OT_HEADERS,
		OT_INFO,
		OT_METAINFO,
		OT_DATAMETAINFO,
		OT_ELEMENTS_COUNT,
		OT_ELEMENT_IDS,
		OT_IMPORT,
		OT_EXPORT,
		OT_OBJECT_TYPE_ID
	};

	virtual QMap<int, QByteArray> GetSupportedCommandIds() const;

	// reimplemented (sdl::imtbase::ImtCollection::CGraphQlHandlerCompBase)
	virtual sdl::imtbase::ImtCollection::CVisualStatus OnGetObjectVisualStatus(
				const sdl::imtbase::ImtCollection::CGetObjectVisualStatusGqlRequest& getObjectVisualStatusRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	
	// reimplemented (imtservergql::CGqlRequestHandlerCompBase)
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;

	// reimplemented (imtgql::IGqlRequestExtractor)
	virtual istd::IChangeable* ExtractObject(const imtgql::CGqlRequest& gqlRequest, QByteArray& newObjectId, QString& errorMessage) const override;


protected:
	void ReplaceComplexFilterFields(imtbase::IComplexCollectionFilter& filter) const;

	virtual bool GetOperationFromRequest(const imtgql::CGqlRequest& gqlRequest, imtgql::CGqlObject& gqlObject, QString& errorMessage, int& operationType) const;
	virtual QByteArray GetObjectIdFromInputParams(const imtgql::CGqlObject &inputParams) const;
	virtual QByteArray GetObjectIdFromRequest(const imtgql::CGqlRequest& gqlRequest) const;
	virtual imtbase::CTreeItemModel* GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual imtbase::CTreeItemModel* InsertObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual imtbase::CTreeItemModel* UpdateObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual imtbase::CTreeItemModel* UpdateCollection(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual imtbase::CTreeItemModel* RenameObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual imtbase::CTreeItemModel* SetObjectDescription(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual imtbase::CTreeItemModel* ListObjects(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual imtbase::CTreeItemModel* GetElementsCount(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual imtbase::CTreeItemModel* GetElementIds(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual imtbase::CTreeItemModel* DeleteObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual imtbase::CTreeItemModel* GetHeaders(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual imtbase::CTreeItemModel* GetTreeItemModel(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual imtbase::CTreeItemModel* GetDependencies(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
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
	QByteArray GetObjectTypeIdFromRequest(const imtgql::CGqlRequest& gqlRequest) const;

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
	[[deprecated]] virtual istd::IChangeable* CreateObjectFromInputParams(const QList<imtgql::CGqlObject>& inputParams, QByteArray &objectId, QString& errorMessage) const;

	/**
		Create object from the GraphQL
	*/
	virtual istd::IChangeable* CreateObjectFromRequest(const imtgql::CGqlRequest& gqlRequest, QByteArray& newObjectId, QString& errorMessage) const;

	/**
		Prepare filters from the GraphQL
	*/
	virtual void PrepareFilters(const imtgql::CGqlRequest& gqlRequest, const imtgql::CGqlObject& viewParamsGql, iprm::CParamsSet& filterParams) const;

	/**
		Set multiple optional additional filters to initially get a more refined collection.
	*/
	virtual void SetAdditionalFilters(const imtgql::CGqlRequest& gqlRequest,const imtgql::CGqlObject& viewParamsGql, iprm::CParamsSet* filterParams) const;
	
	virtual void SetAdditionalFilters(const imtgql::CGqlRequest& gqlRequest, imtbase::CComplexCollectionFilter& complexFilter) const;

	/**
		Set object filter.
	*/
	virtual void SetObjectFilter(const imtgql::CGqlRequest& gqlRequest, const imtbase::CTreeItemModel& objectFilterModel, iprm::CParamsSet& filterParams) const;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual istd::IChangeable* CreateObject(const QByteArray& typeId) const;

private:
	virtual bool DoUpdateObjectFromRequest(const imtgql::CGqlRequest& gqlRequest, istd::IChangeable& object, QByteArray& newObjectId, QString& errorMessage) const;

protected:
	QMap<QByteArray, QByteArray> m_fieldReplacementMap;

protected:
	I_MULTIATTR(QByteArray, m_replaceableFieldsAttrPtr);
	I_MULTIATTR(QByteArray, m_replacementFieldsAttrPtr);
	I_REF(imtbase::IObjectCollection, m_objectCollectionCompPtr);
	I_REF(imtgql::IGqlRequestHandler, m_headersProviderCompPtr);
	I_REF(imtbase::IOperationContextController, m_operationContextControllerCompPtr);

	I_MULTIATTR(QByteArray, m_objectTypeIdAttrPtr);
	I_MULTIATTR(QByteArray, m_objectIconPathsAttrPtr);
	I_MULTIFACT(istd::IChangeable, m_objectFactCompPtr);

	I_MULTIATTR(QString, m_mimeTypeAttrPtr);
	I_MULTIFACT(istd::IChangeable, m_importExportObjectFactCompPtr);
	I_MULTIREF(ifile::IFilePersistence, m_filePersistenceCompPtr);
};


} // namespace imtservergql


