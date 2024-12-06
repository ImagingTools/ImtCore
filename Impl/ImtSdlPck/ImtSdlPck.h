#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtsdl/CGqlSchemaParserComp.h>
#include <imtsdl/CCacheController.h>
#include <imtsdl/CCacheMultiManager.h>


/**
	ImtSdlPck package
*/
namespace ImtSdlPck
{

typedef imtsdl::CGqlSchemaParserComp GqlSchemaParser;

/// \todo make it loggable
typedef icomp::TMakeComponentWrap<
				imtsdl::CCacheController, imtsdl::ICacheController> CacheController;

typedef icomp::TMakeComponentWrap<
					imtsdl::CCacheMultiManager, imtsdl::ICacheMultiManager> CacheMultiManager;


} // namespace ImtSdlPck


