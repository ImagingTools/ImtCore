#pragma once

#include <imtcore/IApplicationInitializer.h>


namespace imtcore
{


class CImtCoreChatDbResourcesInitializer : public IApplicationInitializer
{
public:
	virtual void Initialize() override;
};


} // namespace imtcore
