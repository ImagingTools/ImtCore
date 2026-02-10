// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CQmlProcessorsManagerComp.h"


// Qt includes
#include <QtCore/QElapsedTimer>
#include <QtCore/QFileInfo>
#include <QtCore/QLockFile>
#include <QtCore/QThread>

// ACF includes
#include <istd/CSystem.h>
#include <iprm/TParamsPtr.h>
#include <iprm/IOptionsManager.h>
#include <iprm/CParamsSet.h>
#include <iproc/IProcessor.h>

// ImtCore includes
#include<imtsdl/CSdlTools.h>


namespace imtsdlgenqml
{


iproc::IProcessor::TaskState CQmlProcessorsManagerComp::DoProcessing(
	const iprm::IParamsSet* /*paramsPtr*/,
	const istd::IPolymorphic* /*inputPtr*/,
	istd::IChangeable* /*outputPtr*/,
	ibase::IProgressManager* /*progressManagerPtr*/)
{
	QElapsedTimer timer;
	timer.start();

	const QString outputDirPath =
		imtsdl::CSdlTools::GetCompleteOutputPath(m_sdlSchemaParamsCompPtr, *m_sdlArgumentParserCompPtr, false, m_sdlArgumentParserCompPtr->IsCppEnabled());
	const bool isOutputDirExsists =
		istd::CSystem::EnsurePathExists(outputDirPath);
	if (!isOutputDirExsists){
		SendErrorMessage(
			0,
			QString("Unable to create output directory '%1'").arg(outputDirPath));

		return TS_INVALID;
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
		return TS_INVALID;
	}


	if (IsVerboseEnabled()){
		SendInfoMessage(0, QString::number(qApp->applicationPid()) + ":SDL:processing finished [creation] " + QString::number(timer.elapsed()));
	}

	return TS_OK;
}


bool CQmlProcessorsManagerComp::CreateCode()
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



} // namespace imtsdlgenqml

