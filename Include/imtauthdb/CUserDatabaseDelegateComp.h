#pragma once


// ImtCore includes
#include <imtdb/CSqlJsonDatabaseDelegateComp.h>
#include <imtauth/IUserInfo.h>


namespace imtauthdb
{


class CUserDatabaseDelegateComp: public imtdb::CSqlJsonDatabaseDelegateComp
{
public:
	typedef imtdb::CSqlJsonDatabaseDelegateComp BaseClass;

	I_BEGIN_COMPONENT(CUserDatabaseDelegateComp)
		I_ASSIGN(m_userGroupDatabaseDelegateCompPtr, "UserGroupDatabaseSqlDelegate", "User group database sql delegate", true, "UserGroupDatabaseSqlDelegate");
		I_ASSIGN(m_userGroupCollectionCompPtr, "UserGroupCollection", "User group collection", true, "UserGroupCollection");
	I_END_COMPONENT

	// reimplemented (imtdb::ISqlDatabaseObjectDelegate)
	virtual NewObjectQuery CreateNewObjectQuery(
				const QByteArray& typeId,
				const QByteArray& proposedObjectId,
				const QString& objectName,
				const QString& objectDescription,
				const istd::IChangeable* valuePtr) const override;
	virtual QByteArray CreateUpdateObjectQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				const istd::IChangeable& object,
				bool useExternDelegate = true) const override;

private:
	I_REF(imtdb::ISqlDatabaseObjectDelegate, m_userGroupDatabaseDelegateCompPtr);
	I_REF(imtbase::IObjectCollection, m_userGroupCollectionCompPtr);
};


} // namespace imtauthdb


