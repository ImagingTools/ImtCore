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


namespace imtgql
{


bool operator==(const IGqlObjectCollectionDelegate::ElementInfo& a, const IGqlObjectCollectionDelegate::ElementInfo& b)
{
	return a.id == b.id && a.typeId == b.typeId && a.name == b.name;
}


// public methods

CGqlObjectCollectionComp::CGqlObjectCollectionComp()
{
	qRegisterMetaType<IGqlObjectCollectionDelegate::ElementList>("imtgql::IGqlObjectCollectionDelegate::ElementList");
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


imtbase::ICollectionInfo::Id CGqlObjectCollectionComp::InsertNewBranch(const Id& parentId, const QString& name, const QString& description, const Id& proposedElementId, const idoc::IDocumentMetaInfo* elementMetaInfoPtr)
{
	return Id();
}


imtbase::ICollectionInfo::Id CGqlObjectCollectionComp::InsertNewObject(
			const QByteArray& typeId,
			const QString& name,
			const QString& description,
			DataPtr defaultValuePtr,
			const QByteArray& proposedObjectId,
			const idoc::IDocumentMetaInfo* dataMetaInfoPtr,
			const idoc::IDocumentMetaInfo* collectionItemMetaInfoPtr,
			const Id& parentId)
{
	QByteArray documentId;

	if (m_gqlClientCompPtr.IsValid()){
		IGqlObjectCollectionDelegate* delegatePtr = GetDelegateForType(typeId);
		if (delegatePtr != nullptr){
			const ifile::IFilePersistence* persistencePtr = GetPersistenceForObjectType(typeId);
			if (persistencePtr != nullptr){
				if (defaultValuePtr.IsValid()){
					imtcom::IFileTransfer* fileTransferPtr = delegatePtr->GetFileTransfer();
					if (fileTransferPtr != nullptr){
						imtbase::CTempDir tempDir;
						QString workingPath = tempDir.Path();
						QString docExt = GetDocumentExtension(typeId);
						QString filePath = workingPath + "/" + name + (docExt.isEmpty() ? "" : QString(".") + docExt);
						if (persistencePtr->SaveToFile(*defaultValuePtr, filePath) == ifile::IFilePersistence::OS_OK){
							QFile file(filePath);
							if (file.open(QIODevice::ReadOnly)){
								QByteArray data = file.readAll();

								istd::TDelPtr<imtgql::IGqlRequest> getDocumentUploadUrlRequestPtr(delegatePtr->CreateUploadUrlRequest(name));

								IGqlObjectCollectionDelegate::UploadUrlResponsePtr uploadUrlDocumentResponsePtr = delegatePtr->CreateUploadUrlResponsePtr();
								if (m_gqlClientCompPtr->SendRequest(*getDocumentUploadUrlRequestPtr, *uploadUrlDocumentResponsePtr)){
									QString uploadUrl = uploadUrlDocumentResponsePtr->GetUploadUrl();
									if (!uploadUrl.isEmpty()){
										QNetworkRequest request;
										request.setUrl(QUrl(uploadUrl));
										QNetworkReply* replyPtr = imtcom::CRequestSender::DoSyncPut(request, data, 30000);
										if (replyPtr != nullptr){
											if (!replyPtr->error()){
												istd::TDelPtr<imtgql::IGqlRequest> uploadDocumentRequestPtr(delegatePtr->CreateInsertObjectRequest(name, "", *defaultValuePtr, dataMetaInfoPtr, nullptr, uploadUrl, "", parentId));

												IGqlObjectCollectionDelegate::InsertObjectResponsePtr saveDocumentResponsePtr = delegatePtr->CreateInsertObjectResponsePtr();
												if (m_gqlClientCompPtr->SendRequest(*uploadDocumentRequestPtr, *saveDocumentResponsePtr)){
													documentId = saveDocumentResponsePtr->GetObjectId();
													if (!documentId.isEmpty()){
														istd::IChangeable::ChangeSet changeSet(istd::IChangeable::CF_ANY);
														imtbase::ICollectionInfo::ElementInsertInfo insertInfo;
														insertInfo.elementId = documentId;
														changeSet.SetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENT_INSERTED, QVariant::fromValue(insertInfo));
														istd::CChangeNotifier notifier(this, &changeSet);
													}
													else{
														SendErrorMessage(0, saveDocumentResponsePtr->GetMessage(0), "Document Cloud Controller");
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
					else{
						istd::TDelPtr<imtgql::IGqlRequest> insertDocumentRequestPtr(delegatePtr->CreateInsertObjectRequest(name, "", * defaultValuePtr, dataMetaInfoPtr, nullptr, "", "", parentId));

						IGqlObjectCollectionDelegate::InsertObjectResponsePtr saveDocumentResponsePtr = delegatePtr->CreateInsertObjectResponsePtr();
						if (m_gqlClientCompPtr->SendRequest(*insertDocumentRequestPtr, *saveDocumentResponsePtr)){
							documentId = saveDocumentResponsePtr->GetObjectId();
							if (documentId.isEmpty()){
								SendErrorMessage(0, saveDocumentResponsePtr->GetMessage(0), "Document Cloud Controller");
							}
						}
					}
				}
			}
		}
	}

	return documentId;
}


bool CGqlObjectCollectionComp::RemoveElement(const Id& elementId)
{
	bool retVal = false;

	if (m_gqlClientCompPtr.IsValid()){
		QByteArray typeId = GetObjectTypeId(elementId);

		IGqlObjectCollectionDelegate* delegatePtr = GetDelegateForType(typeId);
		if (delegatePtr != nullptr){
			IGqlObjectCollectionDelegate::RemoveElementResponsePtr responsePtr = delegatePtr->CreateRemoveElementResponsePtr();

			istd::TDelPtr<imtgql::IGqlRequest> queryPtr(delegatePtr->CreateRemoveElementRequest(elementId));
			if (m_gqlClientCompPtr->SendRequest(*queryPtr, *responsePtr)){
				retVal = responsePtr->Result();
				if (retVal){
					istd::IChangeable::ChangeSet changeSet(istd::IChangeable::CF_ANY);
					imtbase::ICollectionInfo::ElementRemoveInfo removeInfo;
					removeInfo.elementId = elementId;
					changeSet.SetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENT_REMOVED, QVariant::fromValue(removeInfo));
					istd::CChangeNotifier notifier(this, &changeSet);
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
	if (m_gqlClientCompPtr.IsValid()){
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
			const Id& objectId,
			const istd::IChangeable& object,
			CompatibilityMode /*mode*/)
{
	bool retVal = false;

	if (m_gqlClientCompPtr.IsValid()){
		QByteArray typeId = GetObjectTypeId(objectId);

		IGqlObjectCollectionDelegate* delegatePtr = GetDelegateForType(typeId);
		if (delegatePtr != nullptr){
			idoc::MetaInfoPtr metaInfoPtr = GetElementMetaInfo(objectId);

			if (!metaInfoPtr.IsValid()){
				return false;
			}

			if (!metaInfoPtr->GetMetaInfoTypes().contains(IGqlObjectCollectionDelegate::MIT_VERSION)){
				return false;
			}

			int version = metaInfoPtr->GetMetaInfo(IGqlObjectCollectionDelegate::MIT_VERSION).toInt();

			QString name = GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toString();

			imtcom::IFileTransfer* fileTransferPtr = delegatePtr->GetFileTransfer();
			if (fileTransferPtr != nullptr){
				const ifile::IFilePersistence* persistencePtr = GetPersistenceForObjectType(typeId);
				imtbase::CTempDir tempDir;
				QString workingPath = tempDir.Path();
				QString docExt = GetDocumentExtension(typeId);
				QString filePath = workingPath + "/" + name + QString(".") + docExt;
				if (persistencePtr->SaveToFile(object, filePath) == ifile::IFilePersistence::OS_OK){
					QFile file(filePath);
					if (file.open(QIODevice::ReadOnly)){
						QByteArray data = file.readAll();

						istd::TDelPtr<imtgql::IGqlRequest> uploadUrlRequestPtr(delegatePtr->CreateUploadUrlRequest(name));
						if (!uploadUrlRequestPtr.IsValid()){
							return false;
						}

						IGqlObjectCollectionDelegate::UploadUrlResponsePtr uploadUrlDocumentResponsePtr = delegatePtr->CreateUploadUrlResponsePtr();
						if (m_gqlClientCompPtr->SendRequest(*uploadUrlRequestPtr, *uploadUrlDocumentResponsePtr)){
							QString uploadUrl = uploadUrlDocumentResponsePtr->GetUploadUrl();
							if (!uploadUrl.isEmpty()){
								QNetworkRequest request;
								request.setUrl(QUrl(uploadUrl));
								QNetworkReply* replyPtr = imtcom::CRequestSender::DoSyncPut(request, data, 30000);
								if (replyPtr != nullptr){
									if (!replyPtr->error()){
										istd::TDelPtr<imtgql::IGqlRequest> updateDocumentRequestPtr(delegatePtr->CreateUpdateObjectContentRequest(m_items[objectId].parentId + "/" + objectId, object, nullptr, nullptr, version, uploadUrl));
										if (updateDocumentRequestPtr.IsValid()){
											IGqlObjectCollectionDelegate::UpdateObjectContentResponsePtr responsePtr = delegatePtr->CreateUpdateObjectContentResponsePtr();
											if (m_gqlClientCompPtr->SendRequest(*updateDocumentRequestPtr, *responsePtr)){
												retVal = responsePtr->Result();
												if (retVal){
													istd::IChangeable::ChangeSet changeSet(istd::IChangeable::CF_ANY);
													imtbase::ICollectionInfo::ElementUpdateInfo updateInfo;
													updateInfo.elementId = objectId;
													changeSet.SetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENT_UPDATED, QVariant::fromValue(updateInfo));
													istd::CChangeNotifier notifier(this, &changeSet);
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
			else{
				istd::TDelPtr<imtgql::IGqlRequest> updateDocumentRequestPtr(delegatePtr->CreateUpdateObjectContentRequest(objectId, object, nullptr, nullptr));
				if (!updateDocumentRequestPtr.IsValid()){
					retVal = false;
				}
				else{
					IGqlObjectCollectionDelegate::UpdateObjectContentResponsePtr responsePtr = delegatePtr->CreateUpdateObjectContentResponsePtr();
					if (m_gqlClientCompPtr->SendRequest(*updateDocumentRequestPtr, *responsePtr)){
						retVal = true;
					}
				}
			}
		}
	}

	return retVal;
}


imtbase::IObjectCollection* CGqlObjectCollectionComp::CreateSubCollection(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr,
			const Id& parentId,
			int iterationFlags) const
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
	if (m_gqlClientCompPtr.IsValid()){
		int delegateCount = m_typesInfo.GetOptionsCount();
		for (int i = 0; i < delegateCount; i++){
			IGqlObjectCollectionDelegate* delegatePtr = m_gqlDatabaseDelegatesCompPtr[i];
			if (delegatePtr != nullptr){
				QByteArray typeId = delegatePtr->GetSupportedObjectTypeId();
				IGqlObjectCollectionDelegate::ElementInfoResponsePtr responsePtr = delegatePtr->CreateElementInfoResponsePtr();
				istd::TDelPtr<imtgql::IGqlRequest> requestPtr = delegatePtr->CreateElementInfoRequest(objectId);
				if (requestPtr.IsValid()){
					if (m_gqlClientCompPtr->SendRequest(*requestPtr, *responsePtr)){
						IGqlObjectCollectionDelegate::ElementInfo info = responsePtr->GetElementInfo();
						if (!info.isBranch && !info.id.isEmpty() && !info.typeId.isEmpty()){
							return info.typeId;
						}
					}
				}
			}
		}

		if (delegateCount > 0) {
			return m_gqlDatabaseDelegatesCompPtr[0]->GetSupportedObjectTypeId();
		}
	}

	return QByteArray();
}


idoc::MetaInfoPtr CGqlObjectCollectionComp::GetDataMetaInfo(const Id& objectId) const
{
	if (m_gqlClientCompPtr.IsValid()){
		QByteArray typeId = GetObjectTypeId(objectId);
		IGqlObjectCollectionDelegate* delegatePtr = GetDelegateForType(typeId);
		if (delegatePtr != nullptr){
			IGqlObjectCollectionDelegate::ElementInfoResponsePtr responsePtr = delegatePtr->CreateElementInfoResponsePtr();
			istd::TDelPtr<imtgql::IGqlRequest> requestPtr(delegatePtr->CreateElementInfoRequest(objectId));
			if (requestPtr.IsValid()){
				if (m_gqlClientCompPtr->SendRequest(*requestPtr, *responsePtr)){
					IGqlObjectCollectionDelegate::ElementInfo info = responsePtr->GetElementInfo();
					if (info.dataMetaInfoPtr.IsValid()){
						idoc::MetaInfoPtr metaInfo;
						metaInfo.SetCastedOrRemove(info.dataMetaInfoPtr->CloneMe());

						return metaInfo;
					}
				}
			}
		}
	}

	return idoc::MetaInfoPtr();
}


// reimplemented (ICollectionInfo)

int CGqlObjectCollectionComp::GetElementsCount(const iprm::IParamsSet* selectionParamsPtr, const Id& parentId, int iterationFlags) const
{
	Q_ASSERT(false);

	//if (!m_delegatesMap.isEmpty()){
	//	QByteArray typeId = m_delegatesMap.firstKey();
	//	IGqlObjectCollectionDelegate* delegatePtr = m_delegatesMap.first();
	//	if (delegatePtr != nullptr){
	//		GetElementCountResponse response(*const_cast<CGqlObjectCollectionComp*>(this), typeId);
	//		istd::TDelPtr<imtgql::IGqlRequest> requestPtr(delegatePtr->CreateElementCountRequest(selectionParamsPtr, parentId, iterationFlags));
	//		if (requestPtr.IsValid()){
	//			if (m_gqlClientCompPtr->SendRequest(*requestPtr, response)){
	//				return response.GetElementCount();
	//			}
	//		}
	//	}
	//}

	return 0;
}


imtbase::ICollectionInfo::Ids CGqlObjectCollectionComp::GetElementIds(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr,
			const Id& parentId,
			int iterationFlags) const
{
	if (!m_delegatesMap.isEmpty()){
		QByteArray typeId = m_delegatesMap.firstKey();
		IGqlObjectCollectionDelegate* delegatePtr = m_delegatesMap.first();
		if (delegatePtr != nullptr){
			IGqlObjectCollectionDelegate::ElementListResponsePtr elementCountResponsePtr = delegatePtr->CreateElementListResponsePtr();
			istd::TDelPtr<imtgql::IGqlRequest> requestPtr(delegatePtr->CreateElementListRequest(
						offset,
						count,
						selectionParamsPtr,
						parentId,
						iterationFlags));
			if (requestPtr.IsValid()){
				if (m_gqlClientCompPtr->SendRequest(*requestPtr, *elementCountResponsePtr)){
					IGqlObjectCollectionDelegate::ElementList elementList = elementCountResponsePtr->GetElementList();
					imtbase::ICollectionInfo::Ids ids;
					for (IGqlObjectCollectionDelegate::ElementInfo& element : elementList){
						// TODO: remove after api changed
						m_items[element.id] = {parentId, element.isBranch, element.name, element.version};

						if (element.isBranch && !element.id.isEmpty() && (iterationFlags & imtbase::ICollectionInfo::IF_BRANCH_ONLY)) {
							ids.append(element.id);
						}

						if (!element.isBranch && (iterationFlags & imtbase::ICollectionInfo::IF_LEAF_ONLY)) {
							ids.append(element.id);
						}
					}

					return ids;
				}
			}
		}
	}

	return imtbase::ICollectionInfo::Ids();
}


bool CGqlObjectCollectionComp::GetSubsetInfo(ICollectionInfo& subsetInfo, int offset, int count, const iprm::IParamsSet* selectionParamsPtr, const Id& parentId, int iterationFlags) const
{
	return false;
}


imtbase::ICollectionInfo::Id CGqlObjectCollectionComp::GetParentId(const Id& elementId) const
{
	Ids ids = GetElementPath(elementId);
	if (!ids.isEmpty()){
		return ids.first();
	}
	
	return Id();
}


imtbase::ICollectionInfo::Ids CGqlObjectCollectionComp::GetElementPath(const Id& elementId) const
{
	if (m_gqlClientCompPtr.IsValid()){
		QByteArray typeId = GetObjectTypeId(elementId);
		IGqlObjectCollectionDelegate* delegatePtr = GetDelegateForType(typeId);
		if (delegatePtr != nullptr){
			IGqlObjectCollectionDelegate::ElementInfoResponsePtr responsePtr = delegatePtr->CreateElementInfoResponsePtr();
			istd::TDelPtr<imtgql::IGqlRequest> requestPtr(delegatePtr->CreateElementInfoRequest(elementId));
			if (requestPtr.IsValid()){
				if (m_gqlClientCompPtr->SendRequest(*requestPtr, *responsePtr)){
					IGqlObjectCollectionDelegate::ElementInfo info = responsePtr->GetElementInfo();
					return info.path;
				}
			}
		}
	}

	return Ids();
}


bool CGqlObjectCollectionComp::IsBranch(const Id& elementId) const
{
	if (m_gqlClientCompPtr.IsValid()){
		if (m_items.contains(elementId)){
			return m_items[elementId].isBranch;
		}

		//QByteArray typeId = GetObjectTypeId(elementId);
		//gmgaws::IGqlObjectCollectionDelegate* delegatePtr = GetDelegateForType(typeId);
		//if (delegatePtr != nullptr){
		//	GetElementInfoResponse response(*const_cast<CGqlObjectCollectionComp*>(this), typeId);
		//	istd::TDelPtr<imtgql::IGqlRequest> requestPtr(delegatePtr->CreateElementInfoRequest(elementId));
		//	if (requestPtr.IsValid()){
		//		if (m_gqlClientCompPtr->SendRequest(*requestPtr, response)){
		//			IGqlObjectCollectionDelegate::ElementInfo info = response.GetElementInfo();
		//			return info.isBranch;
		//		}
		//	}
		//}
	}

	return false;
}


QVariant CGqlObjectCollectionComp::GetElementInfo(const Id& elementId, int infoType) const
{
	if (m_gqlClientCompPtr.IsValid()){
		//QByteArray typeId = GetObjectTypeId(elementId);
		//gmgaws::IGqlObjectCollectionDelegate* delegatePtr = GetDelegateForType(typeId);
		//if (delegatePtr != nullptr){
			//GetElementInfoResponse response(*const_cast<CGqlObjectCollectionComp*>(this), typeId);
			//istd::TDelPtr<imtgql::IGqlRequest> requestPtr(delegatePtr->CreateElementInfoRequest(elementId));
			//if (requestPtr.IsValid()){
			//	if (m_gqlClientCompPtr->SendRequest(*requestPtr, response)){
			//		IGqlObjectCollectionDelegate::ElementInfo info = response.GetElementInfo();
					switch (infoType){
					case EIT_NAME:
						if (m_items.contains(elementId)){
							return m_items[elementId].name;
						}

			//			return info.name;
			//		case EIT_DESCRIPTION:
			//			return info.description;
			//		case EIT_ENABLED:
			//			return true;
					}
			//	}
			//}
	//	}
	}

	return QVariant();
}


idoc::MetaInfoPtr CGqlObjectCollectionComp::GetElementMetaInfo(const Id& elementId) const
{
	idoc::MetaInfoPtr metaInfoPtr;

	if (m_items.contains(elementId)) {
		if (!m_items[elementId].isBranch) {
			metaInfoPtr.SetPtr(new idoc::CStandardDocumentMetaInfo());
			metaInfoPtr->SetMetaInfo(IGqlObjectCollectionDelegate::MIT_VERSION, m_items[elementId].version);
		}
	}

	//if (m_gqlClientCompPtr.IsValid()){
	//	QByteArray typeId = GetObjectTypeId(elementId);
	//	gmgaws::IGqlObjectCollectionDelegate* delegatePtr = GetDelegateForType(typeId);
	//	if (delegatePtr != nullptr){
	//		GetElementInfoResponse response(*const_cast<CGqlObjectCollectionComp*>(this), typeId);
	//		istd::TDelPtr<imtgql::IGqlRequest> requestPtr(delegatePtr->CreateElementInfoRequest(elementId));
	//		if (requestPtr.IsValid()){
	//			if (m_gqlClientCompPtr->SendRequest(*requestPtr, response)){
	//				IGqlObjectCollectionDelegate::ElementInfo info = response.GetElementInfo();
	//				if (info.dataMetaInfoPtr.IsValid()){
	//					idoc::MetaInfoPtr metaInfo;
	//					metaInfo.SetCastedOrRemove(info.elementMetaInfoPtr->CloneMe());

	//					return metaInfo;
	//				}
	//			}
	//		}
	//	}
	//}

	return metaInfoPtr;
}


bool CGqlObjectCollectionComp::SetElementName(const Id& elementId, const QString& name)
{
	//if (m_gqlClientCompPtr.IsValid()){
	//	QByteArray typeId = GetObjectTypeId(elementId);
	//	gmgaws::IGqlObjectCollectionDelegate* delegatePtr = GetDelegateForType(typeId);
	//	if (delegatePtr != nullptr){
	//		IGqlObjectCollectionDelegate::ElementInfoResponsePtr responsePtr = delegatePtr->CreateElementInfoResponsePtr();
	//		istd::TDelPtr<imtgql::IGqlRequest> infoRequestPtr(delegatePtr->CreateElementInfoRequest(elementId));
	//		if (m_gqlClientCompPtr->SendRequest(*infoRequestPtr, *responsePtr)){
	//			int version = responsePtr->GetElementInfo().version;

	//			IGqlObjectCollectionDelegate::ElementDescription elementDescription;
	//			elementDescription.name = name;
	//			elementDescription.description = responsePtr->GetElementInfo().description;

	//			istd::TDelPtr<imtgql::IGqlRequest> requestPtr(delegatePtr->CreateUpdateElementDescriptionRequest(elementId, elementDescription, version));
	//			if (requestPtr.IsValid()){
	//				SetElementNameResponse response(*const_cast<CGqlObjectCollectionComp*>(this), typeId);
	//				if (m_gqlClientCompPtr->SendRequest(*requestPtr, response)){
	//					return response.GetStatus();
	//				}
	//			}
	//		}
	//	}
	//}

	return false;
}


bool CGqlObjectCollectionComp::SetElementDescription(const Id& elementId, const QString& description)
{
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
			const QString& sourceFilePath,
			const ICollectionInfo::Id& parentId) const
{
	return QByteArray();

	if (&collection != this){
		return QByteArray();
	}

	IGqlObjectCollectionDelegate* delegatePtr = GetDelegateForType(typeId);
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

						IGqlObjectCollectionDelegate::UploadUrlResponsePtr uploadUrlDocumentResponsePtr = delegatePtr->CreateUploadUrlResponsePtr();
						istd::TDelPtr<imtgql::IGqlRequest> getDocumentUploadUrlRequestPtr(delegatePtr->CreateUploadUrlRequest(objectName));
						if (m_gqlClientCompPtr->SendRequest(*getDocumentUploadUrlRequestPtr, *uploadUrlDocumentResponsePtr)){
							QString uploadUrl = uploadUrlDocumentResponsePtr->GetUploadUrl();
							if (!uploadUrl.isEmpty()){
								QByteArray documentId;

								QNetworkRequest request;
								request.setUrl(QUrl(uploadUrl));
								QNetworkReply* replyPtr = imtcom::CRequestSender::DoSyncPut(request, data, 30000);
								if (replyPtr != nullptr){
									if (!replyPtr->error()){
										idoc::MetaInfoPtr metaInfoPtr(new idoc::CStandardDocumentMetaInfo());

										istd::TDelPtr<imtgql::IGqlRequest> uploadDocumentRequestPtr(delegatePtr->CreateInsertObjectRequest(fileName, "", *objectPtr, metaInfoPtr.GetPtr(), nullptr, uploadUrl));

										IGqlObjectCollectionDelegate::InsertObjectResponsePtr saveDocumentResponsePtr = delegatePtr->CreateInsertObjectResponsePtr();
										if (m_gqlClientCompPtr->SendRequest(*uploadDocumentRequestPtr, *saveDocumentResponsePtr)){
											documentId = saveDocumentResponsePtr->GetObjectId();
											if (!documentId.isEmpty()){
												//istd::IChangeable::ChangeSet changeSet(istd::IChangeable::CF_ANY);
												//imtbase::ICollectionInfo::ElementInsertInfo insertInfo;
												//insertInfo.elementId = documentId;
												//changeSet.SetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENT_INSERTED, QVariant::fromValue(insertInfo));
												//istd::CChangeNotifier notifier(this, &changeSet);
											}
											else{
												SendErrorMessage(0, saveDocumentResponsePtr->GetMessage(0), "Document Cloud Controller");
											}
										}
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

void CGqlObjectCollectionComp::OnResponseReceived(const QByteArray& subscriptionId, const QByteArray& subscriptionData)
{
	istd::IChangeable::ChangeSet changeSet(istd::IChangeable::CF_ANY);
	imtbase::ICollectionInfo::ElementInsertInfo insertInfo;
	changeSet.SetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENT_INSERTED, QVariant::fromValue(insertInfo));
	istd::CChangeNotifier notifier(this, &changeSet);
}


void CGqlObjectCollectionComp::OnSubscriptionStatusChanged(const QByteArray& subscriptionId, const SubscriptionStatus& status, const QString& message)
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

	// Create map of the GraphQL delegates:
	for (int i = 0; i < m_gqlDatabaseDelegatesCompPtr.GetCount(); ++i){
		IGqlObjectCollectionDelegate* delegatePtr = m_gqlDatabaseDelegatesCompPtr[i];
		if (delegatePtr != nullptr){
			QByteArray typeId = delegatePtr->GetSupportedObjectTypeId();
			Q_ASSERT(!typeId.isEmpty());
			if (!typeId.isEmpty()){
				m_delegatesMap[typeId] = delegatePtr;
			}
		}
	}

	//connect(this, &CGqlObjectCollectionComp::EmitSyncronizeDocumentListWithServer, this, &CGqlObjectCollectionComp::SyncronizeDocumentListWithServer, Qt::QueuedConnection);

	m_gqlClientCompPtr.EnsureInitialized();

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
	m_delegatesMap.clear();

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


IGqlObjectCollectionDelegate* CGqlObjectCollectionComp::GetDelegateForType(const QByteArray& typeId) const
{
	if (m_delegatesMap.contains(typeId)){
		return m_delegatesMap[typeId];
	}

	return nullptr;
}


imtbase::IObjectCollection::DataPtr CGqlObjectCollectionComp::GetDocument(
			const QByteArray& typeId,
			const QByteArray& documentId) const
{
	IGqlObjectCollectionDelegate* delegatePtr = GetDelegateForType(typeId);
	if (delegatePtr == nullptr){
		return nullptr;
	}

	QByteArray localDocumentId = documentId;

	imtbase::IObjectCollection::DataPtr documentPtr;

	imtcom::IFileTransfer* fileTransferPtr = delegatePtr->GetFileTransfer();
	if (fileTransferPtr != nullptr){
		istd::TDelPtr<imtgql::IGqlRequest> queryPtr = delegatePtr->CreateDownloadUrlRequest(documentId);
		if (queryPtr == nullptr){
			return nullptr;
		}

		IGqlObjectCollectionDelegate::DownloadUrlResponsePtr responsePtr = delegatePtr->CreateDownloadUrlResponsePtr();
		if (m_gqlClientCompPtr->SendRequest(*queryPtr, *responsePtr)){
			QString downloadUrl = responsePtr->GetDownloadUrl();
			if (!downloadUrl.isEmpty()){
				QNetworkRequest request;
				request.setUrl(QUrl(downloadUrl));
				QNetworkReply* replyPtr = imtcom::CRequestSender::DoSyncGet(request, 30000);
				if (!replyPtr->error()){
					imtbase::CTempDir tempDir;
					QString workingPath = tempDir.Path();
					QString docExt = GetDocumentExtension(typeId);
					QString docName = GetElementInfo(localDocumentId, EIT_NAME).toString();
					QString docPath = workingPath + "/" + docName + QString(".") + docExt;
					QFile docFile(docPath);
					if (docFile.open(QIODevice::WriteOnly)){
						docFile.write(replyPtr->readAll());
						docFile.close();

						QByteArray typeId = GetObjectTypeId(localDocumentId);
						const ifile::IFilePersistence* persistencePtr = GetPersistenceForObjectType(typeId);
						if (persistencePtr != nullptr){
							documentPtr = CreateObjectInstance(typeId);
							if (documentPtr.IsValid()){
								if (persistencePtr->LoadFromFile(*documentPtr, docPath) != ifile::IFilePersistence::OS_OK){
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

	return documentPtr;
}


} // namespace imtgql


