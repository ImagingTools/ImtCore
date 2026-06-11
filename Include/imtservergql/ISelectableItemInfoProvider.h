// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>

// ACF includes
#include <istd/IPolymorphic.h>
#include <iprm/IParamsSet.h>


namespace imtservergql
{


/**
	Interface for providing additional parameters for selectable items.

	Implementations of this interface are responsible for filling
	extra metadata (as iprm::IParamsSet) for items in a filterable-select list.
	This allows different collection types (grants, relationships, etc.)
	to expose domain-specific information to the client without changing
	the generic FilterableSelect infrastructure.

	The caller is responsible for converting the iprm::IParamsSet output
	into the appropriate SDL representation for serialization.

	\ingroup FilterableSelect
*/
class ISelectableItemInfoProvider: virtual public istd::IPolymorphic
{
public:
	/**
		Fill additional parameters for a selectable item.
		@param objectId ID of the object in the collection.
		@param paramsSet Output params set to add additional parameters to.
		@return true if parameters were successfully provided, false otherwise.
	*/
	virtual bool GetItemParameters(
				const QByteArray& objectId,
				iprm::IParamsSet& paramsSet) const = 0;
};


} // namespace imtservergql
