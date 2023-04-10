#pragma once


// ACF includes
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtbase/IObjectCollectionIterator.h>
#include <imtgql/CGqlRequestHandlerCompBase.h>


#undef GetObject


namespace imtguigql
{


class CObjectCollectionControllerCompBase:
			public imtgql::CGqlRequestHandlerCompBase
{
public:
	typedef imtgql::CGqlRequestHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CObjectCollectionControllerCompBase);
		I_ASSIGN(m_objectCollectionCompPtr, "ObjectCollection", "Object collection", true, "ObjectCollection");
		I_ASSIGN(m_headersProviderCompPtr, "HeadersProvider", "Headers provider", false, "HeadersProvider");
		I_ASSIGN(m_objectViewProviderCompPtr, "ObjectViewProvider", "Object view provider", false, "ObjectViewProvider");
		I_ASSIGN(m_translationManagerCompPtr, "TranslationManager", "Translation manager", false, "TranslationManager");
		I_ASSIGN(m_separatorObjectIdAttrPtr, "SeparatorObjectId", "Separator of the object ID", false, "");
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
		OT_COMMANDS,
		OT_METAINFO,
		OT_OBJECT_VIEW,
		OT_USER_OPERATION = 1000
	};

	// reimplemented (imtgql::CCGqlRepresentationControllerCompBase)
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	virtual bool GetOperationFromRequest(const imtgql::CGqlRequest& gqlRequest, imtgql::CGqlObject& gqlObject, QString& errorMessage, int& operationType) const;
	virtual QByteArray GetObjectIdFromInputParams(const QList<imtgql::CGqlObject>& inputParams) const;
	virtual imtbase::CTreeItemModel* GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual imtbase::CTreeItemModel* InsertObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual imtbase::CTreeItemModel* UpdateObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual imtbase::CTreeItemModel* UpdateCollection(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual imtbase::CTreeItemModel* RenameObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual imtbase::CTreeItemModel* SetObjectDescription(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual imtbase::CTreeItemModel* ListObjects(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual imtbase::CTreeItemModel* DeleteObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual imtbase::CTreeItemModel* GetHeaders(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual imtbase::CTreeItemModel* GetTreeItemModel(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual imtbase::CTreeItemModel* GetDependencies(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual imtbase::CTreeItemModel* GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual imtbase::CTreeItemModel* GetObjectView(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;

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
		Create object from the GraphQL
	*/
	virtual istd::IChangeable* CreateObject(const QList<imtgql::CGqlObject>& inputParams, QByteArray &objectId, QString &name, QString &description, QString& errorMessage) const;

	/**
		Set multiple optional additional filters to initially get a more refined collection.
	*/
	virtual void SetAdditionalFilters(const imtgql::CGqlObject& viewParamsGql, iprm::CParamsSet* filterParams) const;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

protected:
	I_REF(imtbase::IObjectCollection, m_objectCollectionCompPtr);
	I_REF(imtgql::IGqlRequestHandler, m_headersProviderCompPtr);
	I_REF(imtgql::IGqlRequestHandler, m_objectViewProviderCompPtr);
	I_ATTR(QByteArray, m_separatorObjectIdAttrPtr);
};


} // namespace imtguigql


