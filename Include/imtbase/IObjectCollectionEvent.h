// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtbase
{


/**
	Interface for object collection events
	\ingroup Collection
*/
class IObjectCollectionEvent: virtual public istd::IChangeable
{
public:
	enum EventType
	{
		ET_INSERT = 0,
		ET_REMOVE,
		ET_UPDATE
	};

	/**
		Get type of the collection event.
		\sa EventType
	*/
	virtual EventType GetEventType() const = 0;

	/**
		Get ID of the related item in the collection.
	*/
	virtual QByteArray GetItemId() const = 0;
};


} // namespace imtbase


