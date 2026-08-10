#pragma once

#include <imtcore/IApplicationInitializer.h>


namespace imtcore
{


class CImtCoreGuiLightResourcesInitializer : public IApplicationInitializer
{
public:
	virtual void Initialize() override;
};


} // namespace imtcore
