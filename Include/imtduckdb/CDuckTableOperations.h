// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QString>

// ImtCore includes
#include <imtdb/IDatabaseEngine.h>


namespace imtduckdb
{


/**
	Atomically replaces \a liveTableName with the freshly built \a shadowTableName by renaming,
	inside a single transaction on \a engine. Readers on other connections keep seeing the old table
	until the transaction commits, and the live table is never dropped before its replacement is in
	place; on any failure everything is rolled back and the live table is left untouched.

	Handles the first-ever build (no live table yet) by renaming the shadow table straight into
	place. DuckDB has no nested transactions, so pass \a isTransactionActive to join a transaction
	the caller already opened instead of opening a second one.
*/
bool SwapTable(
			imtdb::IDatabaseEngine& engine,
			bool isTransactionActive,
			const QString& liveTableName,
			const QString& shadowTableName,
			QString* errorMessagePtr = nullptr);


} // namespace imtduckdb
