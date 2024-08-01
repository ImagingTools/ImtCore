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
		I_ASSIGN(m_filesJoinerCompPtr, "FilesJoiner", "Compoment, used to join files into a single", false, "FilesJoiner")
	I_END_COMPONENT;

	//reimplemented(iproc::IProcessor)
	virtual int DoProcessing(
				const iprm::IParamsSet* paramsPtr,
				const istd::IPolymorphic* inputPtr,
				istd::IChangeable* outputPtr,
				ibase::IProgressManager* progressManagerPtr = NULL) override;

private:
	bool CloseFiles();
	bool ProcessFiles(const CSdlRequest& sdlRequest, bool addDependenciesInclude, bool addSelfHeaderInclude);
	bool ProcessHeaderClassFile(const CSdlRequest& sdlRequest, bool addDependenciesInclude);
	bool ProcessSourceClassFile(const CSdlRequest& sdlRequest, bool addSelfHeaderInclude);
	void GenerateFieldRequestInfo(QTextStream& stream, const CSdlField& sdlField, uint hIndents = 1, bool createStructDefinition = false);
	void GenerateRequestParsing(QTextStream& ifStream, const CSdlRequest& sdlRequest, uint hIndents = 1);
	void AbortCurrentProcessing();

	// read methods
	bool AddFieldReadFromRequestCode(QTextStream& stream, const CSdlField& field);
	void AddScalarFieldReadFromRequestCode(QTextStream& stream, const CSdlField& field);
	void AddCustomFieldReadFromRequestCode(QTextStream& stream, const CSdlField& field);

	// write methods
	bool AddFieldWriteToRequestCode(QTextStream& stream, const CSdlField& field);
	void AddScalarFieldWriteToRequestCode(QTextStream& stream, const CSdlField& field, uint hIndents = 1);
	void AddCustomFieldWriteToRequestCode(QTextStream& stream, const CSdlField& field, uint hIndents = 1);

	// general help methods for scalar
	void AddExtractValueFromRequestCode(QTextStream& stream, const CSdlField& field, uint hIndents = 1);
	/// \todo add error handling for it.
	void AddDataCheckRequiredValueCode(QTextStream& stream, const CSdlField& field, uint hIndents = 1);
	void AddSetValueToObjectCode(QTextStream& stream, const CSdlField& field);

	// general help methods for custom
	void AddExtractCustomValueFromRequestCode(QTextStream& stream, const CSdlField& field, uint hIndents = 1);
	/// \todo add error handling for it.
	void AddCheckCustomRequiredValueCode(QTextStream& stream, const CSdlField& field, uint hIndents = 1);
	void AddSetCustomValueToObjectCode(QTextStream& stream, const CSdlField& field, uint hIndents = 1);

private:
	I_REF(ISdlProcessArgumentsParser, m_argumentParserCompPtr);
	I_REF(ISdlTypeListProvider, m_sdlTypeListCompPtr);
	I_REF(ISdlRequestListProvider, m_sdlRequestListCompPtr);
	I_REF(iproc::IProcessor, m_filesJoinerCompPtr);

	istd::TDelPtr<QFile> m_headerFilePtr;
	istd::TDelPtr<QFile> m_sourceFilePtr;
};


} // namespace imtsdl

