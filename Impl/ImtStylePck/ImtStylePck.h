#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>
#include <imtstyle/CDesignTokenBasedResourceProviderComp.h>


// ImtCore includes
#include <imtstyle/CDesignTokenBasedResourceProviderComp.h>
#include <imtstyle/CDesignTokenProcessorComp.h>
#include <imtstyle/CDesignTokenArgumentsParserComp.h>
#include <imtstyle/CDesignTokenFileParserComp.h>

/**
	ImtStylePck package
*/
namespace ImtStylePck
{

typedef imtstyle::CDesignTokenBasedResourceProviderComp DesignTokenBasedResourceProvider;
typedef imtstyle::CDesignTokenProcessorComp DesignTokenProcessor;
typedef imtstyle::CDesignTokenArgumentsParserComp DesignTokenArgumentsParser;
typedef imtstyle::CDesignTokenFileParserComp DesignTokenFileParser;

} // namespace ImtStylePck


