#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtsdlgen/CSdlGeneralManagerComp.h>
#include <imtsdlgen/CSdlProcessArgumentsParserComp.h>
#include <imtsdlgen/CBaseClassExtenderComp.h>
#include <imtsdlgen/CQmlCodeGeneratorComp.h>
#include <imtsdlgen/CQmlCodeMetaGeneratorComp.h>
#include <imtsdlgen/CQmlCodeCommandIdGeneratorComp.h>
#include <imtsdlgen/CQmldirFilePersistenceComp.h>
#include <imtsdlgen/CQtResourceFilePersistenceComp.h>
#include <imtsdlgen/CFileCleanupProcessorComp.h>
#include <imtsdlgen/CFileFinalizerComp.h>


/**
	ImtSdlPck package
*/
namespace ImtSdlGenPck
{


typedef imtsdlgen::CSdlGeneralManagerComp SdlGeneralManager;
typedef imtsdlgen::CSdlProcessArgumentsParserComp SdlProcessArgumentsParser;
typedef imtsdlgen::CBaseClassExtenderComp BaseClassExtender;
typedef imtsdlgen::CQmlCodeGeneratorComp QmlCodeGenerator;
typedef imtsdlgen::CQmlCodeMetaGeneratorComp QmlCodeMetaGenerator;
typedef imtsdlgen::CQmlCodeCommandIdGeneratorComp QmlCodeCommandIdGenerator;
typedef imtsdlgen::CQmldirFilePersistenceComp QmldirFilePersistence;
typedef imtsdlgen::CQtResourceFilePersistenceComp QtResourceFilePersistence;
typedef imtsdlgen::CFileCleanupProcessorComp FileCleanupProcessor;
typedef imtsdlgen::CFileFinalizerComp FileFinalizer;


} // namespace ImtSdlGenPck


