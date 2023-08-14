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

	if (retVal && !a.elementMetaInfoPtr.isNull()) {
		retVal &= a.elementMetaInfoPtr->IsEqual(*b.elementMetaInfoPtr);
	}

	if (retVal && !a.dataMetaInfoPtr.isNull()) {
		retVal &= a.dataMetaInfoPtr->IsEqual(*b.dataMetaInfoPtr);
	}

	return retVal;
}


// public methods

CGqlObjectCollectionComp::CGqlObjectCollectionComp()
{
}


// reimplemented (imtbase::IStructuredCollectionFinder)

QByteArrayList CGqlObjectCollectionComp::FindObjectParentNodes(const QByteArray& /*objectId*/) const
{
	return QByteArrayList();
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
			const idoc::IDocumentMetaInfo* /*elementMetaInfoPtr*/,
			const imtbase::IOperationContext* operationContextPtr)
{
	QByteArray documentId;

	if (m_clientCompPtr.IsValid()) {
		IGqlObjectCollectionDelegate* delegatePtr = m_delegateCompPtr.GetPtr();
		if (delegatePtr != nullptr) {
			const ifile::IFilePersistence* persistencePtr = GetPersistenceForObjectType(typeId);
			if (persistencePtr != nullptr) {
				if (defaultValuePtr.IsValid()) {
					imtcom::IFileTransfer* fileTransferPtr = delegatePtr->GetFileTransfer();
					if (fileTransferPtr != nullptr) {
						imtbase::CTempDir tempDir;
						QString workingPath = tempDir.Path();
						QString docExt = GetDocumentExtension(typeId);
						QString filePath = workingPath + "/" + name + (docExt.isEmpty() ? "" : QString(".") + docExt);
						if (persistencePtr->SaveToFile(*defaultValuePtr, filePath) == ifile::IFilePersistence::OS_OK) {
							QFile file(filePath);
							if (file.open(QIODevice::ReadOnly)) {
								QByteArray data = file.readAll();

								Q_ASSERT(false);
								istd::TDelPtr<IGqlRequest> uploadUrlRequestPtr(delegatePtr->CreateUploadUrlsRequest({ name }, "parentId"));
								istd::TDelPtr<IGqlPrimitiveTypeResponse> uploadUrlResponsePtr;
								uploadUrlResponsePtr.SetCastedOrRemove(delegatePtr->CreateResponse(*uploadUrlRequestPtr));
								Q_ASSERT(uploadUrlResponsePtr.IsValid());
								if (m_clientCompPtr->SendRequest(*uploadUrlRequestPtr, *uploadUrlResponsePtr)) {
									QVariant variant;
									if (uploadUrlResponsePtr->IsSuccessfull() && uploadUrlResponsePtr->GetValue(variant)){
										Q_ASSERT(variant.type() == QVariant::String);
										QString uploadUrlVal = variant.toString();
										if (!uploadUrlVal.isEmpty()) {
											QNetworkRequest request;
											request.setUrl(QUrl(uploadUrlVal));
											QNetworkReply* replyPtr = imtcom::CRequestSender::DoSyncPut(request, data, 30000);
											if (replyPtr != nullptr) {
												if (!replyPtr->error()) {
													Q_ASSERT(false);
													istd::TDelPtr<IGqlRequest> insertObjectRequestPtr(delegatePtr->CreateInsertObjectRequest(
																typeId,
																name,
																description,
																defaultValuePtr.GetPtr(),
																uploadUrlVal,
																proposedObjectId,
																parentId,
																dataMetaInfoPtr,
																nullptr,
																operationContextPtr));

													istd::TDelPtr<IGqlPrimitiveTypeResponse> insertObjectResponsePtr;
													insertObjectResponsePtr.SetCastedOrRemove(delegatePtr->CreateResponse(*insertObjectRequestPtr));
													Q_ASSERT(insertObjectResponsePtr.IsValid());
													if (m_clientCompPtr->SendRequest(*insertObjectRequestPtr, *insertObjectResponsePtr)){
														if (insertObjectResponsePtr->IsSuccessfull() && insertObjectResponsePtr->GetValue(variant)){
															Q_ASSERT(variant.type() == QVariant::ByteArray);
															documentId = variant.toByteArray();
															if (!documentId.isEmpty()) {
																istd::IChangeable::ChangeSet changeSet(istd::IChangeable::CF_ANY);
																imtbase::ICollectionInfo::ElementInsertInfo insertInfo;
																insertInfo.elementId = documentId;
																changeSet.SetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENT_INSERTED, QVariant::fromValue(insertInfo));
																istd::CChangeNotifier notifier(this, &changeSet);
															}
															else {
																ilog::IMessageContainer::Messages messages = insertObjectResponsePtr->GetMessages();
																if (!messages.isEmpty()) {
																	SendErrorMessage(0, messages.first()->GetInformationDescription(), "Document Cloud Controller");
																}
															}
														}
													}
												}

												replyPtr->deleteLater();
											}
										}
									}
								}
							}
						}
					}
					else {
						Q_ASSERT(false);
						//istd::TDelPtr<imtgql::IGqlRequest> insertDocumentRequestPtr(delegatePtr->CreateInsertObjectRequest(name, "", *defaultValuePtr, dataMetaInfoPtr, nullptr, "", "", "parentId"));

						//IGqlObjectCollectionDelegate::InsertObjectResponsePtr saveDocumentResponsePtr = delegatePtr->CreateInsertObjectResponsePtr();
						//if (m_clientCompPtr->SendRequest(*insertDocumentRequestPtr, *saveDocumentResponsePtr)) {
						//	documentId = saveDocumentResponsePtr->GetObjectId();
						//	if (documentId.isEmpty()) {
						//		SendErrorMessage(0, saveDocumentResponsePtr->GetMessage(0), "Document Cloud Controller");
						//	}
						//}
					}
				}
			}
		}
	}

	return documentId;
}


// reimpolemented (imtbase::TIStructuredCollectionInfo)

imtbase::IStructuredObjectCollectionInfo::ElementType CGqlObjectCollectionComp::GetElementType(const QByteArray& elementId) const
{
	ElementType type;

	if (GetElementType(elementId, type, true)) {
		return type;
	}

	return ET_UNKNOWN;
}


QByteArrayList CGqlObjectCollectionComp::GetElementBasePath(const QByteArray& elementId) const
{
	return QByteArrayList();
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
	QByteArray retVal;

	if (m_clientCompPtr.IsValid() && m_delegateCompPtr.IsValid()) {
		istd::TDelPtr<IGqlRequest> requestPtr = m_delegateCompPtr->CreateInsertNodeRequest(
					name,
					description,
					proposedNodeId,
					parentNodeId,
					metaInfoPtr,
					operationContextPtr);
		istd::TDelPtr<IGqlPrimitiveTypeResponse> responsePtr;
		responsePtr.SetCastedOrRemove(m_delegateCompPtr->CreateResponse(*requestPtr));
		Q_ASSERT(responsePtr.IsValid());
		if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
			QVariant variant;
			if (responsePtr->IsSuccessfull() && responsePtr->GetValue(variant)){
				if (variant.type() == QVariant::ByteArray){
					retVal = variant.toByteArray();
				}
			}
		}
	}

	return retVal;
}


bool CGqlObjectCollectionComp::SetNodeName(
			const Id& nodeId,
			const QString& name,
			const imtbase::IOperationContext* operationContextPtr)
{
	bool retVal = false;

	if (m_clientCompPtr.IsValid() && m_delegateCompPtr.IsValid()) {
		istd::TDelPtr<IGqlRequest> requestPtr = m_delegateCompPtr->CreateSetNodeNameRequest(
					nodeId,
					name,
					operationContextPtr);
		istd::TDelPtr<IGqlPrimitiveTypeResponse> responsePtr;
		responsePtr.SetCastedOrRemove(m_delegateCompPtr->CreateResponse(*requestPtr));
		Q_ASSERT(responsePtr.IsValid());
		if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)) {
			QVariant variant;
			if (responsePtr->IsSuccessfull() && responsePtr->GetValue(variant)) {
				if (variant.type() == QVariant::Bool) {
					retVal = variant.toBool();
				}
			}
		}
	}

	return retVal;
}


bool CGqlObjectCollectionComp::SetNodeDescription(
			const Id& nodeId,
			const QString& description,
			const imtbase::IOperationContext* operationContextPtr)
{
	bool retVal = false;

	if (m_clientCompPtr.IsValid() && m_delegateCompPtr.IsValid()) {
		istd::TDelPtr<IGqlRequest> requestPtr = m_delegateCompPtr->CreateSetNodeNameRequest(
					nodeId,
					description,
					operationContextPtr);
		istd::TDelPtr<IGqlPrimitiveTypeResponse> responsePtr;
		responsePtr.SetCastedOrRemove(m_delegateCompPtr->CreateResponse(*requestPtr));
		Q_ASSERT(responsePtr.IsValid());
		if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)) {
			QVariant variant;
			if (responsePtr->IsSuccessfull() && responsePtr->GetValue(variant)) {
				if (variant.type() == QVariant::Bool) {
					retVal = variant.toBool();
				}
			}
		}
	}

	return retVal;
}


bool CGqlObjectCollectionComp::SetNodeMetaInfo(
			const Id& nodeId,
			const idoc::IDocumentMetaInfo& metaInfo,
			const imtbase::IOperationContext* operationContextPtr)
{
	bool retVal = false;

	if (m_clientCompPtr.IsValid() && m_delegateCompPtr.IsValid()) {
		istd::TDelPtr<IGqlRequest> requestPtr = m_delegateCompPtr->CreateSetNodeMetaInfoRequest(
					nodeId,
					metaInfo,
					operationContextPtr);
		istd::TDelPtr<IGqlPrimitiveTypeResponse> responsePtr;
		responsePtr.SetCastedOrRemove(m_delegateCompPtr->CreateResponse(*requestPtr));
		Q_ASSERT(responsePtr.IsValid());
		if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)) {
			QVariant variant;
			if (responsePtr->IsSuccessfull() && responsePtr->GetValue(variant)) {
				if (variant.type() == QVariant::Bool) {
					retVal = variant.toBool();
				}
			}
		}
	}

	return retVal;
}


bool CGqlObjectCollectionComp::MoveNode(
			const Id& nodeId,
			const Id& parentNodeId,
			const imtbase::IOperationContext* operationContextPtr)
{
	bool retVal = false;

	if (m_clientCompPtr.IsValid() && m_delegateCompPtr.IsValid()) {
		istd::TDelPtr<IGqlRequest> requestPtr = m_delegateCompPtr->CreateMoveNodeRequest(
					nodeId,
					parentNodeId,
					operationContextPtr);
		istd::TDelPtr<IGqlPrimitiveTypeResponse> responsePtr;
		responsePtr.SetCastedOrRemove(m_delegateCompPtr->CreateResponse(*requestPtr));
		Q_ASSERT(responsePtr.IsValid());
		if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)) {
			QVariant variant;
			if (responsePtr->IsSuccessfull() && responsePtr->GetValue(variant)) {
				if (variant.type() == QVariant::Bool) {
					retVal = variant.toBool();
				}
			}
		}
	}

	return retVal;
}


bool CGqlObjectCollectionComp::RemoveNode(
			const Id& nodeId,
			const imtbase::IOperationContext* operationContextPtr)
{
	bool retVal = false;

	if (m_clientCompPtr.IsValid() && m_delegateCompPtr.IsValid()) {
		istd::TDelPtr<IGqlRequest> requestPtr = m_delegateCompPtr->CreateRemoveNodeRequest(
					nodeId,
					operationContextPtr);
		istd::TDelPtr<IGqlPrimitiveTypeResponse> responsePtr;
		responsePtr.SetCastedOrRemove(m_delegateCompPtr->CreateResponse(*requestPtr));
		Q_ASSERT(responsePtr.IsValid());
		if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)) {
			QVariant variant;
			if (responsePtr->IsSuccessfull() && responsePtr->GetValue(variant)) {
				if (variant.type() == QVariant::Bool) {
					retVal = variant.toBool();
				}
			}
		}
	}

	return retVal;
}


bool CGqlObjectCollectionComp::AddObjectToNode(
			const Id& objectId,
			const Id& nodeId,
			const imtbase::IOperationContext* operationContextPtr)
{
	bool retVal = false;

	if (m_clientCompPtr.IsValid() && m_delegateCompPtr.IsValid()) {
		//Getobje

		istd::TDelPtr<IGqlRequest> requestPtr = m_delegateCompPtr->CreateAddObjectToNodeRequest(
					objectId,
					nodeId,
					-1,
					operationContextPtr);
		istd::TDelPtr<IGqlPrimitiveTypeResponse> responsePtr;
		responsePtr.SetCastedOrRemove(m_delegateCompPtr->CreateResponse(*requestPtr));
		Q_ASSERT(responsePtr.IsValid());
		if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)) {
			QVariant variant;
			if (responsePtr->IsSuccessfull() && responsePtr->GetValue(variant)) {
				if (variant.type() == QVariant::Bool) {
					retVal = variant.toBool();
				}
			}
		}
	}

	return retVal;
}


bool CGqlObjectCollectionComp::MoveObjectToNode(
			const Id& objectId,
			const Id& parentNodeId,
			const Id& newParentNodeId,
			const imtbase::IOperationContext* operationContextPtr)
{
	bool retVal = false;

	if (m_clientCompPtr.IsValid() && m_delegateCompPtr.IsValid()) {
		istd::TDelPtr<IGqlRequest> requestPtr = m_delegateCompPtr->CreateMoveObjectToNodeRequest(
					objectId,
					parentNodeId,
					newParentNodeId,
					-1,
					operationContextPtr);
		istd::TDelPtr<IGqlPrimitiveTypeResponse> responsePtr;
		responsePtr.SetCastedOrRemove(m_delegateCompPtr->CreateResponse(*requestPtr));
		Q_ASSERT(responsePtr.IsValid());
		if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)) {
			QVariant variant;
			if (responsePtr->IsSuccessfull() && responsePtr->GetValue(variant)) {
				if (variant.type() == QVariant::Bool) {
					retVal = variant.toBool();
				}
			}
		}
	}

	return retVal;
}


bool CGqlObjectCollectionComp::RemoveObjectFromNode(
			const Id& objectId,
			const Id& nodeId,
			const imtbase::IOperationContext* operationContextPtr)
{
	bool retVal = false;

	if (m_clientCompPtr.IsValid() && m_delegateCompPtr.IsValid()) {
		istd::TDelPtr<IGqlRequest> requestPtr = m_delegateCompPtr->CreateRemoveObjectFromNodeRequest(
					objectId,
					nodeId,
					-1,
					operationContextPtr);
		istd::TDelPtr<IGqlPrimitiveTypeResponse> responsePtr;
		responsePtr.SetCastedOrRemove(m_delegateCompPtr->CreateResponse(*requestPtr));
		Q_ASSERT(responsePtr.IsValid());
		if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)) {
			QVariant variant;
			if (responsePtr->IsSuccessfull() && responsePtr->GetValue(variant)) {
				if (variant.type() == QVariant::Bool) {
					retVal = variant.toBool();
				}
			}
		}
	}

	return retVal;
}


// reimpolemented (imtbase::ICollectionStructureInfo)

QByteArrayList CGqlObjectCollectionComp::GetNodePath(const QByteArray& nodeId) const
{
	QByteArrayList retVal;

	if (m_clientCompPtr.IsValid() && m_delegateCompPtr.IsValid()) {
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr = m_delegateCompPtr->CreateGetNodeInfoRequest(nodeId);
		istd::TDelPtr<IGqlStructuredCollectionResponse> responsePtr;
		responsePtr.SetCastedOrRemove(m_delegateCompPtr->CreateResponse(*requestPtr));
		Q_ASSERT(responsePtr.IsValid());
		if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
			if (responsePtr->IsSuccessfull()){
				IGqlStructuredCollectionResponse::NodeInfo info;
				if (responsePtr->GetNodeInfo(info)) {
					return retVal = info.path;
				}
			}
		}
	}

	return retVal;
}


QSharedPointer<imtbase::IStructuredObjectCollectionInfo> CGqlObjectCollectionComp::GetNodeContent(const QByteArray& nodeId) const
{
	CGqlObjectCollectionInfo* infoPtr = new CGqlObjectCollectionInfo();

	infoPtr->SetGqlClient(m_clientCompPtr.GetPtr());
	infoPtr->SetGqlDelegate(m_delegateCompPtr.GetPtr());
	infoPtr->SetNodeId(nodeId);

	return QSharedPointer<imtbase::IStructuredObjectCollectionInfo>(infoPtr);
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
	return this;
}


int CGqlObjectCollectionComp::GetOperationFlags(const Id& elementId) const
{
	if (!elementId.isEmpty()){
		return OF_SUPPORT_DELETE | OF_SUPPORT_EDIT;
	}

	return OF_SUPPORT_INSERT | OF_SUPPORT_DELETE | OF_SUPPORT_EDIT | OF_SUPPORT_PAGINATION;
}


imtbase::ICollectionInfo::Id CGqlObjectCollectionComp::InsertNewObject(
			const QByteArray& /*typeId*/,
			const QString& /*name*/,
			const QString& /*description*/,
			DataPtr /*defaultValuePtr*/,
			const QByteArray& /*proposedObjectId*/,
			const idoc::IDocumentMetaInfo* /*dataMetaInfoPtr*/,
			const idoc::IDocumentMetaInfo* /*collectionItemMetaInfoPtr*/,
			const imtbase::IOperationContext* /*operationContextPtr*/)
{
	return QByteArray();
}


bool CGqlObjectCollectionComp::RemoveElement(const Id& elementId, const imtbase::IOperationContext* /*operationContextPtr*/)
{
	bool retVal = false;

	if (m_clientCompPtr.IsValid()){
		QByteArray typeId = GetObjectTypeId(elementId);

		IGqlObjectCollectionDelegate* delegatePtr = m_delegateCompPtr.GetPtr();
		if (delegatePtr != nullptr){
			istd::TDelPtr<imtgql::IGqlRequest> requestPtr(delegatePtr->CreateRemoveObjectRequest(elementId));
			istd::TDelPtr<IGqlPrimitiveTypeResponse> responsePtr;
			responsePtr.SetCastedOrRemove(delegatePtr->CreateResponse(*requestPtr));
			Q_ASSERT(responsePtr.IsValid());
			if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
				if (responsePtr->IsSuccessfull()){
					if (retVal) {
						istd::IChangeable::ChangeSet changeSet(istd::IChangeable::CF_ANY);
						imtbase::ICollectionInfo::ElementRemoveInfo removeInfo;
						removeInfo.elementId = elementId;
						changeSet.SetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENT_REMOVED, QVariant::fromValue(removeInfo));
						istd::CChangeNotifier notifier(this, &changeSet);
					}
				}
			}
		}
	}

	return retVal;
}


const istd::IChangeable* CGqlObjectCollectionComp::GetObjectPtr(const Id& /*objectId*/) const
{
	return nullptr;
}


bool CGqlObjectCollectionComp::GetObjectData(const Id& objectId, DataPtr& dataPtr) const
{
	if (m_clientCompPtr.IsValid()){
		QByteArray typeId = GetObjectTypeId(objectId);

		if (dataPtr.IsValid()){
			DataPtr retVal = GetDocument(typeId, objectId);
			if (retVal.IsValid()){
				dataPtr = retVal;

				return true;
			}
		}
	}

	return false;
}


bool CGqlObjectCollectionComp::SetObjectData(
			const Id& /*objectId*/,
			const istd::IChangeable& /*object*/,
			CompatibilityMode /*mode*/,
			const imtbase::IOperationContext* /*operationContextPtr*/)
{
	bool retVal = false;

	//if (m_clientCompPtr.IsValid()){
	//	QByteArray typeId = GetObjectTypeId(objectId);

	//	IGqlObjectCollectionDelegate* delegatePtr = GetDelegateForType(typeId);
	//	if (delegatePtr != nullptr){
	//		idoc::MetaInfoPtr metaInfoPtr = GetElementMetaInfo(objectId);

	//		if (!metaInfoPtr.IsValid()){
	//			return false;
	//		}

	//		if (!metaInfoPtr->GetMetaInfoTypes().contains(MIT_VERSION)){
	//			return false;
	//		}

	//		int version = metaInfoPtr->GetMetaInfo(MIT_VERSION).toInt();

	//		QString name = GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toString();

	//		imtcom::IFileTransfer* fileTransferPtr = delegatePtr->GetFileTransfer();
	//		if (fileTransferPtr != nullptr){
	//			const ifile::IFilePersistence* persistencePtr = GetPersistenceForObjectType(typeId);
	//			imtbase::CTempDir tempDir;
	//			QString workingPath = tempDir.Path();
	//			QString docExt = GetDocumentExtension(typeId);
	//			QString filePath = workingPath + "/" + name + QString(".") + docExt;
	//			if (persistencePtr->SaveToFile(object, filePath) == ifile::IFilePersistence::OS_OK){
	//				QFile file(filePath);
	//				if (file.open(QIODevice::ReadOnly)){
	//					QByteArray data = file.readAll();

	//					Q_ASSERT(false);
	//					istd::TDelPtr<imtgql::IGqlRequest> uploadUrlRequestPtr(delegatePtr->CreateUploadUrlRequest(name, ""));
	//					if (!uploadUrlRequestPtr.IsValid()){
	//						return false;
	//					}

	//					istd::TDelPtr<IUploadUrlResponse> uploadUrlResponsePtr = delegatePtr->CreateUploadUrlResponse();
	//					if (m_clientCompPtr->SendRequest(*uploadUrlRequestPtr, *uploadUrlResponsePtr)){
	//						QString uploadUrl = uploadUrlResponsePtr->GetValue();
	//						if (!uploadUrl.isEmpty()){
	//							QNetworkRequest request;
	//							request.setUrl(QUrl(uploadUrl));
	//							QNetworkReply* replyPtr = imtcom::CRequestSender::DoSyncPut(request, data, 30000);
	//							if (replyPtr != nullptr){
	//								if (!replyPtr->error()){
	//									istd::TDelPtr<imtgql::IGqlRequest> updateDocumentRequestPtr(delegatePtr->CreateUpdateObjectContentRequest(m_items[objectId].parentId + "/" + objectId, object, nullptr, nullptr, version, uploadUrl));
	//									if (updateDocumentRequestPtr.IsValid()){
	//										IGqlObjectCollectionDelegate::UpdateObjectContentResponsePtr responsePtr = delegatePtr->CreateUpdateObjectContentResponsePtr();
	//										if (m_clientCompPtr->SendRequest(*updateDocumentRequestPtr, *responsePtr)){
	//											retVal = responsePtr->Result();
	//											if (retVal){
	//												istd::IChangeable::ChangeSet changeSet(istd::IChangeable::CF_ANY);
	//												imtbase::ICollectionInfo::ElementUpdateInfo updateInfo;
	//												updateInfo.elementId = objectId;
	//												changeSet.SetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENT_UPDATED, QVariant::fromValue(updateInfo));
	//												istd::CChangeNotifier notifier(this, &changeSet);
	//											}
	//										}
	//									}
	//								}

	//								replyPtr->deleteLater();
	//							}
	//						}
	//					}
	//				}
	//			}
	//		}
	//		else{
	//			istd::TDelPtr<imtgql::IGqlRequest> updateDocumentRequestPtr(delegatePtr->CreateUpdateObjectContentRequest(objectId, object, nullptr, nullptr));
	//			if (!updateDocumentRequestPtr.IsValid()){
	//				retVal = false;
	//			}
	//			else{
	//				IGqlObjectCollectionDelegate::UpdateObjectContentResponsePtr responsePtr = delegatePtr->CreateUpdateObjectContentResponsePtr();
	//				if (m_clientCompPtr->SendRequest(*updateDocumentRequestPtr, *responsePtr)){
	//					retVal = true;
	//				}
	//			}
	//		}
	//	}
	//}

	return retVal;
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
	if (m_clientCompPtr.IsValid() && m_delegateCompPtr.IsValid()){
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr = m_delegateCompPtr->CreateGetObjectInfoRequest(objectId);
		istd::TDelPtr<IGqlStructuredCollectionResponse> responsePtr;
		responsePtr.SetCastedOrRemove(m_delegateCompPtr->CreateResponse(*requestPtr));
		Q_ASSERT(responsePtr.IsValid());
		if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
			if (responsePtr->IsSuccessfull()){
				IGqlStructuredCollectionResponse::ObjectInfo info;
				if (responsePtr->GetObjectInfo(info)) {
					return info.typeId;
				}
			}
		}
	}

	return QByteArray();
}


idoc::MetaInfoPtr CGqlObjectCollectionComp::GetDataMetaInfo(const Id& objectId) const
{
	if (m_clientCompPtr.IsValid() && m_delegateCompPtr.IsValid()){
		istd::TDelPtr<IGqlRequest> requestPtr(m_delegateCompPtr->CreateGetObjectInfoRequest(objectId));
		istd::TDelPtr<IGqlStructuredCollectionResponse> responsePtr;
		responsePtr.SetCastedOrRemove(m_delegateCompPtr->CreateResponse(*requestPtr));
		Q_ASSERT(responsePtr.IsValid());
		if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
			if (responsePtr->IsSuccessfull()){
				IGqlStructuredCollectionResponse::ObjectInfo info;
				if (responsePtr->GetObjectInfo(info)){
					if (!info.dataMetaInfoPtr.isNull()) {
						idoc::MetaInfoPtr metaInfoPtr;
						metaInfoPtr.SetCastedOrRemove(info.dataMetaInfoPtr->CloneMe());

						return metaInfoPtr;
					}
				}
			}
		}
	}

	return idoc::MetaInfoPtr();
}


// reimplemented (ICollectionInfo)

int CGqlObjectCollectionComp::GetElementsCount(const iprm::IParamsSet* /*selectionParamsPtr*/) const
{
	Q_ASSERT(false);

	return -1;
}


imtbase::ICollectionInfo::Ids CGqlObjectCollectionComp::GetElementIds(
			int /*offset*/,
			int /*count*/,
			const iprm::IParamsSet* /*selectionParamsPtr*/) const
{
	Q_ASSERT(false);

	return imtbase::ICollectionInfo::Ids();
}


bool CGqlObjectCollectionComp::GetSubsetInfo(ICollectionInfo& /*subsetInfo*/, int /*offset*/, int /*count*/, const iprm::IParamsSet* /*selectionParamsPtr*/) const
{
	return false;
}


QVariant CGqlObjectCollectionComp::GetElementInfo(const Id& elementId, int infoType) const
{
	IGqlStructuredCollectionResponse::ObjectInfo info;
	bool isValid = false;

	if (GetNodeInfo(elementId, info)) {
		isValid = true;
	}
	else if (GetObjectInfo(elementId, info)) {
		isValid = true;
	}

	if (isValid) {
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

	if (GetNodeInfo(elementId, info)) {
		isValid = true;
	}
	else if (GetObjectInfo(elementId, info)) {
		isValid = true;
	}

	if (isValid) {
		if (!info.elementMetaInfoPtr.isNull()) {
			metaInfoPtr.SetCastedOrRemove(info.elementMetaInfoPtr->CloneMe());
		}
	}

	return metaInfoPtr;
}


bool CGqlObjectCollectionComp::SetElementName(const Id& elementId, const QString& name)
{
	if (m_clientCompPtr.IsValid() && m_delegateCompPtr.IsValid()) {
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr;
		istd::TDelPtr<imtgql::IGqlResponse> responsePtr;
		responsePtr.SetCastedOrRemove(m_delegateCompPtr->CreateResponse(*requestPtr));
		Q_ASSERT(responsePtr.IsValid());

		ElementType type;
		if (GetElementType(elementId, type, true)) {
			switch (type){
			case ET_NODE:
				requestPtr.SetPtr(m_delegateCompPtr->CreateSetNodeNameRequest(elementId, name));
				break;

			case ET_OBJECT:
				requestPtr.SetPtr(m_delegateCompPtr->CreateSetObjectNameRequest(elementId, name));
				break;

			default:
				Q_ASSERT(false);
			}
		}

		if (requestPtr.IsValid()) {
			if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)) {
				if (responsePtr->IsSuccessfull()) {
					return true;
				}
			}
		}
	}

	return false;
}


bool CGqlObjectCollectionComp::SetElementDescription(const Id& elementId, const QString& description)
{
	if (m_clientCompPtr.IsValid() && m_delegateCompPtr.IsValid()) {
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr;
		istd::TDelPtr<imtgql::IGqlResponse> responsePtr;
		responsePtr.SetCastedOrRemove(m_delegateCompPtr->CreateResponse(*requestPtr));
		Q_ASSERT(responsePtr.IsValid());

		ElementType type;
		if (GetElementType(elementId, type, true)) {
			switch (type) {
			case ET_NODE:
				requestPtr.SetPtr(m_delegateCompPtr->CreateSetNodeDescriptionRequest(elementId, description));
				break;

			case ET_OBJECT:
				requestPtr.SetPtr(m_delegateCompPtr->CreateSetObjectDescriptionRequest(elementId, description));
				break;

			default:
				Q_ASSERT(false);
			}
		}

		if (requestPtr.IsValid()) {
			if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)) {
				if (responsePtr->IsSuccessfull()) {
					return true;
				}
			}
		}
	}

	return false;
}


bool CGqlObjectCollectionComp::SetElementEnabled(const Id& /*elementId*/, bool /*isEnabled*/)
{
	return false;
}


// reimplemented (imtbase::ICollectionDataController)

const ifile::IFilePersistence* CGqlObjectCollectionComp::GetPersistenceForObjectType(const QByteArray& typeId) const
{
	int count = m_typesInfo.GetOptionsCount();
	for (int i = 0; i < count; i++){
		if (m_typeIdsAttrPtr[i] == typeId){
			return m_objectPersistenceListCompPtr[i];
		}
	}

	return nullptr;
}


bool CGqlObjectCollectionComp::ExportFile(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId,
			const QString& targetFilePath) const
{
	if (&collection != this){
		return false;
	}

	QByteArray objectTypeId = GetObjectTypeId(objectId);
	if (!objectTypeId.isEmpty()){
		const ifile::IFilePersistence* persistencePtr = GetPersistenceForObjectType(objectTypeId);
		if (persistencePtr != nullptr){
			DataPtr objectDataPtr;
			if (GetObjectData(objectId, objectDataPtr)){
				return persistencePtr->SaveToFile(*objectDataPtr, targetFilePath) == ifile::IFilePersistence::OS_OK;
			}
		}
	}

	return false;
}


QByteArray CGqlObjectCollectionComp::ImportFile(
			imtbase::IObjectCollection& collection,
			const QByteArray& typeId,
			const QString& sourceFilePath) const
{
	Q_ASSERT(false);
	return QByteArray();

	if (&collection != this){
		return QByteArray();
	}

	IGqlObjectCollectionDelegate* delegatePtr = m_delegateCompPtr.GetPtr();
	if (delegatePtr != nullptr){
		const ifile::IFilePersistence* persistencePtr = GetPersistenceForObjectType(typeId);
		if (persistencePtr != nullptr){
			DataPtr objectPtr = CreateObjectInstance(typeId);
			if (objectPtr.IsValid()){
				if (persistencePtr->LoadFromFile(*objectPtr, sourceFilePath) == ifile::IFilePersistence::OS_OK){
					QFile file(sourceFilePath);
					if (file.open(QIODevice::ReadOnly)){
						QByteArray data = file.readAll();

						QFileInfo fileInfo(sourceFilePath);
						QString fileName = fileInfo.completeBaseName();
						QString objectName = fileInfo.completeBaseName();

						Q_ASSERT(false);
						istd::TDelPtr<imtgql::IGqlRequest> getDocumentUploadUrlRequestPtr(delegatePtr->CreateUploadUrlsRequest({ objectName }, "parentId"));
						istd::TDelPtr<IGqlResponse> uploadUrlDocumentResponsePtr;// = delegatePtr->CreateResponse();
						if (m_clientCompPtr->SendRequest(*getDocumentUploadUrlRequestPtr, *uploadUrlDocumentResponsePtr)){
							QString uploadUrl;// = uploadUrlResponsePtr->GetValue();
							if (!uploadUrl.isEmpty()){
								QByteArray documentId;

								QNetworkRequest request;
								request.setUrl(QUrl(uploadUrl));
								QNetworkReply* replyPtr = imtcom::CRequestSender::DoSyncPut(request, data, 30000);
								if (replyPtr != nullptr){
									if (!replyPtr->error()){
										idoc::MetaInfoPtr metaInfoPtr(new idoc::CStandardDocumentMetaInfo());

										//istd::TDelPtr<imtgql::IGqlRequest> uploadDocumentRequestPtr(delegatePtr->CreateInsertObjectRequest(fileName, "", *objectPtr, metaInfoPtr.GetPtr(), nullptr, uploadUrl));

										//IGqlObjectCollectionDelegate::InsertObjectResponsePtr saveDocumentResponsePtr = delegatePtr->CreateInsertObjectResponsePtr();
										//if (m_clientCompPtr->SendRequest(*uploadDocumentRequestPtr, *saveDocumentResponsePtr)){
										//	documentId = saveDocumentResponsePtr->GetObjectId();
										//	if (!documentId.isEmpty()){
										//		//istd::IChangeable::ChangeSet changeSet(istd::IChangeable::CF_ANY);
										//		//imtbase::ICollectionInfo::ElementInsertInfo insertInfo;
										//		//insertInfo.elementId = documentId;
										//		//changeSet.SetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENT_INSERTED, QVariant::fromValue(insertInfo));
										//		//istd::CChangeNotifier notifier(this, &changeSet);
										//	}
										//	else{
										//		SendErrorMessage(0, saveDocumentResponsePtr->GetMessage(0), "Document Cloud Controller");
										//	}
										//}
									}

									replyPtr->deleteLater();
								}

								return documentId;
							}
						}
					}
				}
			}
		}
	}

	return QByteArray();
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
			return DataPtr(DataPtr::RootObjectPtr(compPtr), [compPtr, this]() {
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

	//connect(this, &CGqlObjectCollectionComp::EmitSyncronizeDocumentListWithServer, this, &CGqlObjectCollectionComp::SyncronizeDocumentListWithServer, Qt::QueuedConnection);

	m_clientCompPtr.EnsureInitialized();

	int count = qMin(m_typeIdsAttrPtr.GetCount(), m_typeNamesAttrPtr.GetCount());
	count = qMin(count, m_objectFactoriesCompPtr.GetCount());
	count = qMin(count, m_objectPersistenceListCompPtr.GetCount());

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
	m_subscriptionManagerCompPtr->UnRegisterSubscription(m_addMeasurementSubsriptionId);

	BaseClass::OnComponentDestroyed();
}


// private methods

QString CGqlObjectCollectionComp::GetDocumentExtension(const QByteArray& typeId) const
{
	const ifile::IFileTypeInfo* fileTypeInfoPtr = GetPersistenceForObjectType(typeId);
	if (fileTypeInfoPtr == nullptr){
		return nullptr;
	}

	QStringList extensons;
	if (!fileTypeInfoPtr->GetFileExtensions(extensons)){
		return nullptr;
	}

	QString docExt;
	if (!extensons.isEmpty()){
		docExt = extensons[0];
	}

	return docExt;
}


imtbase::IObjectCollection::DataPtr CGqlObjectCollectionComp::GetDocument(
			const QByteArray& typeId,
			const QByteArray& documentId) const
{
	IGqlObjectCollectionDelegate* delegatePtr = m_delegateCompPtr.GetPtr();
	if (delegatePtr == nullptr){
		return nullptr;
	}

	QByteArray localDocumentId = documentId;

	imtbase::IObjectCollection::DataPtr documentPtr;

	imtcom::IFileTransfer* fileTransferPtr = delegatePtr->GetFileTransfer();
	if (fileTransferPtr != nullptr){
		istd::TDelPtr<imtgql::IGqlRequest> queryPtr = delegatePtr->CreateDownloadUrlsRequest({ documentId });
		if (queryPtr == nullptr){
			return nullptr;
		}

		istd::TDelPtr<IGqlResponse> responsePtr;// = delegatePtr->CreateResponse();
		if (m_clientCompPtr->SendRequest(*queryPtr, *responsePtr)) {
			QVariant variant;

			if (responsePtr->IsSuccessfull() /*&& responsePtr->GetValue(variant)*/){
				QString downloadUrl = variant.toString();
				if (!downloadUrl.isEmpty()) {
					QNetworkRequest request;
					request.setUrl(QUrl(downloadUrl));
					QNetworkReply* replyPtr = imtcom::CRequestSender::DoSyncGet(request, 30000);
					if (!replyPtr->error()) {
						imtbase::CTempDir tempDir;
						QString workingPath = tempDir.Path();
						QString docExt = GetDocumentExtension(typeId);
						QString docName = GetElementInfo(localDocumentId, EIT_NAME).toString();
						QString docPath = workingPath + "/" + docName + QString(".") + docExt;
						QFile docFile(docPath);
						if (docFile.open(QIODevice::WriteOnly)) {
							docFile.write(replyPtr->readAll());
							docFile.close();

							QByteArray localTypeId = GetObjectTypeId(localDocumentId);
							const ifile::IFilePersistence* persistencePtr = GetPersistenceForObjectType(localTypeId);
							if (persistencePtr != nullptr) {
								documentPtr = CreateObjectInstance(localTypeId);
								if (documentPtr.IsValid()) {
									if (persistencePtr->LoadFromFile(*documentPtr, docPath) != ifile::IFilePersistence::OS_OK) {
										documentPtr = imtbase::IObjectCollection::DataPtr();
									}
								}
							}
						}
					}

					replyPtr->deleteLater();
				}
			}
		}
	}

	return documentPtr;
}


bool CGqlObjectCollectionComp::GetElementType(const QByteArray& elementId, ElementType& valueOut, bool tryViaInfo) const
{
	if (m_clientCompPtr.IsValid() && m_delegateCompPtr.IsValid()) {
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr(m_delegateCompPtr->CreateGetElementType(elementId));
		if (requestPtr.IsValid()) {
			istd::TDelPtr<imtgql::IGqlStructuredCollectionResponse> responsePtr;
			responsePtr.SetCastedOrRemove(m_delegateCompPtr->CreateResponse(*requestPtr));
			Q_ASSERT(responsePtr.IsValid());
			if (requestPtr.IsValid()) {
				if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)) {
					if (responsePtr->IsSuccessfull()) {
						IGqlStructuredCollectionResponse::ElementInfo info;
						if (responsePtr->GetElementInfo(info)) {
							valueOut = info.isNode ? ET_NODE : ET_OBJECT;

							return true;
						}

						Q_ASSERT(false);
					}
				}
			}
		}
		else if (tryViaInfo){
			IGqlStructuredCollectionResponse::NodeInfo nodeInfo;
			IGqlStructuredCollectionResponse::ObjectInfo objectInfo;

			if (GetNodeInfo(elementId, nodeInfo)) {
				valueOut = ET_NODE;

				return true;
			}
			else if (GetObjectInfo(elementId, objectInfo)) {
				valueOut = ET_OBJECT;

				return true;
			}
		}
	}

	return false;
}


bool CGqlObjectCollectionComp::GetNodeInfo(const QByteArray& nodeId, IGqlStructuredCollectionResponse::NodeInfo& valueOut) const
{
	if (m_clientCompPtr.IsValid() && m_delegateCompPtr.IsValid()) {
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr(m_delegateCompPtr->CreateGetNodeInfoRequest(nodeId));
		istd::TDelPtr<imtgql::IGqlStructuredCollectionResponse> responsePtr;
		responsePtr.SetCastedOrRemove(m_delegateCompPtr->CreateResponse(*requestPtr));
		Q_ASSERT(responsePtr.IsValid());
		if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)) {
			if (responsePtr->IsSuccessfull()) {
				IGqlStructuredCollectionResponse::NodeInfo info;
				if (responsePtr->GetNodeInfo(info)) {
					valueOut = info;

					return true;
				}
			}
		}
	}

	return false;
}


bool CGqlObjectCollectionComp::GetObjectInfo(const QByteArray& objectId, IGqlStructuredCollectionResponse::ObjectInfo& valueOut) const
{
	if (m_clientCompPtr.IsValid() && m_delegateCompPtr.IsValid()) {
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr(m_delegateCompPtr->CreateGetObjectInfoRequest(objectId));
		istd::TDelPtr<imtgql::IGqlStructuredCollectionResponse> responsePtr;
		responsePtr.SetCastedOrRemove(m_delegateCompPtr->CreateResponse(*requestPtr));
		Q_ASSERT(responsePtr.IsValid());
		if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)) {
			if (responsePtr->IsSuccessfull()) {
				IGqlStructuredCollectionResponse::ObjectInfo info;
				if (responsePtr->GetObjectInfo(info)) {
					valueOut = info;

					return true;
				}
			}
		}
	}

	return false;
}


} // namespace imtgql


