// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtduckdb/CDuckAppender.h>


// ImtCore includes
#include <imtduckdb/CDuckValueConverter.h>


namespace imtduckdb
{


CDuckAppender::CDuckAppender(duckdb_appender appender)
	:m_appender(appender)
{
}


CDuckAppender::~CDuckAppender()
{
	duckdb_appender_destroy(&m_appender);
}


// public static methods

std::unique_ptr<IDuckAppender> CDuckAppender::Create(
			duckdb_connection connection,
			const QString& tableName,
			const QString& schemaName,
			QString* errorMessagePtr)
{
	const QByteArray table = tableName.toUtf8();
	const QByteArray schema = schemaName.toUtf8();

	duckdb_appender appender = nullptr;
	const duckdb_state state = duckdb_appender_create_ext(
				connection,
				nullptr,
				schemaName.isEmpty() ? nullptr : schema.constData(),
				table.constData(),
				&appender);
	if (state == DuckDBSuccess){
		return std::unique_ptr<IDuckAppender>(new CDuckAppender(appender));
	}

	if (errorMessagePtr != nullptr){
		*errorMessagePtr = GetAppenderError(appender);
	}

	duckdb_appender_destroy(&appender);

	return nullptr;
}


// reimplemented (IDuckAppender)

bool CDuckAppender::AppendRow(const QVariantList& rowValues)
{
	if (m_closed){
		m_lastError = QStringLiteral("Appender is already closed");

		return false;
	}

	for (const QVariant& value : rowValues){
		duckdb_value duckValue = CDuckValueConverter::ToDuckDbValue(value);
		if (duckValue == nullptr){
			m_lastError = QStringLiteral("Value '%1' cannot be converted to a DuckDB value").arg(value.toString());

			return false;
		}

		const duckdb_state state = duckdb_append_value(m_appender, duckValue);

		duckdb_destroy_value(&duckValue);

		if (state != DuckDBSuccess){
			m_lastError = GetAppenderError(m_appender);

			return false;
		}
	}

	if (duckdb_appender_end_row(m_appender) != DuckDBSuccess){
		m_lastError = GetAppenderError(m_appender);

		return false;
	}

	return true;
}


bool CDuckAppender::Flush()
{
	if (m_closed){
		m_lastError = QStringLiteral("Appender is already closed");

		return false;
	}

	if (duckdb_appender_flush(m_appender) != DuckDBSuccess){
		m_lastError = GetAppenderError(m_appender);

		return false;
	}

	return true;
}


bool CDuckAppender::Close()
{
	if (m_closed){
		return true;
	}

	m_closed = true;

	if (duckdb_appender_close(m_appender) != DuckDBSuccess){
		m_lastError = GetAppenderError(m_appender);

		return false;
	}

	return true;
}


QString CDuckAppender::GetLastError() const
{
	return m_lastError;
}


// private static methods

QString CDuckAppender::GetAppenderError(duckdb_appender appender)
{
	duckdb_error_data errorData = duckdb_appender_error_data(appender);

	QString retVal = QString::fromUtf8(duckdb_error_data_message(errorData));
	if (retVal.isEmpty()){
		retVal = QStringLiteral("DuckDB appender failed");
	}

	duckdb_destroy_error_data(&errorData);

	return retVal;
}


} // namespace imtduckdb

