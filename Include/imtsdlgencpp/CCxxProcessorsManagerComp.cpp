// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtsdlgencpp/CCxxProcessorsManagerComp.h>


// C includes
#include <iostream>

// Qt includes
#include <QtCore/QTemporaryDir>
#include <QtCore/QLockFile>

// ACF includes
#include <istd/CSystem.h>
#include <iprm/TParamsPtr.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include<imtsdl/CSdlTools.h>
#include<imtsdl/CSdlType.h>
#include<imtsdl/CSdlEnum.h>
#include<imtsdl/CSdlDocumentType.h>
#include<imtsdl/CSdlRequest.h>


namespace imtsdlgencpp
{

// static helpers
CCxxProcessorsManagerComp::FilePtr GetFilePtrForEntry(const imtsdl::CSdlEntryBase& entry, const CCxxProcessorsManagerComp::EntryFileMap& map)
{
	if (map.isEmpty()){
		return CCxxProcessorsManagerComp::FilePtr();
	}

	// if split enabled, the map will contain more than 1 elements
	if (map.size() > 1){
		Q_ASSERT(map.contains(entry.GetName()));

		return map[entry.GetName()];
	}

	return map.first();
}


template <class T>
void WriteDeclareMetaType(const QList<T>& entryList, QTextStream& stream)
{
	for (const T& sdlType: entryList){
		const QString typeNamespace = CSdlGenTools::GetNamespaceFromSchemaParams(sdlType.GetSchemaParams());
		imtsdl::CSdlTools::FeedStream(stream, 1, false);
		stream << QStringLiteral("Q_DECLARE_METATYPE(");
		if (typeNamespace.length() > 0){
			stream << typeNamespace;
			stream << ':' << ':';
		}
		stream << 'C' << sdlType.GetName();
		stream << ':' << ':';
		stream << CSdlGenTools::GetSdlEntryVersion(sdlType);
		stream << QStringLiteral(");");

		imtsdl::CSdlTools::FeedStream(stream, 1, false);
		stream << QStringLiteral("Q_DECLARE_METATYPE(");
		if (typeNamespace.length() > 0){
			stream << typeNamespace;
			stream << ':' << ':';
		}
		stream << 'C' << sdlType.GetName();
		stream << QStringLiteral(");");
	}
}


template <class T>
bool CreateDirectivesFromEntryList(const QList<T>& entryList, QList<imtsdl::IncludeDirective>& output)
{
	for (const T& entry: entryList){
		if (entry.IsExternal()){
			const QString typeIncludePath = '<' + entry.GetTargetHeaderFilePath() + '>';
			if (typeIncludePath.isEmpty()){
				qCritical() << qPrintable(QString("Header path is empty %1 of %2").arg(entry.GetName(), entry.GetSchemaFilePath()));
				I_CRITICAL();

				return false;
			}

			imtsdl::IncludeDirective directive = imtsdl::CSdlTools::CreateCustomDirective(typeIncludePath);
			if (!output.contains(directive)){
				output << directive;
			}
		}
	}

	return true;
}


void CreateBaseDirectives(QList<imtsdl::IncludeDirective>& output)
{
	static QList<imtsdl::IncludeDirective> baseDirectives = {
		imtsdl::CSdlTools::CreateQtDirective("<QtCore/QObject>"),
		imtsdl::CSdlTools::CreateQtDirective("<QtCore/QVariant>"),
		imtsdl::CSdlTools::CreateAcfDirective("<istd/TSharedNullable.h>"),
	};

	output += baseDirectives;
}


// public methods

// reimplemented (iproc::IProcessor)

/**
	\param paramsPtr - params, created from argument parser and schema data
	\param inputPtr - SDL schema file path, prepresented as \c ifile::IFileNameParam
	\return
 */
iproc::IProcessor::TaskState CCxxProcessorsManagerComp::DoProcessing(
	const iprm::IParamsSet* paramsPtr,
	const istd::IPolymorphic* /*inputPtr*/,
	istd::IChangeable* /*outputPtr*/,
	ibase::IProgressManager* /*progressManagerPtr*/)
{
	Q_ASSERT(m_argumentParserCompPtr.IsValid());
	Q_ASSERT(m_schemaParamsCompPtr.IsValid());
	Q_ASSERT(m_dependentSchemaListCompPtr.IsValid());

	if (m_argumentParserCompPtr->IsDependenciesMode() || !m_argumentParserCompPtr->GetDepFilePath().isEmpty()){
		QStringList cumulatedFiles;
		const QString sourceFilePath = CalculateTargetCppFilesFromSchemaParams(*m_schemaParamsCompPtr, *m_argumentParserCompPtr)[imtsdl::ISdlProcessArgumentsParser::s_sourceFileType];
		const QString headerFilePath = CalculateTargetCppFilesFromSchemaParams(*m_schemaParamsCompPtr, *m_argumentParserCompPtr)[imtsdl::ISdlProcessArgumentsParser::s_headerFileType];
		cumulatedFiles << sourceFilePath;
		cumulatedFiles << headerFilePath;

		PrintFiles(std::cout, cumulatedFiles, m_argumentParserCompPtr->GetGeneratorType());
		PrintFiles(m_argumentParserCompPtr->GetDepFilePath(), cumulatedFiles, *m_dependentSchemaListCompPtr);

		if (m_argumentParserCompPtr->IsDependenciesMode()){
			return TS_OK;
		}
	}

	const EntryFileMap headerFiles = CreateHeaderFiles(paramsPtr);
	const EntryFileMap sourceFiles = CreateSourceFiles(paramsPtr);

	if (headerFiles.isEmpty() && sourceFiles.isEmpty()){
		SendErrorMessage(0, "Can't create header files");

		return TS_INVALID;
	}

	// begin files (if it only one)
	if (headerFiles.size() == 1){
		const FilePtr& filePtr = headerFiles.first();
		if (!filePtr){
			I_CRITICAL();

			return TS_INVALID;
		}

		if (!BeginHeaderFile(*m_schemaParamsCompPtr, *filePtr, paramsPtr)){
			SendErrorMessage(0, QString("Unable to begin process file '%1'").arg(filePtr->fileName()));

			return TS_INVALID;
		}
	}
	if (sourceFiles.size() == 1){
		const FilePtr& filePtr = sourceFiles.first();
		if (!filePtr){
			I_CRITICAL();

			return TS_INVALID;
		}

		if (!BeginSourceFile(*m_schemaParamsCompPtr, *filePtr, paramsPtr)){
			SendErrorMessage(0, QString("Unable to begin process file '%1'").arg(filePtr->fileName()));

			return TS_INVALID;
		}
	}

	// process
	if (!ProcessEnums(headerFiles, sourceFiles, paramsPtr)){
		SendErrorMessage(0, "Unable to process enums");

		return TS_INVALID;
	}

	if (!ProcessTypes(headerFiles, sourceFiles, paramsPtr)){
		SendErrorMessage(0, "Unable to process types");

		return TS_INVALID;
	}

	if (!ProcessUnions(headerFiles, sourceFiles, paramsPtr)){
		SendErrorMessage(0, "Unable to process unions");

		return TS_INVALID;
	}

	if (!ProcessRequests(headerFiles, sourceFiles, paramsPtr)){
		SendErrorMessage(0, "Unable to process requests");

		return TS_INVALID;
	}

	if (!ProcessDocumentTypes(headerFiles, sourceFiles, paramsPtr)){
		SendErrorMessage(0, "Unable to process document types");

		return TS_INVALID;
	}

	if (m_autoProcessorCompListPtr.IsValid()){
		const int processorsCount = m_autoProcessorCompListPtr.GetCount();
		imtsdl::CSdlType dummyType;
		FilePtr headerFilePtr = GetFilePtrForEntry(dummyType, headerFiles);
		FilePtr sourceFilePtr = GetFilePtrForEntry(dummyType, sourceFiles);
		Q_ASSERT(headerFilePtr || sourceFilePtr);
		for (int i = 0; i < processorsCount; ++i){
			ICxxFileProcessor* processorPtr = m_autoProcessorCompListPtr[i];
			Q_ASSERT(processorPtr != nullptr);
			const bool ok = processorPtr->ProcessEntry(dummyType, headerFilePtr.get(), sourceFilePtr.get(), paramsPtr);
			if (!ok){
				SendErrorMessage(0, "Processing failed");

				return TS_INVALID;
			}
		}
	}

	// end files (if it only one)
	if (headerFiles.size() == 1){
		const FilePtr& filePtr = headerFiles.first();
		if (!filePtr){
			I_CRITICAL();

			return TS_INVALID;
		}

		if (!EndHeaderFile(*m_schemaParamsCompPtr, *filePtr, paramsPtr)){
			SendErrorMessage(0, QString("Unable to end process file '%1'").arg(filePtr->fileName()));

			return TS_INVALID;
		}
	}
	if (sourceFiles.size() == 1){
		const FilePtr& filePtr = sourceFiles.first();
		if (!filePtr){
			I_CRITICAL();

			return TS_INVALID;
		}

		if (!EndSourceFile(*m_schemaParamsCompPtr, *filePtr, paramsPtr)){
			SendErrorMessage(0, QString("Unable to end process file '%1'").arg(filePtr->fileName()));

			return TS_INVALID;
		}
	}

	return TS_OK;
}


// private methods

bool CCxxProcessorsManagerComp::BeginHeaderFile(
			const iprm::IParamsSet& entryParams,
			QFile& headerFile,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	// write pragma once first
	QTextStream stream(&headerFile);
	stream << QStringLiteral("#pragma once");
	FeedStream(stream, 3, false);

	// special optional qt include
	stream << QStringLiteral("#ifdef QT_QML_LIB");
	FeedStream(stream, 1, false);
	stream << QStringLiteral("#include <QtQml/QQmlEngine>");
	FeedStream(stream, 1, false);
	stream << QStringLiteral("#endif // QT_QML_LIB");
	FeedStream(stream, 2, false);

	// write includes
	QList<imtsdl::Priority> orderList = {
		imtsdl::P_C,
		imtsdl::P_OS_API,
		imtsdl::P_QT,
		imtsdl::P_ACF,
		imtsdl::P_IMT,
		imtsdl::P_CUSTOM
	};

	QList<imtsdl::IncludeDirective> includeDirectivesList;
	CreateBaseDirectives(includeDirectivesList);

	// include all required files
	const int includeProviderListCount = m_includeDirectivesProviderListCompPtr.GetCount();
	for (int i = 0; i < includeProviderListCount; ++i){
		IIncludeDirectivesProvider* includesProviderPtr = m_includeDirectivesProviderListCompPtr[i];
		Q_ASSERT(includesProviderPtr != nullptr);
		includeDirectivesList += includesProviderPtr->GetIncludeDirectives();
	}

	// include all required generated headers
	if (m_sdlEnumListCompPtr.IsValid()){
		imtsdl::SdlEnumList list = m_sdlEnumListCompPtr->GetEnums(false);
		CreateDirectivesFromEntryList(list, includeDirectivesList);
	}
	if (m_sdlUnionListCompPtr.IsValid()){
		imtsdl::SdlUnionList list = m_sdlUnionListCompPtr->GetUnions(false);
		CreateDirectivesFromEntryList(list, includeDirectivesList);
	}
	if (m_sdlTypeListCompPtr.IsValid()){
		imtsdl::SdlTypeList list = m_sdlTypeListCompPtr->GetSdlTypes(false);
		CreateDirectivesFromEntryList(list, includeDirectivesList);
	}
	if (m_sdlDocumentTypeListCompPtr.IsValid()){
		imtsdl::SdlDocumentTypeList list = m_sdlDocumentTypeListCompPtr->GetDocumentTypes(false);
		CreateDirectivesFromEntryList(list, includeDirectivesList);
	}
	if (m_requestsProviderListCompPtr.IsValid()){
		imtsdl::SdlRequestList list = m_requestsProviderListCompPtr->GetRequests(false);
		CreateDirectivesFromEntryList(list, includeDirectivesList);
	}

	QMutableListIterator includeIter(includeDirectivesList);
	while(!orderList.isEmpty()){
		imtsdl::Priority currentPriority = orderList.takeFirst();
		bool addRemark = true;
		bool isAdded = false;
		while(includeIter.hasNext()){
			imtsdl::IncludeDirective directive = includeIter.next();
			if (directive.priority == currentPriority){
				isAdded = true;
				if (addRemark){
					if (!directive.remark.startsWith(QStringLiteral("//"))){
						stream << QStringLiteral("// ");
					}
					stream << directive.remark;
					FeedStream(stream, 1, false);
					addRemark = false;
				}
				stream << QStringLiteral("#include ");
				stream << directive.path;
				FeedStream(stream, 1, false);

				includeIter.remove();
			}
		}
		if (isAdded){
			FeedStream(stream, 1, false);
		}
		includeIter.toFront();
	}

	FeedStream(stream, 1, false);

	// begin namespace
	const QString sdlNamespace = GetNamespaceFromSchemaParams(entryParams);
	stream << QStringLiteral("namespace ");
	stream << sdlNamespace;
	FeedStream(stream, 1, false);

	stream << '{';
	FeedStream(stream, 3, false);


	// add Q_NAMESPACE macro
	stream << QStringLiteral("Q_NAMESPACE");
	FeedStream(stream, 2, false);

	return true;
}


bool CCxxProcessorsManagerComp::BeginSourceFile(
			const iprm::IParamsSet& entryParams,
			QFile& sourceFile,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	QTextStream stream(&sourceFile);

	// include header file
	stream << QStringLiteral("#include \"");
	stream << QFileInfo(sourceFile).baseName();
	stream << QStringLiteral(".h\"");
	FeedStream(stream, 3, false);

	// begin namespace
	const QString sdlNamespace = GetNamespaceFromSchemaParams(entryParams);
	stream << QStringLiteral("namespace ");
	stream << sdlNamespace;
	FeedStream(stream, 1, false);

	stream << '{';
	FeedStream(stream, 3, false);

	return true;
}


bool CCxxProcessorsManagerComp::EndHeaderFile(
			const iprm::IParamsSet& entryParams,
			QFile& headerFile,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	QTextStream stream(&headerFile);

	// end namespace
	const QString sdlNamespace = GetNamespaceFromSchemaParams(entryParams);
	stream << '}';
	stream << QStringLiteral(" // namespace ");
	stream << sdlNamespace;
	FeedStream(stream, 2, false);

	// add qt registration for QVariant
	if (m_sdlTypeListCompPtr.IsValid()){
		imtsdl::SdlTypeList list = m_sdlTypeListCompPtr->GetSdlTypes(true);
		WriteDeclareMetaType(list, stream);
	}
	FeedStream(stream, 1, false);

	return true;
}


bool CCxxProcessorsManagerComp::EndSourceFile(
			const iprm::IParamsSet& entryParams,
			QFile& sourceFile,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	QTextStream stream(&sourceFile);

	// end namespace
	const QString sdlNamespace = GetNamespaceFromSchemaParams(entryParams);
	stream << '}';
	stream << QStringLiteral(" // namespace ");
	stream << sdlNamespace;
	FeedStream(stream, 1, false);

	return true;
}


bool CCxxProcessorsManagerComp::ProcessEnums(
			const EntryFileMap& headerFiles,
			const EntryFileMap& sourceFiles,
			const iprm::IParamsSet* paramsPtr) const
{
	if (!m_enumProcessorCompListPtr.IsValid() || !m_sdlEnumListCompPtr.IsValid() || headerFiles.isEmpty()){
		// nothing todo
		return true;
	}

	if(headerFiles.size() > 1 || sourceFiles.size() > 1){
		Q_ASSERT_X(false, __func__, "Mufli file processing not implemented");

		return false;
	}

	const imtsdl::SdlEnumList enumList = m_sdlEnumListCompPtr->GetEnums(true);
	const int enumProcessorsCount = m_enumProcessorCompListPtr.GetCount();
	for (int i = 0; i < enumProcessorsCount; ++i){
		ICxxFileProcessor* enumProcessorPtr = m_enumProcessorCompListPtr[i];
		Q_ASSERT(enumProcessorPtr != nullptr);
		for (const imtsdl::CSdlEnum& sdlEnum: enumList){
			FilePtr headerFilePtr = GetFilePtrForEntry(sdlEnum, headerFiles);
			Q_ASSERT(headerFilePtr);
			const bool ok = enumProcessorPtr->ProcessEntry(sdlEnum, headerFilePtr.get(), nullptr, paramsPtr);
			if (!ok){
				SendErrorMessage(0, QString("Unable to process enum '%1'").arg(sdlEnum.GetName()));

				return false;
			}
		}
	}

	return true;
}


bool CCxxProcessorsManagerComp::ProcessTypes(
	const EntryFileMap& headerFiles,
	const EntryFileMap& sourceFiles,
	const iprm::IParamsSet* paramsPtr) const
{
	if (!m_typeProcessorCompListPtr.IsValid() || !m_sdlTypeListCompPtr.IsValid()){
		// nothing todo
		return true;
	}

	if(headerFiles.size() > 1 || sourceFiles.size() > 1){
		Q_ASSERT_X(false, __func__, "Mufli file processing not implemented");

		return false;
	}

	// before process types it is required to forward declare of all unions
	const imtsdl::SdlUnionList unionList = m_sdlUnionListCompPtr->GetUnions(true);

	for (const FilePtr& headerFilePtr: headerFiles){
		Q_ASSERT(headerFilePtr);

		QTextStream stream(headerFilePtr.get());
		if (!unionList.isEmpty()){
			stream << QStringLiteral("// forward declaration for union types");
			FeedStream(stream, 1, false);
		}

		for (const imtsdl::CSdlUnion& sdlUinion: unionList){
			stream << QStringLiteral("class ") << sdlUinion.GetName() << ';';
			FeedStream(stream, 1, false);
			stream << QStringLiteral("class C")  << sdlUinion.GetName() << QStringLiteral("Object;");
			FeedStream(stream, 1, false);
			stream << QStringLiteral("class C")  << sdlUinion.GetName() << QStringLiteral("ObjectList;");
			FeedStream(stream, 1, false);
		}
		FeedStream(stream, 1, false);
	}

	const imtsdl::SdlTypeList typeList = m_sdlTypeListCompPtr->GetSdlTypes(true);
	const int typeProcessorsCount = m_typeProcessorCompListPtr.GetCount();
	for (int i = 0; i < typeProcessorsCount; ++i){
		ICxxFileProcessor* typeProcessorPtr = m_typeProcessorCompListPtr[i];
		Q_ASSERT(typeProcessorPtr != nullptr);
		for (const imtsdl::CSdlType& sdlType: typeList){
			FilePtr headerFilePtr = GetFilePtrForEntry(sdlType, headerFiles);
			FilePtr sourceFilePtr = GetFilePtrForEntry(sdlType, sourceFiles);
			const bool ok = typeProcessorPtr->ProcessEntry(sdlType, headerFilePtr.get(), sourceFilePtr.get(), paramsPtr);
			if (!ok){
				SendErrorMessage(0, QString("Unable to process type '%1'").arg(sdlType.GetName()));

				return false;
			}
		}
	}

	return true;
}


bool CCxxProcessorsManagerComp::ProcessUnions(
			const EntryFileMap& headerFiles,
			const EntryFileMap& sourceFiles,
			const iprm::IParamsSet* paramsPtr) const
{
	if (!m_unionProcessorCompListPtr.IsValid() || !m_sdlUnionListCompPtr.IsValid() || (headerFiles.isEmpty() && sourceFiles.isEmpty())){
		// nothing todo
		return true;
	}

	if(headerFiles.size() > 1 || sourceFiles.size() > 1){
		Q_ASSERT_X(false, __func__, "Mufli file processing not implemented");

		return false;
	}

	const imtsdl::SdlUnionList unionList = m_sdlUnionListCompPtr->GetUnions(true);
	const int unionProcessorsCount = m_unionProcessorCompListPtr.GetCount();
	for (int i = 0; i < unionProcessorsCount; ++i){
		ICxxFileProcessor* unionProcessorPtr = m_unionProcessorCompListPtr[i];
		Q_ASSERT(unionProcessorPtr != nullptr);
		for (const imtsdl::CSdlUnion& sdlUinion: unionList){
			FilePtr headerFilePtr = GetFilePtrForEntry(sdlUinion, headerFiles);
			FilePtr sourceFilePtr = GetFilePtrForEntry(sdlUinion, sourceFiles);
			Q_ASSERT(headerFilePtr || sourceFilePtr);
			const bool ok = unionProcessorPtr->ProcessEntry(sdlUinion, headerFilePtr.get(), sourceFilePtr.get(), paramsPtr);
			if (!ok){
				SendErrorMessage(0, QString("Unable to process union '%1'").arg(sdlUinion.GetName()));

				return false;
			}
		}
	}

	return true;
}


bool CCxxProcessorsManagerComp::ProcessDocumentTypes(const EntryFileMap& headerFiles, const EntryFileMap& sourceFiles, const iprm::IParamsSet* paramsPtr) const
{
	if (	!m_documentTypeProcessorCompListPtr.IsValid() ||
			!m_sdlDocumentTypeListCompPtr.IsValid() ||
			(headerFiles.isEmpty() && sourceFiles.isEmpty()))
	{
		// nothing todo
		return true;
	}

	if(headerFiles.size() > 1 || sourceFiles.size() > 1){
		Q_ASSERT_X(false, __func__, "Mufli file processing not implemented");

		return false;
	}

	const imtsdl::SdlDocumentTypeList documentTypesList = m_sdlDocumentTypeListCompPtr->GetDocumentTypes(true);
	const int documentTypeProcessorsCount = m_documentTypeProcessorCompListPtr.GetCount();
	for (int i = 0; i < documentTypeProcessorsCount; ++i){
		ICxxFileProcessor* processorPtr = m_documentTypeProcessorCompListPtr[i];
		Q_ASSERT(processorPtr != nullptr);
		for (const imtsdl::CSdlDocumentType& documentType: documentTypesList){
			FilePtr headerFilePtr = GetFilePtrForEntry(documentType, headerFiles);
			FilePtr sourceFilePtr = GetFilePtrForEntry(documentType, sourceFiles);
			Q_ASSERT(headerFilePtr || sourceFilePtr);
			const bool ok = processorPtr->ProcessEntry(documentType, headerFilePtr.get(), sourceFilePtr.get(), paramsPtr);
			if (!ok){
				SendErrorMessage(0, QString("Unable to process document type '%1'").arg(documentType.GetName()));
			}
		}
	}

	return true;
}


bool CCxxProcessorsManagerComp::ProcessRequests(const EntryFileMap& headerFiles, const EntryFileMap& sourceFiles, const iprm::IParamsSet* paramsPtr) const
{
	if (	!m_requestProcessorCompListPtr.IsValid() ||
			!m_requestsProviderListCompPtr.IsValid() ||
			(headerFiles.isEmpty() && sourceFiles.isEmpty()))
	{
		// nothing todo
		return true;
	}

	if(headerFiles.size() > 1 || sourceFiles.size() > 1){
		Q_ASSERT_X(false, __func__, "Mufli file processing not implemented");

		return false;
	}

	const imtsdl::SdlRequestList requestsList = m_requestsProviderListCompPtr->GetRequests(true);
	const int requestsProcessorsCount = m_requestProcessorCompListPtr.GetCount();
	for (int i = 0; i < requestsProcessorsCount; ++i){
		ICxxFileProcessor* requestProcessorPtr = m_requestProcessorCompListPtr[i];
		Q_ASSERT(requestProcessorPtr != nullptr);
		for (const imtsdl::CSdlRequest& request: requestsList){
			FilePtr headerFilePtr = GetFilePtrForEntry(request, headerFiles);
			FilePtr sourceFilePtr = GetFilePtrForEntry(request, sourceFiles);

			const bool ok = requestProcessorPtr->ProcessEntry(request, headerFilePtr.get(), sourceFilePtr.get(), paramsPtr);
			if (!ok){
				SendErrorMessage(0, QString("Unable to process request '%1'").arg(request.GetName()));

				return false;
			}
		}
	}

	return true;
}


CCxxProcessorsManagerComp::EntryFileMap CCxxProcessorsManagerComp::CreateHeaderFiles(const iprm::IParamsSet* /*paramsPtr*/) const
{
	EntryFileMap retVal;

	imtsdl::ISdlProcessArgumentsParser::CppGenerationMode mode = m_argumentParserCompPtr->GetCppGenerationMode();
	if (mode == imtsdl::ISdlProcessArgumentsParser::CGM_IMPLEMENTATION_ONLY){
		return retVal;
	}

	// create multi files if enabled
	/// \todo (implement it later))

	// create single file
	const QString outputFilePath = CalculateTargetCppFilesFromSchemaParams(*m_schemaParamsCompPtr, *m_argumentParserCompPtr)[imtsdl::ISdlProcessArgumentsParser::s_headerFileType];
	FilePtr singleFilePtr = CreateFile(outputFilePath);
	if (!singleFilePtr){
		return retVal;
	}
	retVal[QString()] = singleFilePtr;

	return retVal;
}


CCxxProcessorsManagerComp::EntryFileMap CCxxProcessorsManagerComp::CreateSourceFiles(const iprm::IParamsSet* /*paramsPtr*/) const
{
	EntryFileMap retVal;
	imtsdl::ISdlProcessArgumentsParser::CppGenerationMode mode = m_argumentParserCompPtr->GetCppGenerationMode();
	if (mode == imtsdl::ISdlProcessArgumentsParser::CGM_HEADER_ONLY){
		return retVal;
	}

	// create multi files if enabled
	/// \todo (implement it later))

	// create single file
	const QString outputFilePath = CalculateTargetCppFilesFromSchemaParams(*m_schemaParamsCompPtr, *m_argumentParserCompPtr)[imtsdl::ISdlProcessArgumentsParser::s_sourceFileType];
	FilePtr singleFilePtr = CreateFile(outputFilePath);
	if (!singleFilePtr){

		return retVal;
	}
	retVal[QString()] = singleFilePtr;

	return retVal;
}


CCxxProcessorsManagerComp::FilePtr CCxxProcessorsManagerComp::CreateFile(const QString& filePath) const
{
	FilePtr nullPtr;

	const bool isPathExists = istd::CSystem::EnsurePathExists(QDir(filePath + "/..").absolutePath());
	if (!isPathExists){
		SendErrorMessage(0, QString("Unable to create output dir: '%1'").arg(filePath));

		return nullPtr;
	}

	FilePtr outputFilePtr(new QFile(filePath));
	const bool isOpen = outputFilePtr->open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate);
	if (!isOpen){
		SendErrorMessage(0, QString("Unable to open file: '%1'. Error: %2").arg(filePath, outputFilePtr->errorString()));

		return nullPtr;
	}

	return outputFilePtr;
}




} // namespace imtsdlgencpp

