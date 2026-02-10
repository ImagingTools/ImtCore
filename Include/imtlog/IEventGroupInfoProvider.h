// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/ICollectionInfo.h>


namespace imtlog
{


class IEventGroupInfoProvider: virtual public istd::IChangeable
{
public:
	/**
		Get the list of all available event groups.
	*/
	virtual const imtbase::ICollectionInfo* GetEventGroupList() const = 0;

	/**
		Get the corresponding group-ID for a given event-ID.
	*/
	virtual QByteArray GetEventGroupId(int eventId) const = 0;
};


} // namespace imtlog


