#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// Python integration
#include <imtpy/CPythonEnvironmentComp.h>
#include <imtpy/CPythonScriptEngineComp.h>


/**
	Python component package
*/
namespace ImtPyPck
{


typedef imtpy::CPythonEnvironmentComp PythonEnvironment;
typedef icomp::TModelCompWrap<imtpy::CPythonScriptEngineComp> PythonScriptEngine;


} // namespace ImtPyPck



