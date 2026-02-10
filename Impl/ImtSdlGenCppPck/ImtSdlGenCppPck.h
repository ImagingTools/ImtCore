// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>

#include <imtsdlgencpp/CCxxProcessorsManagerComp.h>
#include <imtsdlgencpp/CSdlClassCodeGeneratorComp.h>
#include <imtsdlgencpp/CSdlClassJsonModificatorComp.h>
#include <imtsdlgencpp/CSdlClassTreeModelModificatorComp.h>
#include <imtsdlgencpp/CSdlClassGqlModificatorComp.h>
#include <imtsdlgencpp/CGqlWrapClassCodeGeneratorComp.h>
#include <imtsdlgencpp/CGqlHandlerBaseClassGeneratorComp.h>
#include <imtsdlgencpp/CGqlCollectionControllerBaseClassGeneratorComp.h>
#include <imtsdlgencpp/CSdlEnumGeneratorComp.h>
#include <imtsdlgencpp/CSdlUnionGeneratorComp.h>
#include <imtsdlgencpp/CQmlRegisterGeneratorComp.h>
#include <imtsdlgencpp/CBaseClassExtenderComp.h>
#include <imtsdlgencpp/CSdlQObjectGeneratorComp.h>
#include <imtsdlgencpp/CSdlUnionObjectGeneratorComp.h>


/**
	ImtSdlPck package
*/
namespace ImtSdlGenCppPck
{


typedef imtsdlgencpp::CCxxProcessorsManagerComp CxxProcessorsManager;
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
typedef imtsdlgencpp::CSdlQObjectGeneratorComp SdlQObjectGenerator;
typedef imtsdlgencpp::CSdlUnionObjectGeneratorComp SdlUnionObjectGenerator;

} // namespace ImtSdlGenCppPck


