#include "CSdlGeneralManagerComp.h"


// Qt includes
#include <QtCore/QLockFile>


// ACF includes
#include <istd/CSystem.h>
#include <iprm/TParamsPtr.h>
#include <ifile/IFileNameParam.h>


namespace imtsdl
{


void CSdlGeneralManagerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	Q_ASSERT(m_sdlParserCompPtr.IsValid());
	Q_ASSERT(m_sdlArgumentParserCompPtr.IsValid());

	const QString outputDirPath = m_sdlArgumentParserCompPtr->GetOutputDirectoryPath();
	const bool isOutputDirExsists = istd::CSystem::EnsurePathExists(outputDirPath);
	if (!isOutputDirExsists){
		SendErrorMessage(0, QString("Unable to create output directory '%1'").arg(outputDirPath));

		::exit(-1);
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

		::exit(-1);
	}

	// create code
	const int sdlProcessorsCount = m_sdlProcessorsCompListPtr.GetCount();
	for (int i = 0; i < sdlProcessorsCount; ++i){
		iproc::IProcessor* codeGeneratorPtr = m_sdlProcessorsCompListPtr[i];
		Q_ASSERT(codeGeneratorPtr != nullptr);

		if (codeGeneratorPtr != nullptr){
			int processResultResult = codeGeneratorPtr->DoProcessing(nullptr, nullptr, nullptr);

			if (processResultResult != iproc::IProcessor::TS_OK){
				SendErrorMessage(0, "Unable to process");

				::exit(-1);
			}
		}
	}

	lockFile.unlock();

	::exit(0);
}



} // namespace imtsdl

