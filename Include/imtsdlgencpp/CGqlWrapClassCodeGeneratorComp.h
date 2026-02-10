// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
#include <imtsdlgencpp/CSdlGenTools.h>
#include <imtsdlgencpp/CCxxProcessorCompBase.h>


namespace imtsdlgencpp
{

/**
	A C++ class generator of GraphQL wrap for SDL requests
*/
class CGqlWrapClassCodeGeneratorComp:
			public CCxxProcessorCompBase,
			private imtsdl::CSdlTools,
			private CSdlGenTools
{

public:
	typedef CCxxProcessorCompBase BaseClass;

	I_BEGIN_COMPONENT(CGqlWrapClassCodeGeneratorComp)
		I_ASSIGN(m_argumentParserCompPtr, "ArgumentParser", "Command line process argument parser", true, "ArgumentParser")
		I_ASSIGN(m_sdlTypeListCompPtr, "SdlTypeListProvider", "SDL types used to create a code", true, "SdlTypeListProvider")
		I_ASSIGN(m_sdlEnumListCompPtr, "SdlEnumListProvider", "SDL enums used to create a code", true, "SdlEnumListProvider")
		I_ASSIGN(m_sdlUnionListCompPtr, "SdlUnionListProvider", "SDL unions used to create a code", true, "SdlUnionListProvider")
		I_ASSIGN(m_sdlRequestListCompPtr, "SdlRequestListProvider", "SDL requests used to create a GraphQL wrap code", true, "SdlRequestListProvider")
		I_ASSIGN(m_customSchemaParamsCompPtr, "CustomSchemaParams", "Custom schema parameters, that contains additional options", false, "CustomSchemaParams")
		I_ASSIGN(m_originalSchemaNamespaceCompPtr, "OriginalSchemaNamespace", "The namespace of the original(root) schema", true, "OriginalSchemaNamespace")
		I_ASSIGN(m_dependentSchemaListCompPtr, "DependentSchemaList", "The list of dependent schemas, used to generate dependencies of output file", true, "DependentSchemaList")
	I_END_COMPONENT

	// reimplemented (ICxxFileProcessor)
	virtual bool ProcessEntry(
				const imtsdl::CSdlEntryBase& sdlEntry,
				QIODevice* headerDevicePtr,
				QIODevice* sourceDevicePtr = nullptr,
				const iprm::IParamsSet* paramsPtr = nullptr) const override;

	// reimplemented (IIncludeDirectivesProvider)
	virtual QList<imtsdl::IncludeDirective> GetIncludeDirectives() const override;

private:
	bool ProcessHeaderClassFile(const imtsdl::CSdlRequest& sdlRequest, QIODevice* headerDevicePtr, const iprm::IParamsSet* paramsPtr) const;
	bool ProcessSourceClassFile(const imtsdl::CSdlRequest& sdlRequest, QIODevice* sourceDevicePtr, const iprm::IParamsSet* paramsPtr) const;
	bool GenerateFieldRequestInfo(QTextStream& stream, const imtsdl::CSdlField& sdlField, uint hIndents = 1, bool createStructDefinition = false) const;
	void GenerateRequestParsing(QTextStream& stream, const imtsdl::CSdlRequest& sdlRequest, uint hIndents = 1) const;
	void GenerateRequestedFieldsParsing(QTextStream& stream, const imtsdl::CSdlField& sdlField, const QString& idListContainerParamName, const QString& gqlObjectVarName, const QString& complexFieldName = QString(), uint hIndents = 1) const;
	void GenerateRequestedFieldsParsing(QTextStream& stream, const imtsdl::CSdlType& sdlType, const QString& idListContainerParamName, const QString& gqlObjectVarName, const QString& complexFieldName = QString(), uint hIndents = 1) const;
	void GenerateRequestSetup(QTextStream& stream, const imtsdl::CSdlRequest& sdlRequest, uint hIndents = 1) const;

	// header methods
	void AddMethodDeclarations(QTextStream& stream, const imtsdl::CSdlRequest& sdlRequest) const;
	void AddClassProperties(QTextStream& stream, const imtsdl::CSdlRequest& sdlRequest) const;

	// read methods
	bool AddFieldReadFromRequestCode(QTextStream& stream, const imtsdl::CSdlField& field) const;
	void AddScalarFieldReadFromRequestCode(QTextStream& stream, const imtsdl::CSdlField& field) const;
	void AddCustomFieldReadFromRequestCode(QTextStream& stream, const imtsdl::CSdlField& field) const;

	// write methods
	bool AddFieldWriteToRequestCode(QTextStream& stream, const imtsdl::CSdlField& field) const;
	void AddScalarFieldWriteToRequestCode(QTextStream& stream, const imtsdl::CSdlField& field, uint hIndents = 1) const;
	void AddCustomFieldWriteToRequestCode(QTextStream& stream, const imtsdl::CSdlField& field, uint hIndents = 1) const;

	// general help methods for scalar
	void AddExtractValueFromRequestCode(QTextStream& stream, const imtsdl::CSdlField& field, uint hIndents = 1) const;
	/// \todo add error handling for it.
	void AddDataCheckRequiredValueCode(QTextStream& stream, const imtsdl::CSdlField& field, uint hIndents = 1) const;
	void AddSetValueToObjectCode(QTextStream& stream, const imtsdl::CSdlField& field) const;

	// general help methods for custom
	void AddExtractCustomValueFromRequestCode(QTextStream& stream, const imtsdl::CSdlField& field, uint hIndents = 1) const;
	/// \todo add error handling for it.
	void AddCheckCustomRequiredValueCode(QTextStream& stream, const imtsdl::CSdlField& field, uint hIndents = 1) const;
	void AddSetCustomValueToObjectCode(QTextStream& stream, const imtsdl::CSdlField& field, uint hIndents = 1) const;
	void AddSetCustomValueToObjectCodeImpl(QTextStream& stream, const imtsdl::CSdlField& field, const QString& typeVersion = QString(), uint hIndents = 1) const;
	void AddReadFromRequestCode(QTextStream& stream, const imtsdl::CSdlField& field, const QString& typeVersion, const QString& readVariableName, bool optRead, uint hIndents = 1) const;

private:
	I_REF(imtsdl::ISdlProcessArgumentsParser, m_argumentParserCompPtr);
	I_REF(imtsdl::ISdlTypeListProvider, m_sdlTypeListCompPtr);
	I_REF(imtsdl::ISdlEnumListProvider, m_sdlEnumListCompPtr);
	I_REF(imtsdl::ISdlUnionListProvider, m_sdlUnionListCompPtr);
	I_REF(imtsdl::ISdlRequestListProvider, m_sdlRequestListCompPtr);
	I_REF(iprm::IParamsSet, m_customSchemaParamsCompPtr);
	I_REF(iprm::ITextParam, m_originalSchemaNamespaceCompPtr);
	I_REF(iprm::IOptionsManager, m_dependentSchemaListCompPtr);
};


} // namespace imtsdlgencpp

