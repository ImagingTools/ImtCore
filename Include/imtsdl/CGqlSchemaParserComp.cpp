#include <imtsdl/CGqlSchemaParserComp.h>


// Qt includes
#include <QtCore/QDir>
#include <QtCore/QRegularExpression>

// ACF includes
#include <iprm/CParamsSet.h>
#include <iprm/COptionsManager.h>
#include <iprm/TParamsPtr.h>
#include <ifile/CFileNameParam.h>

// ImtCore includes
#include <imtsdl/CSdlTools.h>


namespace imtsdl
{


// public static variables

const QByteArray CGqlSchemaParserComp::s_processedFilesParamId = QByteArrayLiteral("ProcessedFiles");
const QByteArray CGqlSchemaParserComp::s_typeListParamId = QByteArrayLiteral("TypeList");
const QByteArray CGqlSchemaParserComp::s_requestListParamId = QByteArrayLiteral("RequestList");
const QByteArray CGqlSchemaParserComp::s_documentTypeListParamId = QByteArrayLiteral("DocumentTypeList");


// public methods

// reimplemented(iproc::IProcessor)

int CGqlSchemaParserComp::DoProcessing(
			const iprm::IParamsSet* /*paramsPtr*/,
			const istd::IPolymorphic* inputPtr,
			istd::IChangeable* outputPtr,
			ibase::IProgressManager* /*progressManagerPtr*/)
{
	/**
		\param paramsPtr - unused
		\param inputPtr (optional) of \c ifile::IFileNameParam if set, use it to parse schema or from \c ArgumentParser otherwise
		\param outputPtr - (optional) of \c iprm::CParamsSet
						- [ProcessedFiles]	(\c s_processedFilesParamId)	of \c iprm::IOptionsManager if set, will contain list of processed files(absolute path list)
						- [TypeList]		(\c s_typeListParamId)			of \c iprm::CParamsSet of \c CSdlType - SDL type list extracted from schema
						- [RequestList]		(\c s_requestListParamId)		of \c iprm::CParamsSet of \c CSdlRequest - SDL request list extracted from schema
						- [DocumentTypeList](\c s_documentTypeListParamId)	of \c iprm::CParamsSet of \c CSdlDocumentType - SDL document type list extracted from schema
		\param progressManagerPtr - unused
	 */

	if (!SetupSchemaFilePath(inputPtr)){
		SendCriticalMessage(0, "The schema path parameter is invalid");
		I_CRITICAL();

		return TS_INVALID;
	}

	// initialize output params
	iprm::CParamsSet* outputParamsSetPtr = dynamic_cast<iprm::CParamsSet*>(outputPtr);
	iprm::IOptionsManager* processedFilesPtr = nullptr;
	iprm::CParamsSet* typeListParamsPtr = nullptr;
	iprm::CParamsSet* requestListParamsPtr = nullptr;
	iprm::CParamsSet* documentTypeListParamsPtr = nullptr;
	if (outputParamsSetPtr != nullptr){
		processedFilesPtr = dynamic_cast<iprm::IOptionsManager*>(outputParamsSetPtr->GetEditableParameter(s_processedFilesParamId));
		if (processedFilesPtr == nullptr){
			processedFilesPtr = new iprm::COptionsManager;
			outputParamsSetPtr->SetEditableParameter(s_processedFilesParamId, processedFilesPtr, true);
		}

		typeListParamsPtr = dynamic_cast<iprm::CParamsSet*>(outputParamsSetPtr->GetEditableParameter(s_typeListParamId));
		if (typeListParamsPtr == nullptr){
			typeListParamsPtr = new iprm::CParamsSet;
			outputParamsSetPtr->SetEditableParameter(s_typeListParamId, typeListParamsPtr, true);
		}

		requestListParamsPtr = dynamic_cast<iprm::CParamsSet*>(outputParamsSetPtr->GetEditableParameter(s_requestListParamId));
		if (requestListParamsPtr == nullptr){
			requestListParamsPtr = new iprm::CParamsSet;
			outputParamsSetPtr->SetEditableParameter(s_requestListParamId, requestListParamsPtr, true);
		}

		documentTypeListParamsPtr = dynamic_cast<iprm::CParamsSet*>(outputParamsSetPtr->GetEditableParameter(s_documentTypeListParamId));
		if (documentTypeListParamsPtr == nullptr){
			documentTypeListParamsPtr = new iprm::CParamsSet;
			outputParamsSetPtr->SetEditableParameter(s_documentTypeListParamId, documentTypeListParamsPtr, true);
		}
	}

	// first ensure, this file is not processed
	if (processedFilesPtr != nullptr){
		int optionsCount = processedFilesPtr->GetOptionsCount();
		for (int i = 0; i < optionsCount; ++i){
			if (m_currentSchemaFilePath == processedFilesPtr->GetOptionName(i)){
				SendWarningMessage(0, QString("File '%1' already processed. Skipping..."));

				return TS_OK;
			}
		}
	}

	m_processedFilesPtr = processedFilesPtr;

	Q_ASSERT(!m_currentInputFilePtr.IsValid());
	Q_ASSERT(!m_tempDirPtr.IsValid());

	m_tempDirPtr.SetPtr(new imtbase::CTempDir);

	// first remove remarks and save file in temp dir
	QFile inputFile(m_currentSchemaFilePath);

	if (!inputFile.open(QIODevice::ReadOnly)){
		SendErrorMessage(0, QString("Unable to open file '%1'").arg(inputFile.fileName()));

		return TS_INVALID;
	}

	if (processedFilesPtr != nullptr){
		processedFilesPtr->InsertOption(QFileInfo(inputFile).canonicalFilePath(), QByteArray::number(processedFilesPtr->GetOptionsCount()));
	}

	m_includePathList = m_argumentParserCompPtr->GetIncludePaths();

	// permanently add current scheme path to scan imports
	m_includePathList << QFileInfo(inputFile).absoluteDir().absolutePath();

	QTextStream inputStream(&inputFile);
	QString content = inputStream.readAll();

	static QRegularExpression multiLineCommentRegex("#\\[\\[.*\\]\\]", QRegularExpression::MultilineOption | QRegularExpression::DotMatchesEverythingOption);
	content.replace(multiLineCommentRegex, "");

	static QRegularExpression commentRegex("#.*\n");
	content.replace(commentRegex, "\n");

	QString outputFileName = m_tempDirPtr->Path() + QDir::separator() + "schema.sdl";
	QFile outputFile(outputFileName);

	if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Text)){
		SendErrorMessage(0, QString("Cannot open output file '%1'").arg(outputFileName));

		return TS_INVALID;
	}

	QTextStream outputStream(&outputFile);
	outputStream << content;
	outputFile.close();

	// and parse file with removed remarks
	m_currentInputFilePtr.SetPtr(new QFile(outputFileName));
	if (!m_currentInputFilePtr->open(QIODevice::ReadOnly | QIODevice::Text)){
		SendErrorMessage(0, QString("Unable to open file '%1'").arg(m_currentInputFilePtr->fileName()));

		return TS_INVALID;
	}

	BaseClass::SetDevice(*m_currentInputFilePtr);

	const bool isSchemaParsed = ParseGqlSchema();

	if (!isSchemaParsed){
		return TS_INVALID;
	}

	if (m_schemaNamespaceCompPtr.IsValid()){
		const QString curretNamespace = GetNamespaceFromParamsOrArguments(m_schemaParamsPtr.get(), m_argumentParserCompPtr);
		m_schemaNamespaceCompPtr->SetText(curretNamespace);
	}

	// add output params values
	if (typeListParamsPtr != nullptr){
		for (const CSdlType& sdlType: std::as_const(m_sdlTypes)){
			CSdlType* sdlTypePtr = new CSdlType(sdlType);
			typeListParamsPtr->SetEditableParameter(QByteArray::number(typeListParamsPtr->GetParamIds().size()), sdlTypePtr, true);
			SendVerboseMessage(QString("'%1': Add SDL for import. Name:'%2' [%3]").arg(m_currentSchemaFilePath, sdlType.GetName(), QByteArray::number(typeListParamsPtr->GetParamIds().size() - 1)));
		}
	}

	if (requestListParamsPtr != nullptr){
		for (const CSdlRequest& sdlRequest: std::as_const(m_requests)){
			CSdlRequest* sdlRequestPtr = new CSdlRequest(sdlRequest);
			requestListParamsPtr->SetEditableParameter(QByteArray::number(requestListParamsPtr->GetParamIds().size()), sdlRequestPtr, true);
		}
	}

	if (documentTypeListParamsPtr != nullptr){
		for (const CSdlDocumentType& sdlDocumentType: std::as_const(m_documentTypes)){
			CSdlDocumentType* sdlDocumentTypePtr = new CSdlDocumentType(sdlDocumentType);
			documentTypeListParamsPtr->SetEditableParameter(QByteArray::number(documentTypeListParamsPtr->GetParamIds().size()), sdlDocumentTypePtr, true);
		}
	}

	return TS_OK;
}


// protected methods

QStringList CGqlSchemaParserComp::GetPathsFromImportEntry(QString importDirective, const QStringList& searchPathList) const
{
	QStringList foundFiles;
	// get version from directive
	QString version;
	if (importDirective[importDirective.length() - 1].isDigit()){
		for (auto importDirectiveSymbol = importDirective.crbegin(); importDirectiveSymbol != importDirective.crend(); ++importDirectiveSymbol){
			auto nextSymbol = importDirectiveSymbol + 1;
			if (nextSymbol == importDirective.crend()){
				SendErrorMessage(0, QString("Unexpected import directive '%1'").arg(importDirective));
			}
			if (!importDirectiveSymbol->isDigit() && *importDirectiveSymbol != '.'){
				// we are finished version parsing
				break;
			}
			version.prepend(*importDirectiveSymbol);
		}
	}
	qsizetype versionLength = version.length();
	if(version.startsWith('.')){
		version.remove(0, 1);
	}
	importDirective.resize(importDirective.length() - versionLength);

	// check if entry is file
	QStringList pathParts = importDirective.split('.');

	if (!version.isEmpty()){
		pathParts.insert(pathParts.size() - 1, version);
	}

	QString fileEntryPath = pathParts.join('/');
	fileEntryPath.append(QStringLiteral(".sdl"));
	QString foundFilePath = FindFileInList(fileEntryPath, searchPathList);
	if (!foundFilePath.isEmpty()){
		return QStringList({foundFilePath});
	}

	// maybe it is a directory, so rebuild search path
	pathParts = importDirective.split('.');
	if (!version.isEmpty()){
		pathParts << version;
	}
	fileEntryPath = pathParts.join('/');
	foundFiles = FindFilesFromDir(fileEntryPath, searchPathList);

	return foundFiles;
}


QString CGqlSchemaParserComp::FindFileInList(const QString& relativePath, const QStringList& searchPathList) const
{
	for (const QString& searchPath: searchPathList){
		QDir baseDir(searchPath);
		QFileInfo foundFile(baseDir.absoluteFilePath(relativePath));
		if (foundFile.exists() && foundFile.isFile()){
			return foundFile.canonicalFilePath();
		}
	}

	return QString();
}


QStringList CGqlSchemaParserComp::FindFilesFromDir(const QString& relativeDirPath, const QStringList& searchPathList) const
{
	for (const QString& searchPath: searchPathList){
		const QDir baseDir(searchPath);
		const QFileInfo foundDir(baseDir.absoluteFilePath(relativeDirPath));
		if (foundDir.exists() && foundDir.isDir()){
			const QDir moduleDir(foundDir.canonicalFilePath());
			const QFileInfoList infoList = moduleDir.entryInfoList(QStringList({QStringLiteral("*.sdl")}), QDir::Files | QDir::Readable);
			QStringList retVal;
			for (const QFileInfo& fileInfo: infoList){
				retVal << fileInfo.canonicalFilePath();
			}

			return retVal;
		}
	}

	return QStringList();
}


bool CGqlSchemaParserComp::ExtractTypesFromImport(const QStringList& importFilesList)
{
	// process found files
	for (const QString& schemaPath: std::as_const(importFilesList)){
		istd::TDelPtr<iproc::IProcessor> newSchemaProcessor(m_fileSchemaParserCompFactPtr.CreateInstance());
		ifile::CFileNameParam schemaFilePathParam;
		schemaFilePathParam.SetPath(schemaPath);
		iprm::CParamsSet outputParams;
		outputParams.SetEditableParameter(s_processedFilesParamId, m_processedFilesPtr);

		int processingResult = newSchemaProcessor->DoProcessing(nullptr, &schemaFilePathParam, &outputParams);
		if (processingResult != TS_OK){
			SendErrorMessage(0, QString("Unable to process file '%1'").arg(schemaPath));

			return false;
		}

		iprm::IParamsSet* typeListParamsPtr = dynamic_cast<iprm::IParamsSet*>(outputParams.GetEditableParameter(s_typeListParamId));
		if (typeListParamsPtr != nullptr){
			QByteArrayList paramIdList = typeListParamsPtr->GetParamIds().values();

			// sort IDs. We MUST preserve order (by id as int)
			std::sort(paramIdList.begin(), paramIdList.end(), [](const QByteArray& firstString, const QByteArray& secondString){
				int firstDigit = 0;
				int secondDigit = 0;

				bool isFirstDigit = false;
				firstDigit = firstString.toInt(&isFirstDigit);

				if (isFirstDigit){
					bool isSecondDigit = false;
					secondDigit = secondString.toInt(&isSecondDigit);
					if (isSecondDigit){
						return firstDigit < secondDigit;
					}
				}

				return firstString < secondString;
			});

			for (const QByteArray& paramId: paramIdList){
				iprm::TParamsPtr<CSdlType> sdlTypeParam(typeListParamsPtr, paramId, true);
				if (!sdlTypeParam.IsValid()){
					SendCriticalMessage(0, "Import processing failed");

					return false;
				}

				CSdlType copiedType(*sdlTypeParam);
				// maybe already imported from another file
				if (!m_sdlTypes.contains(copiedType)){
					SendVerboseMessage(QString("'%1': Add SDL from import '%2'. Name:'%3' [%4]").arg(m_currentSchemaFilePath, schemaPath, copiedType.GetName(), paramId));
					m_sdlTypes << copiedType;
				}
			}
		}

		iprm::IParamsSet* requestListParamsPtr = dynamic_cast<iprm::IParamsSet*>(outputParams.GetEditableParameter(s_requestListParamId));
		if (requestListParamsPtr != nullptr){
			const iprm::IParamsSet::Ids paramIdList = requestListParamsPtr->GetParamIds();
			for (const QByteArray& paramId: paramIdList){
				iprm::TParamsPtr<CSdlRequest> sdlRequestParam(requestListParamsPtr, paramId, true);
				if (!sdlRequestParam.IsValid()){
					SendCriticalMessage(0, "Import processing failed");

					return false;
				}

				// look for duplicates
				const QString sdlRequestName = sdlRequestParam->GetName();
				for (const CSdlRequest& sdlRequest: std::as_const(m_requests)){
					if (sdlRequest.GetName() == sdlRequestName){
						SendErrorMessage(0, QString("Redifinition of '%1' in '%2'").arg(sdlRequestName, schemaPath));

						return false;
					}
				}

				CSdlRequest copiedRequest(*sdlRequestParam);
				m_requests << copiedRequest;
			}
		}

		iprm::IParamsSet* documentTypeListParamsPtr = dynamic_cast<iprm::IParamsSet*>(outputParams.GetEditableParameter(s_documentTypeListParamId));
		if (documentTypeListParamsPtr != nullptr){
			const iprm::IParamsSet::Ids paramIdList = documentTypeListParamsPtr->GetParamIds();
			for (const QByteArray& paramId: paramIdList){
				iprm::TParamsPtr<CSdlDocumentType> sdlDocumentTypeParam(documentTypeListParamsPtr, paramId, true);
				if (!sdlDocumentTypeParam.IsValid()){
					SendCriticalMessage(0, "Import processing failed");

					return false;
				}

				// look for duplicates
				const QString sdlDocumentName = sdlDocumentTypeParam->GetName();
				for (const CSdlDocumentType& sdlDocumentType: std::as_const(m_documentTypes)){
					if (sdlDocumentType.GetName() == sdlDocumentName){
						SendErrorMessage(0, QString("Redifinition of '%1' in '%2'").arg(sdlDocumentName, schemaPath));

						return false;
					}
				}

				CSdlDocumentType copiedType(*sdlDocumentTypeParam);
				m_documentTypes << copiedType;
			}
		}
	}

	return true;
}


bool CGqlSchemaParserComp::SetupSchemaFilePath(const istd::IPolymorphic* inputPtr)
{
	m_currentSchemaFilePath.clear();
	// First, check the input parameters these are of higher priority
	const ifile::IFileNameParam* schemaFilePathParamPtr = dynamic_cast<const ifile::IFileNameParam*>(inputPtr);
	if (schemaFilePathParamPtr != nullptr){
		m_currentSchemaFilePath = schemaFilePathParamPtr->GetPath();
	}

	if (!m_currentSchemaFilePath.isEmpty()){
		BaseClass::m_originalSchemaFile = m_currentSchemaFilePath;

		return true;
	}

	// Then, check an argument parser
	if (m_argumentParserCompPtr.IsValid()){
		m_currentSchemaFilePath = m_argumentParserCompPtr->GetSchemaFilePath();
		BaseClass::m_originalSchemaFile = m_currentSchemaFilePath;
	}

	if (m_currentSchemaFilePath.isEmpty()){
		SendErrorMessage(0, QString("Unable to setup schema path, because it is empty"));
		return false;
	}

	return true;

}


bool CGqlSchemaParserComp::ProcessFilesImports()
{
	QStringList schemaImportList;

	char foundDelimiter = ' ';
	bool retVal = true;
	do {
		QByteArray importedFilePath;
		retVal = retVal && ReadToDelimeter(QByteArrayLiteral("'\"}"), importedFilePath, &foundDelimiter);
		// we reached end of imports
		if (foundDelimiter == '}'){
			break;
		}

		retVal = retVal && ReadToDelimeter(QByteArray(&foundDelimiter, 1) + '}', importedFilePath);
		if (foundDelimiter == '}'){
			SendErrorMessage(0, QString("Syntax error. In '%1' Expected ' or \" at %3").arg(m_currentSchemaFilePath, QString::number(m_lastReadLine + 1)));

			return false;
		}
		schemaImportList << importedFilePath;

	} while (retVal);

	schemaImportList.removeDuplicates();

	// resolve paths
	QStringList foundSchemaFiles;
	for (const QString& includeDirectoryPath: std::as_const(m_includePathList)){
		QDir includeDirectory(includeDirectoryPath);
		QMutableListIterator importsIterator(schemaImportList);
		while (importsIterator.hasNext())
		{
			const QString importFileName = importsIterator.next();
			const QString absolutePath = includeDirectory.absoluteFilePath(importFileName);
			if (QFile::exists(absolutePath)){
				foundSchemaFiles << QFileInfo(absolutePath).canonicalFilePath();
				importsIterator.remove();
			}
		}
	}
	if (!schemaImportList.isEmpty()){
		SendErrorMessage(0, QString("Unable to find import files in '%1' : %2").arg(m_currentSchemaFilePath, schemaImportList.join("; ")));

		return false;
	}

	return ExtractTypesFromImport(foundSchemaFiles);
}


bool CGqlSchemaParserComp::ProcessJavaStyleImports()
{
	bool retVal = true;
	QByteArray importedFilesSectionData;
	retVal = retVal && MoveToNextReadableSymbol();
	if (m_lastReadChar == '{'){
		retVal = retVal && MoveToNextReadableSymbol();
	}
	retVal = retVal && ReadToDelimeter(QByteArrayLiteral("}"), importedFilesSectionData);

	if (!retVal){
		SendErrorMessage(0, QString("Unable to process schema's imports. File: '%1'").arg(m_currentSchemaFilePath));

		return false;
	}

	QStringList importedFiles;
	QStringList importDirectiveList = QString(importedFilesSectionData).split('\n');
	for (const QString& importDirective: importDirectiveList){
		importedFiles << GetPathsFromImportEntry(importDirective, m_includePathList);
	}

	return ExtractTypesFromImport(importedFiles);
}


// reimplemented (CGqlExtSchemaParser)

bool CGqlSchemaParserComp::ProcessSchemaImports()
{
	if (!m_fileSchemaParserCompFactPtr.IsValid()){
		SendCriticalMessage(0, "Import is detected, but file schema parser component was not configured. 'FileSchemaParserFactory' was not set");

		return BaseClass::ProcessSchemaImports();
	}

	if (*m_useFilesImportAttrPtr){
		return ProcessFilesImports();
	}

	return ProcessJavaStyleImports();
}

bool CGqlSchemaParserComp::ValidateSchema()
{
	if (!BaseClass::ValidateSchema()){
		return false;
	}

	ISdlProcessArgumentsParser::AutoLinkLevel autoLinkLevel = m_argumentParserCompPtr->GetAutoLinkLevel();

	if (!m_argumentParserCompPtr->IsSchemaDependencyModeEnabled() && !m_argumentParserCompPtr->IsDependenciesMode()){
		for (CSdlType& sdlType: m_sdlTypes){
			bool isExternal = sdlType.IsExternal();

			// add namespace prefix to all types
			const QString namespacePrefix = m_argumentParserCompPtr->GetNamespacePrefix() + QStringLiteral("::");
			if (!namespacePrefix.isEmpty()){
				const QString originalTypeNamespace = sdlType.GetNamespace();
				if (!originalTypeNamespace.isEmpty() && !originalTypeNamespace.startsWith(namespacePrefix)){
					sdlType.SetNamespace(namespacePrefix + originalTypeNamespace);
				}
			}

			if (isExternal){
				// if external, that mean, it is already processed
				continue;
			}

			if (autoLinkLevel == ISdlProcessArgumentsParser::ALL_ONLY_FILE){
				if (sdlType.GetTargetHeaderFile().isEmpty()){
					const QMap<QString, QString> targetPathList = CalculateTargetCppFilesFromSchemaParams(*m_schemaParamsPtr, m_argumentParserCompPtr->GetOutputDirectoryPath(), QFileInfo(m_currentSchemaFilePath).fileName());
					const QString headerFilePath = QDir::cleanPath(targetPathList[ISdlProcessArgumentsParser::s_headerFileType]);
					sdlType.SetTargetHeaderFile(headerFilePath);
				}

				isExternal =  bool(QDir::cleanPath(m_currentSchemaFilePath) != QDir::cleanPath(m_argumentParserCompPtr->GetSchemaFilePath()));
			}
			else if (autoLinkLevel == ISdlProcessArgumentsParser::ALL_SAME_NAMESPACE){
				const QString typeNamespace = sdlType.GetNamespace();
				if (!typeNamespace.isEmpty()){
					const QString currentNamespace = namespacePrefix + BuildNamespaceFromParams(*m_schemaParamsPtr);
					isExternal = bool(currentNamespace != typeNamespace);
				}
			}

			sdlType.SetExternal(isExternal);
			if (!isExternal){
				if (autoLinkLevel == ISdlProcessArgumentsParser::ALL_NONE){
					// set same namespace for all types if generate all schemas in single file
					/// \todo change it to \c BuildNamespaceFromParams afterwards
					const QString typeNamespace = GetNamespaceFromParamsOrArguments(m_schemaParamsPtr.get(), m_argumentParserCompPtr);
					sdlType.SetNamespace(typeNamespace);
				}

				const bool isSet = UpdateTypeInfo(sdlType, m_schemaParamsPtr.get(), m_argumentParserCompPtr.GetPtr());
				if (!isSet){
					SendErrorMessage(0, QString("Unable to set output file for type: '%1' in '%2'").arg(sdlType.GetName(), m_currentSchemaFilePath));

					return false;
				}
			}
		}
	}

	QList<CSdlType> uniqueTypes;
	QMutableListIterator typesIter(m_sdlTypes);
	while(typesIter.hasNext()){
		const CSdlType& sdlType = typesIter.next();
		if (uniqueTypes.contains(sdlType)) {
			typesIter.remove();

			continue;
		}

		uniqueTypes << sdlType;
	}

	return true;
}


} // namespace imtsdl


