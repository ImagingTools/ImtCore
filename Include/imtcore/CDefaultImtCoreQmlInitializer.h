#pragma once

#include <imtcore/IApplicationInitializer.h>


namespace imtcore
{


class CDefaultImtCoreQmlInitializer : public IApplicationInitializer
{
public:
	virtual void Initialize() override;
};


} // namespace imtcore
