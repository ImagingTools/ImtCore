#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtsdl/CSdlGeneralManagerComp.h>
#include <imtsdl/CSdlProcessArgumentsParserComp.h>
#include <imtsdl/CGqlSchemaParserComp.h>
#include <imtsdl/CSdlClassCodeGeneratorComp.h>
#include <imtsdl/CBaseClassExtenderComp.h>
#include <imtsdl/CSdlClassTreeModelModificatorComp.h>
#include <imtsdl/CSdlClassGqlModificatorComp.h>
#include <imtsdl/CSdlClassJsonModificatorComp.h>
#include <imtsdl/CQmlCodeGeneratorComp.h>
#include <imtsdl/CQmlCodeMetaGeneratorComp.h>
#include <imtsdl/CQmlCodeCommandIdGeneratorComp.h>
#include <imtsdl/CGqlWrapClassCodeGeneratorComp.h>
#include <imtsdl/CGqlHandlerBaseClassGeneratorComp.h>
#include <imtsdl/CGqlCollectionControllerBaseClassGeneratorComp.h>
#include <imtsdl/CQmldirFilePersistenceComp.h>
#include <imtsdl/CQtResourceFilePersistenceComp.h>
#include <imtsdl/CFileCleanupProcessorComp.h>
#include <imtsdl/CFileFinalizerComp.h>


/**
	ImtSdlPck package
*/
namespace ImtSdlPck
{


typedef imtsdl::CSdlGeneralManagerComp SdlGeneralManager;
typedef imtsdl::CSdlProcessArgumentsParserComp SdlProcessArgumentsParser;
typedef imtsdl::CGqlSchemaParserComp GqlSchemaParser;
typedef imtsdl::CSdlClassCodeGeneratorComp SdlClassCodeGenerator;
typedef imtsdl::CBaseClassExtenderComp BaseClassExtender;
typedef imtsdl::CSdlClassTreeModelModificatorComp SdlClassTreeModelModificator;
typedef imtsdl::CSdlClassGqlModificatorComp SdlClassGqlModificator;
typedef imtsdl::CSdlClassJsonModificatorComp SdlClassJsonModificator;
typedef imtsdl::CQmlCodeGeneratorComp QmlCodeGenerator;
typedef imtsdl::CQmlCodeMetaGeneratorComp QmlCodeMetaGenerator;
typedef imtsdl::CQmlCodeCommandIdGeneratorComp QmlCodeCommandIdGenerator;
typedef imtsdl::CGqlWrapClassCodeGeneratorComp GqlWrapClassCodeGenerator;
typedef imtsdl::CGqlHandlerBaseClassGeneratorComp GqlHandlerBaseClassGenerator;
typedef imtsdl::CGqlCollectionControllerBaseClassGeneratorComp GqlCollectionControllerBaseClassGenerator;
typedef imtsdl::CQmldirFilePersistenceComp QmldirFilePersistence;
typedef imtsdl::CQtResourceFilePersistenceComp QtResourceFilePersistence;
typedef imtsdl::CFileCleanupProcessorComp FileCleanupProcessor;
typedef imtsdl::CFileFinalizerComp FileFinalizer;


} // namespace ImtSdlPck


