#pragma once

#include <imtcore/IApplicationInitializer.h>


namespace imtcore
{


class CImtCoreGuiDarkResourcesInitializer : public IApplicationInitializer
{
public:
	virtual void Initialize() override;
};


} // namespace imtcore
