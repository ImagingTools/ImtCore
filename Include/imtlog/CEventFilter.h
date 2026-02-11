// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtlog/IEventFilter.h>
#include <imtlog/IMessageFilterParams.h>


namespace imtlog
{


class CEventFilter: public imtlog::IEventFilter
{
public:
	// reimplemented (imtlog::IEventFilter)
	virtual bool IsMessageAccepted(
				const istd::IInformationProvider& message,
				const imtlog::IMessageFilterParams* filterParamsPtr) const override;
};


} // namespace imtlog


