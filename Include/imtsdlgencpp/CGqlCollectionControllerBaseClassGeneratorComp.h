// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
	#pragma once


//Qt includes
#include <QtCore/QFile>

// ACF includes
#include <istd/TDelPtr.h>
#include <iprm/IOptionsManager.h>
#include <iprm/ITextParam.h>
#include <iproc/TSyncProcessorCompBase.h>

// ImtCore includes
#include <imtsdl/ISdlProcessArgumentsParser.h>
#include <imtsdl/ISdlTypeListProvider.h>
#include <imtsdl/ISdlRequestListProvider.h>
#include <imtsdl/ISdlDocumentTypeListProvider.h>
#include <imtsdl/CSdlTools.h>
#include <imtsdl/CSdlDocumentType.h>
#include <imtsdlgencpp/CCxxProcessorCompBase.h>
#include <imtsdlgencpp/CSdlGenTools.h>


namespace imtsdlgencpp
{

/**
	A C++ class generator of GraphQL Collection controllers for SDL requests
*/
class CGqlCollectionControllerBaseClassGeneratorComp:
			public CCxxProcessorCompBase,
			private imtsdl::CSdlTools,
			private CSdlGenTools
{

	static const QMap<imtsdl::CSdlDocumentType::OperationType, QString> s_nonTrivialOperationMethodsMap;
public:
	typedef CCxxProcessorCompBase BaseClass;

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
	bool ProcessHeaderClassFile(const imtsdl::CSdlDocumentType& sdlDocumentType, QIODevice* headerDevicePtr, const iprm::IParamsSet* paramsPtr) const;
	bool ProcessSourceClassFile(const imtsdl::CSdlDocumentType& sdlDocumentType, QIODevice* sourceDevicePtr, const iprm::IParamsSet* paramsPtr) const;

	// comfort methods
	void AddRequiredIncludesForDocument(QTextStream& stream, const imtsdl::CSdlDocumentType& sdlDocumentType, uint hIndents = 0) const;
	void AddMethodsForDocument(QTextStream& stream, const imtsdl::CSdlDocumentType& sdlDocumentType, uint hIndents = 0) const;
	void AddMethodForDocument(QTextStream& stream, const imtsdl::CSdlRequest& sdlRequest, imtsdl::CSdlDocumentType::OperationType operationType, const QString& itemClassName, const imtsdl::CSdlDocumentType& sdlDocumentType, uint hIndents = 0) const;
	void AddMethodDeclarationForOperationType(QTextStream& stream, imtsdl::CSdlDocumentType::OperationType operationType, const imtsdl::CSdlRequest& sdlRequest) const;
	void AddBaseMethodDeclarationForOperationType(QTextStream& stream, imtsdl::CSdlDocumentType::OperationType operationType, const QString& className) const;
	void AddImplCodeForSpecialRequest(QTextStream& stream, const imtsdl::CSdlRequest& sdlRequest, imtsdl::CSdlDocumentType::OperationType operationType, uint hIndents) const;
	void AddPayloadModelWriteCode(QTextStream& stream, const imtsdl::CSdlRequest& sdlRequest, imtsdl::CSdlDocumentType::OperationType operationType, uint hIndents) const;

	/**
		Creates method implementations for all document's (and subtype's) operation types
	*/
	void AddOperationRequestMethodImplForDocument(QTextStream& stream, const imtsdl::CSdlDocumentType& sdlDocumentType) const;
	void AddOperationRequestCheck(QTextStream& stream, const imtsdl::CSdlDocumentType& sdlDocumentType) const;
	void AddOperationMapPairs(QTextStream& stream, const imtsdl::CSdlDocumentType& sdlDocumentType) const;
	bool AddCollectionMethodsImplForDocument(QTextStream& stream, const imtsdl::CSdlDocumentType& sdlDocumentType) const;
	bool AddImplCodeForRequests(QTextStream& stream, imtsdl::CSdlDocumentType::OperationType operationType, const QList<ImplGenerationInfo>& requestList, const QString& className, const imtsdl::CSdlDocumentType& sdlDocumentType, uint hIndents = 0) const;
	bool AddImplCodeForRequest(QTextStream& stream, const ImplGenerationInfo& sdlRequest, imtsdl::CSdlDocumentType::OperationType operationType, const imtsdl::CSdlDocumentType& sdlDocumentType, uint hIndents = 0) const;
	void AddSpecialMethodImplCodeForDocument(QTextStream& stream, const imtsdl::CSdlDocumentType& sdlDocumentType) const;
	void AddSpecialMethodImplCode(QTextStream& stream, imtsdl::CSdlDocumentType::OperationType operationType, const QList<ImplGenerationInfo>& requestList, const QString& className, uint hIndents = 1) const;
	QString GetInputExtractionStringForTypeName(const imtsdl::CSdlRequest& sdlRequest, const QString& typeName, const QString& version = QString(), bool* okPtr = nullptr) const;

	/// \param _isRoot - internal recursive param. NEVER SET IT!
	bool FindCallChainForField(const imtsdl::CSdlField& sdlRequest, const QString& typeName, QString& callChain, bool _isRoot = true) const;

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
	I_REF(iprm::IOptionsManager, m_dependentSchemaListCompPtr);
};


} // namespace imtsdlgencpp

