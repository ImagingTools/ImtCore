#include <imtgql/CGqlObjectCollectionComp.h>


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtNetwork/QNetworkReply>

// ACF includes
#include <idoc/CStandardDocumentMetaInfo.h>

// ImtCore includes
#include <imtbase/CTempDir.h>
#include <imtcom/CRequestSender.h>
#include <imtgql/CGqlRequest.h>
#include <imtgql/CGqlObjectCollectionInfo.h>
#include <imtgql/IGqlPrimitiveTypeResponse.h>
#include <imtgql/IGqlStructuredCollectionResponse.h>


namespace imtgql
{

bool operator==(const IGqlStructuredCollectionResponse::ElementInfo::PathItem& a, const IGqlStructuredCollectionResponse::ElementInfo::PathItem& b)
{
	bool retVal = a.id == b.id && a.name == b.name;

	return retVal;
}


bool operator==(const IGqlStructuredCollectionResponse::ElementInfo& a, const IGqlStructuredCollectionResponse::ElementInfo& b)
{
	bool retVal =
		a.id == b.id &&
		a.name == b.name &&
		a.description == b.description &&
		a.path == b.path &&
		a.typeId == b.typeId &&
		a.version == b.version &&
		a.elementMetaInfoPtr.isNull() == b.elementMetaInfoPtr.isNull() &&
		a.elementMetaInfoPtr.isNull() == b.elementMetaInfoPtr.isNull() &&
		a.isNode == b.isNode;

	if (retVal && !a.elementMetaInfoPtr.isNull()){
		retVal &= a.elementMetaInfoPtr->IsEqual(*b.elementMetaInfoPtr);
	}

	if (retVal && !a.dataMetaInfoPtr.isNull()){
		retVal &= a.dataMetaInfoPtr->IsEqual(*b.dataMetaInfoPtr);
	}

	return retVal;
}


// public methods

CGqlObjectCollectionComp::CGqlObjectCollectionComp()
{
}


// reimplemented (imtbase::IStructuredCollectionFinder)

QByteArrayList CGqlObjectCollectionComp::FindObjectParentNodes(const QByteArray& objectId) const
{
	QByteArrayList retVal;

	if (m_delegateCompPtr.IsValid()){
		imtgql::IGqlStructuredCollectionResponse::ElementInfo info;
		if (m_delegateCompPtr->GetObjectInfo(objectId, info)){
			for (imtgql::IGqlStructuredCollectionResponse::ElementInfo::PathItem& pathItem : info.path){
				int index = info.path.count() - 1;
				Q_ASSERT(index >= 0);

				retVal += info.path[info.path.count() - 1].id;
			}
		}
	}

	return retVal;
}


// reimpolemented (imtbase::IStructuredCollectionInserter)

QByteArray CGqlObjectCollectionComp::InsertNewObject(
			const QByteArray& typeId,
			const QString& name,
			const QString& description,
			IObjectCollection::DataPtr defaultValuePtr,
			const QByteArray& proposedObjectId,
			const QByteArray& parentId,
			const idoc::IDocumentMetaInfo* dataMetaInfoPtr,
			const idoc::IDocumentMetaInfo* elementMetaInfoPtr,
			const imtbase::IOperationContext* operationContextPtr)
{
	if (m_delegateCompPtr.IsValid()){
		QByteArray objectId = m_delegateCompPtr->InsertObject(
					typeId,
					name,
					description,
					*defaultValuePtr,
					proposedObjectId,
					parentId,
					dataMetaInfoPtr,
					elementMetaInfoPtr,
					operationContextPtr);

		if (!objectId.isEmpty()){
			istd::IChangeable::ChangeSet changeSet(istd::IChangeable::CF_ANY);
			imtbase::ICollectionInfo::ElementInsertInfo insertInfo;
			insertInfo.elementId = objectId;
			changeSet.SetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENT_INSERTED, QVariant::fromValue(insertInfo));
			istd::CChangeNotifier notifier(this, &changeSet);
		}

		return objectId;
	}

	return QByteArray();
}


// reimpolemented (imtbase::TIStructuredCollectionInfo)

imtbase::IStructuredObjectCollectionInfo::ElementType CGqlObjectCollectionComp::GetElementType(const QByteArray& elementId) const
{
	if (m_delegateCompPtr.IsValid()){
		return m_delegateCompPtr->GetElementType(elementId);
	}

	return ElementType::ET_UNKNOWN;
}


QByteArrayList CGqlObjectCollectionComp::GetElementBasePath(const QByteArray& elementId) const
{
	QByteArrayList retVal;

	IGqlStructuredCollectionResponse::ElementInfo elementInfo;

	if (GetNodeInfo(elementId, elementInfo)){
		for (const IGqlStructuredCollectionResponse::ElementInfo::PathItem& item : elementInfo.path){
			retVal += item.id;
		}
	}
	else if (GetObjectInfo(elementId, elementInfo)){
		for (const IGqlStructuredCollectionResponse::ElementInfo::PathItem& item : elementInfo.path){
			retVal += item.id;
		}
	}

	return retVal;
}


// reimpolemented (imtbase::TICollectionStructure)

QByteArray CGqlObjectCollectionComp::InsertNewNode(
			const QString& name,
			const QString& description,
			const QByteArray& proposedNodeId,
			const QByteArray& parentNodeId,
			const idoc::IDocumentMetaInfo* metaInfoPtr,
			const imtbase::IOperationContext* operationContextPtr)
{
	if (m_delegateCompPtr.IsValid()){
		return m_delegateCompPtr->InsertNode(
					name,
					description,
					proposedNodeId,
					parentNodeId,
					metaInfoPtr,
					operationContextPtr);
	}

	return QByteArray();
}


bool CGqlObjectCollectionComp::SetNodeName(
			const Id& nodeId,
			const QString& name,
			const imtbase::IOperationContext* operationContextPtr)
{
	if (m_delegateCompPtr.IsValid()){
		return m_delegateCompPtr->SetNodeName(
					nodeId,
					name,
					operationContextPtr);
	}

	return false;
}


bool CGqlObjectCollectionComp::SetNodeDescription(
			const Id& nodeId,
			const QString& description,
			const imtbase::IOperationContext* operationContextPtr)
{
	if (m_delegateCompPtr.IsValid()){
		return m_delegateCompPtr->SetNodeDescription(
					nodeId,
					description,
					operationContextPtr);
	}

	return false;
}


bool CGqlObjectCollectionComp::SetNodeMetaInfo(
			const Id& nodeId,
			const idoc::IDocumentMetaInfo& metaInfo,
			const imtbase::IOperationContext* operationContextPtr)
{
	if (m_delegateCompPtr.IsValid()){
		return m_delegateCompPtr->SetNodeMetaInfo(
					nodeId,
					metaInfo,
					operationContextPtr);
	}

	return false;
}


bool CGqlObjectCollectionComp::MoveNode(
			const Id& nodeId,
			const Id& parentNodeId,
			const imtbase::IOperationContext* operationContextPtr)
{
	if (m_delegateCompPtr.IsValid()){
		return  m_delegateCompPtr->MoveNode(
					nodeId,
					parentNodeId,
					operationContextPtr);
	}

	return false;
}


bool CGqlObjectCollectionComp::RemoveNode(
			const Id& nodeId,
			const imtbase::IOperationContext* operationContextPtr)
{
	if (m_delegateCompPtr.IsValid()){
		m_delegateCompPtr->RemoveNode(
					nodeId,
					operationContextPtr);
	}

	return false;
}


bool CGqlObjectCollectionComp::AddObjectToNode(
			const Id& objectId,
			const Id& nodeId,
			const imtbase::IOperationContext* operationContextPtr)
{
	if (m_delegateCompPtr.IsValid()){
		return m_delegateCompPtr->AddObjectToNode(
					objectId,
					nodeId,
					operationContextPtr);
	}

	return false;
}


bool CGqlObjectCollectionComp::MoveObjectToNode(
			const Id& objectId,
			const Id& parentNodeId,
			const Id& newParentNodeId,
			const imtbase::IOperationContext* operationContextPtr)
{
	if (m_delegateCompPtr.IsValid()){
		return m_delegateCompPtr->MoveObjectToNode(
						objectId,
						parentNodeId,
						newParentNodeId,
						operationContextPtr);
	}

	return false;
}


bool CGqlObjectCollectionComp::RemoveObjectFromNode(
			const Id& objectId,
			const Id& nodeId,
			const imtbase::IOperationContext* operationContextPtr)
{
	if (m_delegateCompPtr.IsValid()){
		return m_delegateCompPtr->RemoveObjectFromNode(
						objectId,
						nodeId,
						operationContextPtr);
	}

	return false;
}


// reimpolemented (imtbase::ICollectionStructureInfo)

QByteArrayList CGqlObjectCollectionComp::GetNodePath(const QByteArray& nodeId) const
{
	QByteArrayList retVal;

	if (m_delegateCompPtr.IsValid()){
		IGqlStructuredCollectionResponse::NodeInfo info;
		if (m_delegateCompPtr->GetNodeInfo(nodeId, info)){
			for (const IGqlStructuredCollectionResponse::NodeInfo::PathItem& item : info.path){
				retVal += item.id;
			}
		}
	}

	return retVal;
}


QSharedPointer<imtbase::IStructuredObjectCollectionInfo> CGqlObjectCollectionComp::GetNodeContent(const QByteArray& nodeId) const
{
	if (m_delegateCompPtr.IsValid()){
		return m_delegateCompPtr->GetNodeContent(nodeId);
	}

	return QSharedPointer<imtbase::IStructuredObjectCollectionInfo>();
}


const imtbase::IStructuredCollectionFinder* CGqlObjectCollectionComp::GetCollectionFinder() const
{
	return this;
}


// reimplemented (IObjectCollection)

const imtbase::IRevisionController* CGqlObjectCollectionComp::GetRevisionController() const
{
	return nullptr;
}


const imtbase::ICollectionDataController* CGqlObjectCollectionComp::GetDataController() const
{
	return nullptr;
}


int CGqlObjectCollectionComp::GetOperationFlags(const Id& elementId) const
{
	if (!elementId.isEmpty()){
		return OF_SUPPORT_DELETE | OF_SUPPORT_EDIT;
	}

	return OF_SUPPORT_INSERT | OF_SUPPORT_DELETE | OF_SUPPORT_EDIT | OF_SUPPORT_PAGINATION;
}


imtbase::ICollectionInfo::Id CGqlObjectCollectionComp::InsertNewObject(
			const QByteArray& typeId,
			const QString& name,
			const QString& description,
			DataPtr defaultValuePtr,
			const QByteArray& proposedObjectId,
			const idoc::IDocumentMetaInfo* dataMetaInfoPtr,
			const idoc::IDocumentMetaInfo* collectionItemMetaInfoPtr,
			const imtbase::IOperationContext* operationContextPtr)
{
	return InsertNewObject(
				typeId,
				name,
				description,
				defaultValuePtr,
				proposedObjectId,
				QByteArray(),
				dataMetaInfoPtr,
				collectionItemMetaInfoPtr,
				operationContextPtr);
}


bool CGqlObjectCollectionComp::RemoveElement(const Id& elementId, const imtbase::IOperationContext* operationContextPtr)
{
	if (m_delegateCompPtr.IsValid()){
		if (m_delegateCompPtr->RemoveObject(elementId, operationContextPtr)){
			return true;
		}
		else{
			return m_delegateCompPtr->RemoveNode(elementId, operationContextPtr);
		}
	}

	return false;
}


const istd::IChangeable* CGqlObjectCollectionComp::GetObjectPtr(const Id& /*objectId*/) const
{
	return nullptr;
}


bool CGqlObjectCollectionComp::GetObjectData(const Id& objectId, DataPtr& dataPtr) const
{
	QByteArray typeId = GetObjectTypeId(objectId);

	DataPtr retVal = GetObject(objectId, typeId);
	if (retVal.IsValid()){
		dataPtr = retVal;

		return true;
	}

	return false;
}


bool CGqlObjectCollectionComp::SetObjectData(
			const Id& objectId,
			const istd::IChangeable& object,
			CompatibilityMode mode,
			const imtbase::IOperationContext* operationContextPtr)
{
	if (m_delegateCompPtr.IsValid()){
		IGqlStructuredCollectionResponse::ObjectInfo info;
		if (GetObjectInfo(objectId, info)){
			if (m_delegateCompPtr->SetObject(
						objectId,
						info.typeId,
						object,
						nullptr,
						nullptr,
						operationContextPtr)){
				istd::IChangeable::ChangeSet changeSet(istd::IChangeable::CF_ANY);
				imtbase::ICollectionInfo::ElementUpdateInfo updateInfo;
				updateInfo.elementId = objectId;
				changeSet.SetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENT_UPDATED, QVariant::fromValue(updateInfo));
				istd::CChangeNotifier notifier(this, &changeSet);
			}
		}
	}

	return false;
}


imtbase::IObjectCollection* CGqlObjectCollectionComp::CreateSubCollection(
			int /*offset*/,
			int /*count*/,
			const iprm::IParamsSet* /*selectionParamsPtr*/) const
{
	return nullptr;
}


imtbase::IObjectCollectionIterator* CGqlObjectCollectionComp::CreateObjectCollectionIterator(
			int /*offset*/,
			int /*count*/,
			const iprm::IParamsSet* /*selectionParamsPtr*/) const
{
	return nullptr;
}


// reimplemented (IObjectCollectionInfo)

const iprm::IOptionsList* CGqlObjectCollectionComp::GetObjectTypesInfo() const
{
	return &m_typesInfo;
}



QByteArray CGqlObjectCollectionComp::GetObjectTypeId(const Id& objectId) const
{
	if (m_delegateCompPtr.IsValid()){
		IGqlStructuredCollectionResponse::ObjectInfo info;
		if (m_delegateCompPtr->GetObjectInfo(objectId, info)){
			return info.typeId;
		}
	}

	return QByteArray();
}


idoc::MetaInfoPtr CGqlObjectCollectionComp::GetDataMetaInfo(const Id& objectId) const
{
	if (m_delegateCompPtr.IsValid()){
		IGqlStructuredCollectionResponse::ObjectInfo info;
		if (m_delegateCompPtr->GetObjectInfo(objectId, info)){
			if (!info.dataMetaInfoPtr.isNull()){
				idoc::MetaInfoPtr metaInfoPtr;
				metaInfoPtr.SetCastedOrRemove(info.dataMetaInfoPtr->CloneMe());

				return metaInfoPtr;
			}
		}
	}

	return idoc::MetaInfoPtr();
}


// reimplemented (ICollectionInfo)

int CGqlObjectCollectionComp::GetElementsCount(const iprm::IParamsSet* selectionParamsPtr) const
{
	if (m_delegateCompPtr.IsValid()){
		return m_delegateCompPtr->GetElementCount(selectionParamsPtr);
	}

	return -1;
}


imtbase::ICollectionInfo::Ids CGqlObjectCollectionComp::GetElementIds(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr) const
{
	imtbase::ICollectionInfo::Ids retVal;

	if (m_delegateCompPtr.IsValid()){
		imtgql::IGqlStructuredCollectionResponse::ElementList list = m_delegateCompPtr->GetElementList(
					offset,
					count,
					selectionParamsPtr);

		for (const imtgql::IGqlStructuredCollectionResponse::ElementInfo& element : list){
			retVal += element.id;
		}
	}

	return retVal;
}


bool CGqlObjectCollectionComp::GetSubsetInfo(ICollectionInfo& /*subsetInfo*/, int /*offset*/, int /*count*/, const iprm::IParamsSet* /*selectionParamsPtr*/) const
{
	return false;
}


QVariant CGqlObjectCollectionComp::GetElementInfo(const Id& elementId, int infoType) const
{
	IGqlStructuredCollectionResponse::ObjectInfo info;
	bool isValid = false;

	if (GetObjectInfo(elementId, info)){
		isValid = true;
	}
	else if (GetNodeInfo(elementId, info)){
		isValid = true;
	}

	if (isValid){
		switch (infoType)
		{
		case EIT_NAME:
			return info.name;

		case EIT_DESCRIPTION:
			return info.description;

		case EIT_ENABLED:
			return true;
		}
	}

	return QVariant();
}


idoc::MetaInfoPtr CGqlObjectCollectionComp::GetElementMetaInfo(const Id& elementId) const
{
	idoc::MetaInfoPtr metaInfoPtr;

	IGqlStructuredCollectionResponse::ObjectInfo info;
	bool isValid = false;

	if (GetNodeInfo(elementId, info)){
		isValid = true;
	}
	else if (GetObjectInfo(elementId, info)){
		isValid = true;
	}

	if (isValid){
		if (!info.elementMetaInfoPtr.isNull()){
			metaInfoPtr.SetCastedOrRemove(info.elementMetaInfoPtr->CloneMe());
		}
	}

	return metaInfoPtr;
}


bool CGqlObjectCollectionComp::SetElementName(const Id& elementId, const QString& name)
{
	if (m_delegateCompPtr.IsValid()){
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr;
		istd::TDelPtr<imtgql::IGqlResponse> responsePtr;

		if (m_delegateCompPtr->SetObjectName(elementId, name)){
			return true;
		}
		else{
			return m_delegateCompPtr->SetNodeName(elementId, name);
		}
	}

	return false;
}


bool CGqlObjectCollectionComp::SetElementDescription(const Id& elementId, const QString& description)
{
	if (m_delegateCompPtr.IsValid()){
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr;
		istd::TDelPtr<imtgql::IGqlResponse> responsePtr;

		if (m_delegateCompPtr->SetObjectDescription(elementId, description)){
			return true;
		}
		else{
			return m_delegateCompPtr->SetNodeDescription(elementId, description);
		}
	}

	return false;
}


bool CGqlObjectCollectionComp::SetElementEnabled(const Id& /*elementId*/, bool /*isEnabled*/)
{
	return false;
}


// reimplemented (gmgaws::ISubscriptionClient)

void CGqlObjectCollectionComp::OnResponseReceived(const QByteArray& /*subscriptionId*/, const QByteArray& /*subscriptionData*/)
{
	istd::IChangeable::ChangeSet changeSet(istd::IChangeable::CF_ANY);
	imtbase::ICollectionInfo::ElementInsertInfo insertInfo;
	changeSet.SetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENT_INSERTED, QVariant::fromValue(insertInfo));
	istd::CChangeNotifier notifier(this, &changeSet);
}


void CGqlObjectCollectionComp::OnSubscriptionStatusChanged(
			const QByteArray& /*subscriptionId*/,
			const SubscriptionStatus& /*status*/,
			const QString& /*message*/)
{
}


// protected methods

imtbase::IObjectCollection::DataPtr CGqlObjectCollectionComp::CreateObjectInstance(const QByteArray& typeId) const
{
	int count = m_typesInfo.GetOptionsCount();
	for (int i = 0; i < count; i++){
		if (m_typeIdsAttrPtr[i] == typeId){
			icomp::IComponent* compPtr = m_objectFactoriesCompPtr.CreateComponent(i);
			return DataPtr(DataPtr::RootObjectPtr(compPtr), [compPtr, this](){
				return m_objectFactoriesCompPtr.ExtractInterface(compPtr);
			});
		}
	}

	return DataPtr();
}


// reimplemented (icomp::CComponentBase)

void CGqlObjectCollectionComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	int count = qMin(m_typeIdsAttrPtr.GetCount(), m_typeNamesAttrPtr.GetCount());
	count = qMin(count, m_objectFactoriesCompPtr.GetCount());

	for (int i = 0; i < count; ++i){
		QByteArray typeId = m_typeIdsAttrPtr[i];
		QString typeName = m_typeNamesAttrPtr[i];

		m_typesInfo.InsertOption(typeName, typeId);
	}

	if (m_subscriptionManagerCompPtr.IsValid()){
		imtgql::CGqlRequest gqlRequest(imtgql::IGqlRequest::RT_SUBSCRIPTION, "onAddMeasurement");
		imtgql::CGqlObject subscriptionField("data");
		subscriptionField.InsertField("id");
		subscriptionField.InsertField("name");
		subscriptionField.InsertField("metadata");
		//	subscriptionField.InsertField("folderId");
		subscriptionField.InsertField("createdOn");
		//	subscriptionField.InsertField("version");
		gqlRequest.AddField(subscriptionField);
		m_addMeasurementSubsriptionId = m_subscriptionManagerCompPtr->RegisterSubscription(gqlRequest, this);
	}
}


void CGqlObjectCollectionComp::OnComponentDestroyed()
{
	if (m_subscriptionManagerCompPtr.IsValid() && !m_addMeasurementSubsriptionId.isEmpty()){
		m_subscriptionManagerCompPtr->UnRegisterSubscription(m_addMeasurementSubsriptionId);
	}

	BaseClass::OnComponentDestroyed();
}


// private methods

imtbase::IObjectCollection::DataPtr CGqlObjectCollectionComp::GetObject(
			const QByteArray& objectId,
			const QByteArray& typeId) const
{
	imtbase::IObjectCollection::DataPtr documentPtr;

	if (m_delegateCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr documentPtr = CreateObjectInstance(typeId);
		if (documentPtr.IsValid()){
			if (m_delegateCompPtr->GetObject(objectId, typeId, documentPtr)){
				return documentPtr;
			}
		}
	}

	return imtbase::IObjectCollection::DataPtr();
}


bool CGqlObjectCollectionComp::GetElementType(const QByteArray& elementId, ElementType& valueOut) const
{
	if (m_delegateCompPtr.IsValid()){
		return m_delegateCompPtr->GetElementType(elementId);
	}

	return false;
}


bool CGqlObjectCollectionComp::GetNodeInfo(const QByteArray& nodeId, IGqlStructuredCollectionResponse::NodeInfo& valueOut) const
{
	if (m_delegateCompPtr.IsValid()){
		return m_delegateCompPtr->GetNodeInfo(nodeId, valueOut);
	}

	return false;
}


bool CGqlObjectCollectionComp::GetObjectInfo(const QByteArray& objectId, IGqlStructuredCollectionResponse::ObjectInfo& valueOut) const
{
	if (m_delegateCompPtr.IsValid()){
		return m_delegateCompPtr->GetObjectInfo(objectId, valueOut);
	}

	return false;
}


} // namespace imtgql


