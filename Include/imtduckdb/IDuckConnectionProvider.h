// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <memory>

// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtduckdb/IDuckConnection.h>


namespace imtduckdb
{


/**
	Hands out exclusively-owned connections to a DuckDB database. Implemented by
	CDuckDatabaseEngineComp and registered separately so callers that need their own connection -
	one per request, or a cache builder running off-thread - can depend on just this instead of the
	concrete component type.
*/
class IDuckConnectionProvider: virtual public istd::IPolymorphic
{
public:
	/**
		\return a connection able to run queries, DDL, bulk appends and table swaps independently of
				- and concurrently with - the provider's own connection, or nullptr if the database
				could not be opened.
	*/
	virtual std::unique_ptr<IDuckConnection> CreateConnection() const = 0;
};


} // namespace imtduckdb
