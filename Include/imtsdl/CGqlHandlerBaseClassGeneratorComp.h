#pragma once


// Qt includes
#include <QtCore/QFile>

// ACF includes
#include <istd/TDelPtr.h>
#include <iproc/TSyncProcessorCompBase.h>

// ImtCore includes
#include <imtsdl/ISdlProcessArgumentsParser.h>
#include <imtsdl/ISdlTypeListProvider.h>
#include <imtsdl/ISdlRequestListProvider.h>
#include <imtsdl/CSdlTools.h>


namespace imtsdl
{


/**
	A C++ class generator of GraphQL handlers for SDL requests
*/
class CGqlHandlerBaseClassGeneratorComp:
			public iproc::CSyncProcessorCompBase,
			private CSdlTools
{

public:
	typedef iproc::CSyncProcessorCompBase BaseClass;

	I_BEGIN_COMPONENT(CGqlHandlerBaseClassGeneratorComp)
		I_ASSIGN(m_argumentParserCompPtr, "ArgumentParser", "Command line process argument parser", true, "ArgumentParser")
		I_ASSIGN(m_sdlTypeListCompPtr, "SdlTypeListProvider", "SDL types used to create a code", true, "SdlTypeListProvider")
		I_ASSIGN(m_sdlRequestListCompPtr, "SdlRequestListProvider", "SDL requests used to create a GraphQL wrap code", true, "SdlRequestListProvider")
		I_ASSIGN(m_baseClassExtenderCompPtr, "BaseClassExtender", "Compoment, used to add base class inherits", true, "BaseClassExtender")
		I_ASSIGN(m_filesJoinerCompPtr, "FilesJoiner", "Compoment, used to join files into a single", false, "FilesJoiner")
		I_ASSIGN(m_customSchemaParamsCompPtr, "CustomSchemaParams", "Custom schema parameters, that contains additional options", false, "CustomSchemaParams")

	I_END_COMPONENT;

	//reimplemented(iproc::IProcessor)
	virtual int DoProcessing(
				const iprm::IParamsSet* paramsPtr,
				const istd::IPolymorphic* inputPtr,
				istd::IChangeable* outputPtr,
				ibase::IProgressManager* progressManagerPtr = NULL) override;
private:
	static QString WrapFileName(const QString& ext, const QString& directoryPath = QString());

private:
	bool CloseFiles();
	bool ProcessFiles(bool addDependenciesInclude, bool addSelfHeaderInclude);
	bool ProcessHeaderClassFile(bool addDependenciesInclude);
	bool ProcessSourceClassFile(bool addSelfHeaderInclude);
	void AbortCurrentProcessing();

	// comfort methods
	void AddRequiredIncludesForDocument(QTextStream& stream, uint hIndents = 0);
	void AddMethodsForDocument(QTextStream& stream, uint hIndents = 0);
	void AddMethodForDocument(QTextStream& stream, const CSdlRequest& sdlRequest, uint hIndents = 0);

	/**
		Creates method implementations for all document's (and subtype's) operation types
	 */
	void AddCollectionMethodsImplForDocument(QTextStream& stream, uint hIndents = 0);
	void AddImplCodeForRequests(QTextStream& stream, const SdlRequestList& requestList, const QString& className, uint hIndents = 0);
	void AddImplCodeForRequest(QTextStream& stream, const CSdlRequest& sdlRequest, uint hIndents = 0);

private:
	I_REF(ISdlProcessArgumentsParser, m_argumentParserCompPtr);
	I_REF(ISdlTypeListProvider, m_sdlTypeListCompPtr);
	I_REF(ISdlRequestListProvider, m_sdlRequestListCompPtr);
	I_REF(iproc::IProcessor, m_baseClassExtenderCompPtr);
	I_REF(iproc::IProcessor, m_filesJoinerCompPtr);
	I_REF(iprm::IParamsSet, m_customSchemaParamsCompPtr);

	istd::TDelPtr<QFile> m_headerFilePtr;
	istd::TDelPtr<QFile> m_sourceFilePtr;
};


} // namespace imtsdl

