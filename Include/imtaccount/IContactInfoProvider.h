// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtaccount
{


class IContactInfo;


/**
	Implemented by entities owning a contact info: parties and customers.
	Allows generic code to reach communication channels without knowing the entity type.
	\ingroup Account
*/
class IContactInfoProvider: virtual public istd::IChangeable
{
public:
	/**
		Get communication channels of this entity itself.
		Never returns null: an entity without contact data owns an empty contact info.
	*/
	virtual const IContactInfo* GetContactInfo() const = 0;
};


} // namespace imtaccount
