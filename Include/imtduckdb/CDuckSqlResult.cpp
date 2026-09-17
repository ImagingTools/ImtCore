// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtduckdb/CDuckSqlResult.h>


// Qt includes
#include <QtSql/QSqlError>

// imtduckdb includes
#include <imtduckdb/CDuckSqlDriver.h>


namespace imtduckdb
{


CDuckSqlResult::CDuckSqlResult(const CDuckSqlDriver* driverPtr, duckdb::Connection& connection)
	:QSqlResult(driverPtr),
	m_connection(connection)
{
}


// reimplemented (QSqlResult)

bool CDuckSqlResult::reset(const QString& sqlQuery)
{
	setActive(false);
	setAt(QSql::BeforeFirstRow);

	m_resultPtr = m_connection.Query(sqlQuery.toStdString());

	if (!m_resultPtr || m_resultPtr->HasError()){
		QString errorText = m_resultPtr ?
					QString::fromStdString(m_resultPtr->GetError()) :
					QStringLiteral("DuckDB query could not be executed");

		setLastError(QSqlError(QStringLiteral("Unable to execute statement"), errorText, QSqlError::StatementError));

		return false;
	}

	setSelect(m_resultPtr->properties.return_type == duckdb::StatementReturnType::QUERY_RESULT);
	setActive(true);

	return true;
}


bool CDuckSqlResult::fetch(int index)
{
	if (!m_resultPtr || index < 0 || static_cast<duckdb::idx_t>(index) >= m_resultPtr->RowCount()){
		return false;
	}

	setAt(index);

	return true;
}


bool CDuckSqlResult::fetchFirst()
{
	return fetch(0);
}


bool CDuckSqlResult::fetchLast()
{
	if (!m_resultPtr || m_resultPtr->RowCount() == 0){
		return false;
	}

	return fetch(static_cast<int>(m_resultPtr->RowCount()) - 1);
}


QVariant CDuckSqlResult::data(int index)
{
	if (!m_resultPtr || at() < 0){
		return QVariant();
	}

	return ConvertValue(m_resultPtr->GetValue(static_cast<duckdb::idx_t>(index), static_cast<duckdb::idx_t>(at())));
}


bool CDuckSqlResult::isNull(int index)
{
	if (!m_resultPtr || at() < 0){
		return true;
	}

	return m_resultPtr->GetValue(static_cast<duckdb::idx_t>(index), static_cast<duckdb::idx_t>(at())).IsNull();
}


int CDuckSqlResult::size()
{
	return m_resultPtr ? static_cast<int>(m_resultPtr->RowCount()) : -1;
}


int CDuckSqlResult::numRowsAffected()
{
	if (!m_resultPtr || m_resultPtr->properties.return_type != duckdb::StatementReturnType::CHANGED_ROWS){
		return -1;
	}

	if (m_resultPtr->RowCount() != 1 || m_resultPtr->ColumnCount() != 1){
		return -1;
	}

	const duckdb::Value countValue = m_resultPtr->GetValue(0, 0);
	if (countValue.IsNull()){
		return -1;
	}

	return static_cast<int>(countValue.GetValue<int64_t>());
}


// private methods

QVariant CDuckSqlResult::ConvertValue(const duckdb::Value& value)
{
	if (value.IsNull()){
		return QVariant();
	}

	switch (value.type().id()){
	case duckdb::LogicalTypeId::BOOLEAN:
		return QVariant(value.GetValue<bool>());

	case duckdb::LogicalTypeId::TINYINT:
	case duckdb::LogicalTypeId::SMALLINT:
	case duckdb::LogicalTypeId::INTEGER:
	case duckdb::LogicalTypeId::UTINYINT:
	case duckdb::LogicalTypeId::USMALLINT:
		return QVariant(value.GetValue<int32_t>());

	case duckdb::LogicalTypeId::BIGINT:
	case duckdb::LogicalTypeId::UINTEGER:
		return QVariant(static_cast<qlonglong>(value.GetValue<int64_t>()));

	case duckdb::LogicalTypeId::FLOAT:
		return QVariant(value.GetValue<float>());

	case duckdb::LogicalTypeId::DOUBLE:
	case duckdb::LogicalTypeId::DECIMAL:
		return QVariant(value.GetValue<double>());

	default:
		return QVariant(QString::fromStdString(value.ToString()));
	}
}


} // namespace imtduckdb
