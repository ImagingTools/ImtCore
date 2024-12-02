#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtsdlgen/CSdlGeneralManagerComp.h>
#include <imtsdlgen/CSdlProcessArgumentsParserComp.h>
#include <imtsdlgen/CSdlClassCodeGeneratorComp.h>
#include <imtsdlgen/CBaseClassExtenderComp.h>
#include <imtsdlgen/CSdlClassTreeModelModificatorComp.h>
#include <imtsdlgen/CSdlClassJsonModificatorComp.h>
#include <imtsdlgen/CQmlCodeGeneratorComp.h>
#include <imtsdlgen/CQmlCodeMetaGeneratorComp.h>
#include <imtsdlgen/CQmlCodeCommandIdGeneratorComp.h>
#include <imtsdlgen/CSdlClassGqlModificatorComp.h>
#include <imtsdlgen/CGqlWrapClassCodeGeneratorComp.h>
#include <imtsdlgen/CGqlHandlerBaseClassGeneratorComp.h>
#include <imtsdlgen/CGqlCollectionControllerBaseClassGeneratorComp.h>
#include <imtsdlgen/CQmldirFilePersistenceComp.h>
#include <imtsdlgen/CQtResourceFilePersistenceComp.h>
#include <imtsdlgen/CFileCleanupProcessorComp.h>
#include <imtsdlgen/CFileFinalizerComp.h>
#include <imtsdlgen/CSdlEnumGeneratorComp.h>


/**
	ImtSdlPck package
*/
namespace ImtSdlGenPck
{


typedef imtsdlgen::CSdlGeneralManagerComp SdlGeneralManager;
typedef imtsdlgen::CSdlProcessArgumentsParserComp SdlProcessArgumentsParser;
typedef imtsdlgen::CSdlClassCodeGeneratorComp SdlClassCodeGenerator;
typedef imtsdlgen::CBaseClassExtenderComp BaseClassExtender;
typedef imtsdlgen::CSdlClassTreeModelModificatorComp SdlClassTreeModelModificator;
typedef imtsdlgen::CSdlClassJsonModificatorComp SdlClassJsonModificator;
typedef imtsdlgen::CQmlCodeGeneratorComp QmlCodeGenerator;
typedef imtsdlgen::CQmlCodeMetaGeneratorComp QmlCodeMetaGenerator;
typedef imtsdlgen::CQmlCodeCommandIdGeneratorComp QmlCodeCommandIdGenerator;
typedef imtsdlgen::CSdlClassGqlModificatorComp SdlClassGqlModificator;
typedef imtsdlgen::CGqlWrapClassCodeGeneratorComp GqlWrapClassCodeGenerator;
typedef imtsdlgen::CGqlHandlerBaseClassGeneratorComp GqlHandlerBaseClassGenerator;
typedef imtsdlgen::CGqlCollectionControllerBaseClassGeneratorComp GqlCollectionControllerBaseClassGenerator;
typedef imtsdlgen::CQmldirFilePersistenceComp QmldirFilePersistence;
typedef imtsdlgen::CQtResourceFilePersistenceComp QtResourceFilePersistence;
typedef imtsdlgen::CFileCleanupProcessorComp FileCleanupProcessor;
typedef imtsdlgen::CFileFinalizerComp FileFinalizer;
typedef imtsdlgen::CSdlEnumGeneratorComp SdlEnumGenerator;


} // namespace ImtSdlGenPck


