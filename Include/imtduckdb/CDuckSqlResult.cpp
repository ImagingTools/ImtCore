// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtduckdb/CDuckSqlResult.h>


// Qt includes
#include <QtSql/QSqlError>

// ImtCore includes
#include <imtduckdb/CDuckSqlDriver.h>
#include <imtduckdb/CDuckValueConverter.h>


namespace imtduckdb
{


namespace
{


// Translates Qt's Oracle-style ':name' placeholders into DuckDB's '$name' named-parameter syntax,
// leaving quoted string/identifier literals and '::type' casts untouched.
QString ToDuckDbNamedPlaceholders(const QString& query)
{
	QString result;
	result.reserve(query.size());

	QChar closingQuote;
	const qsizetype queryLength = query.size();

	for (qsizetype i = 0; i < queryLength; ++ i){
		const QChar ch = query.at(i);

		if (!closingQuote.isNull()){
			if (ch == closingQuote){
				closingQuote = QChar();
			}

			result += ch;
			continue;
		}

		const bool isPlaceholderStart = ch == QLatin1Char(':')
					&& (i == 0 || query.at(i - 1) != QLatin1Char(':'))
					&& (i + 1 < queryLength && (query.at(i + 1).isLetterOrNumber() || query.at(i + 1) == QLatin1Char('_')));

		if (isPlaceholderStart){
			result += QLatin1Char('$');
			continue;
		}

		if (ch == QLatin1Char('\'') || ch == QLatin1Char('"') || ch == QLatin1Char('`')){
			closingQuote = ch;
		}

		result += ch;
	}

	return result;
}


} // anonymous namespace


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


bool CDuckSqlResult::prepare(const QString& sqlQuery)
{
	// Populates the base class' placeholder-name bookkeeping (used by bindValue()/boundValueNames()).
	if (!QSqlResult::prepare(sqlQuery)){
		return false;
	}

	m_preparedStatementPtr.reset();

	try{
		m_preparedStatementPtr = m_connection.Prepare(ToDuckDbNamedPlaceholders(sqlQuery).toStdString());
	}
	catch (const std::exception& exception){
		setLastError(QSqlError(QStringLiteral("Unable to prepare statement"), QString::fromUtf8(exception.what()), QSqlError::StatementError));

		return false;
	}

	if (!m_preparedStatementPtr || m_preparedStatementPtr->HasError()){
		QString errorText = m_preparedStatementPtr ?
					QString::fromStdString(m_preparedStatementPtr->GetError()) :
					QStringLiteral("DuckDB statement could not be prepared");

		setLastError(QSqlError(QStringLiteral("Unable to prepare statement"), errorText, QSqlError::StatementError));

		return false;
	}

	return true;
}


bool CDuckSqlResult::exec()
{
	setActive(false);
	setAt(QSql::BeforeFirstRow);

	if (!m_preparedStatementPtr){
		setLastError(QSqlError(QStringLiteral("Unable to execute statement"), QStringLiteral("Statement was not prepared"), QSqlError::StatementError));

		return false;
	}

	duckdb::case_insensitive_map_t<duckdb::BoundParameterData> namedValues;

	const QStringList names = boundValueNames();
	const QVariantList values = boundValues();
	for (qsizetype i = 0; i < names.size() && i < values.size(); ++ i){
		QString name = names.at(i);
		if (name.isEmpty()){
			continue;
		}

		if (name.startsWith(QLatin1Char(':'))){
			name.remove(0, 1);
		}

		namedValues.emplace(name.toStdString(), duckdb::BoundParameterData(CDuckValueConverter::ToDuckDbValue(values.at(i))));
	}

	try{
		auto queryResultPtr = m_preparedStatementPtr->Execute(namedValues, false);

		// allow_stream_result is false above, so the concrete result is always materialized.
		m_resultPtr.reset(static_cast<duckdb::MaterializedQueryResult*>(queryResultPtr.release()));
	}
	catch (const std::exception& exception){
		setLastError(QSqlError(QStringLiteral("Unable to execute statement"), QString::fromUtf8(exception.what()), QSqlError::StatementError));

		return false;
	}

	if (!m_resultPtr || m_resultPtr->HasError()){
		QString errorText = m_resultPtr ?
					QString::fromStdString(m_resultPtr->GetError()) :
					QStringLiteral("DuckDB statement could not be executed");

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
