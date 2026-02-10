// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


namespace imtloggui
{


class ILayerController: virtual public istd::IPolymorphic
{
public:
	virtual QByteArray GetActiveRepresentationId() const = 0;
	virtual bool SetActiveRepresentationId(const QByteArray& representationId) = 0;
};


} // namespace imtloggui


