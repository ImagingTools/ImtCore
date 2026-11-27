// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QVariant>

// 3rdParty includes
#include <duckdb.h>


namespace imtduckdb
{


/**
	Shared QVariant -> DuckDB value conversion, used by both CDuckSqlResult (bound query
	parameters) and CDuckAppender (bulk row appends) so the two code paths cannot drift apart.
*/
class CDuckValueConverter
{
public:
	/**
		Converts \a value to a DuckDB value, which the caller destroys with duckdb_destroy_value(). A null QVariant
		becomes a NULL value, and a QVariantList or QStringList becomes a LIST of its converted items.
		\return nullptr if the value cannot be converted, e.g. when the items of a list cannot share a type.
	*/
	static duckdb_value ToDuckDbValue(const QVariant& value);
};


} // namespace imtduckdb

