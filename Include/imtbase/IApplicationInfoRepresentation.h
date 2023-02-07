#pragma once


// ACF includes
#include <ibase/IApplicationInfo.h>

// ImtCore includes
#include <imtbase/CTreeItemModel.h>


namespace imtbase
{


class IApplicationInfoRepresentation: virtual public istd::IPolymorphic
{
public:
	virtual bool GetRepresentationFromApplicationInfo(const ibase::IApplicationInfo& applicationInfo, CTreeItemModel& representation) const = 0;
};


} // namespace imtbase


