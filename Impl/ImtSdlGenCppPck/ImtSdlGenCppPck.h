#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>

#include <imtsdlgencpp/CSdlProcessArgumentsParserComp.h>
#include <imtsdlgencpp/CSdlClassCodeGeneratorComp.h>
#include <imtsdlgencpp/CSdlClassJsonModificatorComp.h>
#include <imtsdlgencpp/CSdlClassTreeModelModificatorComp.h>
#include <imtsdlgencpp/CSdlGeneralManagerComp.h>
#include <imtsdlgencpp/CSdlClassGqlModificatorComp.h>
#include <imtsdlgencpp/CGqlWrapClassCodeGeneratorComp.h>
#include <imtsdlgencpp/CGqlHandlerBaseClassGeneratorComp.h>
#include <imtsdlgencpp/CGqlCollectionControllerBaseClassGeneratorComp.h>
#include <imtsdlgencpp/CSdlEnumGeneratorComp.h>
#include <imtsdlgencpp/CSdlUnionGeneratorComp.h>
#include <imtsdlgencpp/CQmlRegisterGeneratorComp.h>
#include <imtsdlgencpp/CBaseClassExtenderComp.h>


/**
	ImtSdlPck package
*/
namespace ImtSdlGenCppPck
{


typedef imtsdlgencpp::CSdlGeneralManagerComp SdlGeneralManager;
typedef imtsdlgencpp::CSdlProcessArgumentsParserComp SdlProcessArgumentsParser;
typedef imtsdlgencpp::CSdlClassCodeGeneratorComp SdlClassCodeGenerator;
typedef imtsdlgencpp::CSdlClassTreeModelModificatorComp SdlClassTreeModelModificator;
typedef imtsdlgencpp::CSdlClassGqlModificatorComp SdlClassGqlModificator;
typedef imtsdlgencpp::CSdlClassJsonModificatorComp SdlClassJsonModificator;
typedef imtsdlgencpp::CGqlWrapClassCodeGeneratorComp GqlWrapClassCodeGenerator;
typedef imtsdlgencpp::CGqlHandlerBaseClassGeneratorComp GqlHandlerBaseClassGenerator;
typedef imtsdlgencpp::CGqlCollectionControllerBaseClassGeneratorComp GqlCollectionControllerBaseClassGenerator;
typedef imtsdlgencpp::CSdlEnumGeneratorComp SdlEnumGenerator;
typedef imtsdlgencpp::CSdlUnionGeneratorComp SdlUnionGenerator;
typedef imtsdlgencpp::CQmlRegisterGeneratorComp QmlRegisterGenerator;
typedef imtsdlgencpp::CBaseClassExtenderComp BaseClassExtender;


} // namespace ImtSdlGenCppPck


