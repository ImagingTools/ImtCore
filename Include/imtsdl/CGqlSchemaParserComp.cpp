#include "CGqlSchemaParserComp.h"


namespace imtsdl
{


int CGqlSchemaParserComp::DoProcessing(
			const iprm::IParamsSet* /*paramsPtr*/,
			const istd::IPolymorphic* /*inputPtr*/,
			istd::IChangeable* /*outputPtr*/,
			ibase::IProgressManager* /*progressManagerPtr*/)
{
	Q_ASSERT(m_argumentParserCompPtr.IsValid());

	int retVal = iproc::IProcessor::TS_NONE;

	const QString schemaFilePath = m_argumentParserCompPtr->GetSchemaFilePath();
	if (schemaFilePath.isEmpty()){
		SendCriticalMessage(0, "Schema path is not provided");
		I_CRITICAL();

		return iproc::IProcessor::TS_INVALID;
	}

	Q_ASSERT(!m_currentInputFilePtr.IsValid());

	m_currentInputFilePtr.SetPtr(new QFile(schemaFilePath));
	if (!m_currentInputFilePtr->open(QIODevice::ReadOnly)){
		SendErrorMessage(0, QString("Unable to open file '%1'").arg(m_currentInputFilePtr->fileName()));
		I_CRITICAL();

		return iproc::IProcessor::TS_INVALID;
	}

	BaseClass2::SetDevice(*m_currentInputFilePtr);

	const bool isSchemaParsed = ParseGqlSchema();
	retVal = isSchemaParsed ? iproc::IProcessor::TS_OK : iproc::IProcessor::TS_INVALID;

	return retVal;
}


} // namespace imtsdl

