// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtWidgets/QGraphicsScene>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtloggui
{


class IGraphicsSceneProvider: virtual public istd::IPolymorphic
{
public:
	virtual QGraphicsScene* GetGraphicsScene() = 0;
};


} // namespace imtloggui


