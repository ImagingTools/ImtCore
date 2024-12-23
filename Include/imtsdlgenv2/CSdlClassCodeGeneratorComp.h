#pragma once


//Qt includes
#include <QtCore/QFile>

// ACF includes
#include <istd/TDelPtr.h>
#include <iprm/ITextParam.h>
#include <iproc/TSyncProcessorCompBase.h>

// ImtCore includes
#include <imtsdl/ISdlProcessArgumentsParser.h>
#include <imtsdl/ISdlTypeListProvider.h>
#include <imtsdl/ICacheMultiManager.h>
#include <imtsdl/CSdlTools.h>
#include <imtsdlgen/IIncludeDirectivesProvider.h>
#include <imtsdlgen/ICxxFileProcessor.h>
#include <imtsdlgenv2/CSdlGenTools.h>


namespace imtsdlgenv2
{

/**
	A base C++ class generator of SDL types
*/
class CSdlClassCodeGeneratorComp:
			public iproc::CSyncProcessorCompBase,
			private imtsdl::CSdlTools,
			private CSdlGenTools
{

public:
	typedef iproc::CSyncProcessorCompBase BaseClass;
	typedef imtsdl::CSdlTools BaseClass2;
	typedef CSdlGenTools BaseClass3;

	I_BEGIN_COMPONENT(CSdlClassCodeGeneratorComp)
		I_ASSIGN(m_argumentParserCompPtr, "ArgumentParser", "Command line process argument parser", true, "ArgumentParser")
		I_ASSIGN(m_sdlTypeListCompPtr, "SdlTypeListProvider", "SDL types used to create a code", true, "SdlTypeListProvider")
		I_ASSIGN(m_sdlEnumListCompPtr, "SdlEnumListProvider", "SDL enums used to create a code", true, "SdlEnumListProvider")
		I_ASSIGN(m_baseClassExtenderCompPtr, "BaseClassExtender", "The C++ class extender, that adds inheritance to class files", true, "BaseClassExtender")
		I_ASSIGN(m_filesJoinerCompPtr, "FilesJoiner", "Compoment, used to join files into a single", false, "FilesJoiner")
		I_ASSIGN_MULTI_0(m_codeGeneratorExtenderListCompPtr, "CodeGeneratorExtenderList", "Extenders, used to generate an additional code", false)
		I_ASSIGN(m_customSchemaParamsCompPtr, "CustomSchemaParams", "Custom schema parameters, that contains additional options", false, "CustomSchemaParams")
		I_ASSIGN(m_originalSchemaNamespaceCompPtr, "OriginalSchemaNamespace", "The namespace of the original(root) schema", true, "OriginalSchemaNamespace");
		I_ASSIGN_MULTI_0(m_includeDirectivesProviderListCompPtr, "IncludeDirectivesProviderList", "Providers of include directives, used to generate C(++) directives", false)
		I_ASSIGN(m_cacheManagerCompPtr, "CacheManager", "Manager, that resolves generated dependencies", true, "CacheManager")


	I_END_COMPONENT

	//reimplemented(iproc::IProcessor)
	virtual int DoProcessing(
				const iprm::IParamsSet* paramsPtr,
				const istd::IPolymorphic* inputPtr,
				istd::IChangeable* outputPtr,
				ibase::IProgressManager* progressManagerPtr = NULL) override;

private:
	bool BeginClassFiles(const imtsdl::CSdlType& sdlType, bool addDependenciesInclude, bool addSelfHeaderInclude);
	bool BeginHeaderClassFile(const imtsdl::CSdlType& sdlType, bool addDependenciesInclude);
	bool BeginSourceClassFile(const imtsdl::CSdlType& sdlType, bool addSelfHeaderInclude);
	bool EndClassFiles(const imtsdl::CSdlType& sdlType);
	void AbortCurrentProcessing();
	void GenerateMetaInfo(
				QTextStream& stream,
				const imtsdl::CSdlType& sdlType,
				uint indents = 1);
	void GenerateVersionStruct(
				QTextStream& stream,
				const imtsdl::CSdlType& sdlType,
				uint indents = 1);

private:
	I_REF(imtsdl::ISdlProcessArgumentsParser, m_argumentParserCompPtr);
	I_REF(imtsdl::ISdlTypeListProvider, m_sdlTypeListCompPtr);
	I_REF(imtsdl::ISdlEnumListProvider, m_sdlEnumListCompPtr);
	I_REF(iproc::IProcessor, m_baseClassExtenderCompPtr);
	I_REF(iproc::IProcessor, m_filesJoinerCompPtr);
	I_MULTIREF(imtsdlgen::ICxxFileProcessor, m_codeGeneratorExtenderListCompPtr);
	I_REF(iprm::IParamsSet, m_customSchemaParamsCompPtr);
	I_REF(iprm::ITextParam, m_originalSchemaNamespaceCompPtr);
	I_MULTIREF(imtsdlgen::IIncludeDirectivesProvider, m_includeDirectivesProviderListCompPtr);
	I_REF(imtsdl::ICacheMultiManager, m_cacheManagerCompPtr);

	istd::TDelPtr<QFile> m_headerFilePtr;
	istd::TDelPtr<QFile> m_sourceFilePtr;
};


} // namespace imtsdlgenv2

