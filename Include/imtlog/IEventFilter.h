// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>
#include <istd/IInformationProvider.h>

// ImtCore includes
#include <imtlog/IMessageFilterParams.h>


namespace imtlog
{


class IEventFilter: virtual public istd::IPolymorphic
{
public:
	virtual bool IsMessageAccepted(
				const istd::IInformationProvider& message, 
				const IMessageFilterParams* filterParamsPtr) const = 0;
};


} // namespace imtlog


