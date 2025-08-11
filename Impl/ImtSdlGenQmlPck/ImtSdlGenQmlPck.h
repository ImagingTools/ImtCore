#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtsdlgenqml/CSdlGeneralManagerComp.h>
#include <imtsdlgenqml/CQmlCodeGeneratorComp.h>
#include <imtsdlgenqml/CQmlCodeMetaGeneratorComp.h>
#include <imtsdlgenqml/CQmlCodeCommandIdGeneratorComp.h>
#include <imtsdlgenqml/CQmldirFilePersistenceComp.h>
#include <imtsdlgenqml/CQmlEnumGeneratorComp.h>
#include <imtsdlgenqml/CQmlCodeRequestGeneratorComp.h>


/**
	ImtSdlPck package
*/
namespace ImtSdlGenQmlPck
{


typedef imtsdlgenqml::CSdlGeneralManagerComp SdlGeneralManager;
typedef imtsdlgenqml::CQmlCodeGeneratorComp QmlCodeGenerator;
typedef imtsdlgenqml::CQmlCodeMetaGeneratorComp QmlCodeMetaGenerator;
typedef imtsdlgenqml::CQmlCodeCommandIdGeneratorComp QmlCodeCommandIdGenerator;
typedef imtsdlgenqml::CQmldirFilePersistenceComp QmldirFilePersistence;
typedef imtsdlgenqml::CQmlEnumGeneratorComp QmlEnumGenerator;
typedef imtsdlgenqml::CQmlCodeRequestGeneratorComp QmlCodeRequestGenerator;


} // namespace ImtSdlGenQmlPck


