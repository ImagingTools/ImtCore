// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArrayList>
#include <QtCore/QString>


namespace imtbase
{
	class IObjectCollection;
}


namespace imttag
{


/**
	Case-insensitive lookup of active tags by name.
	\ingroup imttag
*/
class CTagNameLookup
{
public:
	/**
		Normalize a tag name the way it is stored and compared.
	*/
	static QString NormalizeName(const QString& name);

	/**
		Find active tags with the given name.
		\param tenantIdPtr	Tenant scope of the lookup (system tags are always included by the tag delegate).
							If null, tags of all tenants are searched.
		\param excludedTagId	Tag ignored by the lookup, e.g. the tag being renamed.
	*/
	static QByteArrayList FindTagIds(
				const imtbase::IObjectCollection& tagCollection,
				const QString& name,
				const QByteArray* tenantIdPtr,
				const QByteArray& excludedTagId = QByteArray());
};


} // namespace imttag


