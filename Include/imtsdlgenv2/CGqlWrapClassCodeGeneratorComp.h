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
#include <imtsdl/ISdlRequestListProvider.h>
#include <imtsdl/CSdlTools.h>
#include <imtsdlgenv2/CSdlGenTools.h>


namespace imtsdlgenv2
{

/**
	A C++ class generator of GraphQL wrap for SDL requests
*/
class CGqlWrapClassCodeGeneratorComp:
			public iproc::CSyncProcessorCompBase,
			private imtsdl::CSdlTools,
			private CSdlGenTools
{

public:
	typedef iproc::CSyncProcessorCompBase BaseClass;

	I_BEGIN_COMPONENT(CGqlWrapClassCodeGeneratorComp)
		I_ASSIGN(m_argumentParserCompPtr, "ArgumentParser", "Command line process argument parser", true, "ArgumentParser")
		I_ASSIGN(m_sdlTypeListCompPtr, "SdlTypeListProvider", "SDL types used to create a code", true, "SdlTypeListProvider")
		I_ASSIGN(m_sdlEnumListCompPtr, "SdlEnumListProvider", "SDL enums used to create a code", true, "SdlEnumListProvider")
		I_ASSIGN(m_sdlRequestListCompPtr, "SdlRequestListProvider", "SDL requests used to create a GraphQL wrap code", true, "SdlRequestListProvider")
		I_ASSIGN(m_filesJoinerCompPtr, "FilesJoiner", "Compoment, used to join files into a single", false, "FilesJoiner")
		I_ASSIGN(m_customSchemaParamsCompPtr, "CustomSchemaParams", "Custom schema parameters, that contains additional options", false, "CustomSchemaParams")
		I_ASSIGN(m_originalSchemaNamespaceCompPtr, "OriginalSchemaNamespace", "The namespace of the original(root) schema", true, "OriginalSchemaNamespace");
		I_ASSIGN(m_dependentSchemaListCompPtr, "DependentSchemaList", "The list of dependent schemas, used to generate dependencies of output file", true, "DependentSchemaList");

	I_END_COMPONENT;

	//reimplemented(iproc::IProcessor)
	virtual int DoProcessing(
				const iprm::IParamsSet* paramsPtr,
				const istd::IPolymorphic* inputPtr,
				istd::IChangeable* outputPtr,
				ibase::IProgressManager* progressManagerPtr = NULL) override;

private:
	bool CloseFiles();
	bool ProcessFiles(const imtsdl::CSdlRequest& sdlRequest, bool addDependenciesInclude, bool addSelfHeaderInclude);
	bool ProcessHeaderClassFile(const imtsdl::CSdlRequest& sdlRequest, bool addDependenciesInclude);
	bool ProcessSourceClassFile(const imtsdl::CSdlRequest& sdlRequest, bool addSelfHeaderInclude);
	void GenerateFieldRequestInfo(QTextStream& stream, const imtsdl::CSdlField& sdlField, uint hIndents = 1, bool createStructDefinition = false);
	void GenerateRequestParsing(QTextStream& stream, const imtsdl::CSdlRequest& sdlRequest, uint hIndents = 1);
	void GenerateRequestedFieldsParsing(QTextStream& stream, const imtsdl::CSdlField& sdlField, const QString& idListContainerParamName, const QString& gqlObjectVarName, const QString& complexFieldName = QString(), uint hIndents = 1);
	void GenerateRequestedFieldsParsing(QTextStream& stream, const imtsdl::CSdlType& sdlType, const QString& idListContainerParamName, const QString& gqlObjectVarName, const QString& complexFieldName = QString(), uint hIndents = 1);
	void GenerateRequestSetup(QTextStream& stream, const imtsdl::CSdlRequest& sdlRequest, uint hIndents = 1);
	void AbortCurrentProcessing();

	// header methods
	void AddRequiredIncludesToHeaderFile(QTextStream& stream, const imtsdl::CSdlRequest& sdlRequest, bool addDependenciesInclude) const;
	void AddMethodDeclarations(QTextStream& stream, const imtsdl::CSdlRequest& sdlRequest) const;
	void AddClassProperties(QTextStream& stream, const imtsdl::CSdlRequest& sdlRequest) const;

	// read methods
	bool AddFieldReadFromRequestCode(QTextStream& stream, const imtsdl::CSdlField& field);
	void AddScalarFieldReadFromRequestCode(QTextStream& stream, const imtsdl::CSdlField& field);
	void AddCustomFieldReadFromRequestCode(QTextStream& stream, const imtsdl::CSdlField& field);

	// write methods
	bool AddFieldWriteToRequestCode(QTextStream& stream, const imtsdl::CSdlField& field);
	void AddScalarFieldWriteToRequestCode(QTextStream& stream, const imtsdl::CSdlField& field, uint hIndents = 1);
	void AddCustomFieldWriteToRequestCode(QTextStream& stream, const imtsdl::CSdlField& field, uint hIndents = 1);

	// general help methods for scalar
	void AddExtractValueFromRequestCode(QTextStream& stream, const imtsdl::CSdlField& field, uint hIndents = 1);
	/// \todo add error handling for it.
	void AddDataCheckRequiredValueCode(QTextStream& stream, const imtsdl::CSdlField& field, uint hIndents = 1);
	void AddSetValueToObjectCode(QTextStream& stream, const imtsdl::CSdlField& field);

	// general help methods for custom
	void AddExtractCustomValueFromRequestCode(QTextStream& stream, const imtsdl::CSdlField& field, uint hIndents = 1);
	/// \todo add error handling for it.
	void AddCheckCustomRequiredValueCode(QTextStream& stream, const imtsdl::CSdlField& field, uint hIndents = 1);
	void AddSetCustomValueToObjectCode(QTextStream& stream, const imtsdl::CSdlField& field, uint hIndents = 1);
	void AddSetCustomValueToObjectCodeImpl(QTextStream& stream, const imtsdl::CSdlField& field, const QString& typeVersion = QString(), uint hIndents = 1);
	void AddReadFromRequestCode(QTextStream& stream, const imtsdl::CSdlField& field, const QString& typeVersion, const QString& readVariableName, bool optRead, uint hIndents = 1);

private:
	I_REF(imtsdl::ISdlProcessArgumentsParser, m_argumentParserCompPtr);
	I_REF(imtsdl::ISdlTypeListProvider, m_sdlTypeListCompPtr);
	I_REF(imtsdl::ISdlEnumListProvider, m_sdlEnumListCompPtr);
	I_REF(imtsdl::ISdlRequestListProvider, m_sdlRequestListCompPtr);
	I_REF(iproc::IProcessor, m_filesJoinerCompPtr);
	I_REF(iprm::IParamsSet, m_customSchemaParamsCompPtr);
	I_REF(iprm::ITextParam, m_originalSchemaNamespaceCompPtr);
	I_REF(iprm::IOptionsManager, m_dependentSchemaListCompPtr);

	istd::TDelPtr<QFile> m_headerFilePtr;
	istd::TDelPtr<QFile> m_sourceFilePtr;
};


} // namespace imtsdlgenv2

