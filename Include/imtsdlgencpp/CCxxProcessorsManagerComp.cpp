#include "CCxxProcessorsManagerComp.h"


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



namespace imtsdlgencpp
{

iproc::IProcessor::TaskState CCxxProcessorsManagerComp::DoProcessing(
	const iprm::IParamsSet* paramsPtr,
	const istd::IPolymorphic* inputPtr,
	istd::IChangeable* outputPtr,
	ibase::IProgressManager* progressManagerPtr)
{
	// set a temp dir for all processors
	QTemporaryDir tempOutputDir;
	const QString tempOutputDirPath = tempOutputDir.path();
	if (!istd::CSystem::EnsurePathExists(tempOutputDirPath)){
		SendCriticalMessage(0, QString("Unable to create temp dir at '%1'").arg(tempOutputDirPath));

		return TS_INVALID;
	}

	iprm::CParamsSet processorParams;
	ifile::CFileNameParam tempDirParam;
	tempDirParam.SetPath(tempOutputDirPath);
	processorParams.SetEditableParameter(imtsdl::ProcessorParamKeys::TempDirPath, &tempDirParam);

	const int sdlProcessorsCount = m_sdlV2ProcessorsCompListPtr.GetCount();
	for (int i = 0; i < sdlProcessorsCount; ++i){
		iproc::IProcessor* codeGeneratorPtr = m_sdlV2ProcessorsCompListPtr[i];
		Q_ASSERT(codeGeneratorPtr != nullptr);

		if (codeGeneratorPtr != nullptr){
			int processResultResult = codeGeneratorPtr->DoProcessing(&processorParams, nullptr, nullptr);

			if (processResultResult != iproc::IProcessor::TS_OK){
				SendCriticalMessage(0, QString("Unable to process schema: '%1'").arg(m_sdlArgumentParserCompPtr->GetSchemaFilePath()));

				return TS_INVALID;
			}
		}
	}

	return TS_OK;
}




} // namespace imtsdlgencpp

