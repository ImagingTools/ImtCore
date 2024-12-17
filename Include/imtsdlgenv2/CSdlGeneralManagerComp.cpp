#include "CSdlGeneralManagerComp.h"


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



namespace imtsdlgenv2
{

bool CSdlGeneralManagerComp::CreateCode()
{
	// old code generation
	if (!m_sdlArgumentParserV2CompPtr.IsValid() || m_sdlArgumentParserV2CompPtr->GetGenerationVersion() < 2){
		return BaseClass::CreateCode();
	}

	if (!m_sdlArgumentParserV2CompPtr->IsSchemaDependencyModeEnabled()){
		Q_ASSERT(m_cacheGeneratorPtr.IsValid());
		int cacheDenerationResult = m_cacheGeneratorPtr->DoProcessing(nullptr, nullptr, nullptr);
		if (cacheDenerationResult != iproc::IProcessor::TS_OK){
			SendErrorMessage(0, "Unable to generate cache");

			return false;
		}
	}

	// create V2 code
	const int sdlProcessorsCount = m_sdlV2ProcessorsCompListPtr.GetCount();
	for (int i = 0; i < sdlProcessorsCount; ++i){
		iproc::IProcessor* codeGeneratorPtr = m_sdlV2ProcessorsCompListPtr[i];
		Q_ASSERT(codeGeneratorPtr != nullptr);

		if (codeGeneratorPtr != nullptr){
			int processResultResult = codeGeneratorPtr->DoProcessing(nullptr, nullptr, nullptr);

			if (processResultResult != iproc::IProcessor::TS_OK){
				SendCriticalMessage(0, QString("Unable to process schema: '%1'").arg(m_sdlArgumentParserV2CompPtr->GetSchemaFilePath()));

				return false;
			}
		}
	}

	return true;
}




} // namespace imtsdlgenv2

