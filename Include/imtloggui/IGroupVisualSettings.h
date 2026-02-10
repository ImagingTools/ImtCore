// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtGui/QColor>

// ACF includes
#include <istd/IChangeable.h>


namespace imtloggui
{


class IGroupVisualSettings: virtual public istd::IChangeable
{
public:
	virtual QColor GetBackgroundColor() const = 0;
	virtual int GetHeight() const = 0;
};


} // namespace imtloggui


