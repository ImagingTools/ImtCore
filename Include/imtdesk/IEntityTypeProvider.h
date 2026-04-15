// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtdesk
{


/**
	Interface for an entity type provider.

	Components implementing this interface register available entity types
	that can be referenced from support tickets. The document manager
	collects all registered providers via I_MULTIREF to build the list
	of available entity types for the client.

	\ingroup imtdesk
*/
class IEntityTypeProvider: virtual public istd::IPolymorphic
{
public:
	/**
		Get the unique identifier of the entity type (e.g. "User", "License").
	*/
	virtual QString GetEntityTypeId() const = 0;

	/**
		Get the human-readable name of the entity type.
	*/
	virtual QString GetEntityTypeName() const = 0;
};


} // namespace imtdesk
