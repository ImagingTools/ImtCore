#include "CSdlGeneralManagerComp.h"


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



namespace imtsdlgenv2
{

bool CSdlGeneralManagerComp::CreateCode()
{
	// old code generation
	if (!m_sdlArgumentParserV2CompPtr.IsValid() || m_sdlArgumentParserV2CompPtr->GetGenerationVersion() < 2){
		return BaseClass::CreateCode();
	}

	// set a temp dir for all processors
	QTemporaryDir tempOutputDir;
	const QString tempOutputDirPath = tempOutputDir.path();
	if (!istd::CSystem::EnsurePathExists(tempOutputDirPath)){
		SendCriticalMessage(0, QString("Unable to create temp dir at '%1'").arg(tempOutputDirPath));

		return false;
	}

	iprm::CParamsSet processorParams;
	ifile::CFileNameParam tempDirParam;
	tempDirParam.SetPath(tempOutputDirPath);
	processorParams.SetEditableParameter(imtsdl::ProcessorParamKeys::TempDirPath, &tempDirParam);

	// create V2 code
	const int sdlProcessorsCount = m_sdlV2ProcessorsCompListPtr.GetCount();
	for (int i = 0; i < sdlProcessorsCount; ++i){
		iproc::IProcessor* codeGeneratorPtr = m_sdlV2ProcessorsCompListPtr[i];
		Q_ASSERT(codeGeneratorPtr != nullptr);

		if (codeGeneratorPtr != nullptr){
			int processResultResult = codeGeneratorPtr->DoProcessing(&processorParams, nullptr, nullptr);

			if (processResultResult != iproc::IProcessor::TS_OK){
				SendCriticalMessage(0, QString("Unable to process schema: '%1'").arg(m_sdlArgumentParserV2CompPtr->GetSchemaFilePath()));

				return false;
			}
		}
	}

	return true;
}




} // namespace imtsdlgenv2

