// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/IParamsSet.h>

// ImtCore includes
#include <imtsdl/ISdlProcessArgumentsParser.h>
#include <imtsdl/ISdlEnumListProvider.h>
#include <imtsdl/ISdlUnionListProvider.h>
#include <imtsdl/ISdlTypeListProvider.h>
#include <imtsdl/ISdlDocumentTypeListProvider.h>
#include <imtsdl/ISdlRequestListProvider.h>
#include <imtsdl/CSdlTools.h>
#include <imtsdlgencpp/ICxxFileProcessor.h>
#include <imtsdlgencpp/ICxxFilePostProcessor.h>
#include <imtsdlgencpp/CSdlGenTools.h>


namespace imtsdlgencpp
{


/**
	Generates the forward declaration file (_fwd.h) for all generated classes (types, unions, document types, requests).
	This enables consumers to use forward declarations instead of heavy full includes.
*/
class CCxxForwardDeclarationPostProcessorComp:
			public ilog::CLoggerComponentBase,
			virtual public ICxxFilePostProcessor,
			private imtsdl::CSdlTools,
			private CSdlGenTools
{

public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CCxxForwardDeclarationPostProcessorComp)
		I_REGISTER_INTERFACE(ICxxFilePostProcessor)
		I_ASSIGN(m_argumentParserCompPtr, "ArgumentParser", "Command line process argument parser", true, "ArgumentParser")
		I_ASSIGN(m_schemaParamsCompPtr, "CurrentSchemaParams", "Params of the processing (root) shcema", true, "CurrentSchemaParams")
		I_ASSIGN(m_sdlEnumListCompPtr, "SdlEnumListProvider", "SDL enums used to create a code", false, "SdlEnumListProvider")
		I_ASSIGN(m_sdlUnionListCompPtr, "SdlUnionListProvider", "SDL unioins used to create a code", false, "SdlUnionListProvider")
		I_ASSIGN(m_sdlTypeListCompPtr, "SdlTypeListProvider", "SDL types used to create a code", false, "SdlTypeListProvider")
		I_ASSIGN(m_sdlDocumentTypeListCompPtr, "SdlDocumentTypeListProvider", "SDL document types used to create a code", false, "SdlDocumentTypeListProvider")
		I_ASSIGN(m_requestsProviderListCompPtr, "SdlRequestListProvider", "SDL requests used to create a code", false, "SdlRequestListProvider")
		I_ASSIGN_MULTI_0(m_documentTypeProcessorCompListPtr, "DocumentTypeProcessorsList", "The list of C++ processors, that write the definitions of document types classes to the forward declaration file", false)
		I_ASSIGN_MULTI_0(m_autoProcessorCompListPtr, "AutoProcessorsList", "The list of special C++ processors, that write the definitions of requests classes to the forward declaration file", false)
	I_END_COMPONENT

	// reimplemented (ICxxFilePostProcessor)
	virtual bool ProcessFiles(const CxxEntryFileInfo& files, const iprm::IParamsSet* paramsPtr = nullptr) const override;

private:
	I_REF(imtsdl::ISdlProcessArgumentsParser, m_argumentParserCompPtr);
	I_REF(iprm::IParamsSet, m_schemaParamsCompPtr);
	I_REF(imtsdl::ISdlEnumListProvider, m_sdlEnumListCompPtr);
	I_REF(imtsdl::ISdlUnionListProvider, m_sdlUnionListCompPtr);
	I_REF(imtsdl::ISdlTypeListProvider, m_sdlTypeListCompPtr);
	I_REF(imtsdl::ISdlDocumentTypeListProvider, m_sdlDocumentTypeListCompPtr);
	I_REF(imtsdl::ISdlRequestListProvider, m_requestsProviderListCompPtr);
	I_MULTIREF(ICxxFileProcessor, m_documentTypeProcessorCompListPtr);
	I_MULTIREF(ICxxFileProcessor, m_autoProcessorCompListPtr);
};


} // namespace imtsdlgencpp
