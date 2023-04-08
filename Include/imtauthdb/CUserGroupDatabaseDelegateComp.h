#pragma once


// ImtCore includes
#include <imtdb/CSqlDatabaseObjectDelegateCompBase.h>
#include <imtauth/CUserGroupInfo.h>
#include <imtdb/CSqlJsonDatabaseDelegateComp.h>


namespace imtauthdb
{


class CUserGroupDatabaseDelegateComp: public imtdb::CSqlJsonDatabaseDelegateComp
{
public:
	typedef imtdb::CSqlJsonDatabaseDelegateComp BaseClass;

	I_BEGIN_COMPONENT(CUserGroupDatabaseDelegateComp)
	I_END_COMPONENT

	// reimplemented (imtdb::ISqlDatabaseObjectDelegate)
	virtual istd::IChangeable* CreateObjectFromRecord(const QSqlRecord& record) const override;
	virtual NewObjectQuery CreateNewObjectQuery(
				const QByteArray& typeId,
				const QByteArray& proposedObjectId,
				const QString& objectName,
				const QString& objectDescription,
				const istd::IChangeable* valuePtr) const override;
	virtual QByteArray CreateUpdateObjectQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				const istd::IChangeable& object) const override;

	// reimplemented (imtdb::CSqlDatabaseDocumentDelegateComp)
	virtual QString GetBaseSelectionQuery() const override;
};


} // namespace imtauthdb


