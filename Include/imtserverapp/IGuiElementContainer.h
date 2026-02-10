// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtserverapp
{


class IGuiElementModel;


class IGuiElementContainer: virtual public istd::IChangeable
{
public:
	virtual QByteArrayList GetElementIds() const = 0;
	virtual const IGuiElementModel* GetGuiElementModel(const QByteArray& elementId) const = 0;
};


} // namespace imtserverapp


