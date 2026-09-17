// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QByteArrayList>

// ACF includes
#include <istd/IChangeable.h>


namespace imtbase
{


/**
	Access groups assigned to an object, restricting which users may see it.
	An user may access the object if at least one of his groups is listed here.
	\ingroup Collection
*/
class IGroupScopedObject: virtual public istd::IChangeable
{
public:
	typedef QByteArrayList GroupIds;

	/**
		Get groups allowed to access this object.
	*/
	virtual GroupIds GetAccessGroups() const = 0;

	/**
		Add a group to the access list.
		\return false if the group was already present.
	*/
	virtual bool AddAccessGroup(const QByteArray& groupId) = 0;

	/**
		Remove a group from the access list.
		\return false if the group was not present.
	*/
	virtual bool RemoveAccessGroup(const QByteArray& groupId) = 0;
};


} // namespace imtbase
