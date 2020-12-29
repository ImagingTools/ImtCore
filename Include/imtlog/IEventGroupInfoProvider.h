#pragma once


// ACF includes
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


