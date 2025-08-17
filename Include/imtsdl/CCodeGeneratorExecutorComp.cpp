#include "CCodeGeneratorExecutorComp.h"


// std includes
#include <iostream>

// Qt includes
#include <QtCore/QFileInfo>
#include <QtCore/QTemporaryDir>
#include <QtCore/QLockFile>

// ACF includes
#include <istd/CSystem.h>
#include <iprm/TParamsPtr.h>
#include <iprm/IOptionsManager.h>
#include <iprm/CParamsSet.h>
#include <ifile/CFileNameParam.h>

// ImtCore includes
#include<imtsdl/CSdlTools.h>



namespace imtsdl
{

void CCodeGeneratorExecutorComp:: OnComponentCreated()
{
	/// \todo reorganize it, use simple processor and call 'DoProcessing' from main.cpp
	BaseClass::OnComponentCreated();

	Q_ASSERT(m_gqlSchemaParser.IsValid());
	Q_ASSERT(m_qmlProcessorCompPtr.IsValid() || m_cxxProcessorCompPtr.IsValid());


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


	// first parse schema and get complete path to output
	iprm::CParamsSet outputParamsSet;
	int parsingResult =
		m_gqlSchemaParser->DoProcessing(nullptr, nullptr, &outputParamsSet);
	if (parsingResult != iproc::IProcessor::TS_OK){
		SendErrorMessage(
			0, QString("Unable to parse schema '%1'")
				.arg(QFileInfo(m_sdlArgumentParserCompPtr->GetSchemaFilePath())
						 .absoluteFilePath()));

		::exit(3);
	}

	if (m_cxxProcessorCompPtr.IsValid()){
		iproc::IProcessor::TaskState execResult = m_cxxProcessorCompPtr->DoProcessing(nullptr, nullptr, nullptr, nullptr);
		if (execResult != iproc::IProcessor::TS_OK){
			return;
		}
	}

	if (m_qmlProcessorCompPtr.IsValid()){
		iproc::IProcessor::TaskState execResult = m_qmlProcessorCompPtr->DoProcessing(nullptr, nullptr, nullptr, nullptr);
		if (execResult != iproc::IProcessor::TS_OK){
			return;
		}
	}
}




} // namespace imtsdl

