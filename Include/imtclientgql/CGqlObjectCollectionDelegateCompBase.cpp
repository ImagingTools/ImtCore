#include <imtclientgql/CGqlObjectCollectionDelegateCompBase.h>


namespace imtclientgql
{


// public methods

// reimplemented (IGqlObjectCollectionDelegate)

QByteArrayList CGqlObjectCollectionDelegateCompBase::GetSupportedObjectTypeIds() const
{
	QByteArrayList ids;
	if (m_objectTypeIdsAttrPtr.IsValid()) {
		for (int i = 0; i < m_objectTypeIdsAttrPtr.GetCount(); i++){
			QByteArray id = m_objectTypeIdsAttrPtr[i];
			if (!id.isEmpty()) {
				if (!ids.contains(id)) {
					ids.append(id);
				}
			}
		}
	}

	return ids;
}


imtbase::IStructuredObjectCollectionInfo::ElementType CGqlObjectCollectionDelegateCompBase::GetElementType(const QByteArray& elementId) const
{
	if (m_clientCompPtr.IsValid()){
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr(CreateGetElementType(elementId));
		if (requestPtr.IsValid()){
			istd::TDelPtr<imtgql::IGqlStructuredCollectionResponse> responsePtr;
			responsePtr.SetCastedOrRemove(CreateResponse(*requestPtr));
			Q_ASSERT(responsePtr.IsValid());
			if (requestPtr.IsValid()){
				if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
					if (responsePtr->IsSuccessfull()){
						imtgql::IGqlStructuredCollectionResponse::ElementInfo info;
						if (responsePtr->GetElementInfo(info)){
							return info.isNode ? imtbase::IStructuredObjectCollectionInfo::ET_NODE : imtbase::IStructuredObjectCollectionInfo::ET_OBJECT;
						}

						Q_ASSERT(false);
					}
				}
			}
		}
		else{
			imtgql::IGqlStructuredCollectionResponse::NodeInfo nodeInfo;
			imtgql::IGqlStructuredCollectionResponse::ObjectInfo objectInfo;

			if (GetNodeInfo(elementId, nodeInfo)){
				return imtbase::IStructuredObjectCollectionInfo::ET_NODE;
			}
			else if (GetObjectInfo(elementId, objectInfo)){
				return imtbase::IStructuredObjectCollectionInfo::ET_OBJECT;
			}
		}
	}

	return imtbase::IStructuredObjectCollectionInfo::ET_UNKNOWN;
}


bool CGqlObjectCollectionDelegateCompBase::GetNodeInfo(const QByteArray& nodeId, imtgql::IGqlStructuredCollectionResponse::NodeInfo& outInfo) const
{
	if (m_clientCompPtr.IsValid()){
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr(CreateGetNodeInfoRequest(nodeId));
		if (!requestPtr.IsValid()){
			return false;
		}
		istd::TDelPtr<imtgql::IGqlStructuredCollectionResponse> responsePtr;
		responsePtr.SetCastedOrRemove(CreateResponse(*requestPtr));
		if (!responsePtr.IsValid()){
			return false;
		}
		if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
			if (responsePtr->IsSuccessfull()){
				imtgql::IGqlStructuredCollectionResponse::NodeInfo info;
				if (responsePtr->GetNodeInfo(info)){
					outInfo = info;

					return true;
				}
			}
		}
	}

	return false;
}


bool CGqlObjectCollectionDelegateCompBase::GetObjectInfo(const QByteArray& objectId, imtgql::IGqlStructuredCollectionResponse::ObjectInfo& outInfo) const
{
	if (m_clientCompPtr.IsValid()){
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr(CreateGetObjectInfoRequest(objectId));
		istd::TDelPtr<imtgql::IGqlStructuredCollectionResponse> responsePtr;
		responsePtr.SetCastedOrRemove(CreateResponse(*requestPtr));
		Q_ASSERT(responsePtr.IsValid());
		if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
			if (responsePtr->IsSuccessfull()){
				imtgql::IGqlStructuredCollectionResponse::ObjectInfo info;
				if (responsePtr->GetObjectInfo(info)){
					outInfo = info;

					return true;
				}
			}
		}
	}

	return false;
}


QByteArray CGqlObjectCollectionDelegateCompBase::InsertNode(
			const QString& name,
			const QString& description,
			const QByteArray& proposedNodeId,
			const QByteArray& parentNodeId,
			const idoc::IDocumentMetaInfo* elementMetaInfoPtr,
			const imtbase::IOperationContext* operationContextPtr) const
{
	if (m_clientCompPtr.IsValid()){
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr = CreateInsertNodeRequest(
					name,
					description,
					proposedNodeId,
					parentNodeId,
					elementMetaInfoPtr,
					operationContextPtr);
		if (requestPtr.IsValid()){
			istd::TDelPtr<imtgql::IGqlPrimitiveTypeResponse> responsePtr;
			responsePtr.SetCastedOrRemove(CreateResponse(*requestPtr));
			if (requestPtr.IsValid() && responsePtr.IsValid()){
				if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
					QVariant variant;
					if (responsePtr->IsSuccessfull() && responsePtr->GetValue(variant)){
						#if QT_VERSION >= 0x060000
							if (variant.typeId() == QMetaType::QByteArray){
						#else
							if (variant.type() == QMetaType::QByteArray){
						#endif
							return variant.toByteArray();
						}
					}
				}
			}
		}
	}

	return QByteArray();
}

QSharedPointer<imtbase::IStructuredObjectCollectionInfo> CGqlObjectCollectionDelegateCompBase::GetNodeContent(const QByteArray& nodeId) const
{
	CGqlObjectCollectionInfo* infoPtr = new CGqlObjectCollectionInfo();

	infoPtr->SetGqlClient(m_clientCompPtr.GetPtr());
	infoPtr->SetGqlDelegate(this);
	infoPtr->SetNodeId(nodeId);

	return QSharedPointer<imtbase::IStructuredObjectCollectionInfo>(infoPtr);
}


bool CGqlObjectCollectionDelegateCompBase::SetNodeName(const QByteArray& nodeId, const QString& name, const imtbase::IOperationContext* operationContextPtr) const
{
	if (m_clientCompPtr.IsValid()){
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr = CreateSetNodeNameRequest(
					nodeId,
					name,
					operationContextPtr);
		if (requestPtr.IsValid()){
			istd::TDelPtr<imtgql::IGqlPrimitiveTypeResponse> responsePtr;
			responsePtr.SetCastedOrRemove(CreateResponse(*requestPtr));
			if (requestPtr.IsValid() && responsePtr.IsValid()){
				if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
					QVariant variant;
					if (responsePtr->IsSuccessfull() && responsePtr->GetValue(variant)){
					#if QT_VERSION >= 0x060000
						if (variant.typeId() == QMetaType::Bool){
					#else
						if (variant.type() == QMetaType::Bool){
					#endif
							return variant.toBool();
						}
					}
				}
			}
		}
	}

	return false;
}


bool CGqlObjectCollectionDelegateCompBase::SetNodeDescription(const QByteArray& nodeId, const QString& description, const imtbase::IOperationContext* operationContextPtr) const
{
	if (m_clientCompPtr.IsValid()){
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr = CreateSetNodeDescriptionRequest(
					nodeId,
					description,
					operationContextPtr);
		if (requestPtr.IsValid()){
			istd::TDelPtr<imtgql::IGqlPrimitiveTypeResponse> responsePtr;
			responsePtr.SetCastedOrRemove(CreateResponse(*requestPtr));
			if (responsePtr.IsValid()){
				if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
					QVariant variant;
					if (responsePtr->IsSuccessfull() && responsePtr->GetValue(variant)){
					#if QT_VERSION >= 0x060000
						if (variant.typeId() == QMetaType::Bool){
					#else
						if (variant.type() == QMetaType::Bool){
					#endif
							return variant.toBool();
						}
					}
				}
			}
		}
	}

	return false;
}


bool CGqlObjectCollectionDelegateCompBase::SetNodeMetaInfo(const QByteArray& nodeId, const idoc::IDocumentMetaInfo& metaInfo, const imtbase::IOperationContext* operationContextPtr) const
{
	if (m_clientCompPtr.IsValid()){
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr = CreateSetNodeMetaInfoRequest(
					nodeId,
					metaInfo,
					operationContextPtr);
		if (requestPtr.IsValid()){
			istd::TDelPtr<imtgql::IGqlPrimitiveTypeResponse> responsePtr;
			responsePtr.SetCastedOrRemove(CreateResponse(*requestPtr));
			if (responsePtr.IsValid()){
				if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
					QVariant variant;
					if (responsePtr->IsSuccessfull() && responsePtr->GetValue(variant)){
					#if QT_VERSION >= 0x060000
						if (variant.typeId() == QMetaType::Bool){
					#else
						if (variant.type() == QMetaType::Bool){
					#endif
							return variant.toBool();
						}
					}
				}
			}
		}
	}

	return false;
}


bool CGqlObjectCollectionDelegateCompBase::MoveNode(const QByteArray& nodeId, const QByteArray& parentNodeId, const imtbase::IOperationContext* operationContextPtr) const
{
	if (m_clientCompPtr.IsValid()){
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr = CreateMoveNodeRequest(
					nodeId,
					parentNodeId,
					operationContextPtr);
		if (requestPtr.IsValid()){
			istd::TDelPtr<imtgql::IGqlPrimitiveTypeResponse> responsePtr;
			responsePtr.SetCastedOrRemove(CreateResponse(*requestPtr));
			if (responsePtr.IsValid()){
				if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
					QVariant variant;
					if (responsePtr->IsSuccessfull() && responsePtr->GetValue(variant)){
					#if QT_VERSION >= 0x060000
						if (variant.typeId() == QMetaType::Bool){
					#else
						if (variant.type() == QMetaType::Bool){
					#endif
							return variant.toBool();
						}
					}
				}
			}
		}
	}

	return false;
}


bool CGqlObjectCollectionDelegateCompBase::RemoveNode(const QByteArray& nodeId, const imtbase::IOperationContext* operationContextPtr) const
{
	if (m_clientCompPtr.IsValid()){
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr = CreateRemoveNodeRequest(
					nodeId,
					operationContextPtr);
		if (requestPtr.IsValid()){
			istd::TDelPtr<imtgql::IGqlPrimitiveTypeResponse> responsePtr;
			responsePtr.SetCastedOrRemove(CreateResponse(*requestPtr));
			if (responsePtr.IsValid()){
				if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
					QVariant variant;
					if (responsePtr->IsSuccessfull() && responsePtr->GetValue(variant)){
					#if QT_VERSION >= 0x060000
						if (variant.typeId() == QMetaType::Bool){
					#else
						if (variant.type() == QMetaType::Bool){
					#endif
							return variant.toBool();
						}
					}
				}
			}
		}
	}

	return false;
}


bool CGqlObjectCollectionDelegateCompBase::AddObjectToNode(const QByteArray& objectId, const QByteArray& nodeId, const imtbase::IOperationContext* operationContextPtr)
{
	if (m_clientCompPtr.IsValid()){
		imtgql::IGqlStructuredCollectionResponse::ObjectInfo info;
		if (GetObjectInfo(objectId, info)){
			istd::TDelPtr<imtgql::IGqlRequest> requestPtr = CreateAddObjectToNodeRequest(
						objectId,
						nodeId,
						info.version,
						operationContextPtr);
			istd::TDelPtr<imtgql::IGqlPrimitiveTypeResponse> responsePtr;
			responsePtr.SetCastedOrRemove(CreateResponse(*requestPtr));
			if (responsePtr.IsValid()){
				if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
					QVariant variant;
					if (responsePtr->IsSuccessfull() && responsePtr->GetValue(variant)){
					#if QT_VERSION >= 0x060000
						if (variant.typeId() == QMetaType::Bool){
					#else
						if (variant.type() == QMetaType::Bool){
					#endif
							return variant.toBool();
						}
					}
				}
			}
		}
	}

	return false;
}


bool CGqlObjectCollectionDelegateCompBase::MoveObjectToNode(const QByteArray& objectId, const QByteArray& nodeId, const QByteArray& newNodeId, const imtbase::IOperationContext* operationContextPtr)
{
	if (m_clientCompPtr.IsValid()){
		imtgql::IGqlStructuredCollectionResponse::ObjectInfo info;
		if (GetObjectInfo(objectId, info)){
			istd::TDelPtr<imtgql::IGqlRequest> requestPtr = CreateMoveObjectToNodeRequest(
						objectId,
						nodeId,
						newNodeId,
						info.version,
						operationContextPtr);
			istd::TDelPtr<imtgql::IGqlPrimitiveTypeResponse> responsePtr;
			responsePtr.SetCastedOrRemove(CreateResponse(*requestPtr));
			if (responsePtr.IsValid()){
				if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
					QVariant variant;
					if (responsePtr->IsSuccessfull() && responsePtr->GetValue(variant)){
					#if QT_VERSION >= 0x060000
						if (variant.typeId() == QMetaType::Bool){
					#else
						if (variant.type() == QMetaType::Bool){
					#endif
							return variant.toBool();
						}
					}
				}
			}
		}
	}

	return false;
}


bool CGqlObjectCollectionDelegateCompBase::RemoveObjectFromNode(const QByteArray& objectId, const QByteArray& nodeId, const imtbase::IOperationContext* operationContextPtr) const
{
	if (m_clientCompPtr.IsValid()){
		imtgql::IGqlStructuredCollectionResponse::ObjectInfo info;
		if (GetObjectInfo(objectId, info)){
			istd::TDelPtr<imtgql::IGqlRequest> requestPtr = CreateRemoveObjectFromNodeRequest(
						objectId,
						nodeId,
						info.version,
						operationContextPtr);
			istd::TDelPtr<imtgql::IGqlPrimitiveTypeResponse> responsePtr;
			responsePtr.SetCastedOrRemove(CreateResponse(*requestPtr));
			if (responsePtr.IsValid()){
				if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
					QVariant variant;
					if (responsePtr->IsSuccessfull() && responsePtr->GetValue(variant)){
					#if QT_VERSION >= 0x060000
						if (variant.typeId() == QMetaType::Bool){
					#else
						if (variant.type() == QMetaType::Bool){
					#endif
							return variant.toBool();
						}
					}
				}
			}
		}
	}

	return false;
}


bool CGqlObjectCollectionDelegateCompBase::SetObjectName(const QByteArray& objectId, const QString& name, const imtbase::IOperationContext* operationContextPtr) const
{
	if (m_clientCompPtr.IsValid()){
		imtgql::IGqlStructuredCollectionResponse::ObjectInfo info;
		if (GetObjectInfo(objectId, info)){
			istd::TDelPtr<imtgql::IGqlRequest> requestPtr = CreateSetObjectNameRequest(objectId, name, info.version);
			istd::TDelPtr<imtgql::IGqlPrimitiveTypeResponse> responsePtr;
			responsePtr.SetCastedOrRemove(CreateResponse(*requestPtr));
			if (responsePtr.IsValid()){
				if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
					QVariant variant;
					if (responsePtr->IsSuccessfull() && responsePtr->GetValue(variant)){
					#if QT_VERSION >= 0x060000
						if (variant.typeId() == QMetaType::Bool){
					#else
						if (variant.type() == QMetaType::Bool){
					#endif
							return variant.toBool();
						}
					}
				}
			}
		}
	}

	return false;
}


bool CGqlObjectCollectionDelegateCompBase::SetObjectDescription(const QByteArray& objectId, const QString& description, const imtbase::IOperationContext* operationContextPtr) const
{
	if (m_clientCompPtr.IsValid()){
		imtgql::IGqlStructuredCollectionResponse::ObjectInfo info;
		if (GetObjectInfo(objectId, info)){
			istd::TDelPtr<imtgql::IGqlRequest> requestPtr = CreateSetObjectDescriptionRequest(objectId, description, info.version);
			istd::TDelPtr<imtgql::IGqlPrimitiveTypeResponse> responsePtr;
			responsePtr.SetCastedOrRemove(CreateResponse(*requestPtr));
			if (responsePtr.IsValid()){
				if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
					QVariant variant;
					if (responsePtr->IsSuccessfull() && responsePtr->GetValue(variant)){
					#if QT_VERSION >= 0x060000
						if (variant.typeId() == QMetaType::Bool){
					#else
						if (variant.type() == QMetaType::Bool){
					#endif
							return variant.toBool();
						}
					}
				}
			}
		}
	}

	return false;
}


bool CGqlObjectCollectionDelegateCompBase::SetObjectMetaInfo(const QByteArray& objectId, const idoc::IDocumentMetaInfo& dataMetaInfo, const imtbase::IOperationContext* operationContextPtr) const
{
	if (m_clientCompPtr.IsValid()){
		imtgql::IGqlStructuredCollectionResponse::ObjectInfo info;
		if (GetObjectInfo(objectId, info)){
			istd::TDelPtr<imtgql::IGqlRequest> requestPtr = CreateSetObjectMetaInfoRequest(
						objectId,
						dataMetaInfo,
						info.version,
						operationContextPtr);
			istd::TDelPtr<imtgql::IGqlPrimitiveTypeResponse> responsePtr;
			responsePtr.SetCastedOrRemove(CreateResponse(*requestPtr));
			if (responsePtr.IsValid()){
				if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
					QVariant variant;
					if (responsePtr->IsSuccessfull() && responsePtr->GetValue(variant)){
					#if QT_VERSION >= 0x060000
						if (variant.typeId() == QMetaType::Bool){
					#else
						if (variant.type() == QMetaType::Bool){
					#endif
							return variant.toBool();
						}
					}
				}
			}
		}
	}

	return false;
}


bool CGqlObjectCollectionDelegateCompBase::RemoveObject(const QByteArray& objectId, const imtbase::IOperationContext* operationContextPtr) const
{
	imtgql::IGqlStructuredCollectionResponse::ObjectInfo info;
	if (GetObjectInfo(objectId, info)){
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr = CreateRemoveObjectRequest(objectId, info.version, operationContextPtr);
		if (requestPtr.IsValid()){
			istd::TDelPtr<imtgql::IGqlPrimitiveTypeResponse> responsePtr;
			responsePtr.SetCastedOrRemove(CreateResponse(*requestPtr));
			if (responsePtr.IsValid()){
				if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
					QVariant variant;
					if (responsePtr->IsSuccessfull() && responsePtr->GetValue(variant)){
						return true;
					}
				}
			}
		}
	}

	return false;
}


int CGqlObjectCollectionDelegateCompBase::GetElementCount(const iprm::IParamsSet* selectionParamsPtr) const
{
	if (m_clientCompPtr.IsValid()){
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr = CreateGetElementCountRequest(selectionParamsPtr);
		istd::TDelPtr<imtgql::IGqlPrimitiveTypeResponse> responsePtr;
		responsePtr.SetCastedOrRemove(CreateResponse(*requestPtr));
		if (responsePtr.IsValid()){
			if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
				QVariant variant;
				if (responsePtr->IsSuccessfull() && responsePtr->GetValue(variant)){
				#if QT_VERSION >= 0x060000
					if (variant.typeId() == QMetaType::Bool || variant.typeId() == QMetaType::LongLong){
				#else
					if (variant.type() == QMetaType::Bool || variant.type() == QMetaType::LongLong){
				#endif
						return variant.toInt();
					}
				}
			}
		}
	}

	return -1;
}


imtgql::IGqlStructuredCollectionResponse::ElementList CGqlObjectCollectionDelegateCompBase::GetElementList(int offset, int count, const iprm::IParamsSet* selectionParamsPtr) const
{
	if (m_clientCompPtr.IsValid()){
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr = CreateGetElementListRequest(offset, count, selectionParamsPtr);
		istd::TDelPtr<imtgql::IGqlStructuredCollectionResponse> responsePtr;
		responsePtr.SetCastedOrRemove(CreateResponse(*requestPtr));
		if (responsePtr.IsValid()){
			if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
				QVariant variant;
				if (responsePtr->IsSuccessfull()){
					imtgql::IGqlStructuredCollectionResponse::ElementList list;
					if (responsePtr->GetElementList(list)){
						return list;
					}
				}
			}
		}
	}

	return imtgql::IGqlStructuredCollectionResponse::ElementList();
}


bool CGqlObjectCollectionDelegateCompBase::GetSubCollection(
			imtbase::IObjectCollection* subcollection,
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr) const
{
	return false;
}


// protected methods

imtgql::IGqlRequest* CGqlObjectCollectionDelegateCompBase::CreateGetElementType(const QByteArray& elementId) const
{
	return nullptr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateCompBase::CreateGetNodeInfoRequest(const QByteArray& nodeId) const
{
	return nullptr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateCompBase::CreateGetObjectInfoRequest(const QByteArray& objectId) const
{
	return nullptr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateCompBase::CreateInsertNodeRequest(const QString& name, const QString& description, const QByteArray& proposedNodeId, const QByteArray& parentNodeId, const idoc::IDocumentMetaInfo* metaInfoPtr, const imtbase::IOperationContext* operationContextPtr) const
{
	return nullptr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateCompBase::CreateSetNodeNameRequest(const QByteArray& nodeId, const QString& name, const imtbase::IOperationContext* operationContextPtr) const
{
	return nullptr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateCompBase::CreateSetNodeDescriptionRequest(const QByteArray& nodeId, const QString& description, const imtbase::IOperationContext* operationContextPtr) const
{
	return nullptr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateCompBase::CreateSetNodeMetaInfoRequest(const QByteArray& nodeId, const idoc::IDocumentMetaInfo& metaInfo, const imtbase::IOperationContext* operationContextPtr) const
{
	return nullptr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateCompBase::CreateMoveNodeRequest(const QByteArray& nodeId, const QByteArray& parentNodeId, const imtbase::IOperationContext* operationContextPtr) const
{
	return nullptr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateCompBase::CreateRemoveNodeRequest(const QByteArray& nodeId, const imtbase::IOperationContext* operationContextPtr) const
{
	return nullptr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateCompBase::CreateAddObjectToNodeRequest(const QByteArray& objectId, const QByteArray& nodeId, int clientVersion, const imtbase::IOperationContext* operationContextPtr)
{
	return nullptr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateCompBase::CreateMoveObjectToNodeRequest(const QByteArray& objectId, const QByteArray& nodeId, const QByteArray& newNodeId, int clientVersion, const imtbase::IOperationContext* operationContextPtr)
{
	return nullptr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateCompBase::CreateRemoveObjectFromNodeRequest(const QByteArray& objectId, const QByteArray& nodeId, int clientVersion, const imtbase::IOperationContext* operationContextPtr) const
{
	return nullptr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateCompBase::CreateInsertObjectRequest(const QByteArray& typeId, const QString& name, const QString& description, const istd::IChangeable* objectPtr, const QString& uploadUrl, const QByteArray& proposedObjectId, const QByteArray& nodeId, const idoc::IDocumentMetaInfo* dataMetaInfoPtr, const idoc::IDocumentMetaInfo* collectionItemMetaInfoPtr, const imtbase::IOperationContext* operationContextPtr) const
{
	return nullptr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateCompBase::CreateSetObjectNameRequest(const QByteArray& objectId, const QString& name, int clientVersion, const imtbase::IOperationContext* operationContextPtr) const
{
	return nullptr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateCompBase::CreateSetObjectDescriptionRequest(const QByteArray& objectId, const QString& description, int clientVersion, const imtbase::IOperationContext* operationContextPtr) const
{
	return nullptr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateCompBase::CreateSetObjectMetaInfoRequest(const QByteArray& objectId, const idoc::IDocumentMetaInfo& metaInfo, int clientVersion, const imtbase::IOperationContext* operationContextPtr) const
{
	return nullptr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateCompBase::CreateGetObjectRequest(const QByteArray& objectId) const
{
	return nullptr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateCompBase::CreateSetObjectRequest(const QByteArray& objectId, const istd::IChangeable* objectPtr, const QString& uploadUrl, const idoc::IDocumentMetaInfo* dataMetaInfoPtr, const idoc::IDocumentMetaInfo* collectionItemMetaInfoPtr, int clientVersion, const imtbase::IOperationContext* operationContextPtr) const
{
	return nullptr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateCompBase::CreateRemoveObjectRequest(const QByteArray& objectId, int clientElementVersion, const imtbase::IOperationContext* operationContextPtr) const
{
	return nullptr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateCompBase::CreateGetElementCountRequest(const iprm::IParamsSet* selectionParamsPtr) const
{
	return nullptr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateCompBase::CreateGetElementListRequest(int offset, int count, const iprm::IParamsSet* selectionParamsPtr) const
{
	return nullptr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateCompBase::CreateGetSubCollectionRequest(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr) const
{
	return nullptr;
}


} // namespace imtclientgql


