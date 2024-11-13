#include "CSdlGeneralManagerComp.h"


// std includes
#include <iostream>

// Qt includes
#include <QtCore/QFileInfo>
#include <QtCore/QLockFile>

// ACF includes
#include <istd/CSystem.h>
#include <iprm/TParamsPtr.h>
#include <iprm/IOptionsManager.h>
#include <iprm/CParamsSet.h>
#include <ifile/IFileNameParam.h>

// ImtCore includes
#include<imtsdl/CSdlTools.h>



namespace imtsdlgen
{


void CSdlGeneralManagerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	Q_ASSERT(m_sdlParserCompPtr.IsValid());
	Q_ASSERT(m_sdlArgumentParserCompPtr.IsValid());
	Q_ASSERT(m_sdlSchemaDependenciesCollectorCompPtr.IsValid());

	if (m_sdlArgumentParserCompPtr->IsSchemaDependencyModeEnabled()){
		iprm::CParamsSet outputParams;
		int collectionResult = m_sdlSchemaDependenciesCollectorCompPtr->DoProcessing(nullptr, nullptr, &outputParams);
		if (collectionResult != iproc::IProcessor::TS_OK){
			SendErrorMessage(0, QString("Unable to collect dependencies for schema: '%1'").arg(m_sdlArgumentParserCompPtr->GetSchemaFilePath()));

			::exit(1);
		}
		iprm::TParamsPtr<iprm::IOptionsManager> processedFilesPtr(&outputParams, QByteArrayLiteral("ProcessedFiles"), true);
		if (!processedFilesPtr.IsValid()){
			SendCriticalMessage(0, QString("Unexpected dependencies list for schema: '%1'").arg(m_sdlArgumentParserCompPtr->GetSchemaFilePath()));

			::exit(1);
		}

		QStringList cumulatedFiles;
		int optionsCount = processedFilesPtr->GetOptionsCount();
		for (int i = 0; i < optionsCount; ++i){
			cumulatedFiles << processedFilesPtr->GetOptionName(i);
		}
		cumulatedFiles.removeDuplicates();
		//remove input schema
		cumulatedFiles.removeAll(QFileInfo(m_sdlArgumentParserCompPtr->GetSchemaFilePath()).canonicalFilePath());

		imtsdl::CSdlTools::PrintFiles(std::cout, cumulatedFiles, m_sdlArgumentParserCompPtr->GetGeneratorType());

		::exit(0);
	}

	const QString outputDirPath = m_sdlArgumentParserCompPtr->GetOutputDirectoryPath();
	const bool isOutputDirExsists = istd::CSystem::EnsurePathExists(outputDirPath);
	if (!isOutputDirExsists){
		SendErrorMessage(0, QString("Unable to create output directory '%1'").arg(outputDirPath));

		::exit(2);
	}

	QLockFile lockFile(outputDirPath + QStringLiteral("/lock"));
	const bool isLocked = lockFile.lock();
	if (!isLocked){
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
		SendErrorMessage(0, QString("Unable to lock file '%1'. Perhaps you don't have permissions").arg(lockFile.fileName()));
#endif
	}

	// parse schema
	int parsingResult = m_sdlParserCompPtr->DoProcessing(nullptr, nullptr, nullptr);
	if (parsingResult != iproc::IProcessor::TS_OK){
		SendErrorMessage(0, "Unable to parse schema");

		::exit(3);
	}

	// create code
	const int sdlProcessorsCount = m_sdlProcessorsCompListPtr.GetCount();
	for (int i = 0; i < sdlProcessorsCount; ++i){
		iproc::IProcessor* codeGeneratorPtr = m_sdlProcessorsCompListPtr[i];
		Q_ASSERT(codeGeneratorPtr != nullptr);

		if (codeGeneratorPtr != nullptr){
			int processResultResult = codeGeneratorPtr->DoProcessing(nullptr, nullptr, nullptr);

			if (processResultResult != iproc::IProcessor::TS_OK){
				SendCriticalMessage(0, QString("Unable to process schema: '%1'").arg(m_sdlArgumentParserCompPtr->GetSchemaFilePath()));

				::exit(4);
			}
		}
	}

	lockFile.unlock();

	::exit(0);
}



} // namespace imtsdlgen

