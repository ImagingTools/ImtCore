// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtsdlgencpp/CCxxProcessorsManagerComp.h>

// C includes
#include <iostream>

// Qt includes
#include <QtCore/QTemporaryDir>
#include <QtCore/QTemporaryFile>
#include <QtCore/QLockFile>
#include <QtCore/QFileInfo>

// ACF includes
#include <istd/CSystem.h>
#include <iprm/TParamsPtr.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtsdl/CSdlTools.h>
#include <imtsdl/CSdlType.h>
#include <imtsdl/CSdlEnum.h>
#include <imtsdl/CSdlDocumentType.h>
#include <imtsdl/CSdlRequest.h>
#include <imtsdlgencpp/CQmlRegisterGeneratorComp.h>
#include <imtsdlgencpp/CSdlQObjectGeneratorComp.h>
#include <imtsdlgencpp/CSdlUnionObjectGeneratorComp.h>


namespace imtsdlgencpp
{

// static helpers

/**
	The processors that generate the QObject based classes (<Type>Object, <Type>ObjectList, union objects)
	and the QML types registration. Their output goes to the separate '<Schema>Objects' files.
*/
static bool IsQObjectProcessor(const ICxxFileProcessor* processorPtr)
{
	return dynamic_cast<const CSdlQObjectGeneratorComp*>(processorPtr) != nullptr ||
		dynamic_cast<const CSdlUnionObjectGeneratorComp*>(processorPtr) != nullptr ||
		dynamic_cast<const CQmlRegisterGeneratorComp*>(processorPtr) != nullptr;
}


/// \returns path of the file with QObject based classes for the given generated file path, i.e. 'Foo.h' -> 'FooObjects.h'
static QString ToObjectsFilePath(const QString& filePath)
{
	const int extensionIndex = filePath.lastIndexOf('.');
	if (extensionIndex < 0){
		return filePath + QStringLiteral("Objects");
	}

	return filePath.left(extensionIndex) + QStringLiteral("Objects") + filePath.mid(extensionIndex);
}


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
		stream << QStringLiteral(");");
	}
}


template <class T>
bool CreateDirectivesFromEntryList(const QList<T>& entryList, QList<imtsdl::IncludeDirective>& output, bool isObjectsFile = false)
{
	for (const T& entry: entryList){
		if (entry.IsExternal()){
			QString typeIncludePath = '<' + entry.GetTargetHeaderFilePath() + '>';
			if (isObjectsFile){
				typeIncludePath = '<' + ToObjectsFilePath(entry.GetTargetHeaderFilePath()) + '>';
			}

			if (typeIncludePath.isEmpty()){
				qCritical() << QStringLiteral("Header path is empty %1 of %2").arg(entry.GetName(), entry.GetSchemaFilePath());
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
		imtsdl::CSdlTools::CreateAcfDirective("<istd/TNullableValue.h>"),
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
	ibase::IProgressManager* /*progressManagerPtr*/,
	istd::IChangeable* /*processingReportPtr*/)
{
	Q_ASSERT(m_argumentParserCompPtr.IsValid());
	Q_ASSERT(m_schemaParamsCompPtr.IsValid());
	Q_ASSERT(m_dependentSchemaListCompPtr.IsValid());

	if (m_argumentParserCompPtr->IsDependenciesMode() || !m_argumentParserCompPtr->GetDepFilePath().isEmpty()){
		QStringList cumulatedFiles;
		const QString sourceFilePath = CalculateTargetCppFilesFromSchemaParams(*m_schemaParamsCompPtr, *m_argumentParserCompPtr)[imtsdl::ISdlProcessArgumentsParser::s_sourceFileType];
		const QString headerFilePath = CalculateTargetCppFilesFromSchemaParams(*m_schemaParamsCompPtr, *m_argumentParserCompPtr)[imtsdl::ISdlProcessArgumentsParser::s_headerFileType];
		const QString fwdFilePath = headerFilePath.chopped(2) + "_fwd.h";
		cumulatedFiles << sourceFilePath;
		cumulatedFiles << headerFilePath;
		cumulatedFiles << fwdFilePath;
		if (HasObjectEntries()){
			cumulatedFiles << ToObjectsFilePath(headerFilePath);
		}

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

	// the QObject based classes are placed to separate files, so the code that does not use them, does not pay for them
	EntryFileMap objectHeaderFiles;
	EntryFileMap objectSourceFiles;
	if (HasObjectEntries()){
		objectHeaderFiles = CreateObjectHeaderFiles(paramsPtr);
		objectSourceFiles = CreateObjectSourceFiles(paramsPtr);
	}

	if (objectHeaderFiles.size() == 1){
		const FilePtr& filePtr = objectHeaderFiles.first();
		if (!filePtr || !BeginHeaderFile(*m_schemaParamsCompPtr, *filePtr, paramsPtr, true)){
			SendErrorMessage(0, QStringLiteral("Unable to begin process objects header file"));

			return TS_INVALID;
		}
	}
	if (objectSourceFiles.size() == 1){
		const FilePtr& filePtr = objectSourceFiles.first();
		const QString headerFilePath = CalculateTargetCppFilesFromSchemaParams(*m_schemaParamsCompPtr, *m_argumentParserCompPtr)[imtsdl::ISdlProcessArgumentsParser::s_headerFileType];
		if (!filePtr || !BeginSourceFile(*m_schemaParamsCompPtr, *filePtr, paramsPtr, QFileInfo(ToObjectsFilePath(headerFilePath)).fileName())){
			SendErrorMessage(0, QStringLiteral("Unable to begin process objects source file"));

			return TS_INVALID;
		}
	}

	// begin files (if it only one)
	if (headerFiles.size() == 1){
		const FilePtr& filePtr = headerFiles.first();
		if (!filePtr){
			I_CRITICAL();

			return TS_INVALID;
		}

		if (!BeginHeaderFile(*m_schemaParamsCompPtr, *filePtr, paramsPtr)){
			SendErrorMessage(0, QStringLiteral("Unable to begin process file '%1'").arg(filePtr->fileName()));

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
			SendErrorMessage(0, QStringLiteral("Unable to begin process file '%1'").arg(filePtr->fileName()));

			return TS_INVALID;
		}
	}

	// process



	if (!ProcessEnums(headerFiles, sourceFiles, paramsPtr)){
		SendErrorMessage(0, "Unable to process enums");

		return TS_INVALID;
	}

	if (!ProcessTypes(headerFiles, sourceFiles, objectHeaderFiles, objectSourceFiles, paramsPtr)){
		SendErrorMessage(0, "Unable to process types");

		return TS_INVALID;
	}

	if (!ProcessUnions(headerFiles, sourceFiles, objectHeaderFiles, objectSourceFiles, paramsPtr)){
		SendErrorMessage(0, "Unable to process unions");

		return TS_INVALID;
	}

	if (!ProcessRequests(headerFiles, sourceFiles, paramsPtr)){
		SendErrorMessage(0, "Unable to process requests");

		return TS_INVALID;
	}

	imtsdl::ISdlProcessArgumentsParser::CppGenerationMode mode = m_argumentParserCompPtr->GetCppGenerationMode();
	if (mode != imtsdl::ISdlProcessArgumentsParser::CGM_HEADER_ONLY) {
		if (!ProcessDocumentTypes(headerFiles, sourceFiles, paramsPtr)) {
			SendErrorMessage(0, "Unable to process document types");

			return TS_INVALID;
		}
	}

	if (m_autoProcessorCompListPtr.IsValid()){
		const int processorsCount = m_autoProcessorCompListPtr.GetCount();
		imtsdl::CSdlType dummyType;
		FilePtr sourceFilePtr = GetFilePtrForEntry(dummyType, sourceFiles);
		FilePtr headerFilePtr = GetFilePtrForEntry(dummyType, headerFiles);
		FilePtr objectSourceFilePtr = GetFilePtrForEntry(dummyType, objectSourceFiles);
		FilePtr objectHeaderFilePtr = GetFilePtrForEntry(dummyType, objectHeaderFiles);
		for (int i = 0; i < processorsCount; ++i){
			ICxxFileProcessor* processorPtr = m_autoProcessorCompListPtr[i];
			Q_ASSERT(processorPtr != nullptr);
			if (IsQObjectProcessor(processorPtr)){
				if (objectHeaderFilePtr && objectSourceFilePtr){
					if (!processorPtr->ProcessEntry(dummyType, objectHeaderFilePtr.get(), objectSourceFilePtr.get(), paramsPtr)){
						SendErrorMessage(0, "Processing failed");

						return TS_INVALID;
					}
				}

				continue;
			}

			// Only pass headerDevicePtr when sourceDevicePtr is also available.
			// This prevents auto processors (e.g. GqlHandlerBaseClassGenerator) from
			// writing class declarations into the .h file in CGM_HEADER_ONLY mode,
			// where sourceFiles is empty. Class declarations belong exclusively in
			// _fwd.h which is generated by GenerateForwardDeclarationFile().
			QIODevice* headerDevicePtr = sourceFilePtr ? headerFilePtr.get() : nullptr;
			const bool ok = processorPtr->ProcessEntry(dummyType, headerDevicePtr, sourceFilePtr.get(), paramsPtr);
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
			SendErrorMessage(0, QStringLiteral("Unable to end process file '%1'").arg(filePtr->fileName()));

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
			SendErrorMessage(0, QStringLiteral("Unable to end process file '%1'").arg(filePtr->fileName()));

			return TS_INVALID;
		}
	}

	if (objectHeaderFiles.size() == 1){
		const FilePtr& filePtr = objectHeaderFiles.first();
		if (!EndHeaderFile(*m_schemaParamsCompPtr, *filePtr, paramsPtr, true)){
			SendErrorMessage(0, QStringLiteral("Unable to end process file '%1'").arg(filePtr->fileName()));

			return TS_INVALID;
		}
	}
	if (objectSourceFiles.size() == 1){
		const FilePtr& filePtr = objectSourceFiles.first();
		if (!EndSourceFile(*m_schemaParamsCompPtr, *filePtr, paramsPtr)){
			SendErrorMessage(0, QStringLiteral("Unable to end process file '%1'").arg(filePtr->fileName()));

			return TS_INVALID;
		}
	}

	// one source file per schema: the objects implementation and the moc output are compiled with the main one
	if (sourceFiles.size() == 1){
		const FilePtr& filePtr = sourceFiles.first();
		const FilePtr objectSourceFilePtr = objectSourceFiles.isEmpty() ? FilePtr() : objectSourceFiles.first();
		if (!AppendSourceFileTail(*filePtr, objectSourceFilePtr.get())){
			SendErrorMessage(0, QStringLiteral("Unable to end process file '%1'").arg(filePtr->fileName()));

			return TS_INVALID;
		}
	}

	// generate forward declaration file
	if (!GenerateForwardDeclarationFile(paramsPtr)){
		SendErrorMessage(0, "Unable to generate forward declaration file");

		return TS_INVALID;
	}

	return TS_OK;
}


// private methods

bool CCxxProcessorsManagerComp::BeginHeaderFile(
			const iprm::IParamsSet& entryParams,
			QFile& headerFile,
			const iprm::IParamsSet* /*paramsPtr*/,
			bool isObjectsFile) const
{
	// write pragma once first
	QTextStream stream(&headerFile);
	stream << QStringLiteral("#pragma once");
	FeedStream(stream, 2, false);

	if (isObjectsFile){
		// the objects file extends the main one
		stream << QStringLiteral("#include \"");
		stream << QFileInfo(headerFile).baseName().chopped(QStringLiteral("Objects").length());
		stream << QStringLiteral(".h\"");
		FeedStream(stream, 2, false);

		// special optional qt include
		stream << QStringLiteral("#ifdef QT_QML_LIB");
		FeedStream(stream, 1, false);
		stream << QStringLiteral("#include <QtQml/QQmlEngine>");
		FeedStream(stream, 1, false);
		stream << QStringLiteral("#endif // QT_QML_LIB");
		FeedStream(stream, 2, false);

		// the objects of the types from other schemas
		QList<imtsdl::IncludeDirective> objectsDirectivesList;
		objectsDirectivesList << imtsdl::CSdlTools::CreateImtDirective("<imtbase/CItemModelBase.h>");
		objectsDirectivesList << imtsdl::CSdlTools::CreateImtDirective("<imtsdl/TListModelBase.h>");
		if (m_sdlUnionListCompPtr.IsValid()){
			imtsdl::SdlUnionList list = m_sdlUnionListCompPtr->GetUnions(false);
			CreateDirectivesFromEntryList(list, objectsDirectivesList, true);
		}
		if (m_sdlTypeListCompPtr.IsValid()){
			imtsdl::SdlTypeList list = m_sdlTypeListCompPtr->GetSdlTypes(false);
			CreateDirectivesFromEntryList(list, objectsDirectivesList, true);
		}

		QString lastRemark;
		for (const imtsdl::IncludeDirective& directive: objectsDirectivesList){
			if (directive.remark != lastRemark){
				FeedStream(stream, 1, false);
				stream << (directive.remark.startsWith(QStringLiteral("//")) ? QString() : QStringLiteral("// ")) << directive.remark;
				FeedStream(stream, 1, false);
				lastRemark = directive.remark;
			}
			stream << QStringLiteral("#include ") << directive.path;
			FeedStream(stream, 1, false);
		}
		FeedStream(stream, 2, false);

		// begin namespace
		stream << QStringLiteral("namespace ");
		stream << GetNamespaceFromSchemaParams(entryParams);
		FeedStream(stream, 1, false);

		stream << '{';
		FeedStream(stream, 3, false);

		return true;
	}

	// include forward declarations file (contains enum classes)
	stream << QStringLiteral("#include \"");
	stream << QFileInfo(headerFile).baseName();
	stream << QStringLiteral("_fwd.h\"");
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

	return true;
}


bool CCxxProcessorsManagerComp::BeginSourceFile(
			const iprm::IParamsSet& entryParams,
			QFile& sourceFile,
			const iprm::IParamsSet* /*paramsPtr*/,
			const QString& objectsHeaderFileName) const
{
	QTextStream stream(&sourceFile);

	// include header file
	stream << QStringLiteral("#include \"");
	if (objectsHeaderFileName.isEmpty()){
		stream << QFileInfo(sourceFile).baseName();
		stream << QStringLiteral(".h");
	}
	else{
		stream << objectsHeaderFileName;
	}
	stream << '"';
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
			const iprm::IParamsSet* /*paramsPtr*/,
			bool isObjectsFile) const
{
	QTextStream stream(&headerFile);

	// end namespace
	const QString sdlNamespace = GetNamespaceFromSchemaParams(entryParams);
	stream << '}';
	stream << QStringLiteral(" // namespace ");
	stream << sdlNamespace;
	FeedStream(stream, isObjectsFile ? 1 : 2, false);

	if (isObjectsFile){
		return true;
	}

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


bool CCxxProcessorsManagerComp::AppendSourceFileTail(QFile& sourceFile, QFile* objectsSourceFilePtr) const
{
	QTextStream stream(&sourceFile);

	if (objectsSourceFilePtr != nullptr){
		if (!objectsSourceFilePtr->seek(0)){
			return false;
		}

		FeedStream(stream, 1, false);
		stream << QString::fromUtf8(objectsSourceFilePtr->readAll());
	}

	if (m_argumentParserCompPtr->IsMocIncludeEnabled()){
		const QString baseName = QFileInfo(sourceFile).baseName();
		FeedStream(stream, 2, false);
		stream << QStringLiteral("#include \"moc_") << baseName << QStringLiteral(".cpp\"");
		FeedStream(stream, 1, false);
		if (HasObjectEntries()){
			stream << QStringLiteral("#include \"moc_") << baseName << QStringLiteral("Objects.cpp\"");
			FeedStream(stream, 1, false);
		}
	}

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

	// The enums together with their Q_NAMESPACE/Q_ENUM_NS registrations are
	// wrapped in a schema-unique inline namespace. Multiple schemas can
	// contribute to the same C++ namespace (e.g. sdl::V1_0::imtbase), and
	// emitting Q_NAMESPACE in each of them would generate colliding
	// definitions of the namespace staticMetaObject. The inline namespace
	// keeps the enums and the EnumXxx wrapper classes accessible from the
	// parent namespace while giving each schema its own meta object.
	if (enumProcessorsCount > 0){
		const QString headerFilePath = CalculateTargetCppFilesFromSchemaParams(*m_schemaParamsCompPtr, *m_argumentParserCompPtr)[imtsdl::ISdlProcessArgumentsParser::s_headerFileType];

		FilePtr enumFilePtr = headerFiles.first(); // CreateFile(headerFilePath);
		if (!enumFilePtr){
			return TS_INVALID;
		}

		const QString schemaBaseName = QFileInfo(m_argumentParserCompPtr->GetSchemaFilePath()).baseName();

		QTextStream stream(enumFilePtr.get());
		stream << QStringLiteral("inline namespace ") << schemaBaseName << QStringLiteral("SdlEnums");
		FeedStream(stream, 1, false);
		stream << '{';
		FeedStream(stream, 2, false);
		FeedStream(stream, 2, false);

		stream << QStringLiteral("Q_NAMESPACE");
		FeedStream(stream, 2, false);
		stream.flush();

		for (int i = 0; i < enumProcessorsCount; ++i){
			ICxxFileProcessor* enumProcessorPtr = m_enumProcessorCompListPtr[i];
			Q_ASSERT(enumProcessorPtr != nullptr);
			for (const imtsdl::CSdlEnum& sdlEnum: enumList){
				FilePtr headerFilePtr = GetFilePtrForEntry(sdlEnum, headerFiles);
				Q_ASSERT(headerFilePtr);
				const bool ok = enumProcessorPtr->ProcessEntry(sdlEnum, headerFilePtr.get(), nullptr, paramsPtr);
				if (!ok){
					SendErrorMessage(0, QStringLiteral("Unable to process enum '%1'").arg(sdlEnum.GetName()));

					return false;
				}
			}
		}

		FeedStream(stream, 1, false);
		stream << QStringLiteral("} // inline namespace ") << schemaBaseName << QStringLiteral("SdlEnums");
		FeedStream(stream, 2, false);
		stream.flush();
	}

	return true;
}


bool CCxxProcessorsManagerComp::ProcessTypes(
	const EntryFileMap& headerFiles,
	const EntryFileMap& sourceFiles,
	const EntryFileMap& objectHeaderFiles,
	const EntryFileMap& objectSourceFiles,
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
		}
		FeedStream(stream, 1, false);
	}

	for (const FilePtr& objectHeaderFilePtr: objectHeaderFiles){
		Q_ASSERT(objectHeaderFilePtr);

		QTextStream stream(objectHeaderFilePtr.get());
		for (const imtsdl::CSdlUnion& sdlUinion: unionList){
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
		const bool isQObjectProcessor = IsQObjectProcessor(typeProcessorPtr);
		if (isQObjectProcessor && objectHeaderFiles.isEmpty() && objectSourceFiles.isEmpty()){
			// the objects files are not generated
			continue;
		}

		for (const imtsdl::CSdlType& sdlType: typeList){
			FilePtr headerFilePtr = GetFilePtrForEntry(sdlType, isQObjectProcessor ? objectHeaderFiles : headerFiles);
			FilePtr sourceFilePtr = GetFilePtrForEntry(sdlType, isQObjectProcessor ? objectSourceFiles : sourceFiles);
			const bool ok = typeProcessorPtr->ProcessEntry(sdlType, headerFilePtr.get(), sourceFilePtr.get(), paramsPtr);
			if (!ok){
				SendErrorMessage(0, QStringLiteral("Unable to process type '%1'").arg(sdlType.GetName()));

				return false;
			}
		}
	}

	return true;
}


bool CCxxProcessorsManagerComp::ProcessUnions(
			const EntryFileMap& headerFiles,
			const EntryFileMap& sourceFiles,
			const EntryFileMap& objectHeaderFiles,
			const EntryFileMap& objectSourceFiles,
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
		const bool isQObjectProcessor = IsQObjectProcessor(unionProcessorPtr);
		if (isQObjectProcessor && objectHeaderFiles.isEmpty() && objectSourceFiles.isEmpty()){
			// the objects files are not generated
			continue;
		}

		for (const imtsdl::CSdlUnion& sdlUinion: unionList){
			FilePtr headerFilePtr = GetFilePtrForEntry(sdlUinion, isQObjectProcessor ? objectHeaderFiles : headerFiles);
			FilePtr sourceFilePtr = GetFilePtrForEntry(sdlUinion, isQObjectProcessor ? objectSourceFiles : sourceFiles);
			Q_ASSERT(headerFilePtr || sourceFilePtr);
			const bool ok = unionProcessorPtr->ProcessEntry(sdlUinion, headerFilePtr.get(), sourceFilePtr.get(), paramsPtr);
			if (!ok){
				SendErrorMessage(0, QStringLiteral("Unable to process union '%1'").arg(sdlUinion.GetName()));

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
			FilePtr sourceFilePtr = GetFilePtrForEntry(documentType, sourceFiles);
			Q_ASSERT(sourceFilePtr);
			const bool ok = processorPtr->ProcessEntry(documentType, nullptr, sourceFilePtr.get(), paramsPtr);
			if (!ok){
				SendErrorMessage(0, QStringLiteral("Unable to process document type '%1'").arg(documentType.GetName()));
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
				SendErrorMessage(0, QStringLiteral("Unable to process request '%1'").arg(request.GetName()));

				return false;
			}
		}
	}

	return true;
}


bool CCxxProcessorsManagerComp::GenerateForwardDeclarationFile(const iprm::IParamsSet* paramsPtr) const
{
	imtsdl::ISdlProcessArgumentsParser::CppGenerationMode mode = m_argumentParserCompPtr->GetCppGenerationMode();
	if (mode == imtsdl::ISdlProcessArgumentsParser::CGM_IMPLEMENTATION_ONLY){
		return true;
	}

	// calculate fwd file path (same as header but with _fwd suffix)
	const QString headerFilePath = CalculateTargetCppFilesFromSchemaParams(*m_schemaParamsCompPtr, *m_argumentParserCompPtr)[imtsdl::ISdlProcessArgumentsParser::s_headerFileType];
	const QString fwdFilePath = headerFilePath.chopped(2) + "_fwd.h";

	FilePtr fwdFilePtr = CreateFile(fwdFilePath);
	if (!fwdFilePtr){
		return false;
	}

	QTextStream stream(fwdFilePtr.get());

	// write pragma once
	stream << QStringLiteral("#pragma once");
	FeedStream(stream, 3, false);

	// check if we have enums
	bool hasEnums = false;
	if (m_enumProcessorCompListPtr.IsValid() && m_sdlEnumListCompPtr.IsValid()){
		const imtsdl::SdlEnumList enumList = m_sdlEnumListCompPtr->GetEnums(true);
		hasEnums = !enumList.isEmpty();
	}

	// check if we have requests (need forward declaration for GqlHandlerCompBase)
	bool hasRequests = false;
	if (m_requestsProviderListCompPtr.IsValid()){
		const imtsdl::SdlRequestList requestsList = m_requestsProviderListCompPtr->GetRequests(true);
		hasRequests = !requestsList.isEmpty();
	}

	// check if we have document types
	bool hasDocumentTypes = false;
	if (m_sdlDocumentTypeListCompPtr.IsValid()){
		const imtsdl::SdlDocumentTypeList documentTypesList = m_sdlDocumentTypeListCompPtr->GetDocumentTypes(true);
		hasDocumentTypes = !documentTypesList.isEmpty();
	}

	// add include for CObjectCollectionControllerCompBase if we have document types
	if (hasDocumentTypes){
		stream << QStringLiteral("#include <imtservergql/CObjectCollectionControllerCompBase.h>");
		FeedStream(stream, 1, false);
	}

	// add include for GqlHandlerCompBase base class if we have requests
	if (hasRequests){
		stream << QStringLiteral("#include <imtservergql/CPermissibleGqlRequestHandlerComp.h>");
		FeedStream(stream, 1, false);
	}

	if (hasEnums || hasDocumentTypes || hasRequests){
		FeedStream(stream, 2, false);
	}

	// add custom types includes (from external/dependent schemas)
	QList<imtsdl::IncludeDirective> customDirectives;
	if (m_sdlTypeListCompPtr.IsValid()){
		imtsdl::SdlTypeList list = m_sdlTypeListCompPtr->GetSdlTypes(false);
		CreateDirectivesFromEntryList(list, customDirectives);
	}
	if (m_sdlUnionListCompPtr.IsValid()){
		imtsdl::SdlUnionList list = m_sdlUnionListCompPtr->GetUnions(false);
		CreateDirectivesFromEntryList(list, customDirectives);
	}
	if (m_sdlDocumentTypeListCompPtr.IsValid()){
		imtsdl::SdlDocumentTypeList list = m_sdlDocumentTypeListCompPtr->GetDocumentTypes(false);
		CreateDirectivesFromEntryList(list, customDirectives);
	}
	if (m_requestsProviderListCompPtr.IsValid()){
		imtsdl::SdlRequestList list = m_requestsProviderListCompPtr->GetRequests(false);
		CreateDirectivesFromEntryList(list, customDirectives);
	}
	if (m_sdlEnumListCompPtr.IsValid()){
		imtsdl::SdlEnumList list = m_sdlEnumListCompPtr->GetEnums(false);
		CreateDirectivesFromEntryList(list, customDirectives);
	}

	if (!customDirectives.isEmpty()){
		stream << QStringLiteral("// custom types includes");
		FeedStream(stream, 1, false);
		for (const imtsdl::IncludeDirective& directive: customDirectives){
			// Convert .h path to _fwd.h for forward declaration files
			QString fwdPath = directive.path;
			if (fwdPath.endsWith(QStringLiteral(".h>"))){
				fwdPath.replace(fwdPath.length() - 3, 3, QStringLiteral("_fwd.h>"));
			}
			stream << QStringLiteral("#include ");
			stream << fwdPath;
			FeedStream(stream, 1, false);
		}
		FeedStream(stream, 2, false);
	}

	// begin namespace
	const QString sdlNamespace = GetNamespaceFromSchemaParams(*m_schemaParamsCompPtr);
	stream << QStringLiteral("namespace ");
	stream << sdlNamespace;
	FeedStream(stream, 1, false);
	stream << '{';
	FeedStream(stream, 2, false);

	// generate enum classes
	const QString schemaBaseName = QFileInfo(m_argumentParserCompPtr->GetSchemaFilePath()).baseName();

	if (hasEnums){
		// The enums together with their Q_NAMESPACE/Q_ENUM_NS registrations are
		// wrapped in a schema-unique inline namespace. Multiple schemas can
		// contribute to the same C++ namespace (e.g. sdl::V1_0::imtbase), and
		// emitting Q_NAMESPACE in each of them would generate colliding
		// definitions of the namespace staticMetaObject. The inline namespace
		// keeps the enums and the EnumXxx wrapper classes accessible from the
		// parent namespace while giving each schema its own meta object.

		stream << QStringLiteral("inline namespace ") << schemaBaseName << QStringLiteral("SdlEnums");
		FeedStream(stream, 1, false);
		stream << '{';
		FeedStream(stream, 2, false);

		// flush the stream before passing the device to enum processors
		stream.flush();

		const imtsdl::SdlEnumList enumList = m_sdlEnumListCompPtr->GetEnums(true);
		FeedStream(stream, 1, false);
		for (const imtsdl::CSdlEnum& sdlEnum: enumList){
			stream <<  QStringLiteral("enum class ") << sdlEnum.GetName() << ';';
			FeedStream(stream, 1, false);
		}

		FeedStream(stream, 2, false);
		stream << QStringLiteral("} // inline namespace ") << schemaBaseName << QStringLiteral("SdlEnums");
		FeedStream(stream, 2, false);
	}

	stream.flush();

	// re-create stream after enum processors may have written to the file
	QTextStream fwdStream(fwdFilePtr.get());

	// forward declare all types
	if (m_sdlTypeListCompPtr.IsValid()){
		const imtsdl::SdlTypeList typeList = m_sdlTypeListCompPtr->GetSdlTypes(true);
		if (!typeList.isEmpty()){
			fwdStream << QStringLiteral("// type forward declarations");
			FeedStream(fwdStream, 1, false);
			for (const imtsdl::CSdlType& sdlType: typeList){
				fwdStream << QStringLiteral("class C") << sdlType.GetName() << ';';
				FeedStream(fwdStream, 1, false);
			}
			FeedStream(fwdStream, 1, false);
		}
	}

	// forward declare all unions
	if (m_sdlUnionListCompPtr.IsValid()){
		const imtsdl::SdlUnionList unionList = m_sdlUnionListCompPtr->GetUnions(true);
		if (!unionList.isEmpty()){
			fwdStream << QStringLiteral("// union forward declarations");
			FeedStream(fwdStream, 1, false);
			for (const imtsdl::CSdlUnion& sdlUnion: unionList){
				fwdStream << QStringLiteral("class ") << sdlUnion.GetName() << ';';
				FeedStream(fwdStream, 1, false);
				if (HasObjectEntries()){
					fwdStream << QStringLiteral("class C") << sdlUnion.GetName() << QStringLiteral("Object;");
					FeedStream(fwdStream, 1, false);
					fwdStream << QStringLiteral("class C") << sdlUnion.GetName() << QStringLiteral("ObjectList;");
					FeedStream(fwdStream, 1, false);
				}
			}
			FeedStream(fwdStream, 1, false);
		}
	}

	// forward declare requests
	if (hasRequests){
		const imtsdl::SdlRequestList requestsList = m_requestsProviderListCompPtr->GetRequests(true);
		fwdStream << QStringLiteral("// request forward declarations");
		FeedStream(fwdStream, 1, false);
		for (const imtsdl::CSdlRequest& request: requestsList){
			fwdStream << QStringLiteral("struct ") << request.GetName() << QStringLiteral("RequestInfo;");
			FeedStream(fwdStream, 1, false);

			fwdStream << QStringLiteral("class C") << request.GetName() << QStringLiteral("GqlRequest;");
			FeedStream(fwdStream, 1, false);
		}
		FeedStream(fwdStream, 1, false);
	}

	// generate CCollectionControllerCompBase class definitions (moved from .h)
	if (hasDocumentTypes && m_documentTypeProcessorCompListPtr.IsValid()){
		// flush the stream before passing the device to document type processors
		fwdStream.flush();

		const imtsdl::SdlDocumentTypeList documentTypesList = m_sdlDocumentTypeListCompPtr->GetDocumentTypes(true);
		const int documentTypeProcessorsCount = m_documentTypeProcessorCompListPtr.GetCount();
		for (int i = 0; i < documentTypeProcessorsCount; ++i){
			ICxxFileProcessor* processorPtr = m_documentTypeProcessorCompListPtr[i];
			Q_ASSERT(processorPtr != nullptr);
			for (const imtsdl::CSdlDocumentType& documentType: documentTypesList){
				const bool ok = processorPtr->ProcessEntry(documentType, fwdFilePtr.get(), nullptr, paramsPtr);
				if (!ok){
					return false;
				}
			}
		}
	}

	// generate GqlHandlerCompBase class definition (moved from .h)
	if (hasRequests && m_autoProcessorCompListPtr.IsValid()){
		// flush the stream before passing the device to auto processors
		fwdStream.flush();

		const int processorsCount = m_autoProcessorCompListPtr.GetCount();
		imtsdl::CSdlType dummyType;
		for (int i = 0; i < processorsCount; ++i){
			ICxxFileProcessor* processorPtr = m_autoProcessorCompListPtr[i];
			Q_ASSERT(processorPtr != nullptr);
			const bool ok = processorPtr->ProcessEntry(dummyType, fwdFilePtr.get(), nullptr, paramsPtr);
			if (!ok){
				return false;
			}
		}
	}



	// end namespace
	fwdStream << '}';
	fwdStream << QStringLiteral(" // namespace ");
	fwdStream << sdlNamespace;
	FeedStream(fwdStream, 1, false);
	fwdStream.flush();

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


CCxxProcessorsManagerComp::EntryFileMap CCxxProcessorsManagerComp::CreateObjectHeaderFiles(const iprm::IParamsSet* /*paramsPtr*/) const
{
	EntryFileMap retVal;

	imtsdl::ISdlProcessArgumentsParser::CppGenerationMode mode = m_argumentParserCompPtr->GetCppGenerationMode();
	if (mode == imtsdl::ISdlProcessArgumentsParser::CGM_IMPLEMENTATION_ONLY){
		return retVal;
	}

	const QString outputFilePath = CalculateTargetCppFilesFromSchemaParams(*m_schemaParamsCompPtr, *m_argumentParserCompPtr)[imtsdl::ISdlProcessArgumentsParser::s_headerFileType];
	FilePtr singleFilePtr = CreateFile(ToObjectsFilePath(outputFilePath));
	if (!singleFilePtr){
		return retVal;
	}
	retVal[QString()] = singleFilePtr;

	return retVal;
}


CCxxProcessorsManagerComp::EntryFileMap CCxxProcessorsManagerComp::CreateObjectSourceFiles(const iprm::IParamsSet* /*paramsPtr*/) const
{
	EntryFileMap retVal;

	imtsdl::ISdlProcessArgumentsParser::CppGenerationMode mode = m_argumentParserCompPtr->GetCppGenerationMode();
	if (mode == imtsdl::ISdlProcessArgumentsParser::CGM_HEADER_ONLY){
		return retVal;
	}

	// the content is appended to the main source file by AppendSourceFileTail
	std::shared_ptr<QTemporaryFile> singleFilePtr(new QTemporaryFile);
	if (!singleFilePtr->open()){
		SendErrorMessage(0, QStringLiteral("Unable to open a temporary file. Error: %1").arg(singleFilePtr->errorString()));

		return retVal;
	}
	retVal[QString()] = singleFilePtr;

	return retVal;
}


bool CCxxProcessorsManagerComp::HasObjectEntries() const
{
	// the files are created even if the schema has nothing for them, so the build system knows the outputs without parsing the schema
	return m_argumentParserCompPtr->IsObjectsGenerationEnabled();
}


CCxxProcessorsManagerComp::FilePtr CCxxProcessorsManagerComp::CreateFile(const QString& filePath) const
{
	FilePtr nullPtr;

	const bool isPathExists = istd::CSystem::EnsurePathExists(QDir(filePath + "/..").absolutePath());
	if (!isPathExists){
		SendErrorMessage(0, QStringLiteral("Unable to create output dir: '%1'").arg(filePath));

		return nullPtr;
	}

	FilePtr outputFilePtr(new QFile(filePath));
	const bool isOpen = outputFilePtr->open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate);
	if (!isOpen){
		SendErrorMessage(0, QStringLiteral("Unable to open file: '%1'. Error: %2").arg(filePath, outputFilePtr->errorString()));

		return nullPtr;
	}

	return outputFilePtr;
}




} // namespace imtsdlgencpp
