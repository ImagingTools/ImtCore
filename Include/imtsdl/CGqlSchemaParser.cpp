// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CGqlSchemaParser.h"


// Qt includes
#include <QtCore/QDir>

// ACF includes
#include <iprm/CParamsSet.h>
#include <iprm/CTextParam.h>
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtsdl/CSdlTools.h>
#include <imtsdl/CSdlType.h>


namespace imtsdl
{


CGqlSchemaParser::CGqlSchemaParser():
	m_lastReadChar(0),
	m_lastReadLine(0),
	m_useLastReadChar(false),
	m_schemaParamsPtr(new imod::TModelWrap<iprm::CParamsSet>)
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

	retVal = MoveToNextReadableSymbol();
	QByteArray keyword;
	retVal = retVal && ReadToDelimeterOrSpace("{", keyword);

	// if we reached end of data validate and exit
	if (m_stream.atEnd()){
		bool isSchemaValid = ValidateSchema();

		return isSchemaValid;
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
	else if (keyword == QByteArrayLiteral("import")){
		retVal = retVal && ProcessSchemaImports();
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
	else if (!ProcessCustomSection(keyword)){
		SendLogMessage(
					istd::IInformationProvider::InformationCategory::IC_ERROR,
					0,
					QString("Unexpected derictive '%1' at %2 line").arg(keyword, QString::number(m_lastReadLine + 1)),
					"CGqlSchemaParser");

		return false;
	}

	retVal = retVal && ParseGqlSchema();

	return retVal;
}


bool CGqlSchemaParser::SetSchemaName(const QString& schemaName) const
{
	iprm::ITextParam* nameParamPtr = dynamic_cast<iprm::ITextParam*>(m_schemaParamsPtr->GetEditableParameter(SdlCustomSchemaKeys::SchemaName.toUtf8()));
	if (nameParamPtr == nullptr){
		nameParamPtr = new iprm::CTextParam;
	}
	nameParamPtr->SetText(schemaName);
	const bool isSet = m_schemaParamsPtr->SetEditableParameter(SdlCustomSchemaKeys::SchemaName.toUtf8(), nameParamPtr, true);

	return isSet;
}


// reimplemented (ISdlTypeListProvider)

SdlTypeList CGqlSchemaParser::GetSdlTypes(bool onlyLocal) const
{
	/// \todo optimize it (invert)
	SdlTypeList retVal = m_sdlTypes;

	if (onlyLocal){
		QMutableListIterator typesIter(retVal);
		while (typesIter.hasNext()){
			CSdlType type = typesIter.next();
			if (type.IsExternal()){
				typesIter.remove();
			}
		}
	}

	return retVal;
}


QStringList CGqlSchemaParser::GetTypeNames() const
{
	QStringList retVal;

	for (const CSdlType& type: std::as_const(m_sdlTypes)){
		retVal << type.GetName();
	}

	return retVal;
}


SdlFieldList CGqlSchemaParser::GetFields(const QString typeName) const
{
	for (const CSdlType& type: std::as_const(m_sdlTypes)){
		if (type.GetName() == typeName){
			return type.GetFields();
		}
	}
	SendLogMessage(
				istd::IInformationProvider::InformationCategory::IC_ERROR,
				0,
				QString("Unable to find type name '%1'").arg(typeName),
				"CGqlSchemaParser");

	return {CSdlField()};
}


// reimplemented (ISdlRequestListProvider)

SdlRequestList CGqlSchemaParser::GetRequests(bool onlyLocal) const
{
	/// \todo optimize it (invert)
	SdlRequestList retVal = m_requests;
	if (onlyLocal){
		QMutableListIterator requestsIter(retVal);
		while (requestsIter.hasNext()){
			CSdlRequest request = requestsIter.next();
			if (request.IsExternal()){
				requestsIter.remove();
			}
		}
	}

	return retVal;
}


// reimplemented (ISdlEnumListProvider)

SdlEnumList CGqlSchemaParser::GetEnums(bool onlyLocal) const
{
	/// \todo optimize it (invert)
	SdlEnumList retVal = m_enums;

	if (onlyLocal){
		QMutableListIterator enumIter(retVal);
		while (enumIter.hasNext()){
			CSdlEnum enumType = enumIter.next();
			if (enumType.IsExternal()){
				enumIter.remove();
			}
		}
	}

	return retVal;
}


// reimplemented (ISdlUnionListProvider)

SdlUnionList CGqlSchemaParser::GetUnions(bool onlyLocal) const
{
	/// \todo optimize it (invert)
	SdlUnionList retVal = m_unions;

	if (onlyLocal){
		QMutableListIterator unionIter(retVal);
		while (unionIter.hasNext()){
			CSdlUnion unionType = unionIter.next();
			if (unionType.IsExternal()){
				unionIter.remove();
			}
		}
	}

	return retVal;
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
		QByteArray key;
		retVal = retVal && ReadToDelimeter(":}", key, &foundDelimiter);
		if (foundDelimiter == '}'){
			// we at and of declaration nothing to do
			break;
		}

		QByteArray value;
		retVal = retVal && MoveToNextReadableSymbol();
		retVal = retVal && ReadToDelimeterOrSpace("}", value, &foundDelimiter);
		value = value.trimmed();

		if (key == QByteArrayLiteral("query")){
			m_keywordMap[KI_QUERY] = value;
		}
		else if (key == QByteArrayLiteral("mutation")){
			m_keywordMap[KI_MUTATION] = value;
		}
		else if (key == QByteArrayLiteral("subscription")){
			m_keywordMap[KI_SUBSCRIPTION] = value;
		}
		else if (foundDelimiter == '}'){
			// schema parsing is done nothing to do anymore
		}
		else if (!ProcessCustomSchemaValue(key, value)){
			SendLogMessage(
						istd::IInformationProvider::InformationCategory::IC_ERROR,
						0,
						QString("Unknown derictive '%1' at line %2. ").arg(key, QString::number(m_lastReadLine + 1)),
						"CGqlSchemaParser");

			return false;
		}
	}
	while(foundDelimiter != '}');

	return retVal;
}


bool CGqlSchemaParser::ProcessSchemaImports()
{
	I_CRITICAL();

	return false;
}


bool CGqlSchemaParser::ProcessType()
{
	bool retVal = true;

	QByteArray typeName;
	retVal = ReadToDelimeter("{",typeName);

	// first ensure it is not a query or mutation or subscription
	if (typeName == m_keywordMap[KI_QUERY]){
		return ProcessQuery();
	}
	if (typeName == m_keywordMap[KI_MUTATION]){
		return ProcessMutation();
	}
	if (typeName == m_keywordMap[KI_SUBSCRIPTION]){
		return ProcessSubscription();
	}

	// if not, process as common type
	CSdlType sdlType;
	sdlType.SetName(QString(typeName.trimmed()));

	// look for duplicates
	for (const CSdlType& aSdlType: std::as_const(m_sdlTypes)){
		if (aSdlType.GetName() == sdlType.GetName()){
			SendLogMessage(
				istd::IInformationProvider::InformationCategory::IC_ERROR,
				0,
				QString("Redifinition of '%1' at %2").arg(sdlType.GetName(), QString::number(m_lastReadLine)),
				"CGqlSchemaParser");

			return false;
		}
	}

	retVal = retVal && MoveToNextReadableSymbol();
	bool atEnd = false;
	SdlFieldList fieldList;
	while (!atEnd){
		ProcessValue(fieldList, &atEnd);
	}
	sdlType.SetFields(fieldList);
	sdlType.SetSchemaFilePath(m_originalSchemaFile);
	sdlType.SetSchemaParamsPtr(m_schemaParamsPtr);

	// maybe already added from another file
	if (!m_sdlTypes.contains(sdlType)){
		m_sdlTypes << sdlType;
	}

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

	bool retVal = true;

	QByteArray unionName;
	retVal = ReadToDelimeter("=", unionName);
	CSdlUnion currentUnion;
	currentUnion.SetName(unionName);
	currentUnion.SetSchemaParamsPtr(m_schemaParamsPtr);

	bool atEnd = false;
	while (!atEnd){
		retVal = retVal && MoveToNextReadableSymbol() && MoveInside();
		QByteArray unionValue;
		char foundDelimeter = '\n';
		retVal = retVal && ReadToDelimeterOrSpace("|", unionValue, &foundDelimeter);
		if (unionValue.isEmpty()){
			atEnd = bool(!retVal || foundDelimeter == '\n');
			continue;
		}
		currentUnion.AddType(unionValue);
		atEnd = bool(!retVal || foundDelimeter == '\n');
	}

	/// \todo make a common update method \c CreateMetainfo
	currentUnion.SetSchemaFilePath(m_originalSchemaFile);
	currentUnion.SetSchemaParamsPtr(m_schemaParamsPtr);

	m_unions.push_back(currentUnion);

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
	bool retVal = true;

	CSdlEnum currentEnum;
	currentEnum.SetSchemaParamsPtr(m_schemaParamsPtr);
	QByteArray enumName;
	retVal = ReadToDelimeterOrSpace("{",enumName);
	currentEnum.SetName(enumName);

	bool atEnd = false;
	while (!atEnd){
		retVal = retVal && MoveToNextReadableSymbol() && MoveInside();
		QByteArray enumValue;
		char foundDelimeter = '}';
		retVal = retVal && ReadToDelimeterOrSpace("}", enumValue, &foundDelimeter);
		if (enumValue.isEmpty()){
			atEnd = bool(!retVal || foundDelimeter == '}');
			continue;
		}
		QString convertedEnumValue = ConvertEnumValueToStringEquivalent(enumValue);

		currentEnum.AddValue(QPair<QString,QString>(QString(qPrintable(enumValue)), convertedEnumValue));
		currentEnum.SetSchemaFilePath(m_originalSchemaFile);
		currentEnum.SetSchemaParamsPtr(m_schemaParamsPtr);

		atEnd = bool(!retVal || foundDelimeter == '}');
	}

	m_enums << currentEnum;

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
	return ProcessRequests(CSdlRequest::T_QUERY);
}


bool CGqlSchemaParser::ProcessMutation()
{
	return ProcessRequests(CSdlRequest::T_MUTATION);
}


bool CGqlSchemaParser::ProcessSubscription()
{
	return ProcessRequests(CSdlRequest::T_SUBSCRIPTION);
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
		retVal = retVal && ReadToDelimeterOrSpace(",!})", valueType, &foundDelimeter);
		field.SetType(valueType);

		if (foundDelimeter == '!'){
			field.SetIsRequired(true);
		}

		if (foundDelimeter != ')'){
			retVal = retVal && MoveToNextReadableSymbol(&foundDelimeter);
		}
	}

	output << field;

	if (endOfReadPtr != nullptr){
		*endOfReadPtr = bool(foundDelimeter == '}' || foundDelimeter == ')');
	}

	Q_ASSERT(foundDelimeter == '}'|| foundDelimeter == ')' || QChar(foundDelimeter).isLetter());

	return retVal;
}


bool CGqlSchemaParser::ProcessRequests(CSdlRequest::Type type)
{
	bool retVal = true;

	QByteArray devNull;
	retVal = MoveToNextReadableSymbol();

	char foundDelimiter = ':';
	do {
		CSdlRequest request;
		request.SetType(type);

		// get name
		QByteArray requestName;
		QByteArray expectedDelimiters = "(";
		// Queries might not have arguments
		if (type == CSdlRequest::T_QUERY){
			expectedDelimiters.append(':');
		}
		retVal = retVal && ReadToDelimeter(expectedDelimiters, requestName, &foundDelimiter) && MoveToNextReadableSymbol();
		request.SetName(requestName.trimmed());

		for (const CSdlRequest& aSdlRequest: std::as_const(m_requests)){
			if (aSdlRequest.GetName() == request.GetName()){
				SendLogMessage(
					istd::IInformationProvider::InformationCategory::IC_ERROR,
					0,
					QString("Redifinition of '%1' at %2").arg(request.GetName(), QString::number(m_lastReadLine)),
					"CGqlSchemaParser");

				return false;
			}
		}


		SdlFieldList inputArguments;
		if (foundDelimiter != ':'){
			// extract input params
			bool atEnd = false;
			while (!atEnd){
				retVal = retVal && ProcessValue(inputArguments, &atEnd);
				Q_ASSERT(m_lastReadChar != '}');
			}
			request.SetInputArguments(inputArguments);
		}
		else if (type != CSdlRequest::T_QUERY){
			QString errorString = QString("Schema error! Request '%1' is not a Query and has not arguments at line %2")
			.arg(request.GetName(),
				 QString::number(m_lastReadLine + 1));

			SendLogMessage(
				istd::IInformationProvider::InformationCategory::IC_ERROR,
				0,
				errorString,
				"ProcessRequests");

			return false;
		}

		// set output
		if (foundDelimiter != ':'){
			retVal = retVal && ReadToDelimeter(":",devNull) && MoveToNextReadableSymbol(&foundDelimiter);
		}

		Q_ASSERT(foundDelimiter != '[');

		CSdlField outputArgument;
		QByteArray outputArgumentType;
		retVal = retVal && ReadToDelimeterOrSpace("!",outputArgumentType, &foundDelimiter);
		outputArgument.SetType(outputArgumentType);

		Q_ASSERT(foundDelimiter != ']');
		if (foundDelimiter == '!'){
			outputArgument.SetIsRequired(true);
		}
		request.SetOutputArgument(outputArgument);
		request.SetSchemaParamsPtr(m_schemaParamsPtr);
		request.SetSchemaFilePath(m_originalSchemaFile);

		// ensure, all arguments is valid
		SdlFieldList allArguments = inputArguments;
		allArguments << outputArgument;
		for (const CSdlField& argument: allArguments){
			bool isCustom = false;
			CSdlTools::ConvertType(argument, &isCustom);
			if (isCustom && !CSdlTools::EnsureFieldHasValidType(argument, m_sdlTypes, m_enums, m_unions)){
				QString errorString = QString("Schema error! Request '%1' has field '%2' with unknown type '%3' at line %4")
				.arg(request.GetName(),
					 argument.GetId(),
					 argument.GetType(),
					 QString::number(m_lastReadLine + 1));

				SendLogMessage(
					istd::IInformationProvider::InformationCategory::IC_ERROR,
					0,
					errorString,
					"ProcessRequests");

				return false;
			}
		}

		m_requests << request;

		retVal = retVal && MoveToNextReadableSymbol(&foundDelimiter);
	}
	while(foundDelimiter != '}');

	return retVal;
}


bool CGqlSchemaParser::ProcessCustomSchemaValue(const QString& /*key*/, const QString& /*value*/)
{
	return false;
}


bool CGqlSchemaParser::ProcessCustomSection(const QString& sectionName)
{
	QString errorString = QString("Schema error! Unsupported type '%1' at %2")
							  .arg(sectionName, QString::number(m_lastReadLine + 1));

	SendLogMessage(
				istd::IInformationProvider::InformationCategory::IC_ERROR,
				0,
				errorString,
				"ProcessCustomSection");

	return false;
}


bool CGqlSchemaParser::ValidateSchema()
{
	for (const CSdlType& sdlType: std::as_const(m_sdlTypes)){
		for (const CSdlField& sdlField: sdlType.GetFields()){
			bool isCustom = false;
			CSdlTools::ConvertType(sdlField, &isCustom);
			if (isCustom && !CSdlTools::EnsureFieldHasValidType(sdlField, m_sdlTypes, m_enums, m_unions)){
				QString errorString = QString("Schema error! Type '%1' has field '%2' with unknown type '%3'")
							.arg(sdlType.GetName(),
							sdlField.GetId(),
							sdlField.GetType());

				SendLogMessage(
							istd::IInformationProvider::InformationCategory::IC_ERROR,
							0,
							errorString,
							"ValidateSchema");

				return false;
			}
		}
	}

	for (const CSdlUnion& sdlUnion: std::as_const(m_unions)){
		for (const QString& unionName: sdlUnion.GetTypes()){
			bool isCustom = false;
			CSdlTools::ConvertType(unionName, &isCustom);
			if (isCustom && !CSdlTools::FindEntryByName(unionName, m_sdlTypes, m_enums, m_unions)){
				QString errorString = QString("Schema error! Union '%1' has variant '%2' with option type")
							.arg(sdlUnion.GetName(), unionName);

				SendLogMessage(
					istd::IInformationProvider::InformationCategory::IC_ERROR,
					0,
					errorString,
					"ValidateSchema");

				return false;
			}
		}
	}

	return true;
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
		qsizetype foundPosition = delimeters.indexOf(m_lastReadChar);
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
	newDelimeters.append('\t');
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
				<< QChar::Number_DecimalDigit
				<< QChar::Number_Letter
				<< QChar::Number_Other
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


bool CGqlSchemaParser::MoveInside()
{
	if (m_lastReadChar == '{'){
		return MoveToNextReadableSymbol();
	}

	return true;
}


bool CGqlSchemaParser::MoveAfterWord(const QString& word)
{
	if (word.isEmpty()){
		I_CRITICAL();

		return false;
	}

	QByteArray dummy;
	bool isFound = false;
	bool isRead = true;
	do {
		isRead = isRead && ReadToDelimeter(QByteArray(word.toUtf8()[0], 1), dummy, nullptr, false, false);
		for (int i = 1; i < word.length(); ++i){
			char expectedSymbol = word.toUtf8()[i];
			char actualSymbol = ' ';
			isRead = isRead && MoveToNextReadableSymbol(&actualSymbol, false);
			if (expectedSymbol != actualSymbol){
				break;
			}
			isFound = true;
		}
		dummy.clear();
	} while (!isRead || isFound);

	return isFound;
}


} // namespace imtsdl

