#include "CGqlSchemaParserComp.h"


// Qt includes
#include <QtCore/QRegularExpression>

// Acf includes
#include <iprm/CParamsSet.h>
#include <iprm/COptionsManager.h>
#include <iprm/TParamsPtr.h>
#include <ifile/CFileNameParam.h>


namespace imtsdl
{


// public static variables

const QByteArray CGqlSchemaParserComp::s_processedFilesParamId		= QByteArrayLiteral("ProcessedFiles");
const QByteArray CGqlSchemaParserComp::s_typeListParamId			= QByteArrayLiteral("TypeList");
const QByteArray CGqlSchemaParserComp::s_requestListParamId			= QByteArrayLiteral("RequestList");
const QByteArray CGqlSchemaParserComp::s_documentTypeListParamId	= QByteArrayLiteral("DocumentTypeList");


// public methods

int CGqlSchemaParserComp::DoProcessing(
			const iprm::IParamsSet* /*paramsPtr*/,
			const istd::IPolymorphic* inputPtr,
			istd::IChangeable* outputPtr,
			ibase::IProgressManager* /*progressManagerPtr*/)
{
	m_currentSchemaFilePath.clear();
	const ifile::IFileNameParam* schemaFilePathParamPtr = dynamic_cast<const ifile::IFileNameParam*>(inputPtr);
	if (schemaFilePathParamPtr == nullptr && !m_argumentParserCompPtr.IsValid()){
		SendCriticalMessage(0, "Schema path param is not provided");
		I_CRITICAL();

		return TS_INVALID;
	}

	if (schemaFilePathParamPtr != nullptr){
		m_currentSchemaFilePath = schemaFilePathParamPtr->GetPath();
	}
	else {
		m_currentSchemaFilePath = m_argumentParserCompPtr->GetSchemaFilePath();
	}
	if (m_currentSchemaFilePath.isEmpty()){
		SendCriticalMessage(0, "Schema path is not provided");
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
		I_CRITICAL();

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
		qDebug() << "Error: Cannot open output file";
		return -1;
	}

	QTextStream outputStream(&outputFile);
	outputStream << content;
	outputFile.close();

	// and parse file with removed remarks
	m_currentInputFilePtr.SetPtr(new QFile(outputFileName));
	if (!m_currentInputFilePtr->open(QIODevice::ReadOnly)){
		SendErrorMessage(0, QString("Unable to open file '%1'").arg(m_currentInputFilePtr->fileName()));
		I_CRITICAL();

		return TS_INVALID;
	}

	BaseClass::SetDevice(*m_currentInputFilePtr);

	const bool isSchemaParsed = ParseGqlSchema();

	if (!isSchemaParsed){
		return TS_INVALID;
	}

	// add output params values
	if (typeListParamsPtr != nullptr){
		for (const CSdlType& sdlType: std::as_const(m_sdlTypes)){
			CSdlType* sdlTypePtr = new CSdlType(sdlType);
			typeListParamsPtr->SetEditableParameter(QByteArray::number(typeListParamsPtr->GetParamIds().size()), sdlTypePtr, true);
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

bool CGqlSchemaParserComp::ProcessSchemaImports()
{
	if (!m_fileSchemaParserCompFactPtr.IsValid()){
		SendErrorMessage(0, "import is detected, but parser is not configured. (FileSchemaParserFactory) is not set");
		I_CRITICAL();

		return BaseClass::ProcessSchemaImports();
	}

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

	// process found files
	for (const QString& schemaPath: std::as_const(foundSchemaFiles)){
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
			const iprm::IParamsSet::Ids paramIdList = typeListParamsPtr->GetParamIds();
			for (const QByteArray& paramId: paramIdList){
				iprm::TParamsPtr<CSdlType> sdlTypeParam(typeListParamsPtr, paramId, true);
				if (!sdlTypeParam.IsValid()){
					SendCriticalMessage(0, "Import processing failed.");

					return false;
				}

				// look for duplicates
				const QString sdlTypeName = sdlTypeParam->GetName();
				for (const CSdlType& sdlType: std::as_const(m_sdlTypes)){
					if (sdlType.GetName() == sdlTypeName){
						SendErrorMessage(0, QString("Redifinition of '%1' in '%2'").arg(sdlTypeName, schemaPath));

						return false;
					}
				}

				CSdlType copiedType(*sdlTypeParam);
				m_sdlTypes << copiedType;
			}
		}

		iprm::IParamsSet* requestListParamsPtr = dynamic_cast<iprm::IParamsSet*>(outputParams.GetEditableParameter(s_requestListParamId));
		if (requestListParamsPtr != nullptr){
			const iprm::IParamsSet::Ids paramIdList = requestListParamsPtr->GetParamIds();
			for (const QByteArray& paramId: paramIdList){
				iprm::TParamsPtr<CSdlRequest> sdlRequestParam(requestListParamsPtr, paramId, true);
				if (!sdlRequestParam.IsValid()){
					SendCriticalMessage(0, "Import processing failed.");

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
			const iprm::IParamsSet::Ids paramIdList = requestListParamsPtr->GetParamIds();
			for (const QByteArray& paramId: paramIdList){
				iprm::TParamsPtr<CSdlDocumentType> sdlDocumentTypeParam(requestListParamsPtr, paramId, true);
				if (!sdlDocumentTypeParam.IsValid()){
					SendCriticalMessage(0, "Import processing failed.");

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


} // namespace imtsdl

