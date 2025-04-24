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
	Q_ASSERT(m_sdlModuleManaerCompPtr.IsValid());
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

	QElapsedTimer timer;
	timer.start();
	QLockFile lockFile(outputDirPath + QStringLiteral("/lock"));

	/// \todo think about to remove lock
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
		SendErrorMessage(0, QString("Unable to lock file '%1'. Perhaps you don't have permissions").arg(lockFile.fileName()));
#endif
	}
	qDebug() << qApp->applicationPid() << "processing" << timer.elapsed();
	timer.restart();

	QStringList modulesPathList = m_sdlArgumentParserCompPtr->GetModuleIncludePaths();
	if (!modulesPathList.contains(m_sdlArgumentParserCompPtr->GetOutputDirectoryPath())){
		modulesPathList << m_sdlArgumentParserCompPtr->GetOutputDirectoryPath();
	}
	const bool isModulesInitialized = m_sdlModuleManaerCompPtr->Initialize(modulesPathList);
	if (!isModulesInitialized){
		SendErrorMessage(0, "Unable to initialize modules");

		::exit(3);
	}

	// parse schema
	iprm::CParamsSet outputParamsSet;
	int parsingResult = m_sdlParserCompPtr->DoProcessing(nullptr, nullptr, &outputParamsSet);
	if (parsingResult != iproc::IProcessor::TS_OK){
		SendErrorMessage(0, QString("Unable to parse schema '%1'").arg(QFileInfo(m_sdlArgumentParserCompPtr->GetSchemaFilePath()).absoluteFilePath()));

		::exit(4);
	}

	// create module
	if (m_sdlArgumentParserCompPtr->IsModileGenerateEnabled()){
		if (!m_sdlModuleManaerCompPtr->CreateModuleFile()){
			SendErrorMessage(0, QString("Unable to create module for schema '%1'").arg(m_sdlArgumentParserCompPtr->GetSchemaFilePath()));

			::exit(5);
		}
	}

	const bool isCodeCreated = CreateCode();
	lockFile.unlock();

	if (!isCodeCreated){
		::exit(6);
	}

	qDebug() << qApp->applicationPid() << "processing finished" << timer.elapsed();

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

