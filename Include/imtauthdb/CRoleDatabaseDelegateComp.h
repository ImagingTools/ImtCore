// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdb/CSqlDatabaseDocumentDelegateComp.h>


namespace imtauthdb
{


class CRoleDatabaseDelegateComp: public imtdb::CSqlDatabaseDocumentDelegateComp
{
public:
	typedef imtdb::CSqlDatabaseDocumentDelegateComp BaseClass;

	I_BEGIN_COMPONENT(CRoleDatabaseDelegateComp)
		I_ASSIGN(m_userDatabaseDelegateCompPtr, "UserDatabaseSqlDelegate", "User database sql delegate, removes the deleted roles from the users", false, "UserDatabaseSqlDelegate");
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "Users collection", false, "UserCollection");
		I_ASSIGN(m_userGroupDatabaseDelegateCompPtr, "UserGroupDatabaseSqlDelegate", "Group database sql delegate, removes the deleted roles from the groups", false, "UserGroupDatabaseSqlDelegate");
		I_ASSIGN(m_userGroupCollectionCompPtr, "UserGroupCollection", "Groups collection", false, "UserGroupCollection");
	I_END_COMPONENT

	// reimplemented (imtdb::CSqlJsonDatabaseDelegateComp)
	virtual QByteArray CreateDeleteObjectsQuery(
				const imtbase::IObjectCollection& collection,
				const imtbase::ICollectionInfo::Ids& objectIds,
				const imtbase::IOperationContext* operationContextPtr) const override;
	virtual bool SetCollectionItemMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const override;
	virtual QString CreateAdditionalFiltersQuery(const iprm::IParamsSet& filterParams) const override;

private:
	I_REF(imtdb::ISqlDatabaseObjectDelegate, m_userDatabaseDelegateCompPtr);
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
	I_REF(imtdb::ISqlDatabaseObjectDelegate, m_userGroupDatabaseDelegateCompPtr);
	I_REF(imtbase::IObjectCollection, m_userGroupCollectionCompPtr);
};


} // namespace imtauthdb


