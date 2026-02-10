// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/IObjectCollection.h>


namespace imtbase
{


/**
	Interface for a provider of a object collection.
	\ingroup Collection
*/
class IObjectCollectionProvider: virtual public istd::IChangeable
{
public:
	/**
		Get access to the collection instance.
	*/
	virtual const IObjectCollection* GetObjectCollection() const = 0;
};


} // namespace imtbase


