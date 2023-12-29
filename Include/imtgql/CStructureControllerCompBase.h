#pragma once


// ACF includes
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtbase/ICollectionStructure.h>
#include <imtbase/IObjectCollectionIterator.h>
#include <imtbase/IOperationContext.h>
#include <imtbase/IDocumentChangeGenerator.h>
#include <imtgql/CGqlRequestHandlerCompBase.h>
#include <imtgql/IOperationContextController.h>
#include <imtgql/IGqlRequestExtractor.h>


#undef GetObject


namespace imtgql
{


class CStructureControllerCompBase: public imtgql::CGqlRequestHandlerCompBase
{
public:
	typedef imtgql::CGqlRequestHandlerCompBase BaseClass;

    I_BEGIN_COMPONENT(CStructureControllerCompBase);
		I_ASSIGN(m_collectionStructureCompPtr, "CollectionStructure", "Collection structure", true, "CollectionStructure");
		I_ASSIGN(m_objectCollectionCompPtr, "ObjectCollection", "Object collection", true, "ObjectCollection");
		I_ASSIGN(m_gqlRequestExtractorCompPtr, "GqlRequestExtractor", "GraphQL request extractor", true, "GqlRequestExtractor");
		I_ASSIGN(m_operationContextControllerCompPtr, "OperationContextController", "Operation context controller", false, "OperationContextController");
		I_ASSIGN(m_structureIdAttrPtr, "StructureId", "Ctructure ID", true, "");
	I_END_COMPONENT;

	enum OperationType
	{
		OT_UNKNOWN,
        OT_INSERT_NEW_NODE,
        OT_SET_NODE_NAME,
        OT_SET_NODE_DESCRIPTION,
        OT_SET_NODE_METAINFO,
        OT_MOVE_NODE,
        OT_REMOVE_NODE,
        OT_INSERT_NEW_OBJECT,
        OT_MOVE_OBJECT,
        OT_REMOVE_OBJECT,
        OT_GET_NODE_COUNT,
        OT_GET_NODE_IDS,
        OT_GET_NODE_INFO,
        OT_GET_OBJECT_PARENT_NODE_IDS,
		OT_GET_NODES,
		OT_GET_ELEMENTS,
		OT_USER_OPERATION = 1000
	};

    // reimplemented (imtgql::IGqlRequestHandler)
    virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;

	// reimplemented (imtgql::CGqlRequestHandlerCompBase)
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

    // reimplemented (icomp::CComponentBase)
    virtual void OnComponentCreated() override;

protected:
	virtual bool GetOperationFromRequest(const imtgql::CGqlRequest& gqlRequest, imtgql::CGqlObject& gqlObject, QString& errorMessage, int& operationType) const;
	virtual QByteArray GetObjectIdFromInputParams(const QList<imtgql::CGqlObject>& inputParams) const;
    virtual imtbase::CTreeItemModel* InsertNewNode(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
    virtual imtbase::CTreeItemModel* SetNodeName(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
    virtual imtbase::CTreeItemModel* SetNodeDescription(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
    virtual imtbase::CTreeItemModel* SetNodeMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
    virtual imtbase::CTreeItemModel* MoveNode(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
    virtual imtbase::CTreeItemModel* RemoveNode(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
    virtual imtbase::CTreeItemModel* InsertNewObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
    virtual imtbase::CTreeItemModel* MoveObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
    virtual imtbase::CTreeItemModel* RemoveObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
    virtual imtbase::CTreeItemModel* GetNodeCount(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
    virtual imtbase::CTreeItemModel* GetNodeIds(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
    virtual imtbase::CTreeItemModel* GetNodeInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
    virtual imtbase::CTreeItemModel* GetObjectParentNodeIds(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual imtbase::CTreeItemModel* GetNodes(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual imtbase::CTreeItemModel* GetElements(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;

	/**
		Setup a GraphQL item at the given position in the model based on the information about an node in the collection structure.
	*/
	virtual bool SetupNodeItem(
			const imtgql::CGqlRequest& gqlRequest,
			imtbase::CTreeItemModel& model,
			int itemIndex,
			const imtbase::ICollectionStructureIterator* collectionStructureIterator,
			QString& errorMessage) const;

	/**
		Setup a GraphQL item at the given position in the model based on the information about an element in the object collection.
	*/
	virtual bool SetupObjectItem(
			const imtgql::CGqlRequest& gqlRequest,
			imtbase::CTreeItemModel& model,
			int itemIndex,
			const imtbase::IObjectCollectionIterator* objectCollectionIterator,
			QString& errorMessage) const;

	/**
		Extract information-IDs from the GraphQL object.
	*/
	virtual QByteArrayList GetInformationIds(const imtgql::CGqlRequest& gqlRequest, const QByteArray& objectId) const;


protected:
	I_REF(imtbase::ICollectionStructure, m_collectionStructureCompPtr);
	I_REF(imtbase::IObjectCollection, m_objectCollectionCompPtr);
	I_REF(imtgql::IGqlRequestExtractor, m_gqlRequestExtractorCompPtr);
	I_REF(imtgql::IOperationContextController, m_operationContextControllerCompPtr);
    I_ATTR(QByteArray, m_structureIdAttrPtr);
};


} // namespace imtguigql


