// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtlog/CEventFilter.h>


namespace imtlog
{


// public methods

// reimplemented (imtlog::IEventFilter)

bool CEventFilter::IsMessageAccepted(
			const istd::IInformationProvider& message,
			const imtlog::IMessageFilterParams* filterParamsPtr) const
{
	if (filterParamsPtr != nullptr){
		if (!filterParamsPtr->GetFilterTimeRange().Contains(message.GetInformationTimeStamp())){
			return false;
		}

		if (filterParamsPtr->GetFilterMode() == imtlog::IMessageFilterParams::FM_INCLUDE){
			if (!filterParamsPtr->GetMessageFilterIds().contains(message.GetInformationId())){
				return false;
			}
		}
		else{
			if (filterParamsPtr->GetMessageFilterIds().contains(message.GetInformationId())){
				return false;
			}
		}
	}

	return true;
}


} // namespace imtlog


