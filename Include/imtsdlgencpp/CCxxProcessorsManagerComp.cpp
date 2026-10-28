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


namespace imtsdlgencpp
{

// static helpers

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
		auto files = CalculateTargetCppFilesFromSchemaParams(*m_schemaParamsCompPtr, *m_argumentParserCompPtr);
		QStringList cumulatedFiles = files.values();

		PrintFiles(std::cout, cumulatedFiles, m_argumentParserCompPtr->GetGeneratorType());
		PrintFiles(m_argumentParserCompPtr->GetDepFilePath(), cumulatedFiles, *m_dependentSchemaListCompPtr);

		if (m_argumentParserCompPtr->IsDependenciesMode()){
			return TS_OK;
		}
	}

	const EntryFileInfo createdFiles = CreateFiles();

	if (!createdFiles.isValid){
		SendErrorMessage(0, "Can't create required files");

		return TS_INVALID;
	}

	if (createdFiles.headerFile){
		if (!BeginHeaderFile(*m_schemaParamsCompPtr, *createdFiles.headerFile, paramsPtr)){
			SendErrorMessage(0, QStringLiteral("Unable to begin process file '%1'").arg(createdFiles.headerFile->fileName()));

			return TS_INVALID;
		}
	}

	if (createdFiles.objectHeaderFile){
		if (!BeginHeaderFile(*m_schemaParamsCompPtr, *createdFiles.objectHeaderFile, paramsPtr, true)) {
			SendErrorMessage(0, QStringLiteral("Unable to begin process objects header file '%1'").arg(createdFiles.objectHeaderFile->fileName()));

			return TS_INVALID;
		}
	}

	if (createdFiles.sourceFile){
		if (!BeginSourceFile(*m_schemaParamsCompPtr, *createdFiles.sourceFile, paramsPtr)){
			SendErrorMessage(0, QStringLiteral("Unable to begin process file '%1'").arg(createdFiles.sourceFile->fileName()));

			return TS_INVALID;
		}
	}

	if (createdFiles.objectSourceFile){
		if (!createdFiles.objectHeaderFile){
			SendErrorMessage(0, QStringLiteral("Unable to process objects source file because header file is missing for '%1'").arg(createdFiles.objectSourceFile->fileName()));

			return TS_INVALID;
		}

		if (!BeginSourceFile(*m_schemaParamsCompPtr, *createdFiles.objectSourceFile, paramsPtr)){
			SendErrorMessage(0, QStringLiteral("Unable to begin process objects source file '%1'").arg(createdFiles.objectSourceFile->fileName()));

			return TS_INVALID;
		}
	}

	// process

	if (!ProcessEnums(createdFiles, paramsPtr)){
		SendErrorMessage(0, "Unable to process enums");

		return TS_INVALID;
	}

	if (!ProcessTypes(createdFiles, paramsPtr)){
		SendErrorMessage(0, "Unable to process types");

		return TS_INVALID;
	}

	if (!ProcessUnions(createdFiles, paramsPtr)){
		SendErrorMessage(0, "Unable to process unions");

		return TS_INVALID;
	}

	if (!ProcessRequests(createdFiles, paramsPtr)){
		SendErrorMessage(0, "Unable to process requests");

		return TS_INVALID;
	}

	imtsdl::ISdlProcessArgumentsParser::CppGenerationMode mode = m_argumentParserCompPtr->GetCppGenerationMode();
	if (mode != imtsdl::ISdlProcessArgumentsParser::CGM_HEADER_ONLY) {
		if (!ProcessDocumentTypes(createdFiles, paramsPtr)) {
			SendErrorMessage(0, "Unable to process document types");

			return TS_INVALID;
		}
	}

	if (m_autoProcessorCompListPtr.IsValid()){
		const int processorsCount = m_autoProcessorCompListPtr.GetCount();
		imtsdl::CSdlType dummyType;
		for (int i = 0; i < processorsCount; ++i){
			ICxxFileProcessor* processorPtr = m_autoProcessorCompListPtr[i];
			Q_ASSERT(processorPtr != nullptr);

			// Only pass headerDevicePtr when sourceDevicePtr is also available.
			// This prevents auto processors (e.g. GqlHandlerBaseClassGenerator) from
			// writing class declarations into the .h file in CGM_HEADER_ONLY mode,
			// where sourceFiles is empty. Class declarations belong exclusively in
			// _fwd.h which is generated by \c CCxxForwardDeclarationPostProcessorComp.
			QIODevice* headerDevicePtr = createdFiles.sourceFile ? createdFiles.headerFile.get() : nullptr;
			const bool ok = processorPtr->ProcessEntry(dummyType, headerDevicePtr, createdFiles.sourceFile.get(), paramsPtr);
			if (!ok){
				SendErrorMessage(0, "Processing failed");

				return TS_INVALID;
			}
		}
	}

	if (m_autoObjectProcessorCompListPtr.IsValid() && createdFiles.objectHeaderFile && createdFiles.objectSourceFile){
		const int processorsCount = m_autoObjectProcessorCompListPtr.GetCount();
		imtsdl::CSdlType dummyType;
		for (int i = 0; i < processorsCount; ++i){
			ICxxFileProcessor* processorPtr = m_autoObjectProcessorCompListPtr[i];
			Q_ASSERT(processorPtr != nullptr);

			const bool ok = processorPtr->ProcessEntry(dummyType, createdFiles.objectHeaderFile.get(), createdFiles.objectSourceFile.get(), paramsPtr);
			if (!ok){
				SendErrorMessage(0, "Processing failed");

				return TS_INVALID;
			}
		}
	}

	// end files
	if (createdFiles.headerFile){
		if (!EndHeaderFile(*m_schemaParamsCompPtr, *createdFiles.headerFile, paramsPtr)){
			SendErrorMessage(0, QStringLiteral("Unable to end process file '%1'").arg(createdFiles.headerFile->fileName()));

			return TS_INVALID;
		}
	}

	if (createdFiles.sourceFile){
		if (!EndSourceFile(*m_schemaParamsCompPtr, *createdFiles.sourceFile, paramsPtr)){
			SendErrorMessage(0, QStringLiteral("Unable to end process file '%1'").arg(createdFiles.sourceFile->fileName()));

			return TS_INVALID;
		}
	}

	if (createdFiles.objectHeaderFile){
		if (!EndHeaderFile(*m_schemaParamsCompPtr, *createdFiles.objectHeaderFile, paramsPtr, true)){
			SendErrorMessage(0, QStringLiteral("Unable to end process file '%1'").arg(createdFiles.objectHeaderFile->fileName()));

			return TS_INVALID;
		}
	}

	if (createdFiles.objectSourceFile){
		if (!EndSourceFile(*m_schemaParamsCompPtr, *createdFiles.objectSourceFile, paramsPtr)){
			SendErrorMessage(0, QStringLiteral("Unable to end process file '%1'").arg(createdFiles.objectSourceFile->fileName()));

			return TS_INVALID;
		}
	}


	// post processing
	const int postProcessorsCount = m_postProcessorCompListPtr.GetCount();
	for (int i = 0; i < postProcessorsCount; ++i){
		ICxxFilePostProcessor* postProcessorPtr = m_postProcessorCompListPtr[i];
		Q_ASSERT(postProcessorPtr != nullptr);

		if (!postProcessorPtr->ProcessFiles(createdFiles, paramsPtr)){
			SendErrorMessage(0, "Post processing failed");

			return TS_INVALID;
		}
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
			if (!CreateDirectivesFromEntryList(list, objectsDirectivesList, true)){
				SendCriticalMessage(0, "Unable to create include directives from union list for schema " + m_argumentParserCompPtr->GetSchemaFilePath());

				return false;
			}
		}
		if (m_sdlTypeListCompPtr.IsValid()){
			imtsdl::SdlTypeList list = m_sdlTypeListCompPtr->GetSdlTypes(false);
			if (!CreateDirectivesFromEntryList(list, objectsDirectivesList, true)){
				SendCriticalMessage(0, "Unable to create include directives from type list for schema " + m_argumentParserCompPtr->GetSchemaFilePath());

				return false;
			}
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

bool CCxxProcessorsManagerComp::ProcessEnums(
			const EntryFileInfo& files,
			const iprm::IParamsSet* paramsPtr) const
{
	if (!m_enumProcessorCompListPtr.IsValid() || !m_sdlEnumListCompPtr.IsValid() || !files.headerFile){
		// nothing todo
		return true;
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
		QFile* enumFilePtr = files.headerFile.get();

		const QString schemaBaseName = QFileInfo(m_argumentParserCompPtr->GetSchemaFilePath()).baseName();

		QTextStream stream(enumFilePtr);
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
				const bool ok = enumProcessorPtr->ProcessEntry(sdlEnum, enumFilePtr, nullptr, paramsPtr);
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


template <class ProcessorListT, class EntryListT>
bool CCxxProcessorsManagerComp::ProcessEntries(
			const ProcessorListT& processorList,
			const EntryListT& entryList,
			QFile* headerFilePtr,
			QFile* sourceFilePtr,
			const iprm::IParamsSet* paramsPtr,
			const QString& entryKind) const
{
	const int processorsCount = processorList.GetCount();
	for (int i = 0; i < processorsCount; ++i){
		ICxxFileProcessor* processorPtr = processorList[i];
		Q_ASSERT(processorPtr != nullptr);

		for (const auto& sdlEntry: entryList){
			Q_ASSERT(headerFilePtr || sourceFilePtr);
			const bool ok = processorPtr->ProcessEntry(sdlEntry, headerFilePtr, sourceFilePtr, paramsPtr);
			if (!ok){
				SendErrorMessage(0, QStringLiteral("Unable to process %1 '%2'").arg(entryKind, sdlEntry.GetName()));

				return false;
			}
		}
	}

	return true;
}


bool CCxxProcessorsManagerComp::ProcessTypes(
			const EntryFileInfo& files,
			const iprm::IParamsSet* paramsPtr) const
{
	if ((!m_typeProcessorCompListPtr.IsValid() && !m_typeObjectProcessorCompListPtr.IsValid()) || !m_sdlTypeListCompPtr.IsValid()){
		// nothing todo
		return true;
	}

	// before process types it is required to forward declare of all unions
	const imtsdl::SdlUnionList unionList = m_sdlUnionListCompPtr->GetUnions(true);

	if (QFile* headerFilePtr = files.headerFile.get()){
		QTextStream stream(headerFilePtr);
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

	if (QFile* objectHeaderFilePtr = files.objectHeaderFile.get()){
		QTextStream stream(objectHeaderFilePtr);
		for (const imtsdl::CSdlUnion& sdlUinion: unionList){
			stream << QStringLiteral("class C")  << sdlUinion.GetName() << QStringLiteral("Object;");
			FeedStream(stream, 1, false);
			stream << QStringLiteral("class C")  << sdlUinion.GetName() << QStringLiteral("ObjectList;");
			FeedStream(stream, 1, false);
		}
		FeedStream(stream, 1, false);
	}

	const imtsdl::SdlTypeList typeList = m_sdlTypeListCompPtr->GetSdlTypes(true);
	if (!ProcessEntries(m_typeProcessorCompListPtr, typeList, files.headerFile.get(), files.sourceFile.get(), paramsPtr, QStringLiteral("type"))){
		return false;
	}

	if (files.objectHeaderFile || files.objectSourceFile){
		return ProcessEntries(m_typeObjectProcessorCompListPtr, typeList, files.objectHeaderFile.get(), files.objectSourceFile.get(), paramsPtr, QStringLiteral("type"));
	}

	// the objects files are not generated
	return true;
}


bool CCxxProcessorsManagerComp::ProcessUnions(
			const EntryFileInfo& files,
			const iprm::IParamsSet* paramsPtr) const
{
	if ((!m_unionProcessorCompListPtr.IsValid() && !m_unionObjectProcessorCompListPtr.IsValid()) || !m_sdlUnionListCompPtr.IsValid() || (!files.headerFile && !files.sourceFile)){
		// nothing todo
		return true;
	}


	const imtsdl::SdlUnionList unionList = m_sdlUnionListCompPtr->GetUnions(true);
	if (!ProcessEntries(m_unionProcessorCompListPtr, unionList, files.headerFile.get(), files.sourceFile.get(), paramsPtr, QStringLiteral("union"))){
		return false;
	}

	if (files.objectHeaderFile || files.objectSourceFile){
		return ProcessEntries(m_unionObjectProcessorCompListPtr, unionList, files.objectHeaderFile.get(), files.objectSourceFile.get(), paramsPtr, QStringLiteral("union"));
	}

	// the objects files are not generated
	return true;
}


bool CCxxProcessorsManagerComp::ProcessDocumentTypes(
			const EntryFileInfo& files,
			const iprm::IParamsSet* paramsPtr) const
{
	if (	!m_documentTypeProcessorCompListPtr.IsValid() ||
			!m_sdlDocumentTypeListCompPtr.IsValid() ||
			!files.sourceFile)
	{
		// nothing todo
		return true;
	}


	const imtsdl::SdlDocumentTypeList documentTypesList = m_sdlDocumentTypeListCompPtr->GetDocumentTypes(true);
	const int documentTypeProcessorsCount = m_documentTypeProcessorCompListPtr.GetCount();
	for (int i = 0; i < documentTypeProcessorsCount; ++i){
		ICxxFileProcessor* processorPtr = m_documentTypeProcessorCompListPtr[i];
		Q_ASSERT(processorPtr != nullptr);
		for (const imtsdl::CSdlDocumentType& documentType: documentTypesList){
			const bool ok = processorPtr->ProcessEntry(documentType, nullptr, &*files.sourceFile, paramsPtr);
			if (!ok){
				SendErrorMessage(0, QStringLiteral("Unable to process document type '%1'").arg(documentType.GetName()));
			}
		}
	}

	return true;
}


bool CCxxProcessorsManagerComp::ProcessRequests(
			const EntryFileInfo& files,
			const iprm::IParamsSet* paramsPtr) const
{
	if (	!m_requestProcessorCompListPtr.IsValid() ||
			!m_requestsProviderListCompPtr.IsValid() ||
			(!files.headerFile && !files.sourceFile))
	{
		// nothing todo
		return true;
	}

	const imtsdl::SdlRequestList requestsList = m_requestsProviderListCompPtr->GetRequests(true);
	const int requestsProcessorsCount = m_requestProcessorCompListPtr.GetCount();
	for (int i = 0; i < requestsProcessorsCount; ++i){
		ICxxFileProcessor* requestProcessorPtr = m_requestProcessorCompListPtr[i];
		Q_ASSERT(requestProcessorPtr != nullptr);
		for (const imtsdl::CSdlRequest& request: requestsList){

			const bool ok = requestProcessorPtr->ProcessEntry(request, &*files.headerFile, &*files.sourceFile, paramsPtr);
			if (!ok){
				SendErrorMessage(0, QStringLiteral("Unable to process request '%1'").arg(request.GetName()));

				return false;
			}
		}
	}

	return true;
}

CCxxProcessorsManagerComp::EntryFileInfo CCxxProcessorsManagerComp::CreateFiles() const
{
	EntryFileInfo retVal;

	imtsdl::ISdlProcessArgumentsParser::CppGenerationMode mode = m_argumentParserCompPtr->GetCppGenerationMode();
	using MODE_ = imtsdl::ISdlProcessArgumentsParser::CppGenerationMode;
	const auto allFiles = CalculateTargetCppFilesFromSchemaParams(*m_schemaParamsCompPtr, *m_argumentParserCompPtr);

	if (mode == MODE_::CGM_HEADER_ONLY || mode == MODE_::CGM_FULL){
		const QString hFilePath = allFiles[imtsdl::ISdlProcessArgumentsParser::s_headerFileType];

		retVal.headerFile = CreateFile(hFilePath);
		if (!retVal.headerFile){
			return {};
		}

		const QString fwdHFilePath = allFiles[imtsdl::ISdlProcessArgumentsParser::s_fwdHeaderFileType];
		retVal.fwdHeaderFile = CreateFile(fwdHFilePath);
		if (!retVal.fwdHeaderFile){
			return {};
		}

		if (allFiles.contains(imtsdl::ISdlProcessArgumentsParser::s_objectHeaderFileType)){
			const QString objHFilePath = allFiles[imtsdl::ISdlProcessArgumentsParser::s_objectHeaderFileType];
			retVal.objectHeaderFile = CreateFile(objHFilePath);
			if (!retVal.objectHeaderFile){
				return {};
			}
		}
	}

	if (mode == MODE_::CGM_IMPLEMENTATION_ONLY || mode == MODE_::CGM_FULL){
		const QString cppFilePath = allFiles[imtsdl::ISdlProcessArgumentsParser::s_sourceFileType];
		retVal.sourceFile = CreateFile(cppFilePath);
		if (!retVal.sourceFile){
			return {};
		}

		if (allFiles.contains(imtsdl::ISdlProcessArgumentsParser::s_objectSourceFileType)){
			const QString objCppFilePath = allFiles[imtsdl::ISdlProcessArgumentsParser::s_objectSourceFileType];
			// special case. we always deleting object source files after use, so we create them as temporary.
			/// \todo think about this case should we always remove temporary object source files after use mb DON'T APPEND implementation data to .h file never
			retVal.objectSourceFile = CreateFile(objCppFilePath, true);
			if (!retVal.objectSourceFile){
				return {};
			}
		}
	}

	retVal.isValid = true;
	return retVal;
}


CCxxProcessorsManagerComp::FilePtr CCxxProcessorsManagerComp::CreateFile(const QString& filePath, bool temporary) const
{
	FilePtr nullPtr;

	const bool isPathExists = istd::CSystem::EnsurePathExists(QDir(filePath + "/..").absolutePath());
	if (!isPathExists){
		SendErrorMessage(0, QStringLiteral("Unable to create output dir: '%1'").arg(filePath));

		return nullPtr;
	}

	FilePtr outputFilePtr;
	if (temporary){
		outputFilePtr = std::make_shared<QTemporaryFile>(filePath);
	}
	else {
		outputFilePtr = std::make_shared<QFile>(filePath);
	}

	const bool isOpen = outputFilePtr->open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate);
	if (!isOpen){
		SendErrorMessage(0, QStringLiteral("Unable to open file: '%1'. Error: %2").arg(filePath, outputFilePtr->errorString()));

		return nullPtr;
	}

	return outputFilePtr;
}




} // namespace imtsdlgencpp
