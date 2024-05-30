	#pragma once


//Qt includes
#include <QtCore/QFile>

// Acf includes
#include <istd/TDelPtr.h>
#include <iproc/TSyncProcessorCompBase.h>

// imtsdl includes
#include <imtsdl/ISdlProcessArgumentsParser.h>
#include <imtsdl/ISdlTypeListProvider.h>
#include <imtsdl/ISdlRequestListProvider.h>
#include <imtsdl/CSdlTools.h>


namespace imtsdl
{

/**
	A C++ class generator of GraphQL wrap for SDL requests
*/
class CGqlWrapClassCodeGeneratorComp:
			public iproc::CSyncProcessorCompBase,
			private CSdlTools
{

public:
	typedef iproc::CSyncProcessorCompBase BaseClass;

	I_BEGIN_COMPONENT(CGqlWrapClassCodeGeneratorComp)
		I_ASSIGN(m_argumentParserCompPtr, "ArgumentParser", "Command line process argument parser", true, "ArgumentParser")
		I_ASSIGN(m_sdlTypeListCompPtr, "SdlTypeListProvider", "SDL types used to create a code", true, "SdlTypeListProvider")
		I_ASSIGN(m_sdlRequestListCompPtr, "SdlRequestListProvider", "SDL requests used to create a GraphQL wrap code", true, "SdlRequestListProvider")
	I_END_COMPONENT;

	//reimplemented(iproc::IProcessor)
	virtual int DoProcessing(
				const iprm::IParamsSet* paramsPtr,
				const istd::IPolymorphic* inputPtr,
				istd::IChangeable* outputPtr,
				ibase::IProgressManager* progressManagerPtr = NULL) override;

private:
	bool CloseFiles();
	bool ProcessFiles(const CSdlRequest& sdlRequest);
	bool ProcessHeaderClassFile(const CSdlRequest& sdlRequest);
	bool ProcessSourceClassFile(const CSdlRequest& sdlRequest);
	void GenerateFieldRequestInfo(QTextStream& stream, const CSdlField& sdlField, uint hIndents = 1, bool createStructDefinition = false);
	void GenerateRequestParsing(QTextStream& ifStream, const CSdlRequest& sdlRequest, uint hIndents = 1);
	void AbortCurrentProcessing();

private:
	I_REF(ISdlProcessArgumentsParser, m_argumentParserCompPtr);
	I_REF(ISdlTypeListProvider, m_sdlTypeListCompPtr);
	I_REF(ISdlRequestListProvider, m_sdlRequestListCompPtr);

	istd::TDelPtr<QFile> m_headerFilePtr;
	istd::TDelPtr<QFile> m_sourceFilePtr;
};


} // namespace imtsdl

