// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once



// ImCore includes
#include <istd/IChangeable.h>


namespace imtgui
{


class IPageNavigationController: virtual public istd::IChangeable
{
public:
	virtual bool IsNextPageAvailable() const = 0;
	virtual bool IsPreviousPageAvailable() const = 0;
	virtual void GoForward() = 0;
	virtual void GoBack() = 0;
};


} // namespace imtgui


