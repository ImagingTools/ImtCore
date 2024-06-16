#include "CBaseClassExtenderComp.h"


// Qt includes
#include <QtCore/QDir>
#include <QtCore/QRegularExpression>
#include <QtCore/QRegularExpressionMatch>

//Acf includes
#include <istd/CSystem.h>
#include <iprm/TParamsPtr.h>
#include <iprm/IEnableableParam.h>
#include <iprm/IOptionsList.h>
#include <ifile/IFileNameParam.h>

// imtbase includes
#include <imtbase/CTempDir.h>

// imtsdl includes
#include <imtsdl/CSdlType.h>


namespace imtsdl
{


// public methods

//reimplemented(iproc::IProcessor)

int CBaseClassExtenderComp::DoProcessing(
			const iprm::IParamsSet* paramsPtr,
			const istd::IPolymorphic* inputPtr,
			istd::IChangeable* outputPtr,
			ibase::IProgressManager* progressManagerPtr)
{
	int retVal = iproc::IProcessor::TS_OK;

	const iprm::IOptionsList* baseClassOptionListPtr = dynamic_cast<const iprm::IOptionsList*>(inputPtr);
	if (baseClassOptionListPtr == nullptr){
		I_CRITICAL();

		return TS_INVALID;
	}

	imtbase::CTempDir tempDir;
	const QString tempPath = tempDir.Path();

	iprm::TParamsPtr<ifile::IFileNameParam> headerFilePathPtr(paramsPtr, QByteArrayLiteral("HeaderFile"));
	if (!headerFilePathPtr.IsValid()){
		I_CRITICAL();

		return TS_INVALID;
	}

	m_originalHeaderFilePtr.SetPtr(new QFile(headerFilePathPtr->GetPath()));
	m_headerFilePtr.SetPtr(new QFile(tempPath + "/headerFileBuffer"));

	iprm::TParamsPtr<ifile::IFileNameParam> sourceFilePathPtr(paramsPtr, QByteArrayLiteral("SourceFile"), false);
	if (sourceFilePathPtr.IsValid()){
		m_sourceFilePtr.SetPtr(new QFile(tempPath + "/sourceFileBuffer"));
		m_originalSourceFilePtr.SetPtr(new QFile(sourceFilePathPtr->GetPath()));
	}

	bool isOk = BeginClassFiles();
	if (!isOk){
		AbortCurrentProcessing();

		return iproc::IProcessor::TS_INVALID;
	}

	isOk = isOk && ProcessHeaderClassFile(paramsPtr, baseClassOptionListPtr);
	if (!isOk){
		AbortCurrentProcessing();

		return iproc::IProcessor::TS_INVALID;
	}

	isOk = isOk && ProcessSourceClassFile(paramsPtr, baseClassOptionListPtr);
	if (!isOk){
		AbortCurrentProcessing();

		return iproc::IProcessor::TS_INVALID;
	}

	isOk = isOk && EndClassFiles();
	if (!isOk){
		AbortCurrentProcessing();

		return iproc::IProcessor::TS_INVALID;
	}


	return retVal;
}


// protected methods


bool CBaseClassExtenderComp::ProcessHeaderClassFile(
			const iprm::IParamsSet* paramsPtr,
			const iprm::IOptionsList* baseClassList)
{
	/// \todo finish it
	// QTextStream

	return true;
}


bool CBaseClassExtenderComp::ProcessSourceClassFile(
			const iprm::IParamsSet* /*paramsPtr*/,
			const iprm::IOptionsList* /*baseClassList*/)
{
	if (!m_sourceFilePtr.IsValid() || !m_originalSourceFilePtr.IsValid()){
		return true;
	}

	/// \todo implement it. Add overrided methods
	I_CRITICAL();

	return true;
}


bool CBaseClassExtenderComp::BeginClassFiles()
{
	// Create new files in temp path
	if (!m_headerFilePtr->open(QIODevice::WriteOnly)){
		SendErrorMessage(0,
					QString("Unable to open file: '%1'. Error: %2")
						.arg(m_headerFilePtr->fileName(), m_headerFilePtr->errorString()));

		AbortCurrentProcessing();

		return false;
	}

	// source file is optional
	if (m_sourceFilePtr.IsValid() && !m_sourceFilePtr->open(QIODevice::WriteOnly)){
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

	// source file is optional
	if (m_originalSourceFilePtr.IsValid() && !m_originalSourceFilePtr->open(QIODevice::ReadOnly)){
		SendErrorMessage(0,
						 QString("Unable to open file: '%1'. Error: %2")
						 .arg(m_originalSourceFilePtr->fileName(), m_originalSourceFilePtr->errorString()));

		AbortCurrentProcessing();

		return false;
	}

	return true;
}


bool CBaseClassExtenderComp::EndClassFiles()
{
	return CloseFiles();
}


bool CBaseClassExtenderComp::CloseFiles()
{
	bool retVal = true;

	retVal = m_headerFilePtr->flush();
	Q_ASSERT(retVal);	
	m_headerFilePtr->close();

	m_originalHeaderFilePtr->close();

	const QString originalPath = QFileInfo(*m_originalHeaderFilePtr).absolutePath();
	retVal = retVal && istd::CSystem::FileMove(m_headerFilePtr->fileName(), originalPath, true);
	Q_ASSERT(retVal);

	if (m_sourceFilePtr.IsValid() && m_originalSourceFilePtr.IsValid()){
		retVal = m_sourceFilePtr->flush() && retVal;
		Q_ASSERT(retVal);

		m_sourceFilePtr->close();
		m_originalSourceFilePtr->close();

		const QString originalPath = QFileInfo(*m_originalSourceFilePtr).absolutePath();
		retVal = retVal && istd::CSystem::FileMove(m_sourceFilePtr->fileName(), originalPath, true);
		Q_ASSERT(retVal);
	}

	return retVal;
}


void CBaseClassExtenderComp::AbortCurrentProcessing()
{
	m_headerFilePtr->close();
	m_sourceFilePtr->close();

	I_CRITICAL();

	m_headerFilePtr->remove();
	m_sourceFilePtr->remove();
}


} // namespace imtsdl

