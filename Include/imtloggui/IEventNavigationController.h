// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


namespace imtloggui
{


class IEventNavigationController: virtual public istd::IPolymorphic
{
public:
	virtual void JumpToFirstEvent() const = 0;
	virtual void JumpToPreviousEvent() const = 0;
	virtual void JumpToNextEvent() const = 0;
	virtual void JumpToLastEvent() const = 0;
};


} // namespace imtloggui


