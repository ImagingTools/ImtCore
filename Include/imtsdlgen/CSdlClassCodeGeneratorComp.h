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
#include <imtsdl/CSdlTools.h>
#include <imtsdlgen/IIncludeDirectivesProvider.h>


namespace imtsdlgen
{

/**
	A base C++ class generator of SDL types
*/
class  CSdlClassCodeGeneratorComp:
			public iproc::CSyncProcessorCompBase,
			private imtsdl::CSdlTools
{

public:
	typedef iproc::CSyncProcessorCompBase BaseClass;

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

	I_END_COMPONENT

	//reimplemented(iproc::IProcessor)
	virtual int DoProcessing(
				const iprm::IParamsSet* paramsPtr,
				const istd::IPolymorphic* inputPtr,
				istd::IChangeable* outputPtr,
				ibase::IProgressManager* progressManagerPtr = NULL) override;

private:
	bool ReOpenFiles();
	bool CloseFiles();
	bool BeginClassFiles(const imtsdl::CSdlType& sdlType, bool addDependenciesInclude, bool addSelfHeaderInclude);
	bool BeginHeaderClassFile(const imtsdl::CSdlType& sdlType, bool addDependenciesInclude);
	bool EndHeaderClassFile(const imtsdl::CSdlType& sdlType);
	bool BeginSourceClassFile(const imtsdl::CSdlType& sdlType, bool addSelfHeaderInclude);
	bool EndSourceClassFile(const imtsdl::CSdlType& sdlType);
	bool EndClassFiles(const imtsdl::CSdlType& sdlType);
	void AbortCurrentProcessing();
	QString GenerateAccessMethods(
				const imtsdl::CSdlField& sdlField,
				uint indents = 1,
				bool generateGetter = true,
				bool generateSetter = true,
				bool generateExistenceChecks = true);
	void GenerateAccessMethodsImpl(
				QTextStream& stream,
				const QString className,
				const imtsdl::CSdlField& sdlField,
				uint indents = 0,
				bool generateGetter = true,
				bool generateSetter = true,
				bool generateExistenceChecks = true);
	void GenerateResetMethodImpl(
				QTextStream& stream,
				const QString className,
				const imtsdl::CSdlField& sdlField,
				uint indents = 0);
	void GenerateMetaInfo(
				QTextStream& stream,
				const imtsdl::CSdlType& sdlType,
				uint indents = 1);

private:
	I_REF(imtsdl::ISdlProcessArgumentsParser, m_argumentParserCompPtr);
	I_REF(imtsdl::ISdlTypeListProvider, m_sdlTypeListCompPtr);
	I_REF(imtsdl::ISdlEnumListProvider, m_sdlEnumListCompPtr);
	I_REF(iproc::IProcessor, m_baseClassExtenderCompPtr);
	I_REF(iproc::IProcessor, m_filesJoinerCompPtr);
	I_MULTIREF(iproc::IProcessor, m_codeGeneratorExtenderListCompPtr);
	I_MULTIREF(IIncludeDirectivesProvider, m_includeDirectivesProviderListCompPtr);
	I_REF(iprm::IParamsSet, m_customSchemaParamsCompPtr);
	I_REF(iprm::ITextParam, m_originalSchemaNamespaceCompPtr);

	istd::TDelPtr<QFile> m_headerFilePtr;
	istd::TDelPtr<QFile> m_sourceFilePtr;
};


} // namespace imtsdlgen

