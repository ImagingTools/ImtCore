#include <imtsdlgenv2/CSdlProcessArgumentsParserComp.h>


//Qt includes
#include <QtCore/QRegularExpression>
#include <QtCore/QCommandLineParser>

// ImtCore includes
#include <imtsdl/CSdlTools.h>


namespace imtsdlgenv2
{


// public methods

CSdlProcessArgumentsParserComp::CSdlProcessArgumentsParserComp()
	: BaseClass(),
	m_generationVersion(1)
{
}


// public methods

// reimplemented (imtsdlgenv2::ISdlProcessArgumentsParser)

quint16 CSdlProcessArgumentsParserComp::GetGenerationVersion() const
{
	return m_generationVersion;
}


// protected methods

// reimplemented (imtsdlgen::CSdlProcessArgumentsParserComp)

QList<QCommandLineOption> CSdlProcessArgumentsParserComp::PrepareCommandLineOptions()
{
	QList<QCommandLineOption> retVal;

	m_generationCommandLinePtr.reset(new QCommandLineOption(
		{"GG", "generation"},
		"{1|2} Defines the version of the generator",
		"Generation",
		"1"));

	retVal << *m_generationCommandLinePtr;

	return retVal;
}


bool CSdlProcessArgumentsParserComp::ProcessCommandLineOptions(const QCommandLineParser& commandLineParser)
{
	if (commandLineParser.isSet(*m_generationCommandLinePtr)){
		const QString generationVersionData = commandLineParser.value(*m_generationCommandLinePtr);

		bool isDigit = false;
		m_generationVersion = generationVersionData.toUInt(&isDigit);
		if (!isDigit){
			SendErrorMessage(0, QString("Unexpected %1 option value. See help for detales").arg(m_generationCommandLinePtr->names().join('|')));

			return false;
		}
	}

	// additional checks. to be removed in V2
	if (IsGenerateMode()){
		if (!GetJoinRules().isEmpty()){
			SendWarningMessage(0, "-J option is DEPRECATED! WILL NOT BE SUPPORTED LATER!!!! Please avoid to use it.");
		}
		if (!GetNamespace().isEmpty()){
			SendWarningMessage(0, "-N option is DEPRECATED! WILL NOT BE SUPPORTED LATER!!!! Please avoid to use it. Declare namespace in a schema file.");
		}
	}

	return true;
}




} // namespace imtsdlgenv2


