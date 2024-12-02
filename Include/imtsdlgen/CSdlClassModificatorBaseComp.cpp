#include "CSdlClassModificatorBaseComp.h"


// Qt includes
#include <QtCore/QDir>
#include <QtCore/QRegularExpression>
#include <QtCore/QRegularExpressionMatch>

//Acf includes
#include <istd/CSystem.h>

// imtbase includes
#include <imtbase/CTempDir.h>

// ImtCore includes
#include <imtsdl/CSdlType.h>


namespace imtsdlgen
{


// public methods

//reimplemented(iproc::IProcessor)

int CSdlClassModificatorBaseComp::DoProcessing(
			const iprm::IParamsSet* /*paramsPtr*/,
			const istd::IPolymorphic* /*inputPtr*/,
			istd::IChangeable* /*outputPtr*/,
			ibase::IProgressManager* /*progressManagerPtr*/)
{
	Q_ASSERT(m_argumentParserCompPtr.IsValid());
	Q_ASSERT(m_sdlTypeListCompPtr.IsValid());
	Q_ASSERT(m_sdlEnumListCompPtr.IsValid());

	iproc::IProcessor::TaskState retVal = iproc::IProcessor::TS_OK;
	if (!m_argumentParserCompPtr->IsModificatorEnabled(*m_processorModificatorNameAttrPtr)){
		// nothing todo if our modificator is not exsits
		return retVal;
	}

	const QString outputDirectoryPath = QDir::cleanPath(m_argumentParserCompPtr->GetOutputDirectoryPath());
	if (outputDirectoryPath.isEmpty()){
		SendCriticalMessage(0, "Output path is not provided");
		I_CRITICAL();

		return iproc::IProcessor::TS_INVALID;
	}

	if (!istd::CSystem::EnsurePathExists(outputDirectoryPath)){
		SendErrorMessage(0, QString("Unable to create path '%1'").arg(outputDirectoryPath));
		I_CRITICAL();

		return iproc::IProcessor::TS_INVALID;
	}

	imtbase::CTempDir tempDir;
	const QString tempPath = tempDir.Path();

	imtsdl::SdlTypeList sdlTypeList = m_sdlTypeListCompPtr->GetSdlTypes(true);
	for (const imtsdl::CSdlType& sdlType: sdlTypeList){
		/// \todo optimize it. Do not create a temp file for each processor. Do it in \c CSdlClassCodeGeneratorComp instead
		m_headerFilePtr.SetPtr(new QFile(tempPath + "/C" + sdlType.GetName() + ".h"));
		m_sourceFilePtr.SetPtr(new QFile(tempPath + "/C" + sdlType.GetName() + ".cpp"));
		m_originalHeaderFilePtr.SetPtr(new QFile(outputDirectoryPath + "/C" + sdlType.GetName() + ".h"));
		m_originalSourceFilePtr.SetPtr(new QFile(outputDirectoryPath + "/C" + sdlType.GetName() + ".cpp"));

		if (!istd::CSystem::FileCopy(m_originalHeaderFilePtr->fileName(), m_headerFilePtr->fileName(), true)){
			SendErrorMessage(0,
							 QString("C: %3. Unable to copy H file: '%1' to '%2'.")
								 .arg(m_originalHeaderFilePtr->fileName(), m_headerFilePtr->fileName(), m_logTag));

			return TS_INVALID;
		}
		if (!istd::CSystem::FileCopy(m_originalSourceFilePtr->fileName(), m_sourceFilePtr->fileName(), true)){
			SendErrorMessage(0,
							 QString("C: %3. Unable to copy C file: '%1' to '%2'.")
								 .arg(m_originalSourceFilePtr->fileName(), m_sourceFilePtr->fileName(), m_logTag));

			return TS_INVALID;
		}

		bool isOk = BeginClassFiles(sdlType);
		if (!isOk){
			AbortCurrentProcessing();

			return iproc::IProcessor::TS_INVALID;
		}

		isOk = isOk && EndClassFiles(sdlType);
		if (!isOk){
			AbortCurrentProcessing();

			return iproc::IProcessor::TS_INVALID;
		}
	}

	return retVal;
}


// protected methods

bool CSdlClassModificatorBaseComp::BeginClassFiles(const imtsdl::CSdlType& sdlType)
{
	// Create new files in temp path
	if (!m_headerFilePtr->open(QIODevice::WriteOnly | QIODevice::Append)){
		SendCriticalMessage(0,
					QString("Unable to open file: '%1'. Error: %2")
						.arg(m_headerFilePtr->fileName(), m_headerFilePtr->errorString()));

		AbortCurrentProcessing();

		return false;
	}

	if (!m_sourceFilePtr->open(QIODevice::WriteOnly | QIODevice::Append)){
		SendCriticalMessage(0,
						QString("Unable to open file: '%1'. Error: %2")
							.arg(m_sourceFilePtr->fileName(), m_sourceFilePtr->errorString()));

		AbortCurrentProcessing();

		return false;
	}

	bool retVal = true;
	retVal = retVal && ProcessHeaderClassFile(sdlType);
	retVal = retVal && ProcessSourceClassFile(sdlType);

	return retVal;
}


bool CSdlClassModificatorBaseComp::EndClassFiles(const imtsdl::CSdlType& /*sdlType*/)
{
	bool retVal = CloseFiles();

	// Move modificated files to original location
	const QString originalHeaderPath = QFileInfo(*m_originalHeaderFilePtr).absolutePath();
	retVal = retVal && istd::CSystem::FileMove(m_headerFilePtr->fileName(), originalHeaderPath, true);
	if (!retVal){
		SendErrorMessage(0,
						 QString("C: %3. Unable to move H file: '%1' to '%2'.")
							 .arg(m_headerFilePtr->fileName(), originalHeaderPath, m_logTag));
	}

	const QString originaSourcelPath = QFileInfo(*m_originalSourceFilePtr).absolutePath();
	retVal = retVal && istd::CSystem::FileMove(m_sourceFilePtr->fileName(), originaSourcelPath, true);
	if (!retVal){
		SendErrorMessage(0,
						 QString("C: %3. Unable to move C file: '%1' to '%2'.")
							 .arg(m_sourceFilePtr->fileName(), originaSourcelPath, m_logTag));
	}

	return retVal;
}


bool CSdlClassModificatorBaseComp::CloseFiles()
{
	bool retVal = true;

	retVal = m_headerFilePtr->flush();
	if (!retVal){
		SendErrorMessage(0, QString("Unable to write file data. File: '%1'. Error: %2.").arg(m_headerFilePtr->fileName(), m_headerFilePtr->errorString()));

		return false;
	}

	retVal = m_sourceFilePtr->flush() && retVal;
	if (!retVal){
		SendErrorMessage(0, QString("Unable to write file data. File: '%1'. Error: %2.").arg(m_sourceFilePtr->fileName(), m_sourceFilePtr->errorString()));

		return false;
	}

	m_headerFilePtr->close();
	m_sourceFilePtr->close();

	return retVal;
}


void CSdlClassModificatorBaseComp::AbortCurrentProcessing()
{
	m_headerFilePtr->close();
	m_sourceFilePtr->close();

	m_headerFilePtr->remove();
	m_sourceFilePtr->remove();
}


} // namespace imtsdlgen

