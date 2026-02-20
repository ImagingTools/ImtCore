// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtclientgql/CGqlHierarchicalStructureComp.h>


namespace imtclientgql
{


// public methods

// reimplemented (IHierarchicalStructure)

imtbase::IHierarchicalStructureInfo::Id CGqlHierarchicalStructureComp::InsertNewNode(
			const QString& name,
			const QString& description,
			const Id& proposedNodeId,
			const Id& parentNodeId,
			const idoc::IDocumentMetaInfo* metaInfoPtr,
			const imtbase::IOperationContext* operationContextPtr)
{
	Id retVal;

	if (!m_clientCompPtr.IsValid() || !m_delegateCompPtr.IsValid()){
		return retVal;
	}

	IGqlClient::GqlRequestPtr requestPtr(m_delegateCompPtr->CreateInsertNewNodeRequest(
				name,
				description,
				proposedNodeId,
				parentNodeId,
				metaInfoPtr,
				operationContextPtr));

	if (requestPtr.IsValid()){
		IGqlClient::GqlResponsePtr responsePtr = m_clientCompPtr->SendRequest(requestPtr);
		if (responsePtr.IsValid()){
			m_delegateCompPtr->GetNodeId(*responsePtr, retVal);
		}
	}

	return retVal;
}


bool CGqlHierarchicalStructureComp::SetNodeName(
			const Id& nodeId,
			const QString& name,
			const imtbase::IOperationContext* operationContextPtr)
{
	if (!m_clientCompPtr.IsValid() || !m_delegateCompPtr.IsValid()){
		return false;
	}

	IGqlClient::GqlRequestPtr requestPtr(m_delegateCompPtr->CreateSetNodeNameRequest(
				nodeId,
				name,
				operationContextPtr));

	if (requestPtr.IsValid()){
		IGqlClient::GqlResponsePtr responsePtr = m_clientCompPtr->SendRequest(requestPtr);
		if (responsePtr.IsValid()){
			bool retVal = false;
			if (m_delegateCompPtr->GetOperationResult(*responsePtr, retVal)){
				return retVal;
			}
		}
	}

	return false;
}


bool CGqlHierarchicalStructureComp::SetNodeDescription(
			const Id& nodeId,
			const QString& description,
			const imtbase::IOperationContext* operationContextPtr)
{
	if (!m_clientCompPtr.IsValid() || !m_delegateCompPtr.IsValid()){
		return false;
	}

	IGqlClient::GqlRequestPtr requestPtr(m_delegateCompPtr->CreateSetNodeDescriptionRequest(
				nodeId,
				description,
				operationContextPtr));

	if (requestPtr.IsValid()){
		IGqlClient::GqlResponsePtr responsePtr = m_clientCompPtr->SendRequest(requestPtr);
		if (responsePtr.IsValid()){
			bool retVal = false;
			if (m_delegateCompPtr->GetOperationResult(*responsePtr, retVal)){
				return retVal;
			}
		}
	}

	return false;
}


bool CGqlHierarchicalStructureComp::SetNodeMetaInfo(
			const Id& nodeId,
			const idoc::IDocumentMetaInfo& metaInfo,
			const imtbase::IOperationContext* operationContextPtr)
{
	if (!m_clientCompPtr.IsValid() || !m_delegateCompPtr.IsValid()){
		return false;
	}

	IGqlClient::GqlRequestPtr requestPtr(m_delegateCompPtr->CreateSetNodeMetaInfoRequest(
				nodeId,
				metaInfo,
				operationContextPtr));

	if (requestPtr.IsValid()){
		IGqlClient::GqlResponsePtr responsePtr = m_clientCompPtr->SendRequest(requestPtr);
		if (responsePtr.IsValid()){
			bool retVal = false;
			if (m_delegateCompPtr->GetOperationResult(*responsePtr, retVal)){
				return retVal;
			}
		}
	}

	return false;
}


bool CGqlHierarchicalStructureComp::MoveNode(
			const Id& nodeId,
			const Id& parentNodeId,
			const imtbase::IOperationContext* operationContextPtr)
{
	if (!m_clientCompPtr.IsValid() || !m_delegateCompPtr.IsValid()){
		return false;
	}

	IGqlClient::GqlRequestPtr requestPtr(m_delegateCompPtr->CreateMoveNodeRequest(
				nodeId,
				parentNodeId,
				operationContextPtr));

	if (requestPtr.IsValid()){
		IGqlClient::GqlResponsePtr responsePtr = m_clientCompPtr->SendRequest(requestPtr);
		if (responsePtr.IsValid()){
			bool retVal = false;
			if (m_delegateCompPtr->GetOperationResult(*responsePtr, retVal)){
				return retVal;
			}
		}
	}

	return false;
}


bool CGqlHierarchicalStructureComp::RemoveNode(
			const Id& nodeId,
			const imtbase::IOperationContext* operationContextPtr)
{
	if (!m_clientCompPtr.IsValid() || !m_delegateCompPtr.IsValid()){
		return false;
	}

	IGqlClient::GqlRequestPtr requestPtr(m_delegateCompPtr->CreateRemoveNodeRequest(
				nodeId,
				operationContextPtr));

	if (requestPtr.IsValid()){
		IGqlClient::GqlResponsePtr responsePtr = m_clientCompPtr->SendRequest(requestPtr);
		if (responsePtr.IsValid()){
			bool retVal = false;
			if (m_delegateCompPtr->GetOperationResult(*responsePtr, retVal)){
				return retVal;
			}
		}
	}

	return false;
}


bool CGqlHierarchicalStructureComp::InsertLeaf(
			const Id& leafId,
			const Id& nodeId,
			const QString& leafName,
			const imtbase::IOperationContext* operationContextPtr)
{
	if (!m_clientCompPtr.IsValid() || !m_delegateCompPtr.IsValid()){
		return false;
	}

	IGqlClient::GqlRequestPtr requestPtr(m_delegateCompPtr->CreateInsertLeafRequest(
				leafId,
				nodeId,
				leafName,
				operationContextPtr));

	if (requestPtr.IsValid()){
		IGqlClient::GqlResponsePtr responsePtr = m_clientCompPtr->SendRequest(requestPtr);
		if (responsePtr.IsValid()){
			bool retVal = false;
			if (m_delegateCompPtr->GetOperationResult(*responsePtr, retVal)){
				return retVal;
			}
		}
	}

	return false;
}


bool CGqlHierarchicalStructureComp::MoveLeaf(
			const Id& leafId,
			const Id& sourceNodeId,
			const Id& targetNodeId,
			const imtbase::IOperationContext* operationContextPtr)
{
	if (!m_clientCompPtr.IsValid() || !m_delegateCompPtr.IsValid()){
		return false;
	}

	IGqlClient::GqlRequestPtr requestPtr(m_delegateCompPtr->CreateMoveLeafRequest(
				leafId,
				sourceNodeId,
				targetNodeId,
				operationContextPtr));

	if (requestPtr.IsValid()){
		IGqlClient::GqlResponsePtr responsePtr = m_clientCompPtr->SendRequest(requestPtr);
		if (responsePtr.IsValid()){
			bool retVal = false;
			if (m_delegateCompPtr->GetOperationResult(*responsePtr, retVal)){
				return retVal;
			}
		}
	}

	return false;
}


bool CGqlHierarchicalStructureComp::RemoveLeaf(
			const Id& leafId,
			const Id& nodeId,
			const imtbase::IOperationContext* operationContextPtr)
{
	if (!m_clientCompPtr.IsValid() || !m_delegateCompPtr.IsValid()){
		return false;
	}

	IGqlClient::GqlRequestPtr requestPtr(m_delegateCompPtr->CreateRemoveLeafRequest(
					leafId,
					nodeId,
					operationContextPtr));

	if (requestPtr.IsValid()){
		IGqlClient::GqlResponsePtr responsePtr = m_clientCompPtr->SendRequest(requestPtr);
		if (responsePtr.IsValid()){
			bool retVal = false;
			if (m_delegateCompPtr->GetOperationResult(*responsePtr, retVal)){
				return retVal;
			}
		}
	}

	return false;
}


// reimplemented (IHierarchicalStructureInfo)

int CGqlHierarchicalStructureComp::GetItemCount(const iprm::IParamsSet* selectionParamsPtr) const
{
	int retVal = -1;

	if (!m_clientCompPtr.IsValid() || !m_delegateCompPtr.IsValid()){
		return -1;
	}

	IGqlClient::GqlRequestPtr requestPtr(m_delegateCompPtr->CreateGetItemCountRequest(
					selectionParamsPtr));

	if (requestPtr.IsValid()){
		IGqlClient::GqlResponsePtr responsePtr = m_clientCompPtr->SendRequest(requestPtr);
		if (responsePtr.IsValid()){
			int count = 0;
			if (m_delegateCompPtr->GetItemCount(*responsePtr, count)){
				retVal = count;
			}
		}
	}

	return retVal;
}


imtbase::IHierarchicalStructureInfo::Ids CGqlHierarchicalStructureComp::GetItemIds(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr) const
{
	Ids retVal;

	if (!m_clientCompPtr.IsValid() || !m_delegateCompPtr.IsValid()){
		return Ids();
	}

	IGqlClient::GqlRequestPtr requestPtr(m_delegateCompPtr->CreateGetItemIdsRequest(
					offset,
					count,
					selectionParamsPtr));

	if (requestPtr.IsValid()){
		IGqlClient::GqlResponsePtr responsePtr = m_clientCompPtr->SendRequest(requestPtr);
		if (responsePtr.IsValid()){
			m_delegateCompPtr->GetItemIds(*responsePtr, retVal);
		}
	}

	return retVal;
}


imtbase::IHierarchicalStructureInfo::ItemInfoList CGqlHierarchicalStructureComp::GetItemInfos(const Ids& itemIds)
{
	ItemInfoList retVal;

	if (!m_clientCompPtr.IsValid() || !m_delegateCompPtr.IsValid()){
		return ItemInfoList();
	}

	IGqlClient::GqlRequestPtr requestPtr(m_delegateCompPtr->CreateGetItemInfosRequest(itemIds));

	if (requestPtr.IsValid()){
		IGqlClient::GqlResponsePtr responsePtr = m_clientCompPtr->SendRequest(requestPtr);
		if (responsePtr.IsValid()){
			m_delegateCompPtr->GetItemInfos(*responsePtr, retVal);
		}
	}

	return retVal;
}


imtbase::IHierarchicalStructureInfo::Ids CGqlHierarchicalStructureComp::GetItemPath(const Id& itemId) const
{
	Ids retVal;

	if (!m_clientCompPtr.IsValid() || !m_delegateCompPtr.IsValid()){
		return Ids();
	}

	IGqlClient::GqlRequestPtr requestPtr(m_delegateCompPtr->CreateGetItemPathRequest(itemId));
	if (requestPtr.IsValid()){
		IGqlClient::GqlResponsePtr responsePtr = m_clientCompPtr->SendRequest(requestPtr);
		if (responsePtr.IsValid()){
			m_delegateCompPtr->GetItemPath(*responsePtr, retVal);
		}
	}

	return retVal;
}


imtbase::IHierarchicalStructureIterator* CGqlHierarchicalStructureComp::CreateHierarchicalStructureIterator(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr) const
{
	if (m_delegateCompPtr.IsValid()){
		return m_delegateCompPtr->CreateHierarchicalStructureIterator(offset, count, selectionParamsPtr);
	}

	return nullptr;
}


} // namespace imtclientgql


