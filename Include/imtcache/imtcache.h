// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QCryptographicHash>
#include <QtCore/QString>


namespace imtcache
{


/// Bookkeeping table recording how far each cache table has been brought up to date.
struct CacheTable
{
	static const inline QString CACHE_REVISION = QStringLiteral("CacheRevision");
};


/// Column names of the CacheTable::CACHE_REVISION table.
struct CacheRevisionColumn
{
	static const inline QString TABLE_NAME		= QStringLiteral("TableName");
	static const inline QString LAST_REVISION	= QStringLiteral("LastRevision");
};


/// Surrogate id meaning "no parent" in hierarchical cache tables.
inline constexpr quint64 ROOT_PARENT_ID = 0;


/**
	Deterministically derives a UBIGINT surrogate id from a source object's UUID (\a documentId, as
	returned by imtbase::IIdentifiable::GetObjectUuid()). Cache tables keep a stable-across-rebuilds
	surrogate id without needing a two-pass builder that first allocates ids and only then resolves
	parent/foreign-key references - the same UUID always maps to the same surrogate, computed
	independently for each row.

	IMPORTANT: \a documentId must always be the UUID's normalized `QUuid::WithoutBraces` text form
	(i.e. `QUuid(...).toByteArray(QUuid::WithoutBraces)`), never a raw/unnormalized string - callers
	on both the write path (cache builders) and read path (controllers resolving a parent UUID to a
	filter value) must normalize the same way, or the same logical UUID would hash to different
	surrogate ids depending on incidental casing/braces differences between call sites.
*/
inline quint64 DeriveSurrogateId(const QByteArray& documentId)
{
	const QByteArray hash = QCryptographicHash::hash(documentId, QCryptographicHash::Sha256);

	quint64 retVal = 0;
	for (int i = 0; i < 8; ++ i){
		retVal = (retVal << 8) | static_cast<quint8>(hash.at(i));
	}

	return retVal;
}


} // namespace imtcache
