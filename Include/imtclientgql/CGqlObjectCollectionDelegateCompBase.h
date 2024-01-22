#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtgql/CGqlObject.h>
#include <imtbase/IHierarchicalStructureInfo.h>
#include <ifile/IFilePersistence.h>

// GmgCore includes
// #include <imtclientgql/CGqlObjectCollectionInfo.h>
#include <imtclientgql/IGqlObjectCollectionDelegate.h>
#include <imtclientgql/IGqlClient.h>


namespace imtclientgql
{


class CGqlObjectCollectionDelegateCompBase:
			public ilog::CLoggerComponentBase,
			virtual public IGqlObjectCollectionDelegate
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CGqlObjectCollectionDelegateCompBase);
		I_REGISTER_INTERFACE(IGqlObjectCollectionDelegate);
		// I_ASSIGN_MULTI_0(m_objectTypeIdsAttrPtr, "ObjectTypeIds", "ID of the supported object type for this delegate", true);
		I_ASSIGN(m_clientCompPtr, "ApiClient", "GraphQL API client", true, "ApiClient");
	I_END_COMPONENT;

	// reimplemented (IGqlObjectCollectionDelegate)
	// virtual imtgql::IGqlRequest* CreateGetElementType(const QByteArray& elementId) const override;
	virtual imtgql::IGqlRequest* CreateGetObjectInfoRequest(const QByteArray& objectId) const override;
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
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;
	virtual imtgql::IGqlRequest* CreateSetObjectNameRequest(
				const QByteArray& objectId,
				const QString& name,
				int clientVersion = -1,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;
	virtual imtgql::IGqlRequest* CreateSetObjectDescriptionRequest(
				const QByteArray& objectId,
				const QString& description,
				int clientVersion = -1,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;
	virtual imtgql::IGqlRequest* CreateSetObjectMetaInfoRequest(
				const QByteArray& objectId,
				const idoc::IDocumentMetaInfo& metaInfo,
				int clientVersion = -1,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;
	virtual imtgql::IGqlRequest* CreateGetObjectRequest(const QByteArray& objectId) const override;
	virtual imtgql::IGqlRequest* CreateSetObjectRequest(
				const QByteArray& objectId,
				const istd::IChangeable* objectPtr,
				const QString& uploadUrl = QString(),
				const idoc::IDocumentMetaInfo* dataMetaInfoPtr = nullptr,
				const idoc::IDocumentMetaInfo* collectionItemMetaInfoPtr = nullptr,
				int clientVersion = -1,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;
	virtual imtgql::IGqlRequest* CreateRemoveObjectRequest(
				const QByteArray& objectId,
				int clientElementVersion = -1,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;
	virtual imtgql::IGqlRequest* CreateGetElementCountRequest(
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;
	virtual imtgql::IGqlRequest* CreateGetElementListRequest(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;
	virtual imtgql::IGqlRequest* CreateGetSubCollectionRequest(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;

protected:
	// I_MULTIATTR(QByteArray, m_objectTypeIdsAttrPtr);
	I_REF(IGqlClient, m_clientCompPtr);
};


} // namespace imtclientgql


