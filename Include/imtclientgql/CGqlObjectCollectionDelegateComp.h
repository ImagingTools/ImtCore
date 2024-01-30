#pragma once


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtclientgql/IGqlClient.h>
#include <imtclientgql/IGqlObjectCollectionDelegate.h>


namespace imtclientgql
{


class CGqlObjectCollectionDelegateComp:
			public ilog::CLoggerComponentBase,
			virtual public IGqlObjectCollectionDelegate
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CGqlObjectCollectionDelegateComp);
		I_REGISTER_INTERFACE(IGqlObjectCollectionDelegate);
		I_ASSIGN(m_clientCompPtr, "ApiClient", "GraphQL API client", true, "ApiClient");
		I_ASSIGN(m_collectionIdAttrPtr, "Id", "Collection-ID", false, "");
	I_END_COMPONENT;

	virtual imtgql::IGqlRequest* CreateGetObjectInfoRequest(const QByteArray& objectId) const override;
	virtual imtgql::IGqlRequest* CreateGetObjectMetaInfoRequest(const QByteArray& objectId) const override;
	virtual imtgql::IGqlRequest* CreateGetObjectDataMetaInfoRequest(const QByteArray& objectId) const override;
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
	virtual imtgql::IGqlRequest* CreateGetElementCountRequest(const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;
	virtual imtgql::IGqlRequest* CreateGetElementListRequest(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;
	virtual imtgql::IGqlRequest* CreateGetSubCollectionRequest(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;

	virtual bool IsValid(const imtgql::IGqlResponse& reqponse) const override;
	virtual bool GetOperationResult(const imtgql::IGqlResponse& response, bool& out) const override;
	virtual bool GetObjectId(const imtgql::IGqlResponse& response, Id& out) const override;
	virtual bool GetObjectInfo(const imtgql::IGqlResponse& response, ObjectInfo& out) const override;
	virtual bool GetObjectData(const imtgql::IGqlResponse& response, istd::IChangeable& out) const override;
	virtual bool GetMetaInfo(const imtgql::IGqlResponse& response, idoc::IDocumentMetaInfo& out) const override;
	virtual bool GetItemCount(const imtgql::IGqlResponse& response, int& out) const override;
	virtual bool GetItemIds(const imtgql::IGqlResponse& response, Ids& out) const override;
	virtual imtbase::IObjectCollection* GetSubCollection(
				imtbase::IObjectCollection& collection,
				const imtgql::IGqlResponse& reqponse,
				QList<imtbase::IMetaInfoCreator*> metaInfoCreatorList) const override;

private:
	struct ResponseData
	{
		QJsonObject data;
		QJsonObject errors;
		QByteArray commandId;
	};

	ResponseData GetResponseData(const imtgql::IGqlResponse& response) const;
	bool SerializeObject(const istd::IPolymorphic* object, QByteArray& objectData) const;
	bool DeSerializeObject(istd::IPolymorphic* object, const QByteArray& objectData) const;

private:
	I_ATTR(QByteArray, m_collectionIdAttrPtr);
	I_REF(IGqlClient, m_clientCompPtr);
};


} // namespace imtclientgql


