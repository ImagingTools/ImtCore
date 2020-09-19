#include "ImtPyPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtPyPck
{


I_EXPORT_PACKAGE(
			"ImtPyPck",
			"Python integration components",
			IM_PROJECT("ImtCore") IM_COMPANY("ImagingTools") IM_AUTHOR("\"Kirill Lepskiy\""));


I_EXPORT_COMPONENT(
			PythonEnvironment,
			"Python environment",
			"Script Python System Environment");

I_EXPORT_COMPONENT(
			PythonScriptEngine,
			"Python script engine",
			"Script Python System Engine Processor");

} // namespace ImtPyPck


