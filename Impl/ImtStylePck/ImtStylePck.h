#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>
#include <imtstyle/CDesignTokenBasedResourceProviderComp.h>


// ImtCore includes
#include <imtstyle/CDesignTokenBasedResourceProviderComp.h>
#include <imtstyle/CDesignTokenIconProcessorComp.h>
#include <imtstyle/CDesignTokenArgumentsParserComp.h>
#include <imtstyle/CDesignTokenFileParserComp.h>

/**
	ImtStylePck package
*/
namespace ImtStylePck
{

typedef imtstyle::CDesignTokenBasedResourceProviderComp DesignTokenBasedResourceProvider;
typedef imtstyle::CDesignTokenArgumentsParserComp DesignTokenArgumentsParser;
typedef imtstyle::CDesignTokenFileParserComp DesignTokenFileParser;
typedef imtstyle::CDesignTokenIconProcessorComp DesignTokenIconProcessor;

} // namespace ImtStylePck


