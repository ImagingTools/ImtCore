// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QCryptographicHash>
#include <QtCore/QString>
#include <QtCore/QUuid>


namespace imtcache
{


/// Bookkeeping tables of the cache.
struct CacheTable
{
	static const inline QString CACHE_REVISION = QStringLiteral("CacheRevision");

	/// The keys of the rows the mirrors changed, for the tables derived from them.
	static const inline QString CACHE_CHANGE = QStringLiteral("CacheChange");

	/// How far each consumer of CACHE_CHANGE has read.
	static const inline QString CACHE_CHANGE_CURSOR = QStringLiteral("CacheChangeCursor");
};


/// Orders the rows of CacheTable::CACHE_CHANGE.
struct CacheChangeSequence
{
	static const inline QString NAME = QStringLiteral("CacheChangeSeq");
};


/// Column names of the CacheTable::CACHE_CHANGE table.
struct CacheChangeColumn
{
	static const inline QString CHANGE_ID	= QStringLiteral("ChangeId");
	static const inline QString TABLE_NAME	= QStringLiteral("TableName");

	/// Surrogate id of the row; NULL says the whole table was rebuilt, so any row of it may have changed.
	static const inline QString KEY_ID		= QStringLiteral("KeyId");
};


/// Column names of the CacheTable::CACHE_CHANGE_CURSOR table.
struct CacheChangeCursorColumn
{
	static const inline QString CONSUMER			= QStringLiteral("Consumer");
	static const inline QString LAST_CHANGE_ID	= QStringLiteral("LastChangeId");
};


/// Column names of the CacheTable::CACHE_REVISION table.
struct CacheRevisionColumn
{
	static const inline QString TABLE_NAME		= QStringLiteral("TableName");
	static const inline QString LAST_REVISION	= QStringLiteral("LastRevision");
};


/// Surrogate id meaning "no parent" in hierarchical cache tables.
inline constexpr quint64 ROOT_PARENT_ID = 0;


/// Duration for log lines: tenths of a second below a minute, whole seconds above.
inline QString FormatDuration(qint64 milliseconds)
{
	if (milliseconds < 60000){
		return QStringLiteral("%1 s").arg(milliseconds / 1000.0, 0, 'f', 1);
	}

	return QStringLiteral("%1 min %2 s").arg(milliseconds / 60000).arg(milliseconds % 60000 / 1000);
}


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


/// Surrogate id of the source object \a sourceId refers to, or 0 ("refers to nothing") when it is empty or not a UUID.
inline quint64 DeriveReferenceId(const QByteArray& sourceId)
{
	const QUuid uuid = QUuid::fromString(QString::fromUtf8(sourceId));
	if (uuid.isNull()){
		return 0;
	}

	return DeriveSurrogateId(uuid.toByteArray(QUuid::WithoutBraces));
}


} // namespace imtcache
