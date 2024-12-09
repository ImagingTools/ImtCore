#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtsdl/CGqlSchemaParserComp.h>
#include <imtsdl/CCacheController.h>
#include <imtsdl/CCacheMultiManagerComp.h>
#include <imtsdl/CLogFilterComp.h>


/**
	ImtSdlPck package
*/
namespace ImtSdlPck
{

typedef imtsdl::CGqlSchemaParserComp GqlSchemaParser;
typedef imtsdl::CCacheMultiManagerComp CacheMultiManager;
typedef imtsdl::CLogFilterComp LogFilter;


} // namespace ImtSdlPck


