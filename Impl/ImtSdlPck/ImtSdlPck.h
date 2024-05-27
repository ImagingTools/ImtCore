#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtsdl/CSdlGeneralManagerComp.h>
#include <imtsdl/CSdlProcessArgumentsParserComp.h>
#include <imtsdl/CGqlSchemaParserComp.h>
#include <imtsdl/CSdlClassCodeGeneratorComp.h>
#include <imtsdl/CSdlClassTreeModelModificatorComp.h>
#include <imtsdl/CSdlClassGqlModificatorComp.h>
#include <imtsdl/CSdlClassJsonModificatorComp.h>
#include <imtsdl/CQmlCodeGeneratorComp.h>


/**
	ImtSdlPck package
*/
namespace ImtSdlPck
{


typedef imtsdl::CSdlGeneralManagerComp SdlGeneralManager;
typedef imtsdl::CSdlProcessArgumentsParserComp SdlProcessArgumentsParser;
typedef imtsdl::CGqlSchemaParserComp GqlSchemaParser;
typedef imtsdl::CSdlClassCodeGeneratorComp SdlClassCodeGenerator;
typedef imtsdl::CSdlClassTreeModelModificatorComp SdlClassTreeModelModificator;
typedef imtsdl::CSdlClassGqlModificatorComp SdlClassGqlModificator;
typedef imtsdl::CSdlClassJsonModificatorComp SdlClassJsonModificator;
typedef imtsdl::CQmlCodeGeneratorComp QmlCodeGenerator;


} // namespace ImtSdlPck


