#pragma once

#include <istd/IPolymorphic.h>


namespace imtcore
{


class IApplicationInitializer : public istd::IPolymorphic
{
public:
	~IApplicationInitializer() override = default;
	virtual void Initialize() = 0;
	virtual IApplicationInitializer* AddInitializer(IApplicationInitializer* initializer) = 0;
};


} // namespace imtcore
