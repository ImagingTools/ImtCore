// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QString>

// ACF includes
#include <istd/IChangeable.h>


namespace imtdb
{


/**
	Backup and restore of the physical storage of a single tenant.
*/
class ITenantStorageBackup: virtual public istd::IChangeable
{
public:
	/**
		Write a backup of the storage of the given tenant into the file.
		\return \c true if the backup file was written completely.
	*/
	virtual bool BackupTenantStorage(const QByteArray& tenantId, const QString& filePath) = 0;

	/**
		Replace the storage content of the given tenant with the content of the backup file.
		The backup must have been created from the storage of the same tenant.
		On failure the previous storage content is kept.
		\return \c true if the storage was restored.
	*/
	virtual bool RestoreTenantStorage(const QByteArray& tenantId, const QString& filePath) = 0;
};


} // namespace imtdb


