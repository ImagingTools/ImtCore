#pragma once


// ACF includes
#include <ibase/IApplicationInfo.h>

// ImtCore includes
#include <imtbase/CTreeItemModel.h>


namespace imtrest
{


class IApplicationInfoRepresentation: virtual public istd::IPolymorphic
{
public:
	virtual bool GetRepresentationFromApplicationInfo(
				const ibase::IApplicationInfo& applicationInfo,
				imtbase::CTreeItemModel& representation) const = 0;
};


} // namespace imtrest


