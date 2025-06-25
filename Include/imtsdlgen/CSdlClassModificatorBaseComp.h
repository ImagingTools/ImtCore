#pragma once


//Qt includes
#include <QtCore/QFile>

// ACF includes
#include <istd/TDelPtr.h>
#include <iprm/ITextParam.h>
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtsdl/ISdlProcessArgumentsParser.h>
#include <imtsdl/ISdlTypeListProvider.h>
#include <imtsdl/ISdlEnumListProvider.h>
#include <imtsdl/ISdlUnionListProvider.h>
#include <imtsdl/CSdlTools.h>
#include <imtsdl/CSdlEnumConverter.h>
#include <imtsdl/CSdlUnionConverter.h>
#include <imtsdlgen/IIncludeDirectivesProvider.h>
#include <imtsdlgen/ICxxFileProcessor.h>
#include <imtsdlgen/ICxxModifier.h>


namespace imtsdlgen
{


/**
	The base for SDL-C++ class modificators.
*/
class CSdlClassModificatorBaseComp:
			public ilog::CLoggerComponentBase,
			public IIncludeDirectivesProvider,
			public ICxxFileProcessor,
			public ICxxModifier,
			protected imtsdl::CSdlEnumConverter,
			protected imtsdl::CSdlUnionConverter
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CSdlClassModificatorBaseComp)
		I_REGISTER_INTERFACE(IIncludeDirectivesProvider)
		I_REGISTER_INTERFACE(ICxxFileProcessor)
		I_REGISTER_INTERFACE(ICxxModifier)
		I_ASSIGN(m_processorModificatorNameAttrPtr, "ProcessModificatorName", "The name, allows to do processing, received from arguments in 'modificators' section", false, "<NEED_TO_SET!>")
		I_ASSIGN(m_argumentParserCompPtr, "ArgumentParser", "Command line process argument parser", true, "ArgumentParser")
		I_ASSIGN(m_sdlTypeListCompPtr, "SdlTypeListProvider", "SDL types used to create a code", true, "SdlTypeListProvider")
		I_ASSIGN(m_sdlEnumListCompPtr, "SdlEnumListProvider", "SDL enums used to create a code", true, "SdlEnumListProvider")
		I_ASSIGN(m_sdlUnionListCompPtr, "SdlUnionListProvider", "SDL unioins used to create a code", true, "SdlUnionListProvider")
		I_ASSIGN(m_originalSchemaNamespaceCompPtr, "OriginalSchemaNamespace", "The namespace of the original(root) schema", true, "OriginalSchemaNamespace");
	I_END_COMPONENT;

	// reimplemented (ICxxFileProcessor)
	[[nodiscard]] virtual bool ProcessType(const imtsdl::CSdlType& sdlType, QFile* headerFilePtr, QFile* sourceFilePtr) override;


protected: // pure virtual methods
	virtual bool ProcessSourceClassFile(const imtsdl::CSdlType& sdlType) = 0;
	virtual bool ProcessHeaderClassFile(const imtsdl::CSdlType& sdlType) = 0;

protected:
	I_ATTR(QString, m_processorModificatorNameAttrPtr);
	I_REF(imtsdl::ISdlProcessArgumentsParser, m_argumentParserCompPtr);
	I_REF(imtsdl::ISdlTypeListProvider, m_sdlTypeListCompPtr);
	I_REF(imtsdl::ISdlEnumListProvider, m_sdlEnumListCompPtr);
	I_REF(imtsdl::ISdlUnionListProvider, m_sdlUnionListCompPtr);
	I_REF(iprm::ITextParam, m_originalSchemaNamespaceCompPtr);

	QFile* m_headerFilePtr;
	QFile* m_sourceFilePtr;

	QString m_logTag;
};


} // namespace imtsdlgen

