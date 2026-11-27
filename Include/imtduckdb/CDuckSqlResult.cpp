// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtduckdb/CDuckSqlResult.h>


// STL includes
#include <climits>
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
	ClearResult();
	duckdb_destroy_prepare(&m_preparedStatement);
}


// reimplemented (QSqlResult)

bool CDuckSqlResult::reset(const QString& sqlQuery)
{
	ClearResult();

	const duckdb_state state = duckdb_query(m_connection, sqlQuery.toUtf8().constData(), &m_result);

	return AcceptResult(state);
}


bool CDuckSqlResult::prepare(const QString& sqlQuery)
{
	ClearResult();

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
	ClearResult();

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

	const duckdb_state state = duckdb_execute_prepared(m_preparedStatement, &m_result);

	return AcceptResult(state);
}


void CDuckSqlResult::detachFromResultSet()
{
	ClearResult();
}


bool CDuckSqlResult::fetch(int index)
{
	if (index < 0 || !isActive()){
		return false;
	}

	const idx_t row = static_cast<idx_t>(index);
	while (row >= m_fetchedRows){
		if (!FetchNextChunk(isForwardOnly())){
			return false;
		}
	}

	if (m_currentChunk == nullptr || row < m_chunkStart || row - m_chunkStart >= duckdb_data_chunk_get_size(m_currentChunk)){
		auto chunk = m_chunks.upperBound(row);
		if (chunk == m_chunks.begin()){
			return false;
		}

		--chunk;
		m_currentChunk = chunk.value();
		m_chunkStart = chunk.key();

		// size() can have read ahead while preserving the cursor.
		if (isForwardOnly()){
			DiscardChunksBefore(m_chunkStart);
		}
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
	while (FetchNextChunk(isForwardOnly())){
	}

	return isActive() && m_fetchedRows > 0 && m_fetchedRows - 1 <= INT_MAX && fetch(static_cast<int>(m_fetchedRows - 1));
}


QVariant CDuckSqlResult::data(int index)
{
	return CurrentValue(index);
}


bool CDuckSqlResult::isNull(int index)
{
	if (index < 0 || index >= m_columnTypes.size() || at() < 0 || m_currentChunk == nullptr){
		return true;
	}

	duckdb_vector vector = duckdb_data_chunk_get_vector(m_currentChunk, static_cast<idx_t>(index));
	return !duckdb_validity_row_is_valid(duckdb_vector_get_validity(vector), static_cast<idx_t>(at()) - m_chunkStart)
			|| duckdb_get_type_id(m_columnTypes.at(index)) == DUCKDB_TYPE_SQLNULL;
}


int CDuckSqlResult::size()
{
	// The C chunk API reports the exact size only after the final chunk.
	while (FetchNextChunk(false)){
	}

	return isActive() && m_fetchedRows <= INT_MAX ? static_cast<int>(m_fetchedRows) : -1;
}


int CDuckSqlResult::numRowsAffected()
{
	return m_resultType == DUCKDB_RESULT_TYPE_CHANGED_ROWS ? static_cast<int>(m_rowsChanged) : -1;
}


QSqlRecord CDuckSqlResult::record() const
{
	QSqlRecord recordVal = m_record;

	if (at() < 0 || m_currentChunk == nullptr){
		return recordVal;
	}

	for (int i = 0; i < recordVal.count(); ++ i){
		recordVal.setValue(i, CurrentValue(i));
	}

	return recordVal;
}


// private methods

bool CDuckSqlResult::AcceptResult(duckdb_state state)
{
	if (state != DuckDBSuccess){
		const char* errorPtr = duckdb_result_error(&m_result);
		const QString errorText = errorPtr != nullptr ? QString::fromUtf8(errorPtr) : QStringLiteral("DuckDB statement could not be executed");
		ClearResult();
		setLastError(QSqlError(QStringLiteral("Unable to execute statement"), errorText, QSqlError::StatementError));

		return false;
	}

	const idx_t columnCount = duckdb_column_count(&m_result);
	for (idx_t i = 0; i < columnCount; ++i){
		const QString name = QString::fromUtf8(duckdb_column_name(&m_result, i));
		duckdb_logical_type type = duckdb_column_logical_type(&m_result, i);
		m_columnTypes.append(type);

		const QMetaType metaType = ConvertMetaType(type);
		if (!metaType.isValid()){
			ClearResult();
			setLastError(QSqlError(QStringLiteral("Unable to read the statement result"),
						QStringLiteral("Column '%1' has a type without a Qt counterpart. Cast it in the query").arg(name),
						QSqlError::StatementError));

			return false;
		}

		m_record.append(QSqlField(name, metaType));
	}

	m_resultType = duckdb_result_return_type(m_result);
	m_rowsChanged = duckdb_rows_changed(&m_result);
	m_exhausted = false;
	setSelect(m_resultType == DUCKDB_RESULT_TYPE_QUERY_RESULT);
	setActive(true);

	return true;
}


void CDuckSqlResult::ClearResult()
{
	m_currentChunk = nullptr;
	DiscardChunksBefore(m_fetchedRows);

	for (auto& type : m_columnTypes){
		duckdb_destroy_logical_type(&type);
	}
	m_columnTypes.clear();

	duckdb_destroy_result(&m_result);
	m_result = {};
	m_record.clear();
	m_fetchedRows = 0;
	m_exhausted = true;
	m_resultType = DUCKDB_RESULT_TYPE_INVALID;
	m_rowsChanged = 0;
	setActive(false);
	setAt(QSql::BeforeFirstRow);
}


void CDuckSqlResult::DiscardChunksBefore(idx_t firstRow)
{
	while (!m_chunks.isEmpty() && m_chunks.firstKey() < firstRow){
		duckdb_destroy_data_chunk(&m_chunks.first());
		m_chunks.erase(m_chunks.begin());
	}
}


bool CDuckSqlResult::FetchNextChunk(bool releasePrevious)
{
	while (!m_exhausted){
		duckdb_data_chunk chunk = duckdb_fetch_chunk(m_result);
		if (chunk == nullptr){
			const char* errorPtr = duckdb_result_error(&m_result);
			if (errorPtr != nullptr){
				const QString message = QString::fromUtf8(errorPtr);
				ClearResult();
				setLastError(QSqlError(QStringLiteral("Unable to fetch rows"), message, QSqlError::StatementError));

				return false;
			}

			m_exhausted = true;
			break;
		}

		const idx_t count = duckdb_data_chunk_get_size(chunk);
		if (count == 0){
			duckdb_destroy_data_chunk(&chunk);
			continue;
		}

		if (releasePrevious){
			m_currentChunk = nullptr;
			DiscardChunksBefore(m_fetchedRows);
		}

		m_chunks.insert(m_fetchedRows, chunk);
		m_fetchedRows += count;
		return true;
	}

	return false;
}


QVariant CDuckSqlResult::CurrentValue(int column) const
{
	if (column < 0 || column >= m_columnTypes.size() || at() < 0 || m_currentChunk == nullptr){
		return QVariant();
	}

	duckdb_vector vector = duckdb_data_chunk_get_vector(m_currentChunk, static_cast<idx_t>(column));
	return ReadValue(vector, m_columnTypes.at(column), static_cast<idx_t>(at()) - m_chunkStart);
}


QVariant CDuckSqlResult::ReadValue(duckdb_vector vector, duckdb_logical_type type, idx_t row)
{
	if (!duckdb_validity_row_is_valid(duckdb_vector_get_validity(vector), row)){
		return QVariant();
	}

	const duckdb_type typeId = duckdb_get_type_id(type);
	if (typeId == DUCKDB_TYPE_LIST || typeId == DUCKDB_TYPE_ARRAY){
		const bool isArray = typeId == DUCKDB_TYPE_ARRAY;
		idx_t offset;
		idx_t count;
		duckdb_vector child;
		duckdb_logical_type childType;
		if (isArray){
			count = duckdb_array_type_array_size(type);
			offset = row * count;
			child = duckdb_array_vector_get_child(vector);
			childType = duckdb_array_type_child_type(type);
		}
		else{
			const auto entries = static_cast<const duckdb_list_entry*>(duckdb_vector_get_data(vector));
			offset = entries[row].offset;
			count = entries[row].length;
			child = duckdb_list_vector_get_child(vector);
			childType = duckdb_list_type_child_type(type);
		}

		QVariantList retVal;
		retVal.reserve(static_cast<qsizetype>(count));
		for (idx_t i = 0; i < count; ++i){
			retVal.append(ReadValue(child, childType, offset + i));
		}
		duckdb_destroy_logical_type(&childType);

		return retVal;
	}

	if (typeId == DUCKDB_TYPE_STRUCT){
		QVariantMap retVal;
		const idx_t count = duckdb_struct_type_child_count(type);
		for (idx_t i = 0; i < count; ++i){
			char* name = duckdb_struct_type_child_name(type, i);
			duckdb_logical_type childType = duckdb_struct_type_child_type(type, i);
			retVal.insert(QString::fromUtf8(name), ReadValue(duckdb_struct_vector_get_child(vector, i), childType, row));
			duckdb_destroy_logical_type(&childType);
			duckdb_free(name);
		}

		return retVal;
	}

	return ReadScalarValue(duckdb_vector_get_data(vector), type, row);
}


QString CDuckSqlResult::ReadValueText(duckdb_value value)
{
	char* text = duckdb_get_varchar(value);
	const QString retVal = QString::fromUtf8(text);
	duckdb_free(text);
	duckdb_destroy_value(&value);

	return retVal;
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

		if (value.upper == 0){
			return static_cast<qulonglong>(value.lower);
		}

		return ReadValueText(duckdb_create_hugeint(value));
	}

	case DUCKDB_TYPE_UHUGEINT: {
		const duckdb_uhugeint value = static_cast<const duckdb_uhugeint*>(dataPtr)[row];
		if (value.upper == 0){
			return static_cast<qulonglong>(value.lower);
		}

		return ReadValueText(duckdb_create_uhugeint(value));
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
	case DUCKDB_TYPE_TIMESTAMP_TZ: {
		const auto value = static_cast<const duckdb_timestamp*>(dataPtr)[row];
		if (!duckdb_is_finite_timestamp(value)){
			return ReadValueText(duckdb_create_timestamp(value));
		}

		return QDateTime::fromMSecsSinceEpoch(value.micros / 1000 - (value.micros % 1000 < 0), QTimeZone::utc());
	}

	case DUCKDB_TYPE_TIMESTAMP_S: {
		const auto value = static_cast<const duckdb_timestamp_s*>(dataPtr)[row];
		if (!duckdb_is_finite_timestamp_s(value) || value.seconds > INT64_MAX / 1000 || value.seconds < INT64_MIN / 1000){
			return ReadValueText(duckdb_create_timestamp_s(value));
		}

		return QDateTime::fromMSecsSinceEpoch(value.seconds * 1000, QTimeZone::utc());
	}

	case DUCKDB_TYPE_TIMESTAMP_MS: {
		const auto value = static_cast<const duckdb_timestamp_ms*>(dataPtr)[row];
		if (!duckdb_is_finite_timestamp_ms(value)){
			return ReadValueText(duckdb_create_timestamp_ms(value));
		}

		return QDateTime::fromMSecsSinceEpoch(value.millis, QTimeZone::utc());
	}

	case DUCKDB_TYPE_TIMESTAMP_NS:
	case DUCKDB_TYPE_TIMESTAMP_TZ_NS: {
		const auto value = static_cast<const duckdb_timestamp_ns*>(dataPtr)[row];
		if (!duckdb_is_finite_timestamp_ns(value)){
			return ReadValueText(duckdb_create_timestamp_ns(value));
		}

		return QDateTime::fromMSecsSinceEpoch(value.nanos / 1000000 - (value.nanos % 1000000 < 0), QTimeZone::utc());
	}

	/**
		An untyped NULL keeps the NULL type and has no data.
	*/
	default:
		return QVariant();
	}
}


QMetaType CDuckSqlResult::ConvertMetaType(duckdb_logical_type type)
{
	const duckdb_type typeId = duckdb_get_type_id(type);
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
		return QMetaType(QMetaType::LongLong);

	case DUCKDB_TYPE_UBIGINT:
		return QMetaType(QMetaType::ULongLong);

	case DUCKDB_TYPE_HUGEINT:
	case DUCKDB_TYPE_UHUGEINT:
		return QMetaType(QMetaType::QVariant);

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
	case DUCKDB_TYPE_ARRAY: {
		duckdb_logical_type child = typeId == DUCKDB_TYPE_ARRAY ? duckdb_array_type_child_type(type) : duckdb_list_type_child_type(type);
		const bool supported = ConvertMetaType(child).isValid();
		duckdb_destroy_logical_type(&child);
		return supported ? QMetaType(QMetaType::QVariantList) : QMetaType();
	}

	case DUCKDB_TYPE_STRUCT: {
		const idx_t count = duckdb_struct_type_child_count(type);
		for (idx_t i = 0; i < count; ++i){
			duckdb_logical_type child = duckdb_struct_type_child_type(type, i);
			const bool supported = ConvertMetaType(child).isValid();
			duckdb_destroy_logical_type(&child);
			if (!supported){
				return QMetaType();
			}
		}
		return QMetaType(QMetaType::QVariantMap);
	}

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

