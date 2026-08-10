#pragma once

#include <istd/IPolymorphic.h>


namespace imtcore
{


class IApplicationInitializer : public istd::IPolymorphic
{
public:
	virtual void Initialize() = 0;
};


} // namespace imtcore
