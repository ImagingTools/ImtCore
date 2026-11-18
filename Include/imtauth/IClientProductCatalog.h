// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QString>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtauth
{


/**
	The products whose servers may be given a token of this server.
*/
class IClientProductCatalog: virtual public istd::IPolymorphic
{
public:
	/**
		\param isReadPtr set to false when the catalog could not be read; the product is then no client.
	*/
	virtual bool IsClientProduct(const QByteArray& productId, bool* isReadPtr = nullptr) const = 0;

	/**
		Make the product a client of this server; nothing changes when it is one already.
		\param isAddedPtr set to true when the product has been added by this call.
	*/
	virtual bool AddClientProduct(const QByteArray& productId, const QString& name, QString& errorMessage, bool* isAddedPtr = nullptr) = 0;
};


} // namespace imtauth


