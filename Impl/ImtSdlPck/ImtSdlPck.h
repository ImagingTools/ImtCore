// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtsdl/CGqlSchemaParserComp.h>
#include <imtsdl/CLogFilterComp.h>
#include <imtsdl/CBasicRequestsProxyProviderComp.h>
#include <imtsdl/CSimpleFileJoinerComp.h>
#include <imtsdl/CSdlProcessArgumentsParserComp.h>
#include <imtsdl/CQtResourceFilePersistenceComp.h>
#include <imtsdl/CCodeGeneratorExecutorComp.h>



/**
	ImtSdlPck package
*/
namespace ImtSdlPck
{

typedef imtsdl::CGqlSchemaParserComp GqlSchemaParser;
typedef imtsdl::CLogFilterComp LogFilter;
typedef imtsdl::CBasicRequestsProxyProviderComp BasicRequestsProxyProvider;
typedef imtsdl::CSimpleFileJoinerComp SimpleFileJoiner;
typedef imtsdl::CSdlProcessArgumentsParserComp SdlProcessArgumentsParser;
typedef imtsdl::CQtResourceFilePersistenceComp QtResourceFilePersistence;
typedef imtsdl::CCodeGeneratorExecutorComp CodeGeneratorExecutor;




} // namespace ImtSdlPck


