#pragma once


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtclientgql/IGqlClient.h>
#include <imtclientgql/CGqlObjectCollectionDelegateCompBase.h>


namespace imtclientgql
{


class CGqlObjectCollectionDelegateComp:
			public CGqlObjectCollectionDelegateCompBase
{
public:
	typedef CGqlObjectCollectionDelegateCompBase BaseClass;

	I_BEGIN_COMPONENT(CGqlObjectCollectionDelegateComp);
		I_REGISTER_INTERFACE(IGqlObjectCollectionDelegate);
//		I_ASSIGN(m_fileTransferCompPtr, "FileTransfer", "Url based file transfer component", true, "FileTRansfer");
//		I_ASSIGN(m_objectTypeIdAttrPtr, "ObjectTypeId", "Supported object type ID", true, "Measurement");
		I_ASSIGN(m_collectionIdAttrPtr, "Id", "Collection ID", false, "");
	I_END_COMPONENT;

	// reimplemented (IGqlObjectCollectionDelegate)
//	virtual QByteArrayList GetSupportedObjectTypeIds() const override;
//	virtual imtgql::IGqlRequest* CreateGetElementType(const QByteArray& elementId) const override;

	/**
		Infos
	*/
//	virtual imtbase::IStructuredObjectCollectionInfo::ElementType GetElementType(const QByteArray& elementId) const override;
//	virtual bool GetNodeInfo(const QByteArray& nodeId, imtgql::IGqlStructuredCollectionResponse::NodeInfo& outInfo) const override;
//	virtual bool GetObjectInfo(const QByteArray& objectId, imtgql::IGqlStructuredCollectionResponse::ObjectInfo& outInfo) const override;


	/**
		Object manipulations
	*/

//	virtual imtgql::IGqlRequest* CreateGetObjectInfoRequest(const QByteArray& objectId) const override;
//	virtual imtgql::IGqlRequest* CreateSetObjectNameRequest(
//		const QByteArray& objectId,
//		const QString& name,
//		int clientVersion = -1,
//		const imtbase::IOperationContext* operationContextPtr = nullptr) const override;
//	virtual imtgql::IGqlRequest* CreateSetObjectDescriptionRequest(
//		const QByteArray& objectId,
//		const QString& description,
//		int clientVersion = -1,
//		const imtbase::IOperationContext* operationContextPtr = nullptr) const override;
//	virtual imtgql::IGqlRequest* CreateSetObjectMetaInfoRequest(
//		const QByteArray& objectId,
//		const idoc::IDocumentMetaInfo& metaInfo,
//		int clientVersion = -1,
//		const imtbase::IOperationContext* operationContextPtr = nullptr) const override;
	virtual QByteArray InsertObject(
				const QByteArray& typeId,
				const QString& name,
				const QString& description,
				const istd::IChangeable& object,
				const QByteArray& proposedObjectId = QByteArray(),
				const QByteArray& nodeId = QByteArray(),
				const idoc::IDocumentMetaInfo* dataMetaInfoPtr = nullptr,
				const idoc::IDocumentMetaInfo* elementMetaInfoPtr = nullptr,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;
	virtual bool GetObject(
				const QByteArray& objectId,
				const QByteArray& typeId,
				imtbase::IObjectCollection::DataPtr objectPtr) const override;
	virtual bool SetObject(
				const QByteArray& objectId,
				const QByteArray& typeId,
				const istd::IChangeable& object,
				const idoc::IDocumentMetaInfo* dataMetaInfoPtr = nullptr,
				const idoc::IDocumentMetaInfo* collectionItemMetaInfoPtr = nullptr,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;

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

//	virtual imtgql::IGqlRequest* CreateUploadUrlsRequest(const QStringList& fileNames, const QByteArray& nodeId) const override;
//	virtual imtgql::IGqlRequest* CreateDownloadUrlsRequest(const QByteArrayList& objectIds) const override;
//	virtual imtcom::IFileTransfer* GetFileTransfer() const override;

	virtual imtgql::IGqlResponse* CreateResponse(const imtgql::IGqlRequest& request) const override;

private:
//	I_REF(imtcom::IFileTransfer, m_fileTransferCompPtr);
//	I_ATTR(QByteArray, m_objectTypeIdAttrPtr);
	I_ATTR(QByteArray, m_collectionIdAttrPtr);
};


} // namespace imtclientgql


