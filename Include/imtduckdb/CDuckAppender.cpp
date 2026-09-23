// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtduckdb/CDuckAppender.h>


// ImtCore includes
#include <imtduckdb/CDuckValueConverter.h>


namespace imtduckdb
{


CDuckAppender::CDuckAppender(std::unique_ptr<duckdb::Appender> appenderPtr)
	:m_appenderPtr(std::move(appenderPtr))
{
}


// static methods

std::unique_ptr<IDuckAppender> CDuckAppender::Create(
			duckdb::Connection& connection,
			const QString& tableName,
			const QString& schemaName,
			QString* errorMessagePtr)
{
	try{
		std::unique_ptr<duckdb::Appender> appenderPtr = schemaName.isEmpty()
					? std::make_unique<duckdb::Appender>(connection, tableName.toStdString())
					: std::make_unique<duckdb::Appender>(connection, schemaName.toStdString(), tableName.toStdString());

		return std::unique_ptr<IDuckAppender>(new CDuckAppender(std::move(appenderPtr)));
	}
	catch (const std::exception& exception){
		if (errorMessagePtr != nullptr){
			*errorMessagePtr = QString::fromUtf8(exception.what());
		}

		return nullptr;
	}
}


// reimplemented (IDuckAppender)

bool CDuckAppender::AppendRow(const QVariantList& rowValues)
{
	if (m_closed){
		m_lastError = QStringLiteral("Appender is already closed");

		return false;
	}

	try{
		m_appenderPtr->BeginRow();

		for (const QVariant& value : rowValues){
			m_appenderPtr->Append(CDuckValueConverter::ToDuckDbValue(value));
		}

		m_appenderPtr->EndRow();
	}
	catch (const std::exception& exception){
		m_lastError = QString::fromUtf8(exception.what());

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

	try{
		m_appenderPtr->Flush();
	}
	catch (const std::exception& exception){
		m_lastError = QString::fromUtf8(exception.what());

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

	try{
		m_appenderPtr->Close();
	}
	catch (const std::exception& exception){
		m_lastError = QString::fromUtf8(exception.what());

		return false;
	}

	return true;
}


QString CDuckAppender::GetLastError() const
{
	return m_lastError;
}


} // namespace imtduckdb
