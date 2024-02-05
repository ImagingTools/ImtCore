#pragma once


// ACF includes
#include <idoc/IDocumentMetaInfo.h>

// ImtCore includes
#include <imtbase/IMetaInfoCreator.h>
#include <imtbase/IObjectCollection.h>
#include <imtbase/IOperationContext.h>
#include <imtgql/IGqlResponse.h>


namespace imtclientgql
{


class IGqlObjectCollectionDelegate: virtual public istd::IPolymorphic
{
public:
	typedef QByteArray Id;
	typedef QByteArrayList Ids;

	struct ObjectInfo
	{
		ObjectInfo()
			:version(-1)
		{
		}

		QByteArray id;
		QString name;
		QString description;
		QByteArray typeId;

		QSharedPointer<idoc::IDocumentMetaInfo> dataMetaInfoPtr;

		/**
			The server assigns the version to the document to be used when updating the data.
			When we want to change a document or information about it on the server,
			this version must be passed in the request. The version is used to synchronize
			simultaneous access to a document by multiple clients.
		*/
			int version;
	};
	typedef QList<ObjectInfo> ObjectInfoList;

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

	virtual bool IsValid(const imtgql::IGqlResponse& reqponse) const = 0;
	virtual bool GetOperationResult(const imtgql::IGqlResponse& reqponse, bool& out) const = 0;
	virtual bool GetObjectId(const imtgql::IGqlResponse& reqponse, Id& out) const = 0;
	virtual bool GetObjectInfo(const imtgql::IGqlResponse& reqponse, ObjectInfo& out) const = 0;
	virtual bool GetObjectData(const imtgql::IGqlResponse& reqponse, istd::IChangeable& out) const = 0;
	virtual bool GetMetaInfo(const imtgql::IGqlResponse& reqponse, idoc::IDocumentMetaInfo& out) const = 0;
	virtual bool GetItemCount(const imtgql::IGqlResponse& reqponse, int& out) const = 0;
	virtual bool GetItemIds(const imtgql::IGqlResponse& reqponse, Ids& out) const = 0;
	virtual imtbase::IObjectCollection* GetSubCollection(
				imtbase::IObjectCollection& collection,
				const imtgql::IGqlResponse& reqponse,
				QList<imtbase::IMetaInfoCreator*> metaInfoCreatorList) const = 0;
};


} // namespace imtclientgql


