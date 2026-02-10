// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iproc/TSyncProcessorWrap.h>

// ImtCore includes
#include <imtsdl/ISdlProcessArgumentsParser.h>
#include <imtsdl/ISdlTypeListProvider.h>
#include <imtsdl/ISdlRequestListProvider.h>
#include <imtsdl/ISdlDocumentTypeListProvider.h>
#include <imtsdl/CSdlTools.h>
#include <imtsdlgencpp/ICxxFileProcessor.h>
#include <imtsdlgencpp/IIncludeDirectivesProvider.h>
#include <imtsdlgencpp/CSdlGenTools.h>


class QFile;

namespace iprm
{
class CParamsSet;
}


namespace imtsdlgencpp
{

class CxxProcessorsManagerAttr1Comp: public ilog::CLoggerComponentBase
{

public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CxxProcessorsManagerAttr1Comp)
		I_ASSIGN_MULTI_0(m_enumProcessorCompListPtr, "EnumProcessorsList", "The list of C++ processors generation for enums", false)
		I_ASSIGN_MULTI_0(m_unionProcessorCompListPtr, "UnionProcessorsList", "The list of C++ processors generation for unions", false)
		I_ASSIGN_MULTI_0(m_typeProcessorCompListPtr, "TypeProcessorsList", "The list of C++ processors generation for types", false)
		I_ASSIGN_MULTI_0(m_documentTypeProcessorCompListPtr, "DocumentTypeProcessorsList", "The list of C++ processors generation for document types", false)
		I_ASSIGN_MULTI_0(m_requestProcessorCompListPtr, "RequestProcessorsList", "The list of C++ processors generation for requests", false)
		I_ASSIGN_MULTI_0(m_autoProcessorCompListPtr, "AutoProcessorsList", "The list of special C++ processors generation.", false)
		I_ASSIGN_MULTI_0(m_includeDirectivesProviderListCompPtr, "IncludeDirectivesProviderList", "Providers of include directives, used to generate C(++) directives", false)

	I_END_COMPONENT

protected:
	I_MULTIREF(ICxxFileProcessor, m_enumProcessorCompListPtr);
	I_MULTIREF(ICxxFileProcessor, m_unionProcessorCompListPtr);
	I_MULTIREF(ICxxFileProcessor, m_typeProcessorCompListPtr);
	I_MULTIREF(ICxxFileProcessor, m_documentTypeProcessorCompListPtr);
	I_MULTIREF(ICxxFileProcessor, m_requestProcessorCompListPtr);
	I_MULTIREF(ICxxFileProcessor, m_autoProcessorCompListPtr);
	I_MULTIREF(IIncludeDirectivesProvider, m_includeDirectivesProviderListCompPtr);
};

class CCxxProcessorsManagerComp: public CxxProcessorsManagerAttr1Comp,
							public iproc::CSyncProcessorBase,
							private imtsdl::CSdlTools,
							private CSdlGenTools
{


public:
	typedef std::shared_ptr<QFile> FilePtr;
	typedef QMap<QString/*EntryName*/, FilePtr> EntryFileMap;

	typedef CxxProcessorsManagerAttr1Comp BaseClass;

	I_BEGIN_COMPONENT(CCxxProcessorsManagerComp)
		I_REGISTER_INTERFACE(iproc::IProcessor)
		I_ASSIGN(m_argumentParserCompPtr, "ArgumentParser", "Argument parser generation", true, "ArgumentParser")
		I_ASSIGN(m_schemaParamsCompPtr, "CurrentSchemaParams", "Params of the processing (root) shcema", true, "CurrentSchemaParams")
		I_ASSIGN(m_sdlEnumListCompPtr, "SdlEnumListProvider", "SDL enums used to create a code", false, "SdlEnumListProvider")
		I_ASSIGN(m_sdlUnionListCompPtr, "SdlUnionListProvider", "SDL unioins used to create a code", false, "SdlUnionListProvider")
		I_ASSIGN(m_sdlTypeListCompPtr, "SdlTypeListProvider", "SDL types used to create a code", false, "SdlTypeListProvider")
		I_ASSIGN(m_sdlDocumentTypeListCompPtr, "SdlDocumentTypeListProvider", "SDL document types used to create a code", false, "SdlDocumentTypeListProvider")
		I_ASSIGN(m_requestsProviderListCompPtr, "SdlRequestListProvider", "SDL requests used to create a code", false, "SdlRequestListProvider")
		I_ASSIGN(m_baseClassExtenderCompPtr, "BaseClassExtender", "The C++ class extender, that adds inheritance to class files", false, "BaseClassExtender")
		I_ASSIGN(m_dependentSchemaListCompPtr, "DependentSchemaList", "The list of dependent schemas, used to generate dependencies of output file", true, "DependentSchemaList");
	I_END_COMPONENT


	// reimplemented (iproc::IProcessor)
	virtual TaskState DoProcessing(
				const iprm::IParamsSet* paramsPtr,
				const istd::IPolymorphic* inputPtr,
				istd::IChangeable* outputPtr,
				ibase::IProgressManager* progressManagerPtr = nullptr) override;

private:
	[[nodiscard]] bool BeginHeaderFile(const iprm::IParamsSet& entryParams, QFile& headerFile, const iprm::IParamsSet* paramsPtr) const;
	[[nodiscard]] bool BeginSourceFile(const iprm::IParamsSet& entryParams, QFile& headerFile, const iprm::IParamsSet* paramsPtr) const;
	[[nodiscard]] bool EndHeaderFile(const iprm::IParamsSet& entryParams, QFile& headerFile, const iprm::IParamsSet* paramsPtr) const;
	[[nodiscard]] bool EndSourceFile(const iprm::IParamsSet& entryParams, QFile& sourceFile, const iprm::IParamsSet* paramsPtr) const;
	bool ProcessEnums (
				const EntryFileMap& headerFiles,
				const EntryFileMap& sourceFiles,
				const iprm::IParamsSet* paramsPtr) const;
	bool ProcessTypes (
				const EntryFileMap& headerFiles,
				const EntryFileMap& sourceFiles,
				const iprm::IParamsSet* paramsPtr) const;
	bool ProcessUnions(
				const EntryFileMap& headerFiles,
				const EntryFileMap& sourceFiles,
				const iprm::IParamsSet* paramsPtr) const;
	bool ProcessDocumentTypes(
				const EntryFileMap& headerFiles,
				const EntryFileMap& sourceFiles,
				const iprm::IParamsSet* paramsPtr) const;
	bool ProcessRequests(
				const EntryFileMap& headerFiles,
				const EntryFileMap& sourceFiles,
				const iprm::IParamsSet* paramsPtr) const;

	/**
		Creates files for all types if split doesn't enabled, key[EntryName] will be empty, and only one pointer will be created and returned
		\warning all files will be opened!
		\note all file pointers MUST NOT be NULL
	*/
	[[nodiscard]] EntryFileMap CreateHeaderFiles(const iprm::IParamsSet* paramsPtr) const;
	[[nodiscard]] EntryFileMap CreateSourceFiles(const iprm::IParamsSet* paramsPtr) const;
	/**
		Createsn and opens a file by name
		\note all parent directories will be created
	*/
	[[nodiscard]] FilePtr CreateFile(const QString& filePath) const;


private:
	I_REF(imtsdl::ISdlProcessArgumentsParser, m_argumentParserCompPtr);
	I_REF(imtsdl::ISdlEnumListProvider, m_sdlEnumListCompPtr);
	I_REF(imtsdl::ISdlUnionListProvider, m_sdlUnionListCompPtr);
	I_REF(imtsdl::ISdlTypeListProvider, m_sdlTypeListCompPtr);
	I_REF(imtsdl::ISdlDocumentTypeListProvider, m_sdlDocumentTypeListCompPtr);
	I_REF(imtsdl::ISdlRequestListProvider, m_requestsProviderListCompPtr);
	I_REF(iproc::IProcessor, m_baseClassExtenderCompPtr);
	I_REF(iprm::IParamsSet, m_schemaParamsCompPtr);
	I_REF(iprm::IOptionsManager, m_dependentSchemaListCompPtr);
};



} // namespace imtsdlgencpp







