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
#include <imtsdl/CSdlTools.h>
#include <imtsdlgencpp/ICxxModifier.h>
#include <imtsdlgencpp/CSdlGenTools.h>
#include <imtsdlgencpp/CCxxProcessorCompBase.h>


namespace imtsdlgencpp
{

/**
	A base C++ class generator of SDL types
*/
class CSdlClassCodeGeneratorComp:
			public CCxxProcessorCompBase,
			private imtsdl::CSdlTools,
			private CSdlGenTools
{

public:
	typedef CCxxProcessorCompBase BaseClass;
	typedef imtsdl::CSdlTools BaseClass2;
	typedef CSdlGenTools BaseClass3;

	I_BEGIN_COMPONENT(CSdlClassCodeGeneratorComp)
		I_ASSIGN(m_sdlTypeListCompPtr, "SdlTypeListProvider", "SDL types used to create a code", true, "SdlTypeListProvider")
		I_ASSIGN(m_sdlEnumListCompPtr, "SdlEnumListProvider", "SDL enums used to create a code", true, "SdlEnumListProvider")
		I_ASSIGN(m_sdlUnionListCompPtr, "SdlUnionListProvider", "SDL unioins used to create a code", true, "SdlUnionListProvider")
		I_ASSIGN_MULTI_0(m_codeGeneratorExtenderListCompPtr, "CodeGeneratorExtenderList", "Extenders, used to generate an additional code", false)
		I_ASSIGN(m_customSchemaParamsCompPtr, "CustomSchemaParams", "Custom schema parameters, that contains additional options", false, "CustomSchemaParams")
		I_ASSIGN(m_originalSchemaNamespaceCompPtr, "OriginalSchemaNamespace", "The namespace of the original(root) schema", true, "OriginalSchemaNamespace");
		I_ASSIGN_MULTI_0(m_includeDirectivesProviderListCompPtr, "IncludeDirectivesProviderList", "Providers of include directives, used to generate C(++) directives", false)
		I_ASSIGN_MULTI_0(m_modifierListCompPtr, "ModifierList", "Modifiers for specific containers, used to generate C++ methods serialization methods ", false)
		I_ASSIGN(m_dependentSchemaListCompPtr, "DependentSchemaList", "The list of dependent schemas, used to generate dependencies of output file", true, "DependentSchemaList");
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
	enum MetdodType
	{
		MT_READ,
		MT_WRITE,
		MT_OPT_READ,
		MT_OPT_WRITE
	};

	bool BeginClassFiles(const imtsdl::CSdlType& sdlType, QIODevice* headerPtr, QIODevice* sourcePtr, const iprm::IParamsSet* paramsPtr) const;
	bool BeginHeaderClassFile(const imtsdl::CSdlType& sdlType, QIODevice* headerPtr, const iprm::IParamsSet* paramsPtr) const;
	bool BeginSourceClassFile(const imtsdl::CSdlType& sdlType, QIODevice* sourcePtr, const iprm::IParamsSet* paramsPtr) const;
	bool EndClassFiles(const imtsdl::CSdlType& sdlType, QIODevice* headerPtr, QIODevice* sourcePtr, const iprm::IParamsSet* paramsPtr) const;
	void GenerateMetaInfo(
				QTextStream& stream,
				const imtsdl::CSdlType& sdlType,
				uint indents = 1) const;
	void GenerateVersionStruct(
				QTextStream& stream,
				const imtsdl::CSdlType& sdlType,
				uint indents = 1) const;
	void GenerateMethodDefinition(
				QTextStream& stream,
				const imtsdl::CSdlType& sdlType,
				MetdodType methodType,
				ICxxModifier& modifier,
				bool forHeader) const;
	QString GetVersionMemberVariableName(
				const imtsdl::CSdlType& sdlType,
				int versionIndex = -1) const;
	void GenerateVersionMemberDeclaration(
				QTextStream& stream,
				const imtsdl::CSdlType& sdlType,
				bool optWrap = false,
				int versionIndex = -1) const;
	void GenerateMethodImplementation(
				QTextStream& stream,
				const imtsdl::CSdlType& sdlType,
				MetdodType methodType,
				ICxxModifier& modifier) const;
	void GenerateMethodCall(
				QTextStream& stream,
				const imtsdl::CSdlType& sdlType,
				MetdodType methodType,
				ICxxModifier& modifier) const;

private:
	I_REF(imtsdl::ISdlTypeListProvider, m_sdlTypeListCompPtr);
	I_REF(imtsdl::ISdlEnumListProvider, m_sdlEnumListCompPtr);
	I_REF(imtsdl::ISdlUnionListProvider, m_sdlUnionListCompPtr);
	I_MULTIREF(ICxxFileProcessor, m_codeGeneratorExtenderListCompPtr);
	I_REF(iprm::IParamsSet, m_customSchemaParamsCompPtr);
	I_REF(iprm::ITextParam, m_originalSchemaNamespaceCompPtr);
	I_MULTIREF(IIncludeDirectivesProvider, m_includeDirectivesProviderListCompPtr);
	I_MULTIREF(ICxxModifier, m_modifierListCompPtr);
	I_REF(iprm::IOptionsManager, m_dependentSchemaListCompPtr);
};


} // namespace imtsdlgencpp

