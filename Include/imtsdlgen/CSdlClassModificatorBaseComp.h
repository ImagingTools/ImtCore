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
#include <imtsdl/ISdlEnumListProvider.h>
#include <imtsdl/CSdlTools.h>
#include <imtsdl/CSdlEnumConverter.h>
#include <imtsdlgen/IIncludeDirectivesProvider.h>


namespace imtsdlgen
{


/**
	The base for SDL-C++ class modificators.
*/
class CSdlClassModificatorBaseComp:
			public iproc::CSyncProcessorCompBase,
			public IIncludeDirectivesProvider,
			protected imtsdl::CSdlTools,
			protected imtsdl::CSdlEnumConverter
{

	/**
		\todo global + comfort (mb create a base class generator...) \c CSdlClassGqlModificatorComp is most generalized
		* generalize reset
		* generalize read
		* generalize write
		* generalize set to object
		* generalize read from object
	*/

public:
	typedef iproc::CSyncProcessorCompBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CSdlClassModificatorBaseComp)
		I_REGISTER_INTERFACE(IIncludeDirectivesProvider)
		I_ASSIGN(m_processorModificatorNameAttrPtr, "ProcessModificatorName", "The name, allows to do processing, received from arguments in 'modificators' section", false, "<NEED_TO_SET!>")
		I_ASSIGN(m_argumentParserCompPtr, "ArgumentParser", "Command line process argument parser", true, "ArgumentParser")
		I_ASSIGN(m_sdlTypeListCompPtr, "SdlTypeListProvider", "SDL types used to create a code", true, "SdlTypeListProvider")
		I_ASSIGN(m_sdlEnumListCompPtr, "SdlEnumListProvider", "SDL enums used to create a code", true, "SdlEnumListProvider")
		I_ASSIGN(m_originalSchemaNamespaceCompPtr, "OriginalSchemaNamespace", "The namespace of the original(root) schema", true, "OriginalSchemaNamespace");
	I_END_COMPONENT;

	// reimplemented (iproc::IProcessor)
	virtual int DoProcessing(
				const iprm::IParamsSet* paramsPtr,
				const istd::IPolymorphic* inputPtr,
				istd::IChangeable* outputPtr,
				ibase::IProgressManager* progressManagerPtr = NULL) override;

protected: // pure virtual methods
	virtual bool ProcessSourceClassFile(const imtsdl::CSdlType& sdlType) = 0;
	virtual bool ProcessHeaderClassFile(const imtsdl::CSdlType& sdlType) = 0;

protected:
	virtual bool BeginClassFiles(const imtsdl::CSdlType& sdlType);
	virtual bool EndClassFiles(const imtsdl::CSdlType& sdlType);
	virtual bool CloseFiles();
	virtual void AbortCurrentProcessing();

protected:
	I_ATTR(QString, m_processorModificatorNameAttrPtr);
	I_REF(imtsdl::ISdlProcessArgumentsParser, m_argumentParserCompPtr);
	I_REF(imtsdl::ISdlTypeListProvider, m_sdlTypeListCompPtr);
	I_REF(imtsdl::ISdlEnumListProvider, m_sdlEnumListCompPtr);
	I_REF(iprm::ITextParam, m_originalSchemaNamespaceCompPtr);

	istd::TDelPtr<QFile> m_headerFilePtr;
	istd::TDelPtr<QFile> m_sourceFilePtr;

	QString m_logTag;

private:
	istd::TDelPtr<QFile> m_originalHeaderFilePtr;
	istd::TDelPtr<QFile> m_originalSourceFilePtr;
};


} // namespace imtsdlgen

