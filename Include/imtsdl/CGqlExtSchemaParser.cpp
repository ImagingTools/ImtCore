#include <imtsdl/CGqlExtSchemaParser.h>


// imtsdl includes
#include <imtsdl/CSdlTools.h>


namespace imtsdl
{


// protected constructor

CGqlExtSchemaParser::CGqlExtSchemaParser(): BaseClass()
{
}


// public methods

CGqlExtSchemaParser::CGqlExtSchemaParser(QIODevice& device): BaseClass(device)
{
}


// reimplemented (ISdlDocumentTypeListProvider)

SdlDocumentTypeList CGqlExtSchemaParser::GetDocumentTypes() const
{
	return m_documentTypes;
}


// protected methods

bool CGqlExtSchemaParser::ProcessDocumentSchema()
{
	bool retVal = true;

	QByteArray devNull;
	retVal = ReadToDelimeter("{",devNull);

	bool atEnd = false;

	// process types
	while (!atEnd){
		if (retVal){
			retVal = MoveAfterWord(QStringLiteral("type")) && MoveToNextReadableSymbol();
		}
	}


	return retVal;
}


// reimplemented (CGqlSchemaParser)

bool CGqlExtSchemaParser::ProcessCustomSection(const QString& sectionName)
{
	if (sectionName == QStringLiteral("documentSchema")){
		return ProcessDocumentSchema();
	}

	return BaseClass::ProcessCustomSection(sectionName);
}



} // namespace imtsdl

