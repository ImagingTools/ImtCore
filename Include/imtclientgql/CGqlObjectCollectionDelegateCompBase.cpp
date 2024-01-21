#include <imtclientgql/CGqlObjectCollectionDelegateCompBase.h>


namespace imtclientgql
{


// public methods

// reimplemented (IGqlObjectCollectionDelegate)
imtgql::IGqlRequest* CGqlObjectCollectionDelegateCompBase::CreateGetObjectInfoRequest(const QByteArray& objectId) const
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


