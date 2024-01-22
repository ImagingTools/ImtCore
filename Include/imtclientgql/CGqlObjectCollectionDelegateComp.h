#pragma once


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

// ACF includes
#include <icomp/CComponentBase.h>
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtclientgql/IGqlClient.h>
#include <imtclientgql/IGqlObjectCollectionDelegate.h>
#include <imtbase/IMetaInfoCreator.h>


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
		I_ASSIGN(m_collectionIdAttrPtr, "Id", "Collection-ID", false, "");
		I_ASSIGN(m_metaInfoCreatorCompPtr, "MetaInfoCreator", "Meta-info creator", false, "MetaInfoCreator");
	I_END_COMPONENT;

	// reimplemented (IGqlObjectCollectionDelegate)
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
	virtual imtgql::IGqlRequest* CreateGetElementCountRequest(const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;
	virtual imtgql::IGqlRequest* CreateGetElementListRequest(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;
	virtual imtgql::IGqlRequest* CreateGetSubCollectionRequest(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;

	// reimplemented (IGqlResponseCreator)
	virtual imtgql::IGqlResponse* CreateResponse(const imtgql::IGqlRequest& request) const override;

private:
	bool SerializeObject(const istd::IPolymorphic* object, QByteArray& objectData) const;
	bool DeSerializeObject(istd::IPolymorphic* object, const QByteArray& objectData) const;
	I_REF(imtbase::IMetaInfoCreator, m_metaInfoCreatorCompPtr);

private:
	I_ATTR(QByteArray, m_collectionIdAttrPtr);
};


} // namespace imtclientgql


