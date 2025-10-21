#pragma once


// ACF includes
#include <ibase/IApplicationInfo.h>
#include <istd/IChangeable.h>


namespace imtbase
{


class IApplicationInfoController: virtual public ibase::IApplicationInfo, virtual public istd::IChangeable
{
public:
	virtual void SetApplicationAttribute(ApplicationAttribute attributeId, const QString& attributeValue) = 0;
};


} // namespace imtbase


