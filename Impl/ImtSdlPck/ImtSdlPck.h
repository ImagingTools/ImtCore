#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtsdl/CGqlSchemaParserComp.h>
#include <imtsdl/CLogFilterComp.h>
#include <imtsdl/CBasicRequestsProxyProviderComp.h>
#include <imtsdl/CModuleManagerComp.h>


/**
	ImtSdlPck package
*/
namespace ImtSdlPck
{

typedef imtsdl::CGqlSchemaParserComp GqlSchemaParser;
typedef imtsdl::CLogFilterComp LogFilter;
typedef imtsdl::CBasicRequestsProxyProviderComp BasicRequestsProxyProvider;
typedef imtsdl::CModuleManagerComp ModuleManager;



} // namespace ImtSdlPck


