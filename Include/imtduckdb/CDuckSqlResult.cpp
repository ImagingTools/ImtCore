// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtduckdb/CDuckSqlResult.h>


// Qt includes
#include <QtSql/QSqlError>
#include <QtSql/QSqlField>
#include <QtSql/QSqlRecord>

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

	try{
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


QSqlRecord CDuckSqlResult::record() const
{
	QSqlRecord recordVal;

	if (!m_resultPtr){
		return recordVal;
	}

	const duckdb::idx_t columnCount = m_resultPtr->ColumnCount();
	const bool hasCurrentRow = at() >= 0 && static_cast<duckdb::idx_t>(at()) < m_resultPtr->RowCount();

	for (duckdb::idx_t i = 0; i < columnCount; ++ i){
		QSqlField field(QString::fromStdString(m_resultPtr->ColumnName(i)), ConvertMetaType(m_resultPtr->types[i]));

		if (hasCurrentRow){
			field.setValue(ConvertValue(m_resultPtr->GetValue(i, static_cast<duckdb::idx_t>(at()))));
		}

		recordVal.append(field);
	}

	return recordVal;
}


// private methods

QMetaType CDuckSqlResult::ConvertMetaType(const duckdb::LogicalType& type)
{
	switch (type.id()){
	case duckdb::LogicalTypeId::BOOLEAN:
		return QMetaType(QMetaType::Bool);

	case duckdb::LogicalTypeId::TINYINT:
	case duckdb::LogicalTypeId::SMALLINT:
	case duckdb::LogicalTypeId::INTEGER:
	case duckdb::LogicalTypeId::UTINYINT:
	case duckdb::LogicalTypeId::USMALLINT:
		return QMetaType(QMetaType::Int);

	case duckdb::LogicalTypeId::BIGINT:
	case duckdb::LogicalTypeId::UINTEGER:
		return QMetaType(QMetaType::LongLong);

	case duckdb::LogicalTypeId::UBIGINT:
		return QMetaType(QMetaType::ULongLong);

	case duckdb::LogicalTypeId::FLOAT:
		return QMetaType(QMetaType::Float);

	case duckdb::LogicalTypeId::DOUBLE:
	case duckdb::LogicalTypeId::DECIMAL:
		return QMetaType(QMetaType::Double);

	case duckdb::LogicalTypeId::BLOB:
		return QMetaType(QMetaType::QByteArray);

	case duckdb::LogicalTypeId::LIST:
	case duckdb::LogicalTypeId::ARRAY:
		return QMetaType(QMetaType::QVariantList);

	case duckdb::LogicalTypeId::STRUCT:
		return QMetaType(QMetaType::QVariantMap);

	// UUID, TIMESTAMP, DATE and HUGEINT stay text, which is what the callers read them as.
	default:
		return QMetaType(QMetaType::QString);
	}
}


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

	// Surrogate ids use all 64 bits, which a signed type would overflow.
	case duckdb::LogicalTypeId::UBIGINT:
		return QVariant(static_cast<qulonglong>(value.GetValue<uint64_t>()));

	// The text form of a blob escapes every byte, so it cannot be read back.
	case duckdb::LogicalTypeId::BLOB: {
		const std::string& bytes = duckdb::StringValue::Get(value);

		return QVariant(QByteArray(bytes.data(), static_cast<qsizetype>(bytes.size())));
	}

	case duckdb::LogicalTypeId::LIST:
	case duckdb::LogicalTypeId::ARRAY: {
		const duckdb::vector<duckdb::Value>& children = value.type().id() == duckdb::LogicalTypeId::LIST
					? duckdb::ListValue::GetChildren(value)
					: duckdb::ArrayValue::GetChildren(value);

		QVariantList list;
		list.reserve(static_cast<qsizetype>(children.size()));
		for (const duckdb::Value& child : children){
			list.append(ConvertValue(child));
		}

		return list;
	}

	case duckdb::LogicalTypeId::STRUCT: {
		const duckdb::vector<duckdb::Value>& children = duckdb::StructValue::GetChildren(value);

		QVariantMap map;
		for (duckdb::idx_t i = 0; i < children.size(); ++ i){
			map.insert(QString::fromStdString(duckdb::StructType::GetChildName(value.type(), i)), ConvertValue(children[i]));
		}

		return map;
	}

	// UUID, TIMESTAMP, DATE and HUGEINT stay text, which is what the callers read them as.
	default:
		return QVariant(QString::fromStdString(value.ToString()));
	}
}


} // namespace imtduckdb
