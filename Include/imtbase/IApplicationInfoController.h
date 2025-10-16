#pragma once


// ACF includes
#include <ibase/IApplicationInfo.h>


namespace imtbase
{


class IApplicationInfoController: virtual public ibase::IApplicationInfo
{
public:
	virtual void SetApplicationAttribute(ApplicationAttribute attributeId, const QString& attributeValue) = 0;
};


} // namespace imtbase


