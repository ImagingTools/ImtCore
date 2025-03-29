#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>

#include <imtsdlgenv2/CSdlProcessArgumentsParserComp.h>
#include <imtsdlgenv2/CSdlClassCodeGeneratorComp.h>
#include <imtsdlgenv2/CSdlClassJsonModificatorComp.h>
#include <imtsdlgenv2/CSdlClassTreeModelModificatorComp.h>
#include <imtsdlgenv2/CSdlGeneralManagerComp.h>
#include <imtsdlgenv2/CSdlClassGqlModificatorComp.h>
#include <imtsdlgenv2/CGqlWrapClassCodeGeneratorComp.h>
#include <imtsdlgenv2/CGqlHandlerBaseClassGeneratorComp.h>
#include <imtsdlgenv2/CGqlCollectionControllerBaseClassGeneratorComp.h>
#include <imtsdlgenv2/CSdlEnumGeneratorComp.h>
#include <imtsdlgenv2/CSdlUnionGeneratorComp.h>


/**
	ImtSdlPck package
*/
namespace ImtSdlGenV2Pck
{


typedef imtsdlgenv2::CSdlGeneralManagerComp SdlGeneralManager;
typedef imtsdlgenv2::CSdlProcessArgumentsParserComp SdlProcessArgumentsParser;
typedef imtsdlgenv2::CSdlClassCodeGeneratorComp SdlClassCodeGenerator;
typedef imtsdlgenv2::CSdlClassTreeModelModificatorComp SdlClassTreeModelModificator;
typedef imtsdlgenv2::CSdlClassGqlModificatorComp SdlClassGqlModificator;
typedef imtsdlgenv2::CSdlClassJsonModificatorComp SdlClassJsonModificator;
typedef imtsdlgenv2::CGqlWrapClassCodeGeneratorComp GqlWrapClassCodeGenerator;
typedef imtsdlgenv2::CGqlHandlerBaseClassGeneratorComp GqlHandlerBaseClassGenerator;
typedef imtsdlgenv2::CGqlCollectionControllerBaseClassGeneratorComp GqlCollectionControllerBaseClassGenerator;
typedef imtsdlgenv2::CSdlEnumGeneratorComp SdlEnumGenerator;
typedef imtsdlgenv2::CSdlUnionGeneratorComp SdlUnionGenerator;


} // namespace ImtSdlGenV2Pck


