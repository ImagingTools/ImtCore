#pragma once


// ImtCore includes
#include <imtdb/CSqliteJsonDatabaseDelegateComp.h>


namespace imtauthdb
{


class CSqliteUserGroupDatabaseDelegateComp: public imtdb::CSqliteJsonDatabaseDelegateComp
{
public:
	typedef imtdb::CSqliteJsonDatabaseDelegateComp BaseClass;

	I_BEGIN_COMPONENT(CSqliteUserGroupDatabaseDelegateComp)
		I_ASSIGN(m_userDatabaseDelegateCompPtr, "UserDatabaseSqlDelegate", "User database sql delegate", true, "UserDatabaseSqlDelegate");
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "Users collection", true, "UserCollection");
	I_END_COMPONENT

	// reimplemented (imtdb::ISqlDatabaseObjectDelegate)
	virtual NewObjectQuery CreateNewObjectQuery(
				const QByteArray& typeId,
				const QByteArray& proposedObjectId,
				const QString& objectName,
				const QString& objectDescription,
				const istd::IChangeable* valuePtr,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;
	virtual QByteArray CreateUpdateObjectQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				const istd::IChangeable& object,
				const imtbase::IOperationContext* operationContextPtr = nullptr,
				bool useExternDelegate = true) const override;

	// reimplemented (imtdb::CSqlDatabaseDocumentDelegateComp)
	virtual bool SetCollectionItemMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const override;

private:
	I_REF(imtdb::ISqlDatabaseObjectDelegate, m_userDatabaseDelegateCompPtr);
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
};


} // namespace imtauthdb


