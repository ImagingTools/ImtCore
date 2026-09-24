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

QString CTagDbDelegateComp::CreateTenantBindingFilterQuery(const QByteArray& tenantId, imtauth::TenantFilterMode filterMode) const
{
	const QString baseQuery = BaseClass::CreateTenantBindingFilterQuery(tenantId, filterMode);
	if (baseQuery.isEmpty() || (filterMode == imtauth::TFM_EXCLUDE)){
		return baseQuery;
	}

	return QStringLiteral("(%1) OR (%2)").arg(baseQuery, CreateIsSystemCondition());
}


} // namespace imttagdb


