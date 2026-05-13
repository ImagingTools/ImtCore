// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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

// ImtCore includes
#include<imtsdl/CSdlTools.h>



namespace imtsdl
{

void CCodeGeneratorExecutorComp:: OnComponentCreated()
{
	if (qApp == nullptr){
		SendCriticalMessage(0, "Unable to start processing, when QCoreApplication is not initialized");
		I_CRITICAL();

		::exit(-1);
	}

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

			qApp->exit(1);

			return;
		}
		iprm::TParamsPtr<iprm::IOptionsManager> processedFilesPtr(
			&outputParams, QByteArrayLiteral("ProcessedFiles"), true);
		if (!processedFilesPtr.IsValid()){
			SendCriticalMessage(
				0, QString("Unexpected dependencies list for schema: '%1'")
					.arg(m_sdlArgumentParserCompPtr->GetSchemaFilePath()));

			qApp->exit(2);

			return;
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

		qApp->exit(0);

		return;
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

		qApp->exit(3);

		return;
	}


	iprm::CParamsSet params;
	// PrepareParams(params) -> void;

	if (m_cxxProcessorCompPtr.IsValid() && m_sdlArgumentParserCompPtr->IsCppEnabled()){
		iproc::IProcessor::TaskState execResult = m_cxxProcessorCompPtr->DoProcessing(&params, nullptr, nullptr, nullptr);
		if (execResult != iproc::IProcessor::TS_OK){
			qApp->exit(4);

			return;
		}
	}

	if (m_qmlProcessorCompPtr.IsValid() && m_sdlArgumentParserCompPtr->IsQmlEnabled()){
		iproc::IProcessor::TaskState execResult = m_qmlProcessorCompPtr->DoProcessing(&params, nullptr, nullptr, nullptr);
		if (execResult != iproc::IProcessor::TS_OK){
			qApp->exit(5);

			return;
		}
	}

	qApp->exit(0);
}




} // namespace imtsdl

