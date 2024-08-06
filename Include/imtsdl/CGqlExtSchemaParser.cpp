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
	retVal = ReadToDelimeter("{",devNull) && MoveToNextReadableSymbol();
	if (!retVal){
		I_CRITICAL();

		return false;
	}

	// process types
	while (true){

		char foundDelimiter = ' ';
		QByteArray typeName;
		retVal = retVal && ReadToDelimeterOrSpace("{", typeName);
		if (typeName.trimmed() != QByteArrayLiteral("type")){
			I_CRITICAL();

			return false;
		}

		// brgin type entry
		retVal = retVal && ReadToDelimeter("{", typeName);

		CSdlDocumentType documentType;
		documentType.SetName(typeName.trimmed());
		retVal = retVal && ExtractDocumentTypeFromCurrentEntry(documentType);
		m_documentTypes << documentType;

		retVal = retVal && MoveToNextReadableSymbol(&foundDelimiter);

		// check if we at and of section
		if (foundDelimiter == '}' || !retVal){
			break;
		}
	}

	return retVal;
}


bool CGqlExtSchemaParser::ExtractDocumentTypeFromCurrentEntry(CSdlDocumentType& documentType)
{
	bool retVal = true;

	while (!m_stream.atEnd()){
		QByteArray keyword;

		char foundDelimiter = ' ';
		retVal = retVal && MoveToNextReadableSymbol() && ReadToDelimeterOrSpace(":{}", keyword, &foundDelimiter);

		// check if we at end of reading
		if (foundDelimiter == '}'){
			break;
		}

		keyword = keyword.trimmed();

		// fill reference type
		if (keyword == QByteArrayLiteral("ref")){
			QByteArray typeRefName;
			retVal = retVal && MoveToNextReadableSymbol() && ReadToDelimeterOrSpace("", typeRefName);
			Q_ASSERT(!typeRefName.isEmpty());

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
			while (!m_stream.atEnd()) {
				QByteArray operationTypeId;
				retVal = retVal && ReadToDelimeter(":{}", operationTypeId, &foundDelimiter);
				operationTypeId = operationTypeId.trimmed();

				// check if we at end of operations directive
				if (foundDelimiter == '}'){
					break;
				}

				if (foundDelimiter == '{'){
					retVal = retVal && MoveToNextReadableSymbol(&foundDelimiter);
					retVal = retVal && ReadToDelimeter(":{}", operationTypeId, &foundDelimiter);
				}

				CSdlDocumentType::OperationType operationType = CSdlDocumentType::OT_LIST;
				bool isOperationValid = CSdlDocumentType::FromString(operationTypeId, operationType);
				if (!isOperationValid){
					I_CRITICAL();

					return false;
				}

				QByteArray requestName;
				retVal = retVal && MoveToNextReadableSymbol();
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
		}
		// extract subtypes
		else if (keyword == QByteArrayLiteral("subtype")){
			QByteArray subtypeName;
			retVal = retVal && ReadToDelimeter("{", subtypeName);
			CSdlDocumentType documentSubtype;
			documentType.SetName(subtypeName.trimmed());

			retVal = retVal && ExtractDocumentTypeFromCurrentEntry(documentSubtype);
			if (retVal){
				documentType.AddSubtype(documentSubtype);
			}
		}
		else {
			I_CRITICAL();

			return false;
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

