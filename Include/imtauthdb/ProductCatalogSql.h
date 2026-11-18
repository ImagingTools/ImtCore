// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QString>


namespace imtauthdb
{
namespace ProductCatalogSql
{


inline QString ActiveProductIndex(const QString& quotedIndex, const QString& quotedTable)
{
	return QStringLiteral("CREATE UNIQUE INDEX IF NOT EXISTS %1 ON %2 ((CASE WHEN \"State\" = 'Active' THEN \"ProductId\" ELSE NULL END))")
				.arg(quotedIndex, quotedTable);
}


} // namespace ProductCatalogSql
} // namespace imtauthdb
