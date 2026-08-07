#pragma once


// ACF includes
#include <icomp/IComponent.h>

// ImtCore includes
#include <imtcore/IApplicationInitializer.h>


namespace imtcore
{

class CApplicationRunner
{
public:
	CApplicationRunner() = delete;
	static int Run(int argc, char** argv, icomp::IComponent& applicationComponent, bool autoInit);
	static int Run(int argc, char** argv, icomp::IComponent& applicationComponent, bool autoInit, IApplicationInitializer& initializer);
};


} // namespace imtcore