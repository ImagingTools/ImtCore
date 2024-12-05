#include "CCacheGeneratorComp.h"


// Qt includes
#include <QtCore/QFile>
#include <QtCore/QDir>

// Acf includes
#include <istd/CSystem.h>

// ImtCore includes
#include <imtsdl/CSdlType.h>


namespace imtsdlgenv2
{


int CCacheGeneratorComp::DoProcessing(
			const iprm::IParamsSet* paramsPtr,
			const istd::IPolymorphic* inputPtr,
			istd::IChangeable* outputPtr,
			ibase::IProgressManager* progressManagerPtr)
{
	Q_ASSERT(m_argumentParserCompPtr.IsValid());
	Q_ASSERT(m_sdlTypeListCompPtr.IsValid());
	Q_ASSERT(m_cacheMultiManagerCompPtr.IsValid());

	const QString cachePath = m_argumentParserCompPtr->GetCachePath();
	if (cachePath.isEmpty()){
		// nothing todo
		return TS_OK;
	}

	const QString outputDirectoryPath = QDir::cleanPath(m_argumentParserCompPtr->GetOutputDirectoryPath());
	if (outputDirectoryPath.isEmpty()){
		SendCriticalMessage(0, "Output path is not provided");
		I_CRITICAL();

		return TS_INVALID;
	}

	const QString defaultName = QFileInfo(m_argumentParserCompPtr->GetSchemaFilePath()).fileName();
	QMap<QString, QString> joinRules = m_argumentParserCompPtr->GetJoinRules();
	if (m_argumentParserCompPtr->IsAutoJoinEnabled()){
		if (!m_schemaParamsCompPtr.IsValid()){
			SendErrorMessage(0, "Application is not configured with schema parameters. Auto join is not possible. Please specify paths to join explicitly(use -J option), or disable join.");

			return TS_INVALID;
		}

		joinRules = CalculateTargetCppFilesFromSchemaParams(*m_schemaParamsCompPtr, m_argumentParserCompPtr->GetOutputDirectoryPath(), defaultName);
	}

	const bool joinHeaders = joinRules.contains(imtsdl::ISdlProcessArgumentsParser::s_headerFileType);
	const bool joinSources = joinRules.contains(imtsdl::ISdlProcessArgumentsParser::s_sourceFileType);

	imtsdl::SdlTypeList sdlTypeList = m_sdlTypeListCompPtr->GetSdlTypes(true);


	return TS_OK;
}


} // namespace imtsdlgenv2

