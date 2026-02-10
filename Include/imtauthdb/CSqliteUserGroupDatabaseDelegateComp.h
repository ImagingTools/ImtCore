// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdb/CSqliteDatabaseDocumentDelegateComp.h>


namespace imtauthdb
{


class CSqliteUserGroupDatabaseDelegateComp: public imtdb::CSqliteDatabaseDocumentDelegateComp
{
public:
	typedef imtdb::CSqliteDatabaseDocumentDelegateComp BaseClass;

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

private:
	I_REF(imtdb::ISqlDatabaseObjectDelegate, m_userDatabaseDelegateCompPtr);
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
};


} // namespace imtauthdb


