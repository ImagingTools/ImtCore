// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtnotify/INotificationTypeInfo.h>


namespace imtnotify
{


/**
	The notification types of a module and the types of the objects its notifications are about.
*/
class INotificationTypeList: virtual public istd::IPolymorphic
{
public:
	virtual int GetTypesCount() const = 0;
	virtual const INotificationTypeInfo* GetTypeInfo(int index) const = 0;

	virtual int GetObjectTypesCount() const = 0;
	virtual const INotificationTypeInfo* GetObjectTypeInfo(int index) const = 0;
};


} // namespace imtnotify


