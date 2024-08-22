#pragma once


//Qt includes
#include <QtCore/QFile>

// Acf includes
#include <istd/TDelPtr.h>
#include <iproc/TSyncProcessorCompBase.h>

// imtsdl includes
#include <imtsdl/ISdlProcessArgumentsParser.h>
#include <imtsdl/ISdlTypeListProvider.h>
#include <imtsdl/CSdlTools.h>


namespace imtsdl
{

/**
	A base C++ class generator of SDL types
*/
class CSdlClassCodeGeneratorComp:
			public iproc::CSyncProcessorCompBase,
			private CSdlTools
{

public:
	typedef iproc::CSyncProcessorCompBase BaseClass;

	I_BEGIN_COMPONENT(CSdlClassCodeGeneratorComp)
		I_ASSIGN(m_argumentParserCompPtr, "ArgumentParser", "Command line process argument parser", true, "ArgumentParser")
		I_ASSIGN(m_sdlTypeListCompPtr, "SdlTypeListProvider", "SDL types used to create a code", true, "SdlTypeListProvider")
		I_ASSIGN(m_baseClassExtenderCompPtr, "BaseClassExtender", "The C++ class extender, that adds inheritance to class files", true, "BaseClassExtender")
		I_ASSIGN(m_filesJoinerCompPtr, "FilesJoiner", "Compoment, used to join files into a single", false, "FilesJoiner")
		I_ASSIGN_MULTI_0(m_codeGeneratorExtenderListCompPtr, "CodeGeneratorExtenderList", "Extenders, used to generate an additional code", false)
	I_END_COMPONENT;

	//reimplemented(iproc::IProcessor)
	virtual int DoProcessing(
				const iprm::IParamsSet* paramsPtr,
				const istd::IPolymorphic* inputPtr,
				istd::IChangeable* outputPtr,
				ibase::IProgressManager* progressManagerPtr = NULL) override;

private:
	bool ReOpenFiles();
	bool CloseFiles();
	bool BeginClassFiles(const CSdlType& sdlType, bool addDependenciesInclude, bool addSelfHeaderInclude);
	bool BeginHeaderClassFile(const CSdlType& sdlType, bool addDependenciesInclude);
	bool EndHeaderClassFile(const CSdlType& sdlType);
	bool BeginSourceClassFile(const CSdlType& sdlType, bool addSelfHeaderInclude);
	bool EndSourceClassFile(const CSdlType& sdlType);
	bool EndClassFiles(const CSdlType& sdlType);
	void AbortCurrentProcessing();
	QString GenerateAccessMethods(
				const CSdlField& sdlField,
				uint indents = 1,
				bool generateGetter = true,
				bool generateSetter = true,
				bool generateExistenceChecks = true);
	void GenerateAccessMethodsImpl(
				QTextStream& stream,
				const QString className,
				const CSdlField& sdlField,
				uint indents = 0,
				bool generateGetter = true,
				bool generateSetter = true,
				bool generateExistenceChecks = true);
	void GenerateMetaInfo(
				QTextStream& stream,
				const CSdlType& sdlType,
				uint indents = 1);
	void GenerateListUpdateCode(
				QTextStream& stream,
				const CSdlField& sdlField,
				bool isCustom,
				uint indents = 1);

private:
	I_REF(ISdlProcessArgumentsParser, m_argumentParserCompPtr);
	I_REF(ISdlTypeListProvider, m_sdlTypeListCompPtr);
	I_REF(iproc::IProcessor, m_baseClassExtenderCompPtr);
	I_REF(iproc::IProcessor, m_filesJoinerCompPtr);
	I_MULTIREF(iproc::IProcessor, m_codeGeneratorExtenderListCompPtr);

	istd::TDelPtr<QFile> m_headerFilePtr;
	istd::TDelPtr<QFile> m_sourceFilePtr;
};


} // namespace imtsdl

