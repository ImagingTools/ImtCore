// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtduckdb/CDuckSqlResult.h>


// STL includes
#include <cmath>

// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QTimeZone>
#include <QtCore/QUuid>
#include <QtCore/QtEndian>
#include <QtSql/QSqlError>
#include <QtSql/QSqlField>

// ImtCore includes
#include <imtduckdb/CDuckSqlDriver.h>
#include <imtduckdb/CDuckValueConverter.h>


namespace imtduckdb
{


CDuckSqlResult::CDuckSqlResult(const CDuckSqlDriver* driverPtr, duckdb_connection connection)
	:QSqlResult(driverPtr),
	m_connection(connection)
{
}


CDuckSqlResult::~CDuckSqlResult()
{
	duckdb_destroy_prepare(&m_preparedStatement);
}


// reimplemented (QSqlResult)

bool CDuckSqlResult::reset(const QString& sqlQuery)
{
	setActive(false);
	setAt(QSql::BeforeFirstRow);

	duckdb_result result;
	const duckdb_state state = duckdb_query(m_connection, sqlQuery.toUtf8().constData(), &result);

	return AcceptResult(state, result);
}


bool CDuckSqlResult::prepare(const QString& sqlQuery)
{
	/**
		Populates the base class' placeholder-name bookkeeping (used by bindValue()/boundValueNames()).
	*/
	if (!QSqlResult::prepare(sqlQuery)){
		return false;
	}

	duckdb_destroy_prepare(&m_preparedStatement);

	const QByteArray query = ToDuckDbNamedPlaceholders(sqlQuery).toUtf8();
	if (duckdb_prepare(m_connection, query.constData(), &m_preparedStatement) == DuckDBSuccess){
		return true;
	}

	const char* errorPtr = duckdb_prepare_error(m_preparedStatement);
	const QString errorText = errorPtr != nullptr ? QString::fromUtf8(errorPtr) : QStringLiteral("DuckDB statement could not be prepared");

	duckdb_destroy_prepare(&m_preparedStatement);

	setLastError(QSqlError(
				QStringLiteral("Unable to prepare statement"),
				errorText,
				QSqlError::StatementError));

	return false;
}


bool CDuckSqlResult::exec()
{
	setActive(false);
	setAt(QSql::BeforeFirstRow);

	if (m_preparedStatement == nullptr){
		setLastError(QSqlError(
					QStringLiteral("Unable to execute statement"),
					QStringLiteral("Statement was not prepared"),
					QSqlError::StatementError));

		return false;
	}

	duckdb_clear_bindings(m_preparedStatement);

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

		idx_t parameterIndex = 0;
		duckdb_value value = nullptr;

		bool isBound = duckdb_bind_parameter_index(m_preparedStatement, &parameterIndex, name.toUtf8().constData()) == DuckDBSuccess;
		if (isBound){
			value = CDuckValueConverter::ToDuckDbValue(values.at(i));
			isBound = value != nullptr && duckdb_bind_value(m_preparedStatement, parameterIndex, value) == DuckDBSuccess;
		}

		duckdb_destroy_value(&value);

		if (!isBound){
			setLastError(QSqlError(
						QStringLiteral("Unable to execute statement"),
						QStringLiteral("Parameter '%1' could not be bound").arg(name),
						QSqlError::StatementError));

			return false;
		}
	}

	duckdb_result result;
	const duckdb_state state = duckdb_execute_prepared(m_preparedStatement, &result);

	return AcceptResult(state, result);
}


bool CDuckSqlResult::fetch(int index)
{
	if (m_resultType == DUCKDB_RESULT_TYPE_INVALID || index < 0 || index >= m_rowCount){
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
	return fetch(static_cast<int>(m_rowCount) - 1);
}


QVariant CDuckSqlResult::data(int index)
{
	if (index < 0 || index >= m_columns.size() || at() < 0 || at() >= m_rowCount){
		return QVariant();
	}

	return m_columns.at(index).at(at());
}


bool CDuckSqlResult::isNull(int index)
{
	return !data(index).isValid();
}


int CDuckSqlResult::size()
{
	return m_resultType == DUCKDB_RESULT_TYPE_INVALID ? -1 : static_cast<int>(m_rowCount);
}


int CDuckSqlResult::numRowsAffected()
{
	return m_resultType == DUCKDB_RESULT_TYPE_CHANGED_ROWS ? static_cast<int>(m_rowsChanged) : -1;
}


QSqlRecord CDuckSqlResult::record() const
{
	QSqlRecord recordVal = m_record;

	if (at() < 0 || at() >= m_rowCount){
		return recordVal;
	}

	for (qsizetype i = 0; i < m_columns.size(); ++ i){
		recordVal.setValue(i, m_columns.at(i).at(at()));
	}

	return recordVal;
}


// private methods

bool CDuckSqlResult::AcceptResult(duckdb_state state, duckdb_result& result)
{
	m_resultType = DUCKDB_RESULT_TYPE_INVALID;
	m_rowsChanged = 0;
	m_rowCount = 0;
	m_record.clear();
	m_columns.clear();

	if (state != DuckDBSuccess){
		const char* errorPtr = duckdb_result_error(&result);
		const QString errorText = errorPtr != nullptr ? QString::fromUtf8(errorPtr) : QStringLiteral("DuckDB statement could not be executed");

		duckdb_destroy_result(&result);

		setLastError(QSqlError(
					QStringLiteral("Unable to execute statement"),
					errorText,
					QSqlError::StatementError));

		return false;
	}

	/**
		Fetching the rows turns the result into a streamed one, for which DuckDB no longer reports the changed rows.
	*/
	const duckdb_result_type resultType = duckdb_result_return_type(result);
	const idx_t rowsChanged = duckdb_rows_changed(&result);
	const idx_t columnCount = duckdb_column_count(&result);

	QList<duckdb_logical_type> columnTypes;
	QSqlRecord record;
	for (idx_t i = 0; i < columnCount; ++ i){
		duckdb_logical_type columnType = duckdb_column_logical_type(&result, i);

		columnTypes.append(columnType);
		record.append(QSqlField(QString::fromUtf8(duckdb_column_name(&result, i)), ConvertMetaType(duckdb_get_type_id(columnType))));
	}

	QList<QVariantList> columns(static_cast<qsizetype>(columnCount));
	qsizetype rowCount = 0;
	QString errorText;

	duckdb_data_chunk chunk = duckdb_fetch_chunk(result);
	while (chunk != nullptr){
		const idx_t chunkSize = duckdb_data_chunk_get_size(chunk);

		for (qsizetype i = 0; i < columns.size() && errorText.isEmpty(); ++ i){
			if (!ReadValues(duckdb_data_chunk_get_vector(chunk, static_cast<idx_t>(i)), columnTypes.at(i), 0, chunkSize, columns[i])){
				errorText = QStringLiteral("Column '%1' has a type without a Qt counterpart. Cast it in the query").arg(record.fieldName(i));
			}
		}

		rowCount += static_cast<qsizetype>(chunkSize);

		duckdb_destroy_data_chunk(&chunk);

		chunk = errorText.isEmpty() ? duckdb_fetch_chunk(result) : nullptr;
	}

	/**
		A failure while fetching is reported on the result.
	*/
	const char* fetchErrorPtr = duckdb_result_error(&result);
	if (errorText.isEmpty() && fetchErrorPtr != nullptr){
		errorText = QString::fromUtf8(fetchErrorPtr);
	}

	for (duckdb_logical_type columnType : std::as_const(columnTypes)){
		duckdb_destroy_logical_type(&columnType);
	}

	duckdb_destroy_result(&result);

	if (!errorText.isEmpty()){
		setLastError(QSqlError(
					QStringLiteral("Unable to read the statement result"),
					errorText,
					QSqlError::StatementError));

		return false;
	}

	m_resultType = resultType;
	m_rowsChanged = rowsChanged;
	m_rowCount = rowCount;
	m_record = record;
	m_columns = columns;

	setSelect(m_resultType == DUCKDB_RESULT_TYPE_QUERY_RESULT);
	setActive(true);

	return true;
}


bool CDuckSqlResult::ReadValues(
			duckdb_vector vector,
			duckdb_logical_type type,
			idx_t offset,
			idx_t count,
			QVariantList& values)
{
	const duckdb_type typeId = duckdb_get_type_id(type);
	if (!ConvertMetaType(typeId).isValid()){
		return false;
	}

	if (typeId == DUCKDB_TYPE_LIST || typeId == DUCKDB_TYPE_ARRAY){
		return ReadListValues(vector, type, offset, count, values);
	}

	if (typeId == DUCKDB_TYPE_STRUCT){
		return ReadStructValues(vector, type, offset, count, values);
	}

	const void* dataPtr = duckdb_vector_get_data(vector);
	uint64_t* validityPtr = duckdb_vector_get_validity(vector);

	for (idx_t row = offset; row < offset + count; ++ row){
		values.append(duckdb_validity_row_is_valid(validityPtr, row) ? ReadScalarValue(dataPtr, type, row) : QVariant());
	}

	return true;
}


bool CDuckSqlResult::ReadListValues(
			duckdb_vector vector,
			duckdb_logical_type type,
			idx_t offset,
			idx_t count,
			QVariantList& values)
{
	const bool isArray = duckdb_get_type_id(type) == DUCKDB_TYPE_ARRAY;

	duckdb_logical_type childType = isArray ? duckdb_array_type_child_type(type) : duckdb_list_type_child_type(type);
	const duckdb_vector childVector = isArray ? duckdb_array_vector_get_child(vector) : duckdb_list_vector_get_child(vector);
	const idx_t arraySize = isArray ? duckdb_array_type_array_size(type) : 0;
	const auto entriesPtr = static_cast<const duckdb_list_entry*>(duckdb_vector_get_data(vector));
	uint64_t* validityPtr = duckdb_vector_get_validity(vector);

	bool retVal = true;
	for (idx_t row = offset; retVal && row < offset + count; ++ row){
		if (!duckdb_validity_row_is_valid(validityPtr, row)){
			values.append(QVariant());

			continue;
		}

		const idx_t childOffset = isArray ? row * arraySize : entriesPtr[row].offset;
		const idx_t childCount = isArray ? arraySize : entriesPtr[row].length;

		QVariantList list;
		retVal = ReadValues(childVector, childType, childOffset, childCount, list);

		values.append(QVariant(list));
	}

	duckdb_destroy_logical_type(&childType);

	return retVal;
}


bool CDuckSqlResult::ReadStructValues(
			duckdb_vector vector,
			duckdb_logical_type type,
			idx_t offset,
			idx_t count,
			QVariantList& values)
{
	const idx_t childCount = duckdb_struct_type_child_count(type);

	QStringList childNames;
	QList<QVariantList> childValues(static_cast<qsizetype>(childCount));

	bool retVal = true;
	for (idx_t i = 0; retVal && i < childCount; ++ i){
		char* childNamePtr = duckdb_struct_type_child_name(type, i);
		childNames.append(QString::fromUtf8(childNamePtr));
		duckdb_free(childNamePtr);

		duckdb_logical_type childType = duckdb_struct_type_child_type(type, i);
		retVal = ReadValues(duckdb_struct_vector_get_child(vector, i), childType, offset, count, childValues[i]);
		duckdb_destroy_logical_type(&childType);
	}

	if (!retVal){
		return false;
	}

	uint64_t* validityPtr = duckdb_vector_get_validity(vector);

	for (idx_t row = 0; row < count; ++ row){
		if (!duckdb_validity_row_is_valid(validityPtr, offset + row)){
			values.append(QVariant());

			continue;
		}

		QVariantMap map;
		for (qsizetype i = 0; i < childNames.size(); ++ i){
			map.insert(childNames.at(i), childValues.at(i).at(static_cast<qsizetype>(row)));
		}

		values.append(map);
	}

	return true;
}


QVariant CDuckSqlResult::ReadScalarValue(
			const void* dataPtr,
			duckdb_logical_type type,
			idx_t row)
{
	switch (duckdb_get_type_id(type)){
	case DUCKDB_TYPE_BOOLEAN:
		return static_cast<const bool*>(dataPtr)[row];

	case DUCKDB_TYPE_TINYINT:
		return static_cast<int>(static_cast<const int8_t*>(dataPtr)[row]);

	case DUCKDB_TYPE_SMALLINT:
		return static_cast<int>(static_cast<const int16_t*>(dataPtr)[row]);

	case DUCKDB_TYPE_INTEGER:
		return static_cast<int>(static_cast<const int32_t*>(dataPtr)[row]);

	case DUCKDB_TYPE_UTINYINT:
		return static_cast<int>(static_cast<const uint8_t*>(dataPtr)[row]);

	case DUCKDB_TYPE_USMALLINT:
		return static_cast<int>(static_cast<const uint16_t*>(dataPtr)[row]);

	case DUCKDB_TYPE_BIGINT:
		return static_cast<qlonglong>(static_cast<const int64_t*>(dataPtr)[row]);

	case DUCKDB_TYPE_UINTEGER:
		return static_cast<qlonglong>(static_cast<const uint32_t*>(dataPtr)[row]);

	/**
		Surrogate ids use all 64 bits, which a signed type would overflow.
	*/
	case DUCKDB_TYPE_UBIGINT:
		return static_cast<qulonglong>(static_cast<const uint64_t*>(dataPtr)[row]);

	case DUCKDB_TYPE_FLOAT:
		return static_cast<const float*>(dataPtr)[row];

	case DUCKDB_TYPE_DOUBLE:
		return static_cast<const double*>(dataPtr)[row];

	case DUCKDB_TYPE_DECIMAL: {
		double unscaledValue = 0;

		switch (duckdb_decimal_internal_type(type)){
		case DUCKDB_TYPE_SMALLINT:
			unscaledValue = static_cast<const int16_t*>(dataPtr)[row];
			break;

		case DUCKDB_TYPE_INTEGER:
			unscaledValue = static_cast<const int32_t*>(dataPtr)[row];
			break;

		case DUCKDB_TYPE_BIGINT:
			unscaledValue = static_cast<double>(static_cast<const int64_t*>(dataPtr)[row]);
			break;

		default:
			unscaledValue = duckdb_hugeint_to_double(static_cast<const duckdb_hugeint*>(dataPtr)[row]);
			break;
		}

		return unscaledValue / std::pow(10.0, duckdb_decimal_scale(type));
	}

	/**
		SUM over integers yields a HUGEINT. It is read as a 64-bit integer where it fits.
	*/
	case DUCKDB_TYPE_HUGEINT: {
		const duckdb_hugeint value = static_cast<const duckdb_hugeint*>(dataPtr)[row];

		const bool isInt64 = (value.upper == 0 && value.lower <= static_cast<uint64_t>(INT64_MAX))
					|| (value.upper == -1 && value.lower > static_cast<uint64_t>(INT64_MAX));
		if (isInt64){
			return static_cast<qlonglong>(static_cast<int64_t>(value.lower));
		}

		return duckdb_hugeint_to_double(value);
	}

	case DUCKDB_TYPE_UHUGEINT: {
		const duckdb_uhugeint value = static_cast<const duckdb_uhugeint*>(dataPtr)[row];
		if (value.upper == 0){
			return static_cast<qulonglong>(value.lower);
		}

		return duckdb_uhugeint_to_double(value);
	}

	case DUCKDB_TYPE_VARCHAR: {
		duckdb_string_t text = static_cast<const duckdb_string_t*>(dataPtr)[row];

		return QString::fromUtf8(duckdb_string_t_data(&text), static_cast<qsizetype>(duckdb_string_t_length(text)));
	}

	/**
		The text form of a blob escapes every byte, so it cannot be read back.
	*/
	case DUCKDB_TYPE_BLOB: {
		duckdb_string_t bytes = static_cast<const duckdb_string_t*>(dataPtr)[row];

		return QByteArray(duckdb_string_t_data(&bytes), static_cast<qsizetype>(duckdb_string_t_length(bytes)));
	}

	/**
		DuckDB stores a UUID with the top bit flipped, so that UUIDs sort like their text. The text has no braces,
		as ids are compared as text.
	*/
	case DUCKDB_TYPE_UUID: {
		const duckdb_hugeint value = static_cast<const duckdb_hugeint*>(dataPtr)[row];

		QByteArray bytes(16, Qt::Uninitialized);
		qToBigEndian(static_cast<quint64>(value.upper) ^ (quint64(1) << 63), bytes.data());
		qToBigEndian(static_cast<quint64>(value.lower), bytes.data() + 8);

		return QUuid::fromRfc4122(bytes).toString(QUuid::WithoutBraces);
	}

	case DUCKDB_TYPE_ENUM: {
		idx_t index = 0;

		switch (duckdb_enum_internal_type(type)){
		case DUCKDB_TYPE_UTINYINT:
			index = static_cast<const uint8_t*>(dataPtr)[row];
			break;

		case DUCKDB_TYPE_USMALLINT:
			index = static_cast<const uint16_t*>(dataPtr)[row];
			break;

		default:
			index = static_cast<const uint32_t*>(dataPtr)[row];
			break;
		}

		char* textPtr = duckdb_enum_dictionary_value(type, index);
		const QString retVal = QString::fromUtf8(textPtr);
		duckdb_free(textPtr);

		return retVal;
	}

	case DUCKDB_TYPE_DATE:
		return QDate(1970, 1, 1).addDays(static_cast<const duckdb_date*>(dataPtr)[row].days);

	case DUCKDB_TYPE_TIME:
		return QTime::fromMSecsSinceStartOfDay(static_cast<int>(static_cast<const duckdb_time*>(dataPtr)[row].micros / 1000));

	/**
		A TIMESTAMP has no time zone. Its wall-clock value is kept and tagged as UTC.
	*/
	case DUCKDB_TYPE_TIMESTAMP:
	case DUCKDB_TYPE_TIMESTAMP_TZ:
		return QDateTime::fromMSecsSinceEpoch(static_cast<const duckdb_timestamp*>(dataPtr)[row].micros / 1000, QTimeZone::utc());

	case DUCKDB_TYPE_TIMESTAMP_S:
		return QDateTime::fromMSecsSinceEpoch(static_cast<const duckdb_timestamp_s*>(dataPtr)[row].seconds * 1000, QTimeZone::utc());

	case DUCKDB_TYPE_TIMESTAMP_MS:
		return QDateTime::fromMSecsSinceEpoch(static_cast<const duckdb_timestamp_ms*>(dataPtr)[row].millis, QTimeZone::utc());

	case DUCKDB_TYPE_TIMESTAMP_NS:
	case DUCKDB_TYPE_TIMESTAMP_TZ_NS:
		return QDateTime::fromMSecsSinceEpoch(static_cast<const duckdb_timestamp_ns*>(dataPtr)[row].nanos / 1000000, QTimeZone::utc());

	/**
		An untyped NULL keeps the NULL type and has no data.
	*/
	default:
		return QVariant();
	}
}


QMetaType CDuckSqlResult::ConvertMetaType(duckdb_type typeId)
{
	switch (typeId){
	case DUCKDB_TYPE_BOOLEAN:
		return QMetaType(QMetaType::Bool);

	case DUCKDB_TYPE_TINYINT:
	case DUCKDB_TYPE_SMALLINT:
	case DUCKDB_TYPE_INTEGER:
	case DUCKDB_TYPE_UTINYINT:
	case DUCKDB_TYPE_USMALLINT:
		return QMetaType(QMetaType::Int);

	case DUCKDB_TYPE_BIGINT:
	case DUCKDB_TYPE_UINTEGER:
	case DUCKDB_TYPE_HUGEINT:
		return QMetaType(QMetaType::LongLong);

	case DUCKDB_TYPE_UBIGINT:
	case DUCKDB_TYPE_UHUGEINT:
		return QMetaType(QMetaType::ULongLong);

	case DUCKDB_TYPE_FLOAT:
		return QMetaType(QMetaType::Float);

	case DUCKDB_TYPE_DOUBLE:
	case DUCKDB_TYPE_DECIMAL:
		return QMetaType(QMetaType::Double);

	case DUCKDB_TYPE_VARCHAR:
	case DUCKDB_TYPE_UUID:
	case DUCKDB_TYPE_ENUM:
		return QMetaType(QMetaType::QString);

	case DUCKDB_TYPE_BLOB:
		return QMetaType(QMetaType::QByteArray);

	case DUCKDB_TYPE_DATE:
		return QMetaType(QMetaType::QDate);

	case DUCKDB_TYPE_TIME:
		return QMetaType(QMetaType::QTime);

	case DUCKDB_TYPE_TIMESTAMP:
	case DUCKDB_TYPE_TIMESTAMP_S:
	case DUCKDB_TYPE_TIMESTAMP_MS:
	case DUCKDB_TYPE_TIMESTAMP_NS:
	case DUCKDB_TYPE_TIMESTAMP_TZ:
	case DUCKDB_TYPE_TIMESTAMP_TZ_NS:
		return QMetaType(QMetaType::QDateTime);

	case DUCKDB_TYPE_LIST:
	case DUCKDB_TYPE_ARRAY:
		return QMetaType(QMetaType::QVariantList);

	case DUCKDB_TYPE_STRUCT:
		return QMetaType(QMetaType::QVariantMap);

	case DUCKDB_TYPE_SQLNULL:
		return QMetaType(QMetaType::Nullptr);

	default:
		return QMetaType();
	}
}


QString CDuckSqlResult::ToDuckDbNamedPlaceholders(const QString& query)
{
	QString retVal;
	retVal.reserve(query.size());

	QChar closingQuote;
	const qsizetype queryLength = query.size();

	for (qsizetype i = 0; i < queryLength; ++ i){
		const QChar ch = query.at(i);

		if (!closingQuote.isNull()){
			if (ch == closingQuote){
				closingQuote = QChar();
			}

			retVal += ch;
			continue;
		}

		const bool isPlaceholderStart = ch == QLatin1Char(':')
					&& (i == 0 || query.at(i - 1) != QLatin1Char(':'))
					&& (i + 1 < queryLength && (query.at(i + 1).isLetterOrNumber() || query.at(i + 1) == QLatin1Char('_')));

		if (isPlaceholderStart){
			retVal += QLatin1Char('$');
			continue;
		}

		if (ch == QLatin1Char('\'') || ch == QLatin1Char('"') || ch == QLatin1Char('`')){
			closingQuote = ch;
		}

		retVal += ch;
	}

	return retVal;
}


} // namespace imtduckdb

