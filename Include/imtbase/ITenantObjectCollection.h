// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/IObjectCollection.h>


namespace imtbase
{


/**
	Object collection bound to the data storage of a single tenant.
	All operations of the collection address only the data of this tenant.
	\ingroup Collection
*/
class ITenantObjectCollection: virtual public IObjectCollection
{
public:
	/**
		Change notification info with the ID of the tenant whose data was changed.
		Set in the change notifications of the collection providing the tenant collection.
	*/
	static const QByteArray CN_TENANT_ID;

	/**
		Get ID of the tenant the collection is bound to.
	*/
	virtual QByteArray GetTenantId() const = 0;
};


} // namespace imtbase


