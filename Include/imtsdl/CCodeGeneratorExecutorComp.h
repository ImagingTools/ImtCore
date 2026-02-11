// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


//Qt includes
#include <QtCore/QFile>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iproc/TSyncProcessorWrap.h>

// ImtCore includes
#include <imtsdl/ISdlProcessArgumentsParser.h>


namespace imtsdl
{

/// \todo rename it to CSdlCppManager
class CCodeGeneratorExecutorComp:public ilog::CLoggerComponentBase
{


public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CCodeGeneratorExecutorComp)
		I_ASSIGN(m_cxxProcessorCompPtr, "CxxProcessor", "The CXX processors manager", false, "CxxProcessor")
		I_ASSIGN(m_qmlProcessorCompPtr, "QmlProcessor", "The QML processors manager", false, "QmlProcessor")
		I_ASSIGN(m_gqlSchemaParser, "SchemaParser", "The GQL schema parser", true, "SchemaParser")
		I_ASSIGN(m_sdlArgumentParserCompPtr, "ArgumentsParser", "The GQL processor arguments parser", true, "ArgumentsParser")
		I_ASSIGN(m_sdlSchemaDependenciesCollectorCompPtr, "SdlSchemaDependenciesCollector", "Processor, used to collect a list of all schemas that affect the generated code", true, "SdlSchemaDependenciesCollector")
	I_END_COMPONENT;

protected:
	// reimplemented (iproc::IProcessor)
	virtual void OnComponentCreated() override;

private:
	I_REF(iproc::IProcessor, m_cxxProcessorCompPtr);
	I_REF(iproc::IProcessor, m_qmlProcessorCompPtr);
	I_REF(iproc::IProcessor, m_gqlSchemaParser);
	I_REF(ISdlProcessArgumentsParser, m_sdlArgumentParserCompPtr);
	I_REF(iproc::IProcessor, m_sdlSchemaDependenciesCollectorCompPtr);
};


} // namespace imtsdl







