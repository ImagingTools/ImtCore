#include <imtsdl/CGqlExtSchemaParser.h>


// ACF includes
#include <iprm/CTextParam.h>

// ImtCore includes
#include <imtsdl/CSdlTools.h>
#include <imtsdl/CSdlEnumConverter.h>



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

bool CGqlExtSchemaParser::ProcessCollectionSchema()
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
			SendLogMessage(
						istd::IInformationProvider::IC_ERROR,
						0,
						QString("Unexpected directive '%1' at %2").arg(typeName, QString::number(m_lastReadLine + 1)),
						__func__);
			I_CRITICAL();

			return false;
		}

		// brgin type entry
		retVal = retVal && ReadToDelimeter("{", typeName);

		CSdlDocumentType documentType;
		documentType.SetName(typeName.trimmed());

		for (const CSdlRequest& aDocumentType: std::as_const(m_requests)){
			if (aDocumentType.GetName() == documentType.GetName()){
				SendLogMessage(
					istd::IInformationProvider::InformationCategory::IC_ERROR,
					0,
					QString("Redifinition of '%1' at %2").arg(documentType.GetName(), QString::number(m_lastReadLine)),
					"CGqlSchemaParser");

				return false;
			}
		}

		retVal = retVal && ExtractDocumentTypeFromCurrentEntry(documentType);
		documentType.SetSchemaFilePath(m_originalSchemaFile);
		documentType.SetSchemaParamsPtr(m_schemaParamsPtr);
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
			if (typeRefName.isEmpty()){
				SendLogMessage(
							istd::IInformationProvider::IC_ERROR,
							0,
							QString("Reference for '%1' at %2").arg(documentType.GetName(), QString::number(m_lastReadLine + 1)),
							__func__);
				
				return false;
			}

			auto foundIterator = std::find_if(m_sdlTypes.cbegin(), m_sdlTypes.cend(), [&typeRefName](const CSdlType& type){
				return (type.GetName() == typeRefName);
			});
			if (foundIterator == m_sdlTypes.cend()){
				SendLogMessage(
							istd::IInformationProvider::IC_ERROR,
							0,
							QString("Unable to find type '%1' at %2").arg(typeRefName, QString::number(m_lastReadLine + 1)),
							__func__);

				return false;
			}
			documentType.SetReferenceType(*foundIterator);
		}
		// extract operations
		else if (keyword == QByteArrayLiteral("operations")){
			while (!m_stream.atEnd()){
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
					SendLogMessage(
								istd::IInformationProvider::IC_ERROR,
								0,
								QString("Unexpected operation type :'%1' at %2").arg(operationTypeId, QString::number(m_lastReadLine + 1)),
								__func__);
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
					SendLogMessage(
								istd::IInformationProvider::IC_ERROR,
								0,
								QString("Unable to find request '%1' at %2").arg(requestName, QString::number(m_lastReadLine + 1)),
								__func__);
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
			documentSubtype.SetName(subtypeName.trimmed());

			retVal = retVal && ExtractDocumentTypeFromCurrentEntry(documentSubtype);
			if (retVal){
				documentType.AddSubtype(documentSubtype);
			}
		}
		else {
			SendLogMessage(
						istd::IInformationProvider::IC_ERROR,
						0,
						QString("Unexpected directive '%1' at %2").arg(keyword, QString::number(m_lastReadLine + 1)),
						__func__);
			I_CRITICAL();

			return false;
		}
	}

	return retVal;
}


// reimplemented (CGqlSchemaParser)

bool CGqlExtSchemaParser::ProcessCustomSection(const QString& sectionName)
{
	if (sectionName == QStringLiteral("collectionSchema")){
		return ProcessCollectionSchema();
	}

	return BaseClass::ProcessCustomSection(sectionName);
}


bool CGqlExtSchemaParser::ValidateSchema()
{
	if (!BaseClass::ValidateSchema()){
		return false;
	}

	// all document types MUST have a reference
	for (const CSdlDocumentType& sdlDocumentType: m_documentTypes){
		if (sdlDocumentType.GetReferenceType().GetName().isEmpty()){
			SendLogMessage(
				istd::IInformationProvider::IC_ERROR,
				0,
				"Document type '%1' does not have a reference! define 'ref' for it.",
				"ValidateDocumentSchema");
			return false;
		}
	}

	const QString fieldNamespace = CSdlTools::BuildNamespaceFromParams(*m_schemaParamsPtr);
	/// \todo don't use a suffix. Instead, define it elsewhere...
	const QString fieldQmlImportDeclaration = CSdlTools::BuildQmlImportDeclarationFromParams(*m_schemaParamsPtr, QStringLiteral("Sdl"));
	// set namespace and QML import for all types
	for (CSdlType& sdlType: m_sdlTypes){
		if (!sdlType.IsExternal() || sdlType.GetNamespace().isEmpty()){
			sdlType.SetNamespace(fieldNamespace);
		}

		if (!sdlType.IsExternal() || sdlType.GetQmlImportDeclaration().isEmpty()){
			sdlType.SetQmlImportDeclaration(fieldQmlImportDeclaration);
		}
	}

	return true;
}


bool CGqlExtSchemaParser::ProcessCustomSchemaValue(const QString& key, const QString& value)
{
	static QStringList acceptableKeys = {
				SdlCustomSchemaKeys::SchemaName,
				SdlCustomSchemaKeys::SchemaNamespace,
				SdlCustomSchemaKeys::VersionName
	};

	// setup default enum conversion
	if (key == SdlCustomSchemaKeys::EnumConversionType){
		if (value == EnumConversionTypes::AsIs){
			CSdlEnumConverter::s_defaultConversionType = CSdlEnumConverter::CT_AS_IS;
		}
		else if (value == EnumConversionTypes::UpperCamel){
			CSdlEnumConverter::s_defaultConversionType = CSdlEnumConverter::CT_UPPER_CAMEL_CASE;
		}
		else if (value == EnumConversionTypes::LowerCamel){
			CSdlEnumConverter::s_defaultConversionType = CSdlEnumConverter::CT_LOWER_CAMEL_CASE;
		}
		else if (value == EnumConversionTypes::UpperSnake){
			CSdlEnumConverter::s_defaultConversionType = CSdlEnumConverter::CT_UPPER_SNAKE_CASE;
		}
		else if (value == EnumConversionTypes::LowerSnake){
			CSdlEnumConverter::s_defaultConversionType = CSdlEnumConverter::CT_LOWER_SNAKE_CASE;
		}
		else if (value == EnumConversionTypes::UpperKebab){
			CSdlEnumConverter::s_defaultConversionType = CSdlEnumConverter::CT_UPPER_KEBAB_CASE;
		}
		else if (value == EnumConversionTypes::LowerKebab){
			CSdlEnumConverter::s_defaultConversionType = CSdlEnumConverter::CT_LOWER_KEBAB_CASE;
		}
		else {
			SendLogMessage(
				istd::IInformationProvider::IC_ERROR,
				0,
				QString("Unexpected enumConversion type. Actual: '%1'. Expected one of: {%2}.").arg(
							value,
							QStringList({
										EnumConversionTypes::AsIs,
										EnumConversionTypes::UpperCamel,
										EnumConversionTypes::LowerCamel,
										EnumConversionTypes::UpperSnake,
										EnumConversionTypes::LowerSnake,
										EnumConversionTypes::UpperKebab,
										EnumConversionTypes::LowerKebab}).join("|")),
				__func__);

			return false;
		}

		return true;
	}

	if (acceptableKeys.contains(key)){
		iprm::CTextParam* nameParamPtr = new iprm::CTextParam;
		nameParamPtr->SetText(value);
		const bool isSet = m_schemaParamsPtr->SetEditableParameter(key.toUtf8(), nameParamPtr, true);
		Q_ASSERT(isSet);

		return isSet;
	}

	return BaseClass::ProcessCustomSchemaValue(key, value);
}



} // namespace imtsdl

