#pragma once

#include <imtcore/IApplicationInitializer.h>


namespace imtcore
{


class CAcfLocInitializer : public IApplicationInitializer
{
public:
	virtual void Initialize() override;
};


} // namespace imtcore
