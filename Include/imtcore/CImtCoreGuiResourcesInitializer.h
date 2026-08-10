#pragma once

#include <imtcore/IApplicationInitializer.h>


namespace imtcore
{


class CImtCoreGuiResourcesInitializer : public IApplicationInitializer
{
public:
	virtual void Initialize() override;
};


} // namespace imtcore
