// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtduckdb/CDuckSqlDriver.h>


// ImtCore includes
#include <imtduckdb/CDuckSqlResult.h>


namespace imtduckdb
{


CDuckSqlDriver::CDuckSqlDriver(duckdb_connection connection)
	:m_connection(connection)
{
	setOpen(true);
	setOpenError(false);
}


bool CDuckSqlDriver::hasFeature(DriverFeature feature) const
{
	switch (feature){
	case QuerySize:
	case PreparedQueries:
	case NamedPlaceholders:
		return true;
	default:
		return false;
	}
}


bool CDuckSqlDriver::open(
			const QString& /*db*/,
			const QString& /*user*/,
			const QString& /*password*/,
			const QString& /*host*/,
			int /*port*/,
			const QString& /*connOpts*/)
{
	return isOpen();
}


void CDuckSqlDriver::close()
{
	setOpen(false);
}


QSqlResult* CDuckSqlDriver::createResult() const
{
	return new CDuckSqlResult(this, m_connection);
}


bool CDuckSqlDriver::ExecuteStatement(const char* statement, QString* errorMessagePtr) const
{
	duckdb_result result;
	const bool retVal = duckdb_query(m_connection, statement, &result) == DuckDBSuccess;

	if (!retVal && errorMessagePtr != nullptr){
		const char* errorPtr = duckdb_result_error(&result);

		*errorMessagePtr = errorPtr != nullptr ? QString::fromUtf8(errorPtr) : QStringLiteral("unknown error");
	}

	duckdb_destroy_result(&result);

	return retVal;
}


} // namespace imtduckdb

