// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtbase/IObjectCollectionIterator.h>
#include <imtbase/IOperationContext.h>
#include <imtbase/IDocumentChangeGenerator.h>
#include <imtbase/IOperationContextController.h>
#include <imtservergql/CPermissibleGqlRequestHandlerComp.h>
#include <imtserverapp/CTimeFilterParamRepresentationController.h>
#include <imtgql/IGqlRequestExtractor.h>
#include <imtbase/IComplexCollectionFilter.h>


#undef GetObject


namespace imtservergql
{


class CLegacyObjectCollectionControllerCompBase:
			public imtgql::IGqlRequestExtractor,
			public imtservergql::CPermissibleGqlRequestHandlerComp
{
public:
	typedef imtservergql::CPermissibleGqlRequestHandlerComp BaseClass;

	I_BEGIN_BASE_COMPONENT(CLegacyObjectCollectionControllerCompBase);
		I_REGISTER_INTERFACE(imtgql::IGqlRequestExtractor)
		I_ASSIGN_MULTI_0(m_replaceableFieldsAttrPtr, "ReplaceableFilterFields", "List of filter fields to be replaced", false);
		I_ASSIGN_MULTI_0(m_replacementFieldsAttrPtr, "ReplacementFilterFields", "List of filter fields to replace with", false);
		I_ASSIGN(m_objectCollectionCompPtr, "ObjectCollection", "Object collection", true, "ObjectCollection");
		I_ASSIGN(m_documentChangeGeneratorCompPtr, "DocumentChangeGenerator", "Change generator for the collection object", false, "DocumentChangeGenerator");
		I_ASSIGN(m_headersProviderCompPtr, "HeadersProvider", "Headers provider", false, "HeadersProvider");
		I_ASSIGN(m_translationManagerCompPtr, "TranslationManager", "Translation manager", false, "TranslationManager");
		I_ASSIGN(m_operationContextControllerCompPtr, "OperationContextController", "Operation context controller", false, "OperationContextController");
		I_ASSIGN_MULTI_0(m_objectTypeIdAttrPtr, "ObjectTypeIds", "Object type IDs", false);
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
		OT_ELEMENT_HISTORY,
		OT_IMPORT,
		OT_EXPORT,
		OT_USER_OPERATION = 1000
	};

	// reimplemented (imtservergql::CGqlRequestHandlerCompBase)
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

	// reimplemented (imtgql::IGqlRequestExtractor)
	virtual istd::IChangeableUniquePtr ExtractObject(const imtgql::CGqlRequest& gqlRequest, QByteArray& newObjectId, QString& errorMessage) const override;

protected:
	void ReplaceComplexFilterFields(imtbase::IComplexCollectionFilter& filter) const;

	virtual bool GetOperationFromRequest(const imtgql::CGqlRequest& gqlRequest, imtgql::CGqlParamObject& gqlObject, QString& errorMessage, int& operationType) const;
	virtual QByteArray GetObjectIdFromInputParams(const imtgql::CGqlParamObject &inputParams) const;
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
	virtual imtbase::CTreeItemModel* GetObjectHistory(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual imtbase::CTreeItemModel* ImportObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual imtbase::CTreeItemModel* ExportObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;

	virtual bool ConvertObject(const istd::IChangeable& source, istd::IChangeable& target) const;
	virtual int GetObjectTypeIdIndex(const QByteArray& typeId) const;
	virtual int GetMimeTypeIndex(const QString& mimeType) const;

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
	[[deprecated]] virtual istd::IChangeableUniquePtr CreateObjectFromInputParams(const QList<imtgql::CGqlParamObject>& inputParams, QByteArray &objectId, QString &name, QString &description, QString& errorMessage) const;

	/**
		Create object from the GraphQL
	*/
	virtual istd::IChangeableUniquePtr CreateObjectFromRequest(const imtgql::CGqlRequest& gqlRequest, QByteArray& newObjectId, QString& name, QString& description, QString& errorMessage) const;

	/**
		Prepare filters from the GraphQL
	*/
	virtual void PrepareFilters(const imtgql::CGqlRequest& gqlRequest, const imtgql::CGqlParamObject& viewParamsGql, iprm::CParamsSet& filterParams) const;

	/**
		Set multiple optional additional filters to initially get a more refined collection.
	*/
	virtual void SetAdditionalFilters(const imtgql::CGqlRequest& gqlRequest,const imtgql::CGqlParamObject& viewParamsGql, iprm::CParamsSet* filterParams) const;

	/**
		Set object filter.
	*/
	virtual void SetObjectFilter(const imtgql::CGqlRequest& gqlRequest, const imtbase::CTreeItemModel& objectFilterModel, iprm::CParamsSet& filterParams) const;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

	virtual istd::IChangeableUniquePtr CreateObject(const QByteArray& typeId);

protected:
	QMap<QByteArray, QByteArray> m_fieldReplacementMap;

protected:
	I_MULTIATTR(QByteArray, m_replaceableFieldsAttrPtr);
	I_MULTIATTR(QByteArray, m_replacementFieldsAttrPtr);
	I_REF(imtbase::IDocumentChangeGenerator, m_documentChangeGeneratorCompPtr);
	I_REF(imtbase::IObjectCollection, m_objectCollectionCompPtr);
	I_REF(imtgql::IGqlRequestHandler, m_headersProviderCompPtr);
	I_REF(imtbase::IOperationContextController, m_operationContextControllerCompPtr);

	I_MULTIATTR(QByteArray, m_objectTypeIdAttrPtr);
	I_MULTIFACT(istd::IChangeable, m_objectFactCompPtr);

	I_MULTIATTR(QString, m_mimeTypeAttrPtr);
	I_MULTIFACT(istd::IChangeable, m_importExportObjectFactCompPtr);
	I_MULTIREF(ifile::IFilePersistence, m_filePersistenceCompPtr);

	imtserverapp::CTimeFilterParamRepresentationController m_timeFilterParamRepresentationController;
};


} // namespace imtservergql


