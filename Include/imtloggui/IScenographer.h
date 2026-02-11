// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


namespace imtloggui
{


class IScenographer: virtual public istd::IPolymorphic
{
public:
	virtual void AddItemsToScene() = 0;
	virtual void RemoveItemsFromScene() = 0;
};


} // namespace imtloggui


