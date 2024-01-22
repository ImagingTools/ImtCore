#include "CGqlSchemaParser.h"


namespace imtsdl
{


CGqlSchemaParser::CGqlSchemaParser():
	m_lastReadChar(0),
	m_lastReadLine(0),
	m_useLastReadChar(false)
{
}


// public methods

CGqlSchemaParser::CGqlSchemaParser(QIODevice& device): CGqlSchemaParser()
{
	SetDevice(device);
}


bool CGqlSchemaParser::ParseGqlSchema()
{
	bool retVal = true;

	retVal = retVal && MoveToNextReadableSymbol();
	QByteArray keyword;
	retVal = retVal && ReadToDelimeterOrSpace("{", keyword);

	if (m_stream.atEnd()){
		return true;
	}
	keyword = keyword.trimmed();

	if (keyword == QByteArrayLiteral("schema")){
		retVal = retVal && ProcessSchema();
	}
	else if (keyword == QByteArrayLiteral("type")){
		retVal = retVal && ProcessType();
	}
	else if (keyword == QByteArrayLiteral("interface")){
		retVal = retVal && ProcessInterface();
	}
	else if (keyword == QByteArrayLiteral("union")){
		retVal = retVal && ProcessUnion();
	}
	else if (keyword == QByteArrayLiteral("scalar")){
		retVal = retVal && ProcessScalar();
	}
	else if (keyword == QByteArrayLiteral("enum")){
		retVal = retVal && ProcessEnum();
	}
	else if (keyword == QByteArrayLiteral("input")){
		retVal = retVal && ProcessInput();
	}
	else if (keyword == QByteArrayLiteral("extend")){
		retVal = retVal && ProcessExtend();
	}
	else if (keyword == QByteArrayLiteral("directive")){
		retVal = retVal && ProcessDirective();
	}
	else if (keyword == m_keywordMap[KI_QUERY]){
		retVal = retVal && ProcessQuery();
	}
	else if (keyword == m_keywordMap[KI_MUTATION]){
		retVal = retVal && ProcessMutation();
	}
	else if (keyword == m_keywordMap[KI_SUBSCRIPTION]){
		retVal = retVal && ProcessSubscription();
	}
	else {
		SendLogMessage(
					istd::IInformationProvider::InformationCategory::IC_ERROR,
					0,
					QString("Unexpected derictive '%1' at %2 line").arg(keyword, QString::number(m_lastReadLine + 1)),
					"CGqlSchemaParser");
		Q_ASSERT(false);

		return false;
	}

	retVal = retVal && ParseGqlSchema();

	Q_ASSERT(retVal);

	return retVal;
}


// reimplemented (ISdlTypeListProvider)

SdlTypeList CGqlSchemaParser::GetSdlTypes() const
{
	return m_sdlTypes;
}


QStringList CGqlSchemaParser::GetTypeNames() const
{
	QStringList retVal;

	for (const CSdlType& type: m_sdlTypes){
		retVal << type.GetName();
	}

	return retVal;
}


SdlFieldList CGqlSchemaParser::GetFields(const QString typeName) const
{
	for (const CSdlType& type: m_sdlTypes){
		if (type.GetName() == typeName){
			return type.GetFields();
		}
	}
	SendLogMessage(
				istd::IInformationProvider::InformationCategory::IC_ERROR,
				0,
				QString("Unable to find type name '%1'").arg(typeName),
				"CGqlSchemaParser");
	Q_ASSERT(false);

	return {CSdlField()};
}


// protected methods

bool CGqlSchemaParser::ProcessSchema()
{
	bool retVal = true;
	m_keywordMap.clear();

	QByteArray devNull;
	retVal = ReadToDelimeter("{",devNull);

	char foundDelimiter = ':';
	do {
		QByteArray schemaType;
		retVal = retVal && ReadToDelimeter(":}", schemaType, &foundDelimiter);
		if (foundDelimiter == '}'){
			// we at and of declaration nothing to do
			break;
		}

		QByteArray schemaSynonym;
		retVal = retVal && MoveToNextReadableSymbol();
		retVal = retVal && ReadToDelimeterOrSpace("}", schemaSynonym, &foundDelimiter);
		schemaSynonym = schemaSynonym.trimmed();

		if (schemaType == QByteArrayLiteral("query")){
			m_keywordMap[KI_QUERY] = schemaSynonym;
		}
		else if (schemaType == QByteArrayLiteral("mutation")){
			m_keywordMap[KI_MUTATION] = schemaSynonym;
		}
		else if (schemaType == QByteArrayLiteral("subscription")){
			m_keywordMap[KI_SUBSCRIPTION] = schemaSynonym;
		}
		else if (foundDelimiter == '}'){
			// schema parsing is done nothing to do anymore
		}
		else {
			SendLogMessage(
						istd::IInformationProvider::InformationCategory::IC_ERROR,
						0,
						QString("Unexpected derictive '%1' at %2 line").arg(schemaType, QString::number(m_lastReadLine + 1)),
						"CGqlSchemaParser");
			Q_ASSERT(false);
		}
	}
	while(foundDelimiter != '}');

	if (m_keywordMap.isEmpty()){
		SendLogMessage(
					istd::IInformationProvider::InformationCategory::IC_WARNING,
					0,
					QString("Schema does not contains keywords"),
					"CGqlSchemaParser");
	}

	return retVal;
}


bool CGqlSchemaParser::ProcessType()
{
	bool retVal = true;

	QByteArray typeName;
	retVal = ReadToDelimeter("{",typeName);
	CSdlType gqlType;
	gqlType.SetName(QString(typeName.trimmed()));

	retVal = retVal && MoveToNextReadableSymbol();
	bool atEnd = false;
	SdlFieldList fieldList;
	while (!atEnd){
		ProcessValue(fieldList, &atEnd);
	}
	gqlType.SetFields(fieldList);
	m_sdlTypes << gqlType;

	return retVal;
}


bool CGqlSchemaParser::ProcessInterface()
{
	bool retVal = false;
	SendLogMessage(
				istd::IInformationProvider::InformationCategory::IC_CRITICAL,
				0,
				QString("Process interface is not implemented yet"),
				"CGqlSchemaParser");
	Q_ASSERT(retVal);

	return retVal;
}


bool CGqlSchemaParser::ProcessUnion()
{
	bool retVal = false;
		SendLogMessage(
				istd::IInformationProvider::InformationCategory::IC_CRITICAL,
				0,
				QString("Process union is not implemented yet"),
				"CGqlSchemaParser");
	Q_ASSERT(retVal);

	return retVal;
}


bool CGqlSchemaParser::ProcessScalar()
{
	bool retVal = false;
		SendLogMessage(
				istd::IInformationProvider::InformationCategory::IC_CRITICAL,
				0,
				QString("Process scalar is not implemented yet"),
				"CGqlSchemaParser");
	Q_ASSERT(retVal);

	return retVal;
}


bool CGqlSchemaParser::ProcessEnum()
{
	bool retVal = false;
		SendLogMessage(
				istd::IInformationProvider::InformationCategory::IC_CRITICAL,
				0,
				QString("Process enum is not implemented yet"),
				"CGqlSchemaParser");
	Q_ASSERT(retVal);

	return retVal;
}


bool CGqlSchemaParser::ProcessInput()
{
	return ProcessType();
}


bool CGqlSchemaParser::ProcessExtend()
{
	bool retVal = false;
		SendLogMessage(
				istd::IInformationProvider::InformationCategory::IC_CRITICAL,
				0,
				QString("Process extend is not implemented yet"),
				"CGqlSchemaParser");
	Q_ASSERT(retVal);

	return retVal;
}


bool CGqlSchemaParser::ProcessDirective()
{
	bool retVal = false;
		SendLogMessage(
				istd::IInformationProvider::InformationCategory::IC_CRITICAL,
				0,
				QString("Process directive is not implemented yet"),
				"CGqlSchemaParser");
	Q_ASSERT(retVal);

	return retVal;
}


bool CGqlSchemaParser::ProcessQuery()
{
	bool retVal = false;
		SendLogMessage(
				istd::IInformationProvider::InformationCategory::IC_CRITICAL,
				0,
				QString("Process query is not implemented yet"),
				"CGqlSchemaParser");
	Q_ASSERT(retVal);

	return retVal;
}


bool CGqlSchemaParser::ProcessMutation()
{
	bool retVal = false;
		SendLogMessage(
				istd::IInformationProvider::InformationCategory::IC_CRITICAL,
				0,
				QString("Process mutation is not implemented yet"),
				"CGqlSchemaParser");
	Q_ASSERT(retVal);

	return retVal;
}


bool CGqlSchemaParser::ProcessSubscription()
{
	bool retVal = false;
		SendLogMessage(
				istd::IInformationProvider::InformationCategory::IC_CRITICAL,
				0,
				QString("Process subscription is not implemented yet"),
				"CGqlSchemaParser");
	Q_ASSERT(retVal);

	return retVal;
}


bool CGqlSchemaParser::ProcessValue(SdlFieldList& output, bool* endOfReadPtr)
{
	bool retVal = true;

	CSdlField field;
	QByteArray valueId;
	char foundDelimeter = ':';
	retVal = retVal && ReadToDelimeter(":}", valueId, &foundDelimeter);
	if (foundDelimeter == ':'){
		field.SetId(valueId);
	}
	retVal = retVal && MoveToNextReadableSymbol();
	// process arrays
	if (m_lastReadChar == '['){
		retVal = retVal && MoveToNextReadableSymbol();
		field.SetIsArray(true);
		QByteArray valueType;
		char foundDelimeter = ';';
		retVal = retVal && ReadToDelimeter("!]", valueType, &foundDelimeter);
		if (foundDelimeter == '!'){
			field.SetIsNonEmpty(true);
		}
		field.SetType(valueType);
		if (foundDelimeter != ']'){
			retVal = retVal && ReadToDelimeter("]", valueType);
		}

		retVal = retVal && MoveToNextReadableSymbol(&foundDelimeter);
		if (m_lastReadChar == '!'){
			field.SetIsRequired(true);
			retVal = retVal && MoveToNextReadableSymbol(&foundDelimeter);
		}
	}
	// process scalar values
	else {
		QByteArray valueType;
		retVal = retVal && ReadToDelimeterOrSpace(",!}", valueType, &foundDelimeter);
		field.SetType(valueType);

		if (foundDelimeter == '!'){
			field.SetIsRequired(true);
		}
		retVal = retVal && MoveToNextReadableSymbol(&foundDelimeter);

		if (endOfReadPtr != nullptr){
			*endOfReadPtr = bool(foundDelimeter == '}');
		}
	}

	output << field;

	if (endOfReadPtr != nullptr){
		*endOfReadPtr = bool(m_lastReadChar == '}');
	}

	Q_ASSERT(m_lastReadChar == '}' || QChar(m_lastReadChar).isLetter());

	return retVal;
}


void CGqlSchemaParser::SetDevice(QIODevice& device)
{
	m_stream.setDevice(&device);

	if (!device.isReadable()){
		SendLogMessage(
					istd::IInformationProvider::InformationCategory::IC_ERROR,
					0,
					"Unable to open device",
					"CGqlSchemaParser");
		Q_ASSERT(false);

		return;
	}
}


bool CGqlSchemaParser::ReadToDelimeter(const QByteArray& delimeters,
			QByteArray& result,
			char* foundDelimeterPtr,
			bool allowEmptyResult,
			bool skipDelimeter)
{
	int cutFromPos = -2;
	int cutToPos = -2;

	QByteArray readString;

	if (!m_useLastReadChar){
		m_stream >> m_lastReadChar;

		if (m_lastReadChar == '\n'){
			++m_lastReadLine;
		}
	}

	while (!m_stream.atEnd()){
		int foundPosition = delimeters.indexOf(m_lastReadChar);
		if (foundPosition >= 0){
			m_useLastReadChar = !skipDelimeter;

			if (cutFromPos < 0){
				if (cutToPos < 0){
					result = "";

					if (allowEmptyResult && foundDelimeterPtr != NULL){
						*foundDelimeterPtr = delimeters.at(foundPosition);
					}

					return true;
				}

				cutFromPos = 0;
			}

			if (cutToPos < 0){
				cutToPos = int(readString.size());
			}

			result = readString.mid(cutFromPos, cutToPos - cutFromPos);

			if (foundDelimeterPtr != NULL){
				*foundDelimeterPtr = delimeters.at(foundPosition);
			}

			return true;
		}

		readString += m_lastReadChar;

		if (!isspace((unsigned char)m_lastReadChar) && !iscntrl((unsigned char)m_lastReadChar)){
			cutToPos = int(readString.size());

			if (cutFromPos < 0){
				cutFromPos = cutToPos - 1;
			}
		}

		m_stream >> m_lastReadChar;

		if (m_lastReadChar == '\n'){
			++m_lastReadLine;
		}
	}

	return false;
}


bool CGqlSchemaParser::ReadToDelimeterOrSpace(const QByteArray& delimeters,
			QByteArray& result,
			char* foundDelimeterPtr,
			bool allowEmptyResult,
			bool skipDelimeter)
{
	QByteArray newDelimeters = delimeters;
	newDelimeters.append(' ');
	newDelimeters.append('\n');
	newDelimeters.append('\r');
	return ReadToDelimeter(
				newDelimeters,
				result,
				foundDelimeterPtr,
				allowEmptyResult,
				skipDelimeter);
}


bool CGqlSchemaParser::MoveToCharType(
			QChar::Category category,

			char* foundDelimeterPtr,
			bool skipDelimeter)
{
	return MoveToCharType(QList<QChar::Category>({category}), foundDelimeterPtr, skipDelimeter);
}


bool CGqlSchemaParser::MoveToCharType(
			const QList<QChar::Category>& categoryList,
			char* foundDelimeterPtr,
			bool skipDelimeter)
{
	QChar currentChar(m_lastReadChar);

	while(!m_stream.atEnd()){
		m_stream >> currentChar;
		if (currentChar == '\n'){
			++m_lastReadLine;
		}
		QChar::Category currentCharCategoty = currentChar.category();
		if (categoryList.contains(currentCharCategoty)){
			m_useLastReadChar = !skipDelimeter;
			char foundChar = QString(currentChar).toUtf8()[0];
			m_lastReadChar = foundChar;
			if (foundDelimeterPtr != nullptr){
				*foundDelimeterPtr = foundChar;
			}

			break;
		}
	}

	return !currentChar.isNull();
}


bool CGqlSchemaParser::MoveToNextReadableSymbol(char* foundDelimeterPtr, bool skipDelimeter)
{
	return MoveToCharType(QList<QChar::Category>()
				<< QChar::Letter_Uppercase
				<< QChar::Letter_Lowercase
				<< QChar::Letter_Titlecase
				<< QChar::Letter_Modifier
				<< QChar::Letter_Other
				<< QChar::Punctuation_Connector
				<< QChar::Punctuation_Dash
				<< QChar::Punctuation_Open
				<< QChar::Punctuation_Close
				<< QChar::Punctuation_InitialQuote
				<< QChar::Punctuation_FinalQuote
				<< QChar::Punctuation_Other
				<< QChar::Symbol_Math
				<< QChar::Symbol_Currency
				<< QChar::Symbol_Modifier
				<< QChar::Symbol_Other,
				foundDelimeterPtr, skipDelimeter);
}


} // namespace imtsdl

