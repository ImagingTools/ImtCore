#include "CGqlSchemaParserComp.h"


// Acf includes
#include <iprm/TParamsPtr.h>
#include <ifile/IFileNameParam.h>


namespace imtsdl
{


int CGqlSchemaParserComp::DoProcessing(
			const iprm::IParamsSet* paramsPtr,
			const istd::IPolymorphic* /*inputPtr*/,
			istd::IChangeable* /*outputPtr*/,
			ibase::IProgressManager* progressManagerPtr)
{
	/*
	*/

	int retVal = iproc::IProcessor::TS_NONE;

	iprm::TParamsPtr<ifile::IFileNameParam> inputFileNameParamPtr(paramsPtr, "InputFilePath");
	if (!inputFileNameParamPtr.IsValid()){
		SendCriticalMessage(0, "'InputFilePath' param is not provided");
		I_CRITICAL();

		return iproc::IProcessor::TS_INVALID;
	}

	Q_ASSERT(!m_currentInputFile.IsValid());

	m_currentInputFile.SetPtr(new QFile(inputFileNameParamPtr->GetPath()));
	if (!m_currentInputFile->open(QIODevice::ReadOnly)){
		SendErrorMessage(0, QString("Unable to open file '%1'").arg(m_currentInputFile->fileName()));
		I_CRITICAL();

		return iproc::IProcessor::TS_INVALID;
	}

	BaseClass2::SetDevice(*m_currentInputFile);

	bool isSchemaParsed = BaseClass2::ParseGqlSchema();
	retVal = isSchemaParsed ? iproc::IProcessor::TS_OK : iproc::IProcessor::TS_INVALID;

	return retVal;
}


} // namespace imtsdl

