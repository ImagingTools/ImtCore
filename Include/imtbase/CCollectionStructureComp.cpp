#include <imtbase/CCollectionStructureComp.h>


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtNetwork/QNetworkReply>

// ACF includes
#include <idoc/CStandardDocumentMetaInfo.h>
#include <imod/TModelWrap.h>
#include <istd/CChangeNotifier.h>

// ImtCore includes
#include <imtbase/CTempDir.h>
#include <imtcom/CRequestSender.h>
#include <imtgql/CGqlRequest.h>
// #include <imtclientgql/CGqlObjectCollectionInfo.h>
#include <imtgql/IGqlPrimitiveTypeResponse.h>
#include <imtgql/IGqlStructuredCollectionResponse.h>
// #include <imtbase/CFilterCollectionProxy.h>


namespace imtbase
{


// public methods

// reimpolemented (imtbase::ICollectionStructure)

QByteArray CCollectionStructureComp::InsertNewNode(
			const QString& name,
			const QString& description,
			const QByteArray& proposedNodeId,
			const QByteArray& parentNodeId,
			const idoc::IDocumentMetaInfo* metaInfoPtr,
			const imtbase::IOperationContext* operationContextPtr)
{
	Id id;

	if (m_delegateCompPtr.IsValid()){
		istd::IChangeable::ChangeSet changeSet;
		NodeInsertInfo info;
		info.id = id;
		info.parentNodeId = parentNodeId;
		istd::CChangeNotifier notifier(this, &changeSet);
		id = m_delegateCompPtr->InsertNewNode(
					name,
					description,
					proposedNodeId,
					parentNodeId,
					metaInfoPtr,
					operationContextPtr);

		if (id.isEmpty()){
			notifier.Abort();
		}
	}

	return id;
}


bool CCollectionStructureComp::SetNodeName(
			const QByteArray& nodeId,
			const QString& name,
			const imtbase::IOperationContext* operationContextPtr)
{
	bool retVal = false;

	if (m_delegateCompPtr.IsValid()){
		istd::IChangeable::ChangeSet changeSet;
		NodeUpdateInfo info;
		info.id = nodeId;
		istd::CChangeNotifier notifier(this, &changeSet);
		retVal = m_delegateCompPtr->SetNodeName(
					nodeId,
					name,
					operationContextPtr);

		if (!retVal){
			notifier.Abort();
		}
	}

	return retVal;
}


bool CCollectionStructureComp::SetNodeDescription(
			const QByteArray& nodeId,
			const QString& description,
			const imtbase::IOperationContext* operationContextPtr)
{
	bool retVal = false;

	if (m_delegateCompPtr.IsValid()){
		istd::IChangeable::ChangeSet changeSet;
		NodeUpdateInfo info;
		info.id = nodeId;
		istd::CChangeNotifier notifier(this, &changeSet);
		retVal = m_delegateCompPtr->SetNodeDescription(
					nodeId,
					description,
					operationContextPtr);

		if (!retVal){
			notifier.Abort();
		}
	}

	return retVal;
}


bool CCollectionStructureComp::SetNodeMetaInfo(
			const QByteArray& nodeId,
			const idoc::IDocumentMetaInfo& metaInfo,
			const imtbase::IOperationContext* operationContextPtr)
{
	bool retVal = false;

	if (m_delegateCompPtr.IsValid()){
		istd::IChangeable::ChangeSet changeSet;
		NodeUpdateInfo info;
		info.id = nodeId;
		istd::CChangeNotifier notifier(this, &changeSet);
		retVal = m_delegateCompPtr->SetNodeMetaInfo(
					nodeId,
					metaInfo,
					operationContextPtr);

		if (!retVal){
			notifier.Abort();
		}
	}

	return retVal;
}


bool CCollectionStructureComp::MoveNode(
			const QByteArray& nodeId,
			const QByteArray& parentNodeId,
			const imtbase::IOperationContext* operationContextPtr)
{
	bool retVal = false;

	if (m_delegateCompPtr.IsValid()){
		istd::IChangeable::ChangeSet changeSet;
		NodeMoveInfo info;
		info.id = nodeId;
		info.parentNodeId = parentNodeId;
		istd::CChangeNotifier notifier(this, &changeSet);
		retVal = m_delegateCompPtr->MoveNode(
					nodeId,
					parentNodeId,
					operationContextPtr);

		if (!retVal){
			notifier.Abort();
		}
	}

	return retVal;
}


bool CCollectionStructureComp::RemoveNode(
			const QByteArray& nodeId,
			const imtbase::IOperationContext* operationContextPtr)
{
	bool retVal = false;

	if (m_delegateCompPtr.IsValid()){
		istd::IChangeable::ChangeSet changeSet;
		NodeRemoveInfo info;
		info.id = nodeId;
		istd::CChangeNotifier notifier(this, &changeSet);
		retVal = m_delegateCompPtr->RemoveNode(
					nodeId,
					operationContextPtr);

		if (!retVal){
			notifier.Abort();
		}
	}

	return retVal;
}


bool CCollectionStructureComp::AssignObject(
			const QByteArray& objectId,
			const QByteArray& nodeId,
			const imtbase::IOperationContext* operationContextPtr)
{
	bool retVal = false;

	if (m_delegateCompPtr.IsValid()){
		istd::IChangeable::ChangeSet changeSet;
		ObjectInsertInfo info;
		info.id = objectId;
		info.parentNodeId = nodeId;
		istd::CChangeNotifier notifier(this, &changeSet);
		retVal =  m_delegateCompPtr->AssignObject(
					objectId,
					nodeId,
					operationContextPtr);

		if (!retVal){
			notifier.Abort();
		}
	}

	return retVal;
}


bool CCollectionStructureComp::MoveObject(
			const QByteArray& objectId,
			const QByteArray& sourceNodeId,
			const QByteArray& targetNodeId,
			const imtbase::IOperationContext* operationContextPtr)
{
	bool retVal = false;

	if (m_delegateCompPtr.IsValid()){
		istd::IChangeable::ChangeSet changeSet;
		ObjectMoveInfo info;
		info.id = objectId;
		info.sourceNodeId = sourceNodeId;
		info.targetNodeId = targetNodeId;
		istd::CChangeNotifier notifier(this, &changeSet);
		retVal = m_delegateCompPtr->MoveObject(
						objectId,
						sourceNodeId,
						targetNodeId,
						operationContextPtr);

		if (!retVal){
			notifier.Abort();
		}
	}

	return retVal;
}


bool CCollectionStructureComp::RemoveObject(
			const QByteArray& objectId,
			const QByteArray& nodeId,
			const imtbase::IOperationContext* operationContextPtr)
{
	bool retVal = false;

	if (m_delegateCompPtr.IsValid()){
		istd::IChangeable::ChangeSet changeSet;
		ObjectRemoveInfo info;
		info.id = objectId;
		info.parentNodeId = nodeId;
		istd::CChangeNotifier notifier(this, &changeSet);
		retVal = m_delegateCompPtr->RemoveObject(
					objectId,
					nodeId,
					operationContextPtr);

		if (!retVal){
			notifier.Abort();
		}
	}

	return retVal;
}


ICollectionStructureController* CCollectionStructureComp::GetHierarchicalStructureController()
{
	return this;
}


// reimpolemented (imtbase::ICollectionStructureInfo)
int CCollectionStructureComp::GetNodeCount(const iprm::IParamsSet* /*selectionParamsPtr*/) const
{
    return 0;
}


imtbase::ICollectionStructureInfo::Ids CCollectionStructureComp::GetNodeIds(
            int offset,
            int count ,
            const iprm::IParamsSet* /*selectionParamsPtr*/) const
{
    return imtbase::ICollectionStructureInfo::Ids();
}


imtbase::ICollectionStructureInfo::NodeInfo CCollectionStructureComp::GetNodeInfo(const QByteArray& nodeId)
{
    return imtbase::ICollectionStructureInfo::NodeInfo();
}


imtbase::ICollectionStructureInfo::Ids CCollectionStructureComp::GetObjectParentNodeIds(const QByteArray& objectId) const
{
    imtbase::ICollectionStructureInfo::Ids retVal;

	//if (m_delegateCompPtr.IsValid()){
	//	imtgql::IGqlStructuredCollectionResponse::NodeInfo info;
	//	if (m_delegateCompPtr->GetNodeInfo(objectId, info)){
	//		for (const imtgql::IGqlStructuredCollectionResponse::NodeInfo::PathItem& item : info.path){
	//			retVal += item.id;
	//		}
	//	}
	//}

	return retVal;
}


ICollectionStructureInfo::NodePath CCollectionStructureComp::GetNodePath(const Id& nodeId) const
{
	NodePath retVal;

	if (m_delegateCompPtr.IsValid()){
		retVal = m_delegateCompPtr->GetNodePath(nodeId);
	}

	return retVal;
}


// reimplemented (imtbase::ICollectionStructureController)
imtbase::ICollectionStructureInfo::Id CCollectionStructureComp::InsertNewObjectIntoCollection(
	IObjectCollection*  objectCollectionPtr,
	const Id& nodeId,
	const QByteArray& typeId,
	const QString& name,
	const QString& description,
	IObjectCollection::DataPtr defaultValuePtr,
	const Id& proposedElementId,
	const idoc::IDocumentMetaInfo* dataMetaInfoPtr,
	const idoc::IDocumentMetaInfo* elementMetaInfoPtr,
	const IOperationContext* operationContextPtr)
{
	if (objectCollectionPtr == nullptr || name.isEmpty()){
			return QByteArray();
	}

	QByteArray newObjectId = objectCollectionPtr->InsertNewObject(
				typeId,
				name,
				description,
				defaultValuePtr,
				proposedElementId,
				dataMetaInfoPtr,
				elementMetaInfoPtr,
				operationContextPtr);

	if (!newObjectId.isEmpty()){
		AssignObject(newObjectId, nodeId);

		return newObjectId;
	}

	return QByteArray();
}



} // namespace imtbase


