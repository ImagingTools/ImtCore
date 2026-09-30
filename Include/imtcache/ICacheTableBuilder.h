// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QString>

// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtduckdb/IDuckConnection.h>


namespace imtcache
{


/**
	Builds one cache table. CCacheBuilderComp owns a list of these and runs them in dependency
	order, so adding an entity to the cache means adding one small component rather than another
	branch inside the builder.

	Implementations come in two kinds:
	- source mirrors (Device, AddressElement, Abonent, ...) read their rows from a PostgreSQL
	  collection and depend on nothing else;
	- derived tables (AddressClosure, MapCluster, UserVisible*) are computed in SQL from tables
	  already in the cache and declare those through GetRequiredCacheTables(). They must not go back
	  to PostgreSQL: joining locally is what the columnar store is for.
*/
class ICacheTableBuilder: virtual public istd::IPolymorphic
{
public:
	struct BuildResult
	{
		bool isOk = false;
		bool wasFullRebuild = false;
		int rowsWritten = 0;
		int rowsDeleted = 0;
		/// Newest source modification time covered by this run; stored as the next run's starting point.
		QDateTime lastSourceUpdateTime;
		QString errorMessage;
	};

	/// Name of the cache table maintained by this builder.
	virtual QString GetCacheTableName() const = 0;

	/// Cache tables that must be up to date before this one can be built.
	virtual QStringList GetRequiredCacheTables() const = 0;

	/// Rebuilds the table from scratch. Readers keep seeing the previous contents until it completes.
	virtual BuildResult Rebuild(imtduckdb::IDuckConnection& connection) const = 0;

	/**
		Applies only what changed after \a lastSourceUpdateTime. Callers fall back to Rebuild() when
		no previous update time is known.
	*/
	virtual BuildResult ApplyChanges(imtduckdb::IDuckConnection& connection, const QDateTime& lastSourceUpdateTime) const = 0;
};


} // namespace imtcache
