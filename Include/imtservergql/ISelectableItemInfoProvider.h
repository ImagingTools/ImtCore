// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QVariant>

// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtsdl/TElementList.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/ImtBaseTypes_fwd.h>


namespace imtservergql
{


/**
	Interface for providing additional parameters for selectable items.

	Implementations of this interface are responsible for filling
	extra metadata (as Parameter array) for items in a filterable-select list.
	This allows different collection types (grants, relationships, etc.)
	to expose domain-specific information to the client without changing
	the generic FilterableSelect infrastructure.

	\ingroup FilterableSelect
*/
class ISelectableItemInfoProvider: virtual public istd::IPolymorphic
{
public:
	/**
		Fill additional parameters for a selectable item.
		@param objectId ID of the object in the collection.
		@param parameters Output list to append additional parameters to.
		@return true if parameters were successfully provided, false otherwise.
	*/
	virtual bool GetItemParameters(
				const QByteArray& objectId,
				imtsdl::TElementList<sdl::V1_0::imtbase::CParameter>& parameters) const = 0;
};


} // namespace imtservergql
