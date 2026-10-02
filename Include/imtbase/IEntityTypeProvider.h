// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtbase
{


/**
	Interface for an entity type provider.

	Components implementing this interface register entity types that other
	subsystems (support tickets, tags) can reference. Consumers collect all
	registered providers via I_MULTIREF.

	\ingroup Collection
*/
class IEntityTypeProvider: virtual public istd::IPolymorphic
{
public:
	/**
		Get the unique identifier of the entity type.
		This ID also serves as the collection ID for browsing entities of this type.
	*/
	virtual QString GetEntityTypeId() const = 0;

	/**
		Get the human-readable name of the entity type.
	*/
	virtual QString GetEntityTypeName() const = 0;
};


} // namespace imtbase


