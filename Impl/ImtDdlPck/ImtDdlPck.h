#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtddl/CDdlCodeCreatorProcessorsControllerComp.h>
#include <imtddl/CDdlCodeCreatorArgumentsParserComp.h>
#include <imtddl/CConstVarCppCreatorProcessorComp.h>
#include <imtddl/CConstVarHCreatorProcessorComp.h>
#include <imtddl/CConstVarQmlCreatorProcessorComp.h>
#include <imtddl/CConstVarQmldirCreatorProcessorComp.h>
#include <imtddl/CConstVarQrcCreatorProcessorComp.h>


/**
	ImtDdlPck package
*/
namespace ImtDdlPck
{


typedef imtddl::CDdlCodeCreatorProcessorsControllerComp DdlCodeCreatorProcessorsController;
typedef imtddl::CDdlCodeCreatorArgumentsParserComp DdlCodeCreatorArgumentsParser;
typedef imtddl::CConstVarCppCreatorProcessorComp ConstVarCppCreatorProcessor;
typedef imtddl::CConstVarHCreatorProcessorComp ConstVarHCreatorProcessor;
typedef imtddl::CConstVarQmlCreatorProcessorComp ConstVarQmlCreatorProcessor;
typedef imtddl::CConstVarQmldirCreatorProcessorComp ConstVarQmldirCreatorProcessor;
typedef imtddl::CConstVarQrcCreatorProcessorComp ConstVarQrcCreatorProcessor;


} // namespace ImtDdlPck


