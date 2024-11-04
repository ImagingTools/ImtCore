#pragma once


//Qt includes
#include <QtCore/QFile>

// ACF includes
#include <istd/TDelPtr.h>
#include <iprm/ITextParam.h>
#include <iproc/TSyncProcessorCompBase.h>

// ImtCore includes
#include <imtsdl/ISdlProcessArgumentsParser.h>
#include <imtsdl/ISdlTypeListProvider.h>
#include <imtsdl/CSdlTools.h>


namespace imtsdl
{


/**
	The base for SDL-C++ class modificators.
*/
class CSdlClassModificatorBaseComp:
			public iproc::CSyncProcessorCompBase,
			protected CSdlTools
{

public:
	typedef iproc::CSyncProcessorCompBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CSdlClassModificatorBaseComp)
		I_ASSIGN(m_processorModificatorNameAttrPtr, "ProcessModificatorName", "The name, allows to do processing, received from arguments in 'modificators' section", false, "<NEED_TO_SET!>")
		I_ASSIGN(m_argumentParserCompPtr, "ArgumentParser", "Command line process argument parser", true, "ArgumentParser")
		I_ASSIGN(m_sdlTypeListCompPtr, "SdlTypeListProvider", "SDL types used to create a code", true, "SdlTypeListProvider")
		I_ASSIGN(m_originalSchemaNamespaceCompPtr, "OriginalSchemaNamespace", "The namespace of the original(root) schema", true, "OriginalSchemaNamespace");
	I_END_COMPONENT;

	// reimplemented (iproc::IProcessor)
	virtual int DoProcessing(
				const iprm::IParamsSet* paramsPtr,
				const istd::IPolymorphic* inputPtr,
				istd::IChangeable* outputPtr,
				ibase::IProgressManager* progressManagerPtr = NULL) override;

protected: // pure virtual methods
	virtual bool ProcessSourceClassFile(const CSdlType& sdlType) = 0;
	virtual bool ProcessHeaderClassFile(const CSdlType& sdlType) = 0;

protected:
	virtual bool BeginClassFiles(const CSdlType& sdlType);
	virtual bool EndClassFiles(const CSdlType& sdlType);
	virtual bool CloseFiles();
	virtual void AbortCurrentProcessing();

protected:
	I_ATTR(QString, m_processorModificatorNameAttrPtr);
	I_REF(ISdlProcessArgumentsParser, m_argumentParserCompPtr);
	I_REF(ISdlTypeListProvider, m_sdlTypeListCompPtr);
	I_REF(iprm::ITextParam, m_originalSchemaNamespaceCompPtr);

	istd::TDelPtr<QFile> m_headerFilePtr;
	istd::TDelPtr<QFile> m_sourceFilePtr;

	istd::TDelPtr<QFile> m_originalHeaderFilePtr;
	istd::TDelPtr<QFile> m_originalSourceFilePtr;
};


} // namespace imtsdl

