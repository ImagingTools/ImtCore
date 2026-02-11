// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtservice/IConnectionCollection.h>
#include <imtservice/IConnectionCollectionPlugin.h>


namespace imtservice
{


/**
	Interface for describing a service connection collection.
	\ingroup Service
*/
class IConnectionCollectionProvider: virtual public istd::IPolymorphic
{
public:
	// TODO: Raw pointer ???
	virtual imtservice::IConnectionCollection* GetConnectionCollectionByServicePath(const QString& servicePath) const = 0;
	virtual imtservice::IConnectionCollection* GetConnectionCollectionByServiceId(const QByteArray& serviceId) const = 0;
};


} // namespace imtservice


