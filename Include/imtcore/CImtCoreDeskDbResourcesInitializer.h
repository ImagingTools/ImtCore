#pragma once

#include <imtcore/IApplicationInitializer.h>


namespace imtcore
{


class CImtCoreDeskDbResourcesInitializer : public IApplicationInitializer
{
public:
	virtual void Initialize() override;
};


} // namespace imtcore
