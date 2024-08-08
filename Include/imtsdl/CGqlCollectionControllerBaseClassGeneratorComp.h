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
#include <imtsdl/ISdlDocumentTypeListProvider.h>
#include <imtsdl/CSdlTools.h>


namespace imtsdl
{

/**
	A C++ class generator of GraphQL Collection controllers for SDL requests
*/
class CGqlCollectionControllerBaseClassGeneratorComp:
			public iproc::CSyncProcessorCompBase,
			private CSdlTools
{

public:
	typedef iproc::CSyncProcessorCompBase BaseClass;

	I_BEGIN_COMPONENT(CGqlCollectionControllerBaseClassGeneratorComp)
		I_ASSIGN(m_argumentParserCompPtr, "ArgumentParser", "Command line process argument parser", true, "ArgumentParser")
		I_ASSIGN(m_sdlTypeListCompPtr, "SdlTypeListProvider", "SDL types used to create a code", true, "SdlTypeListProvider")
		I_ASSIGN(m_sdlRequestListCompPtr, "SdlRequestListProvider", "SDL requests used to create a GraphQL wrap code", true, "SdlRequestListProvider")
		I_ASSIGN(m_sdlDocumentListCompPtr, "SdlDocumentListProvider", "SDL documents used to create a GraphQL wrap code", true, "SdlDocumentListProvider")
		I_ASSIGN(m_baseClassExtenderCompPtr, "BaseClassExtender", "Compoment, used to add base class inherits", true, "BaseClassExtender")
		I_ASSIGN(m_filesJoinerCompPtr, "FilesJoiner", "Compoment, used to join files into a single", false, "FilesJoiner")

	I_END_COMPONENT;

	//reimplemented(iproc::IProcessor)
	virtual int DoProcessing(
				const iprm::IParamsSet* paramsPtr,
				const istd::IPolymorphic* inputPtr,
				istd::IChangeable* outputPtr,
				ibase::IProgressManager* progressManagerPtr = NULL) override;
private:
	static QString WrapFileName(const QString& baseName, const QString& ext, const QString& directoryPath = QString());

private:
	bool CloseFiles();
	bool ProcessFiles(const CSdlDocumentType& sdlRequest, bool addDependenciesInclude, bool addSelfHeaderInclude);
	bool ProcessHeaderClassFile(const CSdlDocumentType& sdlRequest, bool addDependenciesInclude);
	bool ProcessSourceClassFile(const CSdlDocumentType& sdlRequest, bool addSelfHeaderInclude);
	void AbortCurrentProcessing();


private:
	I_REF(ISdlProcessArgumentsParser, m_argumentParserCompPtr);
	I_REF(ISdlTypeListProvider, m_sdlTypeListCompPtr);
	I_REF(ISdlRequestListProvider, m_sdlRequestListCompPtr);
	I_REF(ISdlDocumentTypeListProvider, m_sdlDocumentListCompPtr);
	I_REF(iproc::IProcessor, m_baseClassExtenderCompPtr);
	I_REF(iproc::IProcessor, m_filesJoinerCompPtr);

	istd::TDelPtr<QFile> m_headerFilePtr;
	istd::TDelPtr<QFile> m_sourceFilePtr;
};


} // namespace imtsdl

