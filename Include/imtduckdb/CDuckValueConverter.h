// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QVariant>

// 3rdParty includes
#include <duckdb.hpp>


namespace imtduckdb
{


/**
	Shared QVariant <-> duckdb::Value conversion, used by both CDuckSqlResult (bound query
	parameters) and CDuckAppender (bulk row appends) so the two code paths cannot drift apart.
*/
class CDuckValueConverter
{
public:
	//! Converts \a value to a duckdb::Value. A null QVariant becomes a duckdb NULL value.
	static duckdb::Value ToDuckDbValue(const QVariant& value);
};


} // namespace imtduckdb
