#pragma once


//Qt includes
#include <QtCore/QFile>

// Acf includes
#include <istd/TDelPtr.h>
#include <iproc/TSyncProcessorCompBase.h>

// imtsdl includes
#include <imtsdl/ISdlProcessArgumentsParser.h>
#include <imtsdl/CGqlSchemaParser.h>
#include <imtsdl/CSdlTools.h>


namespace imtsdl
{

/**
	A base C++ class generator of SDL types
*/
class CSdlClassCodeGeneratorComp:
			public iproc::CSyncProcessorCompBase,
			public CGqlSchemaParser,
			private CSdlTools
{

public:
	typedef iproc::CSyncProcessorCompBase BaseClass;
	typedef CGqlSchemaParser BaseClass2;

	I_BEGIN_COMPONENT(CSdlClassCodeGeneratorComp)
		I_ASSIGN(m_argumentParserCompPtr, "ArgumentParser", "Command line process argument parser", true, "ArgumentParser")
		I_ASSIGN(m_sdlTypeListCompPtr, "SdlTypeListProvider", "SDL types used to create a code", true, "SdlTypeListProvider")
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
	bool BeginClassFiles(const CSdlType& sdlType);
	bool BeginHeaderClassFile(const CSdlType& sdlType);
	bool EndHeaderClassFile(const CSdlType& sdlType);
	bool BeginSourceClassFile(const CSdlType& sdlType);
	bool EndClassFiles(const CSdlType& sdlType);
	void AbortCurrentProcessing();
	QString GenerateAccessMethods(const CSdlField& sdlField, uint indents = 1, bool generateGetter = true, bool generateSetter = true);
	QString GenerateAccessMethodsImpl(const QString className, const CSdlField& sdlField, uint indents = 0, bool generateGetter = true, bool generateSetter = true);

private:
	I_REF(ISdlProcessArgumentsParser, m_argumentParserCompPtr);
	I_REF(ISdlTypeListProvider, m_sdlTypeListCompPtr);
	I_MULTIREF(iproc::IProcessor, m_codeGeneratorExtenderListCompPtr);

	istd::TDelPtr<QFile> m_headerFilePtr;
	istd::TDelPtr<QFile> m_sourceFilePtr;
};


} // namespace imtsdl

