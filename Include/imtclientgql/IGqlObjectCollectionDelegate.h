#pragma once


// ACF includes
#include <idoc/IDocumentMetaInfo.h>

// ImtCore includes
#include <imtbase/TIStructuredCollectionInfo.h>
#include <imtbase/IObjectCollection.h>
#include <imtbase/IOperationContext.h>
#include <imtclientgql/IGqlResponseCreator.h>


namespace imtclientgql
{


class IGqlObjectCollectionDelegate: virtual public IGqlResponseCreator
{
public:
	virtual imtgql::IGqlRequest* CreateGetObjectInfoRequest(const QByteArray& objectId) const = 0;
	virtual imtgql::IGqlRequest* CreateGetObjectMetaInfoRequest(const QByteArray& objectId) const = 0;
	virtual imtgql::IGqlRequest* CreateGetObjectDataMetaInfoRequest(const QByteArray& objectId) const = 0;
	virtual imtgql::IGqlRequest* CreateInsertObjectRequest(
				const QByteArray& typeId,
				const QString& name,
				const QString& description,
				const istd::IChangeable* objectPtr,
				const QString& uploadUrl = QString(),
				const QByteArray& proposedObjectId = QByteArray(),
				const QByteArray& nodeId = QByteArray(),
				const idoc::IDocumentMetaInfo* dataMetaInfoPtr = nullptr,
				const idoc::IDocumentMetaInfo* collectionItemMetaInfoPtr = nullptr,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;
	virtual imtgql::IGqlRequest* CreateSetObjectNameRequest(
				const QByteArray& objectId,
				const QString& name,
				int clientVersion = -1,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;
	virtual imtgql::IGqlRequest* CreateSetObjectDescriptionRequest(
				const QByteArray& objectId,
				const QString& description,
				int clientVersion = -1,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;
	virtual imtgql::IGqlRequest* CreateSetObjectMetaInfoRequest(
				const QByteArray& objectId,
				const idoc::IDocumentMetaInfo& metaInfo,
				int clientVersion = -1,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;
	virtual imtgql::IGqlRequest* CreateGetObjectRequest(const QByteArray& objectId) const = 0;
	virtual imtgql::IGqlRequest* CreateSetObjectRequest(
				const QByteArray& objectId,
				const istd::IChangeable* objectPtr,
				const QString& uploadUrl = QString(),
				const idoc::IDocumentMetaInfo* dataMetaInfoPtr = nullptr,
				const idoc::IDocumentMetaInfo* collectionItemMetaInfoPtr = nullptr,
				int clientVersion = -1,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;
	virtual imtgql::IGqlRequest* CreateRemoveObjectRequest(
				const QByteArray& objectId,
				int clientElementVersion = -1,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;
	virtual imtgql::IGqlRequest* CreateGetElementCountRequest(
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const = 0;
	virtual imtgql::IGqlRequest* CreateGetElementListRequest(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const = 0;
	virtual imtgql::IGqlRequest* CreateGetSubCollectionRequest(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const = 0;
};


} // namespace imtclientgql


