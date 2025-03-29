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
#include <imtsdl/ISdlRequestListProvider.h>
#include <imtsdl/ISdlDocumentTypeListProvider.h>
#include <imtsdl/CSdlTools.h>


namespace imtsdlgen
{

/**
	A C++ class generator of GraphQL Collection controllers for SDL requests
*/
class CGqlCollectionControllerBaseClassGeneratorComp:
			public iproc::CSyncProcessorCompBase,
			private imtsdl::CSdlTools
{
	static const QMap<imtsdl::CSdlDocumentType::OperationType, QString> s_nonTrivialOperationMethodsMap;
public:
	typedef iproc::CSyncProcessorCompBase BaseClass;

	I_BEGIN_COMPONENT(CGqlCollectionControllerBaseClassGeneratorComp)
		I_ASSIGN(m_argumentParserCompPtr, "ArgumentParser", "Command line process argument parser", true, "ArgumentParser")
		I_ASSIGN(m_sdlTypeListCompPtr, "SdlTypeListProvider", "SDL types used to create a code", true, "SdlTypeListProvider")
		I_ASSIGN(m_sdlEnumListCompPtr, "SdlEnumListProvider", "SDL enums used to create a code", true, "SdlEnumListProvider")
		I_ASSIGN(m_sdlUnionListCompPtr, "SdlUnionListProvider", "SDL unioins used to create a code", true, "SdlUnionListProvider")
		I_ASSIGN(m_sdlRequestListCompPtr, "SdlRequestListProvider", "SDL requests used to create a GraphQL wrap code", true, "SdlRequestListProvider")
		I_ASSIGN(m_sdlDocumentListCompPtr, "SdlDocumentListProvider", "SDL documents used to create a GraphQL wrap code", true, "SdlDocumentListProvider")
		I_ASSIGN(m_baseClassExtenderCompPtr, "BaseClassExtender", "Compoment, used to add base class inherits", true, "BaseClassExtender")
		I_ASSIGN(m_filesJoinerCompPtr, "FilesJoiner", "Compoment, used to join files into a single", false, "FilesJoiner")
		I_ASSIGN(m_customSchemaParamsCompPtr, "CustomSchemaParams", "Custom schema parameters, that contains additional options", false, "CustomSchemaParams")
		I_ASSIGN(m_originalSchemaNamespaceCompPtr, "OriginalSchemaNamespace", "The namespace of the original(root) schema", true, "OriginalSchemaNamespace");

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
	struct ImplGenerationInfo
	{
		ImplGenerationInfo(const imtsdl::CSdlRequest& r, const QString& n)
			:request(r), containerClassName(n)
		{
		}
		imtsdl::CSdlRequest request;
		QString containerClassName;
	};

private:
	bool CloseFiles();
	bool ProcessFiles(const imtsdl::CSdlDocumentType& sdlDocumentType, bool addDependenciesInclude, bool addSelfHeaderInclude);
	bool ProcessHeaderClassFile(const imtsdl::CSdlDocumentType& sdlDocumentType, bool addDependenciesInclude);
	bool ProcessSourceClassFile(const imtsdl::CSdlDocumentType& sdlDocumentType, bool addSelfHeaderInclude);
	void AbortCurrentProcessing();

	// comfort methods
	void AddMethodDeclarationForOperationType(QTextStream& stream, imtsdl::CSdlDocumentType::OperationType operationType, const imtsdl::CSdlRequest& sdlRequest);
	void AddBaseMethodDeclarationForOperationType(QTextStream& stream, imtsdl::CSdlDocumentType::OperationType operationType);
	void AddRequiredIncludesForDocument(QTextStream& stream, const imtsdl::CSdlDocumentType& sdlDocumentType, uint hIndents = 0);
	void AddMethodsForDocument(QTextStream& stream, const imtsdl::CSdlDocumentType& sdlDocumentType, uint hIndents = 0);
	void AddMethodForDocument(QTextStream& stream, const imtsdl::CSdlRequest& sdlRequest, imtsdl::CSdlDocumentType::OperationType operationType, const QString& itemClassName, uint hIndents = 0);
	void AddImplCodeForSpecialRequest(QTextStream& stream, const imtsdl::CSdlRequest& sdlRequest, imtsdl::CSdlDocumentType::OperationType operationType, uint hIndents);

	/**
		Creates method implementations for all document's (and subtype's) operation types
	 */
	void AddOperationRequestMethodImplForDocument(QTextStream& stream, const imtsdl::CSdlDocumentType& sdlDocumentType);
	void AddOperationRequestCheck(QTextStream& stream, const imtsdl::CSdlDocumentType& sdlDocumentType);
	void AddCollectionMethodsImplForDocument(QTextStream& stream, const imtsdl::CSdlDocumentType& sdlDocumentType);
	void AddImplCodeForRequests(QTextStream& stream, imtsdl::CSdlDocumentType::OperationType operationType, const QList<ImplGenerationInfo>& requestList, const QString& className, uint hIndents = 0);
	void AddSpecialMethodImplCodeForDocument(QTextStream& stream, const imtsdl::CSdlDocumentType& sdlDocumentType);
	void AddSpecialMethodImplCode(QTextStream& stream, imtsdl::CSdlDocumentType::OperationType operationType, const QList<ImplGenerationInfo>& requestList, const QString& className, uint hIndents = 1);
	void AddImplCodeForRequest(QTextStream& stream, const ImplGenerationInfo& sdlRequest, imtsdl::CSdlDocumentType::OperationType operationType, uint hIndents = 0);
	QString GetInputExtractionStringForTypeName(const imtsdl::CSdlRequest& sdlRequest, const QString typeName) const;
	bool FindCallChainForField(const imtsdl::CSdlField& sdlRequest, const QString typeName, QString& callChain) const;

private:
	I_REF(imtsdl::ISdlProcessArgumentsParser, m_argumentParserCompPtr);
	I_REF(imtsdl::ISdlTypeListProvider, m_sdlTypeListCompPtr);
	I_REF(imtsdl::ISdlEnumListProvider, m_sdlEnumListCompPtr);
	I_REF(imtsdl::ISdlUnionListProvider, m_sdlUnionListCompPtr);
	I_REF(imtsdl::ISdlRequestListProvider, m_sdlRequestListCompPtr);
	I_REF(imtsdl::ISdlDocumentTypeListProvider, m_sdlDocumentListCompPtr);
	I_REF(iproc::IProcessor, m_baseClassExtenderCompPtr);
	I_REF(iproc::IProcessor, m_filesJoinerCompPtr);
	I_REF(iprm::IParamsSet, m_customSchemaParamsCompPtr);
	I_REF(iprm::ITextParam, m_originalSchemaNamespaceCompPtr);

	istd::TDelPtr<QFile> m_headerFilePtr;
	istd::TDelPtr<QFile> m_sourceFilePtr;
};


} // namespace imtsdlgen

