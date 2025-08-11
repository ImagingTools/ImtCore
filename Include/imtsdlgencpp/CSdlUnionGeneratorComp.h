#pragma once


//Qt includes
#include <QtCore/QFile>

// ACF includes
#include <istd/TDelPtr.h>
#include <iprm/ITextParam.h>
#include <iprm/IOptionsManager.h>
#include <iproc/TSyncProcessorCompBase.h>

// ImtCore includes
#include <imtsdl/ISdlProcessArgumentsParser.h>
#include <imtsdl/ISdlTypeListProvider.h>
#include <imtsdl/CSdlTools.h>
#include <imtsdlgencpp/CSdlUnionConverter.h>
#include <imtsdlgencpp/IIncludeDirectivesProvider.h>
#include <imtsdlgencpp/CSdlGenTools.h>


namespace imtsdlgencpp
{

/**
	A base C++ union generator
*/
class CSdlUnionGeneratorComp:
			public iproc::CSyncProcessorCompBase,
			private imtsdl::CSdlTools,
			private CSdlGenTools,
			private imtsdlgencpp::CSdlUnionConverter
{

public:
	typedef iproc::CSyncProcessorCompBase BaseClass;

	I_BEGIN_COMPONENT(CSdlUnionGeneratorComp)
		I_ASSIGN(m_argumentParserCompPtr, "ArgumentParser", "Command line process argument parser", true, "ArgumentParser")
		I_ASSIGN(m_sdlTypeListCompPtr, "SdlTypeListProvider", "SDL types used to create a code", true, "SdlTypeListProvider")
		I_ASSIGN(m_sdlEnumListCompPtr, "SdlEnumListProvider", "SDL enums used to create a code", true, "SdlEnumListProvider")
		I_ASSIGN(m_sdlUnionListCompPtr, "SdlUnionListProvider", "SDL unioins used to create a code", true, "SdlUnionListProvider")
		I_ASSIGN(m_customSchemaParamsCompPtr, "CustomSchemaParams", "Custom schema parameters, that contains additional options", false, "CustomSchemaParams")
		I_ASSIGN(m_originalSchemaNamespaceCompPtr, "OriginalSchemaNamespace", "The namespace of the original(root) schema", true, "OriginalSchemaNamespace");
		I_ASSIGN(m_dependentSchemaListCompPtr, "DependentSchemaList", "The list of dependent schemas, used to generate dependencies of output file", true, "DependentSchemaList");

	I_END_COMPONENT

	//reimplemented(iproc::IProcessor)
	virtual TaskState DoProcessing(
				const iprm::IParamsSet* paramsPtr,
				const istd::IPolymorphic* inputPtr,
				istd::IChangeable* outputPtr,
				ibase::IProgressManager* progressManagerPtr = NULL) override;

private:
	I_REF(imtsdl::ISdlProcessArgumentsParser, m_argumentParserCompPtr);
	I_REF(imtsdl::ISdlTypeListProvider, m_sdlTypeListCompPtr);
	I_REF(imtsdl::ISdlEnumListProvider, m_sdlEnumListCompPtr);
	I_REF(imtsdl::ISdlUnionListProvider, m_sdlUnionListCompPtr);
	I_REF(iprm::IParamsSet, m_customSchemaParamsCompPtr);
	I_REF(iprm::ITextParam, m_originalSchemaNamespaceCompPtr);
	I_REF(iprm::IOptionsManager, m_dependentSchemaListCompPtr);
};


} // namespace imtsdlgencpp

