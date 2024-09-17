#pragma once


// ImtCore includes
#include <imtdb/CSqliteJsonDatabaseDelegateComp.h>


namespace imtauthdb
{


class CSqliteRoleDatabaseDelegateComp: public imtdb::CSqliteJsonDatabaseDelegateComp
{
public:
	typedef imtdb::CSqliteJsonDatabaseDelegateComp BaseClass;

	I_BEGIN_COMPONENT(CSqliteRoleDatabaseDelegateComp)
	I_END_COMPONENT

protected:
	// reimplemented (imtdb::CSqliteJsonDatabaseDelegateComp)
	virtual istd::IChangeable* CreateObjectFromRecord(const QSqlRecord& record) const override;
	virtual imtdb::IDatabaseObjectDelegate::NewObjectQuery CreateNewObjectQuery(
				const QByteArray& typeId,
				const QByteArray& proposedObjectId,
				const QString& objectName,
				const QString& objectDescription,
				const istd::IChangeable* valuePtr,
				const imtbase::IOperationContext* operationContextPtr) const override;
	virtual QByteArray CreateUpdateObjectQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				const istd::IChangeable& object,
				const imtbase::IOperationContext* operationContextPtr,
				bool useExternDelegate = true) const override;
};


} // namespace imtauthdb


