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
	if (!retVal){
		I_CRITICAL();

		return false;
	}

	bool atEnd = false;

	// process types
	while (!atEnd){
		retVal = retVal && MoveAfterWord(QStringLiteral("type")) && MoveToNextReadableSymbol();

		QByteArray typeName;
		// brgin type entry
		retVal = retVal && ReadToDelimeter("{", typeName);

		CSdlDocumentType documentType;
		documentType.SetName(typeName.trimmed());
		retVal = retVal && ExtractDocumentTypeFromCurrentEntry(documentType);

		// end type entry
		retVal = retVal && ReadToDelimeter("}", typeName);

		char foundDelimiter = ' ';
		retVal = retVal && MoveToNextReadableSymbol(&foundDelimiter);

		atEnd = bool(foundDelimiter == '}' || !retVal);
	}

	return retVal;
}

bool CGqlExtSchemaParser::ExtractDocumentTypeFromCurrentEntry(CSdlDocumentType& documentType)
{
	bool retVal = true;

	QByteArray keyword;
	retVal = retVal && MoveToNextReadableSymbol() && ReadToDelimeter(":{}", keyword);
	keyword = keyword.trimmed();

	// fill reference type
	if (keyword == QByteArrayLiteral("ref")){
		QByteArray typeRefName;
		retVal = retVal && ReadToDelimeterOrSpace("", typeRefName);
		auto foundIterator = std::find_if(m_sdlTypes.cbegin(), m_sdlTypes.cend(), [&typeRefName](const CSdlType& type){
			return (type.GetName() == typeRefName);
		});
		if (foundIterator == m_sdlTypes.cend()){
			I_CRITICAL();

			return false;
		}
		documentType.SetReferenceType(*foundIterator);
	}
	// extract operations
	else if (keyword == QByteArrayLiteral("operations")){
		char foundDelimiter = ' ';
		retVal = retVal && MoveToNextReadableSymbol(&foundDelimiter) && MoveToNextReadableSymbol();
		Q_ASSERT(foundDelimiter == '{');

		QByteArray operationTypeId;
		retVal = retVal && ReadToDelimeter(":", operationTypeId);
		operationTypeId = operationTypeId.trimmed();
		CSdlDocumentType::OperationType operationType = CSdlDocumentType::OT_LIST;
		bool isOperationValid = CSdlDocumentType::FromString(operationTypeId, operationType);
		if (!isOperationValid){
			I_CRITICAL();

			return false;
		}

		QByteArray requestName;
		retVal = retVal && ReadToDelimeterOrSpace("}", requestName);
		requestName = requestName.trimmed();

		auto foundIterator = std::find_if(m_requests.cbegin(), m_requests.cend(), [&requestName](const CSdlRequest& request){
			return (request.GetName() == requestName);
		});
		if (foundIterator == m_requests.cend()){
			I_CRITICAL();

			return false;
		}
		documentType.AddOperation(operationType, *foundIterator);
	}
	else {
		I_CRITICAL();

		return false;
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

