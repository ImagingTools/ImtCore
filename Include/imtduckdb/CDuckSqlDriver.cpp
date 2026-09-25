// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtduckdb/CDuckSqlDriver.h>


// ImtCore includes
#include <imtduckdb/CDuckSqlResult.h>


namespace imtduckdb
{


CDuckSqlDriver::CDuckSqlDriver(duckdb::Connection& connection)
	:m_connection(connection)
{
	setOpen(true);
	setOpenError(false);
}


bool CDuckSqlDriver::hasFeature(DriverFeature feature) const
{
	switch (feature){
	case QuerySize:
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


duckdb::Connection& CDuckSqlDriver::GetConnection() const
{
	return m_connection;
}


} // namespace imtduckdb

