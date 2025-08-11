#include <imtsdl/CFileFinalizerComp.h>


// Qt includes
#include <QtCore/QString>
#include <QtCore/QFile>
#include <QtCore/QTemporaryFile>
#include <QtCore/QDir>
#include <QtCore/QLockFile>
#include <QtCore/QRegularExpression>
#include <QtCore/QRegularExpressionMatch>

// ACF includes
#include <istd/CSystem.h>
#include <iprm/COptionsManager.h>
#include <iprm/CParamsSet.h>
#include <ifile/CFileNameParam.h>

// ImtCore includes
#include <imtsdl/CSdlTools.h>
#include <imtsdl/CFileCleanupProcessorComp.h>


namespace imtsdl
{


// public methods

// reimplemented (iproc::IProcessor)

iproc::IProcessor::TaskState CFileFinalizerComp::DoProcessing(
			const iprm::IParamsSet* /*paramsPtr*/,
			const istd::IPolymorphic* /*inputPtr*/,
			istd::IChangeable* /*outputPtr*/,
			ibase::IProgressManager* /*progressManagerPtr*/)
{
	if (!m_argumentParserCompPtr.IsValid() ||
		!m_customSchemaParamsCompPtr.IsValid())
	{
		SendCriticalMessage(0, "FileFinalizer: Invalid configuration");
		I_CRITICAL();

		return TS_INVALID;
	}

	if (m_argumentParserCompPtr->IsDependenciesMode()){
		// nothing todo
		return TS_OK;
	}

	const QString defaultName = QFileInfo(m_argumentParserCompPtr->GetSchemaFilePath()).fileName();
	QMap<QString, QString> joinRules = m_argumentParserCompPtr->GetJoinRules();
	if (m_argumentParserCompPtr->IsAutoJoinEnabled()){
		joinRules = imtsdl::CSdlTools::CalculateTargetCppFilesFromSchemaParams(*m_customSchemaParamsCompPtr, *m_argumentParserCompPtr);
	}
	const QString joinedHeaderFilePath = joinRules[imtsdl::ISdlProcessArgumentsParser::s_headerFileType];
	const QString joinedSourceFilePath = joinRules[imtsdl::ISdlProcessArgumentsParser::s_sourceFileType];

	iproc::IProcessor::TaskState retVal = TS_OK;

	if (!joinedHeaderFilePath.isEmpty()){
		const QFileInfo joinedHeaderFileInfo(joinedHeaderFilePath);
		if (joinedHeaderFileInfo.exists() && joinedHeaderFileInfo.isFile()){
			retVal = CleanupFile(joinedHeaderFilePath);
			if (retVal != TS_OK){
				return retVal;
			}
		}
		else {
			SendWarningMessage(0, QStringLiteral("File '%1' does not exist. Cleanup skipped").arg(joinedHeaderFilePath));
		}
	}

	if (!joinedSourceFilePath.isEmpty()){
		const QFileInfo joinedSourceFileInfo(joinedSourceFilePath);
		if (joinedSourceFileInfo.exists() && joinedSourceFileInfo.isFile()){
			retVal = CleanupFile(joinedSourceFilePath);
			if (retVal != TS_OK){
				return retVal;
			}
		}
		else {
			SendWarningMessage(0, QStringLiteral("File '%1' does not exist. Cleanup skipped").arg(joinedSourceFilePath));
		}
	}

	return TS_OK;
}


iproc::IProcessor::TaskState CFileFinalizerComp::CleanupFile(const QString& filePath)
{
	if (!m_filesCleanupProcessorComp.IsValid() ||
		filePath.isEmpty())
	{
		return TS_OK;
	}

	iprm::CParamsSet params;

	ifile::CFileNameParam sourceFilePathParam;
	sourceFilePathParam.SetPath(filePath);
	params.SetEditableParameter(CFileCleanupProcessorComp::s_sourceFilePathParamId, &sourceFilePathParam);
	params.SetEditableParameter(CFileCleanupProcessorComp::s_targetFilePathParamId, &sourceFilePathParam);

	iprm::COptionsManager regExpList;
	regExpList.InsertOption(QStringLiteral("^\\s*\\#\\s*include\\s\\<.*\\>$"), "1");
	regExpList.InsertOption(QStringLiteral("^\\s*\\#\\s*include\\s\\\".*\\\"$"), "2");
	regExpList.InsertOption(QStringLiteral("^\\s*\\#\\s*pragma\\s+once$"), "3");
	regExpList.InsertOption(QStringLiteral("^.*includes\\s*$"), "4");

	iproc::IProcessor::TaskState result = m_filesCleanupProcessorComp->DoProcessing(&params, &regExpList, nullptr);

	return result;
}


} // namespace imtsdl


