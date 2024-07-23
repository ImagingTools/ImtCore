#include "CSdlClassModificatorBaseComp.h"


// Qt includes
#include <QtCore/QDir>
#include <QtCore/QRegularExpression>
#include <QtCore/QRegularExpressionMatch>

//Acf includes
#include <istd/CSystem.h>

// imtbase includes
#include <imtbase/CTempDir.h>

// imtsdl includes
#include <imtsdl/CSdlType.h>


namespace imtsdl
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

	int retVal = iproc::IProcessor::TS_OK;
	if (!m_argumentParserCompPtr->IsModificatorEnabled(*m_processorModificatorNameAttrPtr)){
		// nothing todo if our modificator is not exsits
		return retVal;
	}

	if (*m_isVariantMapModificatorRequiredAttrPtr){
		if (!m_argumentParserCompPtr->IsModificatorEnabled(s_variantMapModificatorArgumentName)){
			SendErrorMessage(1, QString("Variant map modificator is not enabled. Use -M%1 argument to enable it").arg(s_variantMapModificatorArgumentName));

			return iproc::IProcessor::TS_INVALID;
		}
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

	SdlTypeList sdlTypeList = m_sdlTypeListCompPtr->GetSdlTypes();
	for (const CSdlType& sdlType: sdlTypeList){
		m_headerFilePtr.SetPtr(new QFile(tempPath + "/C" + sdlType.GetName() + ".h"));
		m_sourceFilePtr.SetPtr(new QFile(tempPath + "/C" + sdlType.GetName() + ".cpp"));
		m_originalHeaderFilePtr.SetPtr(new QFile(outputDirectoryPath + "/C" + sdlType.GetName() + ".h"));
		m_originalSourceFilePtr.SetPtr(new QFile(outputDirectoryPath + "/C" + sdlType.GetName() + ".cpp"));
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

bool CSdlClassModificatorBaseComp::BeginClassFiles(const CSdlType& sdlType)
{
	// Create new files in temp path
	if (!m_headerFilePtr->open(QIODevice::WriteOnly)){
		SendErrorMessage(0,
					QString("Unable to open file: '%1'. Error: %2")
						.arg(m_headerFilePtr->fileName(), m_headerFilePtr->errorString()));

		AbortCurrentProcessing();

		return false;
	}

	if (!m_sourceFilePtr->open(QIODevice::WriteOnly)){
		SendErrorMessage(0,
						QString("Unable to open file: '%1'. Error: %2")
							.arg(m_sourceFilePtr->fileName(), m_sourceFilePtr->errorString()));

		AbortCurrentProcessing();

		return false;
	}

	// Open original files to read data
	if (!m_originalHeaderFilePtr->open(QIODevice::ReadOnly)){
		SendErrorMessage(0,
						 QString("Unable to open file: '%1'. Error: %2")
						 .arg(m_originalHeaderFilePtr->fileName(), m_originalHeaderFilePtr->errorString()));

		AbortCurrentProcessing();

		return false;
	}

	if (!m_originalSourceFilePtr->open(QIODevice::ReadOnly)){
		SendErrorMessage(0,
						 QString("Unable to open file: '%1'. Error: %2")
						 .arg(m_originalSourceFilePtr->fileName(), m_originalSourceFilePtr->errorString()));

		AbortCurrentProcessing();

		return false;
	}

	bool retVal = true;
	retVal = retVal && ProcessHeaderClassFile(sdlType);
	retVal = retVal && ProcessSourceClassFile(sdlType);

	return retVal;
}


bool CSdlClassModificatorBaseComp::EndClassFiles(const CSdlType& /*sdlType*/)
{
	return CloseFiles();
}


bool CSdlClassModificatorBaseComp::CloseFiles()
{
	bool retVal = true;

	retVal = m_headerFilePtr->flush();
	Q_ASSERT(retVal);

	retVal = m_sourceFilePtr->flush() && retVal;
	Q_ASSERT(retVal);

	// Close files
	m_headerFilePtr->close();
	m_sourceFilePtr->close();

	m_originalHeaderFilePtr->close();
	m_originalSourceFilePtr->close();

	// Then move modificated files to original location
	const QString originalPath = QFileInfo(*m_originalHeaderFilePtr).absolutePath();
	retVal = retVal && istd::CSystem::FileMove(m_headerFilePtr->fileName(), originalPath, true);
	Q_ASSERT(retVal);

	retVal = retVal && istd::CSystem::FileMove(m_sourceFilePtr->fileName(), originalPath, true);
	Q_ASSERT(retVal);

	return retVal;
}


void CSdlClassModificatorBaseComp::AbortCurrentProcessing()
{
	m_headerFilePtr->close();
	m_sourceFilePtr->close();

	I_CRITICAL();

	m_headerFilePtr->remove();
	m_sourceFilePtr->remove();
}


} // namespace imtsdl

