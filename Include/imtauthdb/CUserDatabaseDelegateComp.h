// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdb/CSqlDatabaseDocumentDelegateComp.h>


namespace imtauthdb
{


class CUserDatabaseDelegateComp: public imtdb::CSqlDatabaseDocumentDelegateComp
{
public:
	typedef imtdb::CSqlDatabaseDocumentDelegateComp BaseClass;

	I_BEGIN_COMPONENT(CUserDatabaseDelegateComp)
		I_ASSIGN(m_userGroupDatabaseDelegateCompPtr, "UserGroupDatabaseSqlDelegate", "User group database sql delegate", false, "UserGroupDatabaseSqlDelegate");
		I_ASSIGN(m_userGroupCollectionCompPtr, "UserGroupCollection", "User group collection", false, "UserGroupCollection");
	I_END_COMPONENT

	// reimplemented (imtdb::ISqlDatabaseObjectDelegate)
	virtual NewObjectQuery CreateNewObjectQuery(
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
	virtual QByteArray CreateDeleteObjectsQuery(
				const imtbase::IObjectCollection& collection,
				const imtbase::ICollectionInfo::Ids& objectIds,
				const imtbase::IOperationContext* operationContextPtr) const override;
	virtual bool CreateObjectFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const override;
	virtual bool CreateSortQuery(const imtbase::IComplexCollectionFilter& collectionFilter, QString& sortQuery) const override;
	virtual QByteArray CreateJoinTablesQuery() const override;
	virtual QByteArray GetCustomColumnsQuery() const override;

private:
	I_REF(imtdb::ISqlDatabaseObjectDelegate, m_userGroupDatabaseDelegateCompPtr);
	I_REF(imtbase::IObjectCollection, m_userGroupCollectionCompPtr);
};


} // namespace imtauthdb


