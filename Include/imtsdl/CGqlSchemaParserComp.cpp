#include "CGqlSchemaParserComp.h"


// Qt includes
#include <QtCore/QRegularExpression>


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
	Q_ASSERT(!m_tempDirPtr.IsValid());

	m_tempDirPtr.SetPtr(new imtbase::CTempDir);

	// first remove remarks and save file in temp dir
	QFile inputFile(schemaFilePath);

	if (!inputFile.open(QIODevice::ReadOnly)){
		SendErrorMessage(0, QString("Unable to open file '%1'").arg(inputFile.fileName()));
		I_CRITICAL();

		return iproc::IProcessor::TS_INVALID;
	}

	QTextStream inputStream(&inputFile);
	QString content = inputStream.readAll();

	static QRegularExpression multiLineCommentRegex("#\\[\\[.*\\]\\]", QRegularExpression::MultilineOption | QRegularExpression::DotMatchesEverythingOption);
	content.replace(multiLineCommentRegex, "");

	static QRegularExpression commentRegex("#.*\n");
	content.replace(commentRegex, "\n");

	QString outputFileName = m_tempDirPtr->Path() + QDir::separator() + "schema.sdl";
	QFile outputFile(outputFileName);

	if(!outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
		qDebug() << "Error: Cannot open output file";
		return -1;
	}

	QTextStream outputStream(&outputFile);
	outputStream << content;
	outputFile.close();

	// and parse file with removed remarks
	m_currentInputFilePtr.SetPtr(new QFile(outputFileName));
	if (!m_currentInputFilePtr->open(QIODevice::ReadOnly)){
		SendErrorMessage(0, QString("Unable to open file '%1'").arg(m_currentInputFilePtr->fileName()));
		I_CRITICAL();

		return iproc::IProcessor::TS_INVALID;
	}

	BaseClass::SetDevice(*m_currentInputFilePtr);

	const bool isSchemaParsed = ParseGqlSchema();
	retVal = isSchemaParsed ? iproc::IProcessor::TS_OK : iproc::IProcessor::TS_INVALID;

	return retVal;
}


} // namespace imtsdl

