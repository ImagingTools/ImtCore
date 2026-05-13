// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtbase/IHierarchicalStructure.h>
#include <imtbase/IDocumentChangeGenerator.h>
#include <imtservergql/CGqlRequestHandlerCompBase.h>
#include <imtbase/IOperationContextController.h>
#include <imtgql/IGqlRequestExtractor.h>


#undef GetObject


namespace imtservergql
{


class CStructureControllerCompBase: public imtservergql::CGqlRequestHandlerCompBase
{
public:
	typedef imtservergql::CGqlRequestHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CStructureControllerCompBase);
		I_ASSIGN(m_collectionStructureCompPtr, "CollectionStructure", "Collection structure", true, "CollectionStructure");
		I_ASSIGN(m_objectCollectionCompPtr, "ObjectCollection", "Object collection", true, "ObjectCollection");
		I_ASSIGN(m_gqlRequestExtractorCompPtr, "GqlRequestExtractor", "GraphQL request extractor", true, "GqlRequestExtractor");
		I_ASSIGN(m_operationContextControllerCompPtr, "OperationContextController", "Operation context controller", false, "OperationContextController");
		I_ASSIGN(m_structureIdAttrPtr, "StructureId", "Structure-ID", true, "");
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

	// reimplemented (imtservergql::CGqlRequestHandlerCompBase)
	virtual QJsonObject CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	virtual bool GetOperationFromRequest(const imtgql::CGqlRequest& gqlRequest, imtgql::CGqlParamObject& gqlObject, QString& errorMessage, int& operationType) const;
	virtual QByteArray GetObjectIdFromInputParams(const QList<imtgql::CGqlParamObject>& inputParams) const;
	virtual QJsonObject InsertNewNode(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual QJsonObject SetNodeName(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual QJsonObject SetNodeDescription(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual QJsonObject SetNodeMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual QJsonObject MoveNode(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual QJsonObject RemoveNode(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual QJsonObject InsertNewObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual QJsonObject MoveObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual QJsonObject RemoveObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual QJsonObject GetNodeCount(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual QJsonObject GetNodeIds(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual QJsonObject GetItemInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual QJsonObject GetObjectParentNodeIds(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual QJsonObject GetNodes(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual QJsonObject GetElements(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;

	/**
		Setup a GraphQL item in the json object based on the information about a node in the collection structure.
	*/
	virtual bool SetupNodeItem(
			const imtgql::CGqlRequest& gqlRequest,
			QJsonObject& itemObj,
			const imtbase::IHierarchicalStructureIterator* collectionStructureIterator,
			QString& errorMessage) const;

	/**
		Setup a GraphQL item in the json object based on the information about an element in the object collection.
	*/
	virtual bool SetupObjectItem(
			const imtgql::CGqlRequest& gqlRequest,
			QJsonObject& itemObj,
			const imtbase::IObjectCollectionIterator* objectCollectionIterator,
			QString& errorMessage) const;

	/**
		Extract information-IDs from the GraphQL object.
	*/
	virtual QByteArrayList GetInformationIds(const imtgql::CGqlRequest& gqlRequest, const QByteArray& objectId) const;

protected:
	I_REF(imtbase::IHierarchicalStructure, m_collectionStructureCompPtr);
	I_REF(imtbase::IObjectCollection, m_objectCollectionCompPtr);
	I_REF(imtgql::IGqlRequestExtractor, m_gqlRequestExtractorCompPtr);
	I_REF(imtbase::IOperationContextController, m_operationContextControllerCompPtr);
	I_ATTR(QByteArray, m_structureIdAttrPtr);
};


} // namespace imtservergql


