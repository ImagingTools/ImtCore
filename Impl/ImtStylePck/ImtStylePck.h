#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtstyle/CDesignTokenBasedResourceProviderComp.h>
#include <imtstyle/CDesignTokenIconProcessorComp.h>
#include <imtstyle/CDesignTokenStyleSheetProcessorComp.h>
#include <imtstyle/CDesignTokenArgumentsParserComp.h>
#include <imtstyle/CDesignTokenFileParserComp.h>
#include <imtstyle/CDesignTokenProcessorsControllerComp.h>
#include <imtstyle/CDesignTokenQrcUpdaterComp.h>
#include <imtstyle/CDesignTokenSplitProcessorComp.h>

/**
	ImtStylePck package
*/
namespace ImtStylePck
{

typedef imtstyle::CDesignTokenBasedResourceProviderComp DesignTokenBasedResourceProvider;
typedef imtstyle::CDesignTokenArgumentsParserComp DesignTokenArgumentsParser;
typedef imtstyle::CDesignTokenFileParserComp DesignTokenFileParser;
typedef imtstyle::CDesignTokenIconProcessorComp DesignTokenIconProcessor;
typedef imtstyle::CDesignTokenStyleSheetProcessorComp DesignTokenStyleSheetProcessor;
typedef imtstyle::CDesignTokenProcessorsControllerComp DesignTokenProcessorsController;
typedef imtstyle::CDesignTokenQrcUpdaterComp DesignTokenQrcUpdater;
typedef imtstyle::CDesignTokenSplitProcessorComp DesignTokenSplitProcessor;

} // namespace ImtStylePck


