#include <imtclientgql/CGqlStructureDelegateCompBase.h>


namespace imtclientgql
{


// public methods

// reimplemented (IGqlStructureDelegate)

imtbase::ICollectionStructureInfo::Id CGqlStructureDelegateCompBase::InsertNewNode(
			const QString& name,
			const QString& description,
			const Id& proposedNodeId,
			const Id& parentNodeId,
			const idoc::IDocumentMetaInfo* metaInfoPtr,
			const imtbase::IOperationContext* operationContextPtr)
{
	if (m_clientCompPtr.IsValid()){
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr = CreateInsertNewNodeRequest(
					name,
					description,
					proposedNodeId,
					parentNodeId,
					metaInfoPtr,
					operationContextPtr);
		if (requestPtr.IsValid()){
			istd::TDelPtr<imtgql::IGqlPrimitiveTypeResponse> responsePtr;
			responsePtr.SetCastedOrRemove(CreateResponse(*requestPtr));
			if (responsePtr.IsValid()){
				if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
					QVariant variant;
					if (responsePtr->IsSuccessfull() && responsePtr->GetValue(variant)){
						if (variant.type() == QVariant::ByteArray){
							return variant.toByteArray();
						}
					}
				}
			}
		}
	}

	return QByteArray();
}


bool CGqlStructureDelegateCompBase::SetNodeName(
			const Id& nodeId,
			const QString& name,
			const imtbase::IOperationContext* operationContextPtr)
{
	if (m_clientCompPtr.IsValid()){
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr = CreateSetNodeNameRequest(
					nodeId,
					name,
					operationContextPtr);
		if (requestPtr.IsValid()){
			istd::TDelPtr<imtgql::IGqlPrimitiveTypeResponse> responsePtr;
			responsePtr.SetCastedOrRemove(CreateResponse(*requestPtr));
			if (responsePtr.IsValid()){
				if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
					QVariant variant;
					if (responsePtr->IsSuccessfull() && responsePtr->GetValue(variant)){
						if (variant.type() == QVariant::Bool){
							return variant.toBool();
						}
					}
				}
			}
		}
	}

	return false;
}


bool CGqlStructureDelegateCompBase::SetNodeDescription(const Id& nodeId, const QString& description, const imtbase::IOperationContext* operationContextPtr)
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
						if (variant.type() == QVariant::Bool){
							return variant.toBool();
						}
					}
				}
			}
		}
	}

	return false;
}


bool CGqlStructureDelegateCompBase::SetNodeMetaInfo(
			const Id& nodeId,
			const idoc::IDocumentMetaInfo& metaInfo,
			const imtbase::IOperationContext* operationContextPtr)
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
						if (variant.type() == QVariant::Bool){
							return variant.toBool();
						}
					}
				}
			}
		}
	}

	return false;
}


bool CGqlStructureDelegateCompBase::MoveNode(
			const Id& nodeId,
			const Id& parentNodeId,
			const imtbase::IOperationContext* operationContextPtr)
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
						if (variant.type() == QVariant::Bool){
							return variant.toBool();
						}
					}
				}
			}
		}
	}

	return false;
}


bool CGqlStructureDelegateCompBase::RemoveNode(const Id& nodeId, const imtbase::IOperationContext* operationContextPtr)
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
						if (variant.type() == QVariant::Bool){
							return variant.toBool();
						}
					}
				}
			}
		}
	}

	return false;
}


bool CGqlStructureDelegateCompBase::AssignObject(
			const Id& objectId,
			const Id& nodeId,
			const imtbase::IOperationContext* operationContextPtr)
{
	if (m_clientCompPtr.IsValid()){
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr = CreateInsertNewObjectRequest(
					objectId,
					nodeId,
					operationContextPtr);
		if (requestPtr.IsValid()){
			istd::TDelPtr<imtgql::IGqlPrimitiveTypeResponse> responsePtr;
			responsePtr.SetCastedOrRemove(CreateResponse(*requestPtr));
			if (responsePtr.IsValid()){
				if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
					QVariant variant;
					if (responsePtr->IsSuccessfull() && responsePtr->GetValue(variant)){
						if (variant.type() == QVariant::Bool){
							return variant.toBool();
						}
					}
				}
			}
		}
	}

	return false;
}


bool CGqlStructureDelegateCompBase::MoveObject(
			const Id& objectId,
			const Id& sourceNodeId,
			const Id& targetNodeId,
			const imtbase::IOperationContext* operationContextPtr)
{
	if (m_clientCompPtr.IsValid()){
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr = CreateMoveObjectRequest(
					objectId,
					sourceNodeId,
					targetNodeId,
					operationContextPtr);
		if (requestPtr.IsValid()){
			istd::TDelPtr<imtgql::IGqlPrimitiveTypeResponse> responsePtr;
			responsePtr.SetCastedOrRemove(CreateResponse(*requestPtr));
			if (responsePtr.IsValid()){
				if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
					QVariant variant;
					if (responsePtr->IsSuccessfull() && responsePtr->GetValue(variant)){
						if (variant.type() == QVariant::Bool){
							return variant.toBool();
						}
					}
				}
			}
		}
	}

	return false;
}


bool CGqlStructureDelegateCompBase::RemoveObject(
			const Id& objectId,
			const Id& nodeId,
			const imtbase::IOperationContext* operationContextPtr)
{
	if (m_clientCompPtr.IsValid()){
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr = CreateRemoveObjectRequest(
					objectId,
					nodeId,
					operationContextPtr);
		if (requestPtr.IsValid()){
			istd::TDelPtr<imtgql::IGqlPrimitiveTypeResponse> responsePtr;
			responsePtr.SetCastedOrRemove(CreateResponse(*requestPtr));
			if (responsePtr.IsValid()){
				if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
					QVariant variant;
					if (responsePtr->IsSuccessfull() && responsePtr->GetValue(variant)){
						if (variant.type() == QVariant::Bool){
							return variant.toBool();
						}
					}
				}
			}
		}
	}

	return false;
}


int CGqlStructureDelegateCompBase::GetNodeCount(const iprm::IParamsSet* selectionParamsPtr) const
{
	if (m_clientCompPtr.IsValid()){
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr = CreateGetNodeCountRequest(selectionParamsPtr);
		if (requestPtr.IsValid()){
			istd::TDelPtr<imtgql::IGqlPrimitiveTypeResponse> responsePtr;
			responsePtr.SetCastedOrRemove(CreateResponse(*requestPtr));
			if (responsePtr.IsValid()){
				if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
					QVariant variant;
					if (responsePtr->IsSuccessfull() && responsePtr->GetValue(variant)){
						if (variant.type() == QVariant::Int){
							return variant.toInt();
						}
					}
				}
			}
		}
	}

	return 0;
}


imtbase::ICollectionStructureInfo::Ids CGqlStructureDelegateCompBase::GetNodeIds(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr) const
{
	if (m_clientCompPtr.IsValid()){
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr = CreateGetNodeIdsRequest(
					offset,
					count,
					selectionParamsPtr);
		if (requestPtr.IsValid()){
            istd::TDelPtr<imtclientgql::IGqlStructureResponse> responsePtr;
			responsePtr.SetCastedOrRemove(CreateResponse(*requestPtr));
			if (responsePtr.IsValid()){
				if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
					QVariant variant;
					if (responsePtr->IsSuccessfull()){
						return responsePtr->GetNodeIds();
					}
				}
			}
		}
	}

	return Ids();
}


imtbase::ICollectionStructureIterator* CGqlStructureDelegateCompBase::CreateCollectionStructureIterator(
			int /*offset*/,
			int /*count*/,
			const iprm::IParamsSet* /*selectionParamsPtr*/) const
{
	return nullptr;
}


imtbase::ICollectionStructureInfo::NodeInfo CGqlStructureDelegateCompBase::GetNodeInfo(const Id& nodeId)
{
	if (m_clientCompPtr.IsValid()){
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr = CreateGetNodeInfoRequest(nodeId);
		if (requestPtr.IsValid()){
            istd::TDelPtr<imtclientgql::IGqlStructureResponse> responsePtr;
			responsePtr.SetCastedOrRemove(CreateResponse(*requestPtr));
			if (responsePtr.IsValid()){
				if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
					QVariant variant;
					if (responsePtr->IsSuccessfull()){
						return responsePtr->GetNodeInfo();
					}
				}
			}
		}
	}

	return NodeInfo();
}


imtbase::ICollectionStructureInfo::Ids CGqlStructureDelegateCompBase::GetObjectParentNodeIds(const Id& objectId) const
{
	if (m_clientCompPtr.IsValid()){
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr = CreateGetObjectParentNodeIdsRequest(objectId);
		if (requestPtr.IsValid()){
            istd::TDelPtr<imtclientgql::IGqlStructureResponse> responsePtr;
			responsePtr.SetCastedOrRemove(CreateResponse(*requestPtr));
			if (responsePtr.IsValid()){
				if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
					QVariant variant;
					if (responsePtr->IsSuccessfull()){
						return responsePtr->GetNodeIds();
					}
				}
			}
		}
	}

	return Ids();
}


imtbase::ICollectionStructureController* CGqlStructureDelegateCompBase::GetHierarchicalStructureController()
{
	return nullptr;
}


imtbase::ICollectionStructure::NodePath CGqlStructureDelegateCompBase::GetNodePath(const Id& nodeId) const
{
	return imtbase::ICollectionStructure::NodePath();
}


} // namespace imtclientgql


