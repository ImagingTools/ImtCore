#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtstyle/CDesignTokenBasedResourceProviderComp.h>
#include <imtdesign/CDesignTokenIconProcessorComp.h>
#include <imtdesign/CDesignTokenStyleSheetProcessorComp.h>
#include <imtdesign/CDesignTokenArgumentsParserComp.h>
#include <imtdesign/CDesignTokenFileParserComp.h>
#include <imtdesign/CDesignTokenProcessorsControllerComp.h>
#include <imtdesign/CDesignTokenQrcUpdaterComp.h>
#include <imtdesign/CDesignTokenSplitProcessorComp.h>

/**
	ImtStylePck package
*/
namespace ImtDesignPck
{


typedef imtdesign::CDesignTokenArgumentsParserComp DesignTokenArgumentsParser;
typedef imtdesign::CDesignTokenFileParserComp DesignTokenFileParser;
typedef imtdesign::CDesignTokenIconProcessorComp DesignTokenIconProcessor;
typedef imtdesign::CDesignTokenStyleSheetProcessorComp DesignTokenStyleSheetProcessor;
typedef imtdesign::CDesignTokenProcessorsControllerComp DesignTokenProcessorsController;
typedef imtdesign::CDesignTokenQrcUpdaterComp DesignTokenQrcUpdater;
typedef imtdesign::CDesignTokenSplitProcessorComp DesignTokenSplitProcessor;


} // namespace ImtDesignPck


