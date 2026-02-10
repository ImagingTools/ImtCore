// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
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
	A C++ class generator of GraphQL handlers for SDL requests
*/
class CGqlHandlerBaseClassGeneratorComp:
			public CCxxProcessorCompBase,
			private imtsdl::CSdlTools,
			private CSdlGenTools
{

public:
	typedef CCxxProcessorCompBase BaseClass;

	I_BEGIN_COMPONENT(CGqlHandlerBaseClassGeneratorComp)
		I_ASSIGN(m_argumentParserCompPtr, "ArgumentParser", "Command line process argument parser", true, "ArgumentParser")
		I_ASSIGN(m_sdlTypeListCompPtr, "SdlTypeListProvider", "SDL types used to create a code", true, "SdlTypeListProvider")
		I_ASSIGN(m_sdlEnumListCompPtr, "SdlEnumListProvider", "SDL enums used to create a code", true, "SdlEnumListProvider")
		I_ASSIGN(m_sdlUnionListCompPtr, "SdlUnionListProvider", "SDL unioins used to create a code", true, "SdlUnionListProvider")
		I_ASSIGN(m_sdlRequestListCompPtr, "SdlRequestListProvider", "SDL requests used to create a GraphQL wrap code", true, "SdlRequestListProvider")
		I_ASSIGN(m_baseClassExtenderCompPtr, "BaseClassExtender", "Compoment, used to add base class inherits", true, "BaseClassExtender")
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
	bool ProcessHeaderClassFile(const imtsdl::CSdlEntryBase& sdlEntry, QIODevice* headerDevicePtr) const;
	bool ProcessSourceClassFile(const imtsdl::CSdlEntryBase& sdlEntry, QIODevice* sourceDevicePtr) const;

	// comfort methods
	void AddMethodsForDocument(QTextStream& stream, uint hIndents = 0) const;
	void AddMethodForDocument(QTextStream& stream, const imtsdl::CSdlRequest& sdlRequest, uint hIndents = 0) const;

	/**
		Creates method implementations for all document's (and subtype's) operation types
	 */
	void AddCollectionMethodsImplForDocument(QTextStream& stream, uint hIndents = 0) const;
	void AddImplCodeForRequests(QTextStream& stream, const imtsdl::SdlRequestList& requestList, const QString& className, uint hIndents = 0) const;
	void AddImplCodeForRequest(QTextStream& stream, const imtsdl::CSdlRequest& sdlRequest, uint hIndents = 0) const;

private:
	I_REF(imtsdl::ISdlProcessArgumentsParser, m_argumentParserCompPtr);
	I_REF(imtsdl::ISdlTypeListProvider, m_sdlTypeListCompPtr);
	I_REF(imtsdl::ISdlEnumListProvider, m_sdlEnumListCompPtr);
	I_REF(imtsdl::ISdlUnionListProvider, m_sdlUnionListCompPtr);
	I_REF(imtsdl::ISdlRequestListProvider, m_sdlRequestListCompPtr);
	I_REF(iproc::IProcessor, m_baseClassExtenderCompPtr);
	I_REF(iprm::IParamsSet, m_customSchemaParamsCompPtr);
	I_REF(iprm::ITextParam, m_originalSchemaNamespaceCompPtr);
	I_REF(iprm::IOptionsManager, m_dependentSchemaListCompPtr);
};


} // namespace imtsdlgencpp

