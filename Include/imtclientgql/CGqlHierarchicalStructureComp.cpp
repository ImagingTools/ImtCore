#include <imtclientgql/CGqlHierarchicalStructureComp.h>


// Qt includes
#include <QtNetwork/QNetworkReply>

// ACF includes
#include <idoc/CStandardDocumentMetaInfo.h>
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtbase/CTempDir.h>
#include <imtgql/CGqlRequest.h>
#include <imtgql/IGqlPrimitiveTypeResponse.h>
#include <imtgql/IGqlHierarchicalStructureResponse.h>
#include <imtbase/CFilterCollectionProxy.h>


namespace imtclientgql
{


//bool operator==(
//			const imtgql::IGqlStructuredCollectionResponse::ElementInfo::PathItem& a,
//			const imtgql::IGqlStructuredCollectionResponse::ElementInfo::PathItem& b)
//{
//	bool retVal = a.id == b.id && a.name == b.name;
//
//	return retVal;
//}
//
//
//bool operator==(
//			const imtgql::IGqlHierarchicalStructureResponse::ElementInfo& a,
//			const imtgql::IGqlHierarchicalStructureResponse::ElementInfo& b)
//{
//	bool retVal =
//				a.id == b.id &&
//				a.name == b.name &&
//				a.description == b.description &&
//				a.path == b.path &&
//				a.typeId == b.typeId &&
//				a.version == b.version &&
//				a.elementMetaInfoPtr.isNull() == b.elementMetaInfoPtr.isNull() &&
//				a.elementMetaInfoPtr.isNull() == b.elementMetaInfoPtr.isNull() &&
//				a.isNode == b.isNode;
//
//	if (retVal && !a.elementMetaInfoPtr.isNull()){
//		retVal &= a.elementMetaInfoPtr->IsEqual(*b.elementMetaInfoPtr);
//	}
//
//	if (retVal && !a.dataMetaInfoPtr.isNull()){
//		retVal &= a.dataMetaInfoPtr->IsEqual(*b.dataMetaInfoPtr);
//	}
//
//	return retVal;
//}


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
	if (m_clientCompPtr.IsValid() && m_delegateCompPtr.IsValid()){
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr(m_delegateCompPtr->CreateInsertNewNodeRequest(
					name,
					description,
					proposedNodeId,
					parentNodeId,
					metaInfoPtr,
					operationContextPtr));
		if (requestPtr.IsValid()){
			istd::TDelPtr<imtgql::IGqlResponse> responsePtr(m_delegateCompPtr->CreateResponse(*requestPtr));
			Q_ASSERT(responsePtr.IsValid());
			if (responsePtr.IsValid()){
				if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
					if (responsePtr->IsSuccessful()){
						QVariant result = responsePtr->GetResult();
						Q_ASSERT(result.isValid());
						if (result.isValid()){
							if (result.canConvert(QMetaType(QMetaType::QByteArray))){
								return result.toByteArray();
							}
						}
					}
				}
			}
		}
	}

	return Id();
}


bool CGqlHierarchicalStructureComp::SetNodeName(
			const Id& nodeId,
			const QString& name,
			const imtbase::IOperationContext* operationContextPtr)
{
	if (m_clientCompPtr.IsValid() && m_delegateCompPtr.IsValid()){
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr(m_delegateCompPtr->CreateSetNodeNameRequest(
					nodeId,
					name,
					operationContextPtr));
		if (requestPtr.IsValid()){
			istd::TDelPtr<imtgql::IGqlResponse> responsePtr(m_delegateCompPtr->CreateResponse(*requestPtr));
			Q_ASSERT(responsePtr.IsValid());
			if (responsePtr.IsValid()){
				if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
					if (responsePtr->IsSuccessful()){
						QVariant result = responsePtr->GetResult();
						Q_ASSERT(result.isValid());
						if (result.isValid()){
							if (result.canConvert(QMetaType(QMetaType::Bool))){
								return result.toBool();
							}
						}
					}
				}
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
	if (m_clientCompPtr.IsValid() && m_delegateCompPtr.IsValid()){
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr(m_delegateCompPtr->CreateSetNodeDescriptionRequest(
					nodeId,
					description,
					operationContextPtr));
		if (requestPtr.IsValid()){
			istd::TDelPtr<imtgql::IGqlResponse> responsePtr(m_delegateCompPtr->CreateResponse(*requestPtr));
			Q_ASSERT(responsePtr.IsValid());
			if (responsePtr.IsValid()){
				if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
					if (responsePtr->IsSuccessful()){
						QVariant result = responsePtr->GetResult();
						Q_ASSERT(result.isValid());
						if (result.isValid()){
							if (result.canConvert(QMetaType(QMetaType::Bool))){
								return result.toBool();
							}
						}
					}
				}
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
	if (m_clientCompPtr.IsValid() && m_delegateCompPtr.IsValid()){
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr(m_delegateCompPtr->CreateSetNodeMetaInfoRequest(
					nodeId,
					metaInfo,
					operationContextPtr));
		if (requestPtr.IsValid()){
			istd::TDelPtr<imtgql::IGqlResponse> responsePtr(m_delegateCompPtr->CreateResponse(*requestPtr));
			Q_ASSERT(responsePtr.IsValid());
			if (responsePtr.IsValid()){
				if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
					if (responsePtr->IsSuccessful()){
						QVariant result = responsePtr->GetResult();
						Q_ASSERT(result.isValid());
						if (result.isValid()){
							if (result.canConvert(QMetaType(QMetaType::Bool))){
								return result.toBool();
							}
						}
					}
				}
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
	if (m_clientCompPtr.IsValid() && m_delegateCompPtr.IsValid()){
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr(m_delegateCompPtr->CreateMoveNodeRequest(
					nodeId,
					parentNodeId,
					operationContextPtr));
		if (requestPtr.IsValid()){
			istd::TDelPtr<imtgql::IGqlResponse> responsePtr(m_delegateCompPtr->CreateResponse(*requestPtr));
			Q_ASSERT(responsePtr.IsValid());
			if (responsePtr.IsValid()){
				if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
					if (responsePtr->IsSuccessful()){
						QVariant result = responsePtr->GetResult();
						Q_ASSERT(result.isValid());
						if (result.isValid()){
							if (result.canConvert(QMetaType(QMetaType::Bool))){
								return result.toBool();
							}
						}
					}
				}
			}
		}
	}

	return false;
}


bool CGqlHierarchicalStructureComp::RemoveNode(
			const Id& nodeId,
			const imtbase::IOperationContext* operationContextPtr)
{
	if (m_clientCompPtr.IsValid() && m_delegateCompPtr.IsValid()){
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr(m_delegateCompPtr->CreateRemoveNodeRequest(
					nodeId,
					operationContextPtr));
		if (requestPtr.IsValid()){
			istd::TDelPtr<imtgql::IGqlResponse> responsePtr(m_delegateCompPtr->CreateResponse(*requestPtr));
			Q_ASSERT(responsePtr.IsValid());
			if (responsePtr.IsValid()){
				if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
					if (responsePtr->IsSuccessful()){
						QVariant result = responsePtr->GetResult();
						Q_ASSERT(result.isValid());
						if (result.isValid()){
							if (result.canConvert(QMetaType(QMetaType::Bool))){
								return result.toBool();
							}
						}
					}
				}
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
	if (m_clientCompPtr.IsValid() && m_delegateCompPtr.IsValid()){
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr(m_delegateCompPtr->CreateInsertLeafRequest(
					leafId,
					nodeId,
					leafName,
					operationContextPtr));
		if (requestPtr.IsValid()){
			istd::TDelPtr<imtgql::IGqlResponse> responsePtr(m_delegateCompPtr->CreateResponse(*requestPtr));
			Q_ASSERT(responsePtr.IsValid());
			if (responsePtr.IsValid()){
				if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
					if (responsePtr->IsSuccessful()){
						QVariant result = responsePtr->GetResult();
						Q_ASSERT(result.isValid());
						if (result.isValid()){
							if (result.canConvert(QMetaType(QMetaType::Bool))){
								return result.toBool();
							}
						}
					}
				}
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
	if (m_clientCompPtr.IsValid() && m_delegateCompPtr.IsValid()){
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr(m_delegateCompPtr->CreateMoveLeafRequest(
					leafId,
					sourceNodeId,
					targetNodeId,
					operationContextPtr));
		if (requestPtr.IsValid()){
			istd::TDelPtr<imtgql::IGqlResponse> responsePtr(m_delegateCompPtr->CreateResponse(*requestPtr));
			Q_ASSERT(responsePtr.IsValid());
			if (responsePtr.IsValid()){
				if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
					if (responsePtr->IsSuccessful()){
						QVariant result = responsePtr->GetResult();
						Q_ASSERT(result.isValid());
						if (result.isValid()){
							if (result.canConvert(QMetaType(QMetaType::Bool))){
								return result.toBool();
							}
						}
					}
				}
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
	if (m_clientCompPtr.IsValid() && m_delegateCompPtr.IsValid()){
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr(m_delegateCompPtr->CreateRemoveLeafRequest(
					leafId,
					nodeId,
					operationContextPtr));
		if (requestPtr.IsValid()){
			istd::TDelPtr<imtgql::IGqlResponse> responsePtr(m_delegateCompPtr->CreateResponse(*requestPtr));
			Q_ASSERT(responsePtr.IsValid());
			if (responsePtr.IsValid()){
				if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
					if (responsePtr->IsSuccessful()){
						QVariant result = responsePtr->GetResult();
						Q_ASSERT(result.isValid());
						if (result.isValid()){
							if (result.canConvert(QMetaType(QMetaType::Bool))){
								return result.toBool();
							}
						}
					}
				}
			}
		}
	}

	return false;
}


// reimplemented (IHierarchicalStructureInfo)

int CGqlHierarchicalStructureComp::GetItemCount(const iprm::IParamsSet* selectionParamsPtr) const
{
	if (m_clientCompPtr.IsValid() && m_delegateCompPtr.IsValid()){
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr(m_delegateCompPtr->CreateGetItemCountRequest(
					selectionParamsPtr));
		if (requestPtr.IsValid()){
			istd::TDelPtr<imtgql::IGqlResponse> responsePtr(m_delegateCompPtr->CreateResponse(*requestPtr));
			Q_ASSERT(responsePtr.IsValid());
			if (responsePtr.IsValid()){
				if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
					if (responsePtr->IsSuccessful()){
						QVariant result = responsePtr->GetResult();
						Q_ASSERT(result.isValid());
						if (result.isValid()){
							if (result.canConvert(QMetaType(QMetaType::Int))){
								return result.toInt();
							}
						}
					}
				}
			}
		}
	}

	return -1;
}


imtbase::IHierarchicalStructureInfo::Ids CGqlHierarchicalStructureComp::GetItemIds(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr) const
{
	if (m_clientCompPtr.IsValid() && m_delegateCompPtr.IsValid()){
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr(m_delegateCompPtr->CreateGetItemIdsRequest(
					offset,
					count,
					selectionParamsPtr));
		if (requestPtr.IsValid()){
			istd::TDelPtr<imtgql::IGqlResponse> responsePtr(m_delegateCompPtr->CreateResponse(*requestPtr));
			Q_ASSERT(responsePtr.IsValid());
			if (responsePtr.IsValid()){
				if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
					if (responsePtr->IsSuccessful()){
						QVariant result = responsePtr->GetResult();
						Q_ASSERT(result.isValid());
						if (result.isValid()){
							if (result.canConvert(QMetaType(QMetaType::QByteArrayList))){
								Ids retVal = result.value<QByteArrayList>();

								return retVal;
							}
						}
					}
				}
			}
		}
	}

	return Ids();
}


imtbase::IHierarchicalStructureInfo::ItemInfoList CGqlHierarchicalStructureComp::GetItemInfos(const Ids& itemIds)
{
	if (m_clientCompPtr.IsValid() && m_delegateCompPtr.IsValid()){
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr(m_delegateCompPtr->CreateGetItemInfosRequest(itemIds));
		if (requestPtr.IsValid()){
			istd::TDelPtr<imtgql::IGqlResponse> responsePtr(m_delegateCompPtr->CreateResponse(*requestPtr));
			Q_ASSERT(responsePtr.IsValid());
			if (responsePtr.IsValid()){
				if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
					if (responsePtr->IsSuccessful()){
						QVariant result = responsePtr->GetResult();
						Q_ASSERT(result.isValid());
						if (result.isValid()){
							if (result.canConvert<ItemInfoList>()){
								ItemInfoList retVal = result.value<ItemInfoList>();

								return retVal;
							}
						}
					}
				}
			}
		}
	}

	return ItemInfoList();
}


imtbase::IHierarchicalStructureInfo::Ids CGqlHierarchicalStructureComp::GetItemPath(const Id& itemId) const
{
	if (m_clientCompPtr.IsValid() && m_delegateCompPtr.IsValid()){
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr(m_delegateCompPtr->CreateGetItemPathRequest(itemId));
		if (requestPtr.IsValid()){
			istd::TDelPtr<imtgql::IGqlResponse> responsePtr(m_delegateCompPtr->CreateResponse(*requestPtr));
			Q_ASSERT(responsePtr.IsValid());
			if (responsePtr.IsValid()){
				if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
					if (responsePtr->IsSuccessful()){
						QVariant result = responsePtr->GetResult();
						Q_ASSERT(result.isValid());
						if (result.isValid()){
							if (result.canConvert(QMetaType(QMetaType::QByteArrayList))){
								Ids retVal = result.value<QByteArrayList>();

								return retVal;
							}
						}
					}
				}
			}
		}
	}

	return Ids();
}


imtbase::IHierarchicalStructureIterator* CGqlHierarchicalStructureComp::CreateHierarchicalStructureIterator(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr) const
{
	if (m_delegateCompPtr.IsValid()){
		return m_delegateCompPtr->CreateHierarchicalStructureIterator(offset, count, selectionParamsPtr);
	}
}


} // namespace imtclientgql


