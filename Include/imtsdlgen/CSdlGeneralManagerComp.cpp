#include "CSdlGeneralManagerComp.h"


// std includes
#include <iostream>

// Qt includes
#include <QtCore/QFileInfo>
#include <QtCore/QLockFile>
#include <QtCore/QThread>

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
		int collectionResult =
			m_sdlSchemaDependenciesCollectorCompPtr->DoProcessing(nullptr, nullptr, &outputParams);
		if (collectionResult != iproc::IProcessor::TS_OK){
			SendErrorMessage(
				0, QString("Unable to collect dependencies for schema: '%1'")
					.arg(m_sdlArgumentParserCompPtr->GetSchemaFilePath()));

			::exit(1);
		}
		iprm::TParamsPtr<iprm::IOptionsManager> processedFilesPtr(
			&outputParams, QByteArrayLiteral("ProcessedFiles"), true);
		if (!processedFilesPtr.IsValid()){
			SendCriticalMessage(
				0, QString("Unexpected dependencies list for schema: '%1'")
					.arg(m_sdlArgumentParserCompPtr->GetSchemaFilePath()));

			::exit(2);
		}

		QStringList cumulatedFiles;
		int optionsCount = processedFilesPtr->GetOptionsCount();
		for (int i = 0; i < optionsCount; ++i){
			cumulatedFiles << processedFilesPtr->GetOptionName(i);
		}
		cumulatedFiles.removeDuplicates();

		//remove input schema
		cumulatedFiles.removeAll(
			QFileInfo(m_sdlArgumentParserCompPtr->GetSchemaFilePath())
				.canonicalFilePath());

		imtsdl::CSdlTools::PrintFiles(
			std::cout, cumulatedFiles,
			m_sdlArgumentParserCompPtr->GetGeneratorType());

		::exit(0);
	}

	QElapsedTimer timer;
	timer.start();

	// first parse schema and get complete path to output
	iprm::CParamsSet outputParamsSet;
	int parsingResult =
		m_sdlParserCompPtr->DoProcessing(nullptr, nullptr, &outputParamsSet);
	if (parsingResult != iproc::IProcessor::TS_OK){
		SendErrorMessage(
			0, QString("Unable to parse schema '%1'")
				.arg(QFileInfo(m_sdlArgumentParserCompPtr->GetSchemaFilePath())
						 .absoluteFilePath()));

		::exit(3);
	}

	const QString outputDirPath =
		imtsdl::CSdlTools::GetCompleteOutputPath(m_sdlSchemaParamsCompPtr, *m_sdlArgumentParserCompPtr, false, m_sdlArgumentParserCompPtr->IsCppEnabled());
	const bool isOutputDirExsists =
		istd::CSystem::EnsurePathExists(outputDirPath);
	if (!isOutputDirExsists){
		SendErrorMessage(
			0,
			QString("Unable to create output directory '%1'").arg(outputDirPath));

		::exit(4);
	}

	QLockFile lockFile(outputDirPath + QStringLiteral("/SDL_GEN.lock"));
	if (m_sdlArgumentParserCompPtr->IsGenerateMode()){
		bool isLockRequired = true;
		while (isLockRequired){
			isLockRequired = !lockFile.tryLock(500);
			QThread::currentThread()->msleep(100);
			if (timer.elapsed() >= 30000){
				break;
			}
		}

		const bool isLocked = !isLockRequired || lockFile.lock();
		if (!isLocked){
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
			SendErrorMessage(
				0, QString(
					"Unable to lock file '%1'. Perhaps you don't have permissions")
					.arg(lockFile.fileName()));
#endif
		}
		if (IsVerboseEnabled()){
			SendInfoMessage(0, QString::number(qApp->applicationPid()) + ":SDL:processing [parse] " + m_sdlArgumentParserCompPtr->GetSchemaFilePath() + QString::number(timer.elapsed()));
		}
	}

	timer.restart();

	const bool isCodeCreated = CreateCode();
	lockFile.unlock();

	if (!isCodeCreated){
		::exit(5);
	}


	if (IsVerboseEnabled()){
		SendInfoMessage(0, QString::number(qApp->applicationPid()) + ":SDL:processing finished [creation] " + QString::number(timer.elapsed()));
	}

	::exit(0);
}


bool CSdlGeneralManagerComp::CreateCode()
{
	// create code
	const int sdlProcessorsCount = m_sdlProcessorsCompListPtr.GetCount();
	for (int i = 0; i < sdlProcessorsCount; ++i){
		iproc::IProcessor* codeGeneratorPtr = m_sdlProcessorsCompListPtr[i];
		Q_ASSERT(codeGeneratorPtr != nullptr);

		if (codeGeneratorPtr != nullptr){
			int processResultResult = codeGeneratorPtr->DoProcessing(nullptr, nullptr, nullptr);

			if (processResultResult != iproc::IProcessor::TS_OK){
				SendCriticalMessage(0, QString("Unable to process schema: '%1'").arg(m_sdlArgumentParserCompPtr->GetSchemaFilePath()));

				return false;
			}
		}
	}

	return true;
}



} // namespace imtsdlgen

