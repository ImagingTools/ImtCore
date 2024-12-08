#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtsdl/CGqlSchemaParserComp.h>
#include <imtsdl/CCacheController.h>
#include <imtsdl/CCacheMultiManagerComp.h>


/**
	ImtSdlPck package
*/
namespace ImtSdlPck
{

typedef imtsdl::CGqlSchemaParserComp GqlSchemaParser;

/// \todo make it loggable
typedef icomp::TMakeComponentWrap<
				imtsdl::CCacheController, imtsdl::ICacheController> CacheController;

typedef imtsdl::CCacheMultiManagerComp CacheMultiManager;


} // namespace ImtSdlPck


