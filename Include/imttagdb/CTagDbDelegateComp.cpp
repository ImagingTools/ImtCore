// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imttagdb/CTagDbDelegateComp.h>


namespace imttagdb
{


// protected methods

QString CTagDbDelegateComp::CreateIsSystemCondition() const
{
	const bool isSqlite = m_databaseEngineCompPtr.IsValid() && (m_databaseEngineCompPtr->GetDatabaseDriverId().toUpper() == "QSQLITE");
	if (isSqlite){
		return QStringLiteral(R"(json_extract(root."Document", '$.IsSystem') IN (1, 'true'))");
	}

	return QStringLiteral(R"(COALESCE(root."Document"->>'IsSystem', 'false') = 'true')");
}


// reimplemented (imtdb::CSqlDatabaseDocumentDelegateCompBase)

QByteArray CTagDbDelegateComp::CreateDeleteObjectsQuery(
			const imtbase::IObjectCollection& collection,
			const imtbase::ICollectionInfo::Ids& objectIds,
			const imtbase::IOperationContext* operationContextPtr) const
{
	QByteArray query = BaseClass::CreateDeleteObjectsQuery(collection, objectIds, operationContextPtr);
	if (query.isEmpty()){
		return query;
	}

	QStringList quotedIds;
	for (const QByteArray& tagId : objectIds){
		quotedIds.append(QStringLiteral("'%1'").arg(SqlEncode(QString::fromUtf8(tagId))));
	}

	const QString assignmentsQuery = QStringLiteral(R"(DELETE FROM %1 WHERE "TagId" IN (%2);)")
				.arg(CreateTagTableReference(GetTagAssignmentsTableName()), quotedIds.join(','));

	return query + assignmentsQuery.toUtf8();
}


QString CTagDbDelegateComp::CreateTenantBindingFilterQuery(const QByteArray& tenantId, imtauth::TenantFilterMode filterMode) const
{
	const QString baseQuery = BaseClass::CreateTenantBindingFilterQuery(tenantId, filterMode);
	if (baseQuery.isEmpty() || (filterMode == imtauth::TFM_EXCLUDE)){
		return baseQuery;
	}

	return QStringLiteral("(%1) OR (%2)").arg(baseQuery, CreateIsSystemCondition());
}


QByteArray CTagDbDelegateComp::CreateTenantBindingDeleteQuery(const QByteArrayList& /*entityIds*/) const
{
	// Deletion is soft; the binding keeps the owner of a deleted tag so that only this tenant can restore it.
	return QByteArray();
}


} // namespace imttagdb


