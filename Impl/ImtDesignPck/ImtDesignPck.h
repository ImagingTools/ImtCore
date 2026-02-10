// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
#include <imtdesign/CDesignTokenCommonQrcGeneratorComp.h>
#include <imtdesign/CDesignTokenWebQrcGeneratorComp.h>
#include <imtdesign/CDesignTokenSplitProcessorComp.h>

#include <imtdesign/CDesignTokenIconQmlGeneratorComp.h>

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
typedef imtdesign::CDesignTokenCommonQrcGeneratorComp CDesignTokenCommonQrcGenerator;
typedef imtdesign::CDesignTokenWebQrcGeneratorComp DesignTokenWebQrcGenerator;
typedef imtdesign::CDesignTokenSplitProcessorComp DesignTokenSplitProcessor;
typedef imtdesign::CDesignTokenIconQmlGeneratorComp DesignTokenIconQmlGenerator;


} // namespace ImtDesignPck


