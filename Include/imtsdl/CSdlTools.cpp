#include <imtsdl/CSdlTools.h>


// STD includes
#include <ostream>

// Qt includes
#include <QtCore/QRegularExpression>

// ACF includes
#include <iprm/IParamsSet.h>
#include <iprm/ITextParam.h>
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <imtsdl/ISdlProcessArgumentsParser.h>
#include <imtsdl/CSdlField.h>
#include <imtsdl/CSdlType.h>
#include <imtsdl/CSdlRequest.h>


namespace imtsdl
{

// public static variables
const QString CSdlTools::s_variantMapClassMemberName = QStringLiteral("_m_settedFields"); // add first underscore to avoid ambiguity


// public static methods

QString CSdlTools::ConvertType(const CSdlField& sdlField, bool* isCustomPtr, bool* isComplexPtr, bool* isArrayPtr)
{
	QString retVal;
	if (sdlField.IsArray()){
		retVal += QStringLiteral("QList<");
		if (isArrayPtr != nullptr){
			*isArrayPtr = true;
		}
	}
	else {
		if (isArrayPtr != nullptr){
			*isArrayPtr = false;
		}
	}

	retVal += ConvertType(sdlField.GetType(), isCustomPtr, isComplexPtr);

	if (sdlField.IsArray()){
		retVal += QStringLiteral(">");
	}

	return retVal;
}


QString CSdlTools::ConvertType(const QString& sdlType, bool* isCustomPtr, bool* isComplexPtr)
{
	// A signed 32‐bit integer
	if (sdlType == QStringLiteral("Int") || sdlType == QStringLiteral("Integer")){
		if (isCustomPtr != nullptr){
			*isCustomPtr = false;
		}
		if (isComplexPtr != nullptr){
			*isComplexPtr = false;
		}

		return QStringLiteral("int");
	}

	if (sdlType == QStringLiteral("Long")){
		if (isCustomPtr != nullptr){
			*isCustomPtr = false;
		}
		if (isComplexPtr != nullptr){
			*isComplexPtr = false;
		}

		return QStringLiteral("long");
	}

	if (sdlType == QStringLiteral("LongLong")){
		if (isCustomPtr != nullptr){
			*isCustomPtr = false;
		}
		if (isComplexPtr != nullptr){
			*isComplexPtr = false;
		}

		return QStringLiteral("qlonglong");
	}

	// A signed double-precision floating-point value
	if (sdlType == QStringLiteral("Float")){
		if (isCustomPtr != nullptr){
			*isCustomPtr = false;
		}
		if (isComplexPtr != nullptr){
			*isComplexPtr = false;
		}

		return QStringLiteral("float");
	}

	// A signed double-precision floating-point value
	if (sdlType == QStringLiteral("Double")){
		if (isCustomPtr != nullptr){
			*isCustomPtr = false;
		}
		if (isComplexPtr != nullptr){
			*isComplexPtr = false;
		}

		return QStringLiteral("double");
	}

	// A UTF‐8 character sequence
	if (sdlType == QStringLiteral("String")){
		if (isCustomPtr != nullptr){
			*isCustomPtr = false;
		}
		if (isComplexPtr != nullptr){
			*isComplexPtr = true;
		}

		return QStringLiteral("QString");
	}

	// true or false
	if (sdlType == QStringLiteral("Boolean") || sdlType == QStringLiteral("Bool")){
		if (isCustomPtr != nullptr){
			*isCustomPtr = false;
		}
		if (isComplexPtr != nullptr){
			*isComplexPtr = false;
		}

		return QStringLiteral("bool");
	}

	// (serialized as a String): A unique identifier that's often used to refetch an object or as the key for a cache.
	if (sdlType == QStringLiteral("ID")){
		if (isCustomPtr != nullptr){
			*isCustomPtr = false;
		}
		if (isComplexPtr != nullptr){
			*isComplexPtr = true;
		}

		return QStringLiteral("QByteArray");
	}

	// custom type (another class)
	if (isCustomPtr != nullptr){
		*isCustomPtr = true;
	}
	if (isComplexPtr != nullptr){
		*isComplexPtr = true;
	}

	return QStringLiteral("C") + sdlType;
}


QString CSdlTools::QmlConvertType(const QString& sdlType, bool* isCustomPtr)
{
	// A signed 32‐bit integer
	if (sdlType == QStringLiteral("Int") || sdlType == QStringLiteral("Integer")){
		if (isCustomPtr != nullptr){
			*isCustomPtr = false;
		}

		return QStringLiteral("int");
	}

	// QML for long types uses 'real' type
	if (sdlType == QStringLiteral("Long")){
		if (isCustomPtr != nullptr){
			*isCustomPtr = false;
		}

		return QStringLiteral("real");
	}

	if (sdlType == QStringLiteral("LongLong")){
		if (isCustomPtr != nullptr){
			*isCustomPtr = false;
		}

		return QStringLiteral("real");
	}

	// A signed double-precision floating-point value
	if (sdlType == QStringLiteral("Float")){
		if (isCustomPtr != nullptr){
			*isCustomPtr = false;
		}

		return QStringLiteral("real");
	}

	// A signed double-precision floating-point value
	if (sdlType == QStringLiteral("Double")){
		if (isCustomPtr != nullptr){
			*isCustomPtr = false;
		}

		return QStringLiteral("double");
	}

	// A UTF‐8 character sequence
	if (sdlType == QStringLiteral("String")){
		if (isCustomPtr != nullptr){
			*isCustomPtr = false;
		}

		return QStringLiteral("string");
	}

	// true or false
	if (sdlType == QStringLiteral("Boolean") || sdlType == QStringLiteral("Bool")){
		if (isCustomPtr != nullptr){
			*isCustomPtr = false;
		}

		return QStringLiteral("bool");
	}

	// (serialized as a String): A unique identifier that's often used to refetch an object or as the key for a cache.
	if (sdlType == QStringLiteral("ID")){
		if (isCustomPtr != nullptr){
			*isCustomPtr = false;
		}

		return QStringLiteral("string");
	}

	// custom type (another class)
	if (isCustomPtr != nullptr){
		*isCustomPtr = true;
	}

	return GetCapitalizedValue(sdlType);
}


void CSdlTools::FeedStream(QTextStream& stream, uint lines, bool flush)
{
	for (uint i = 0; i < lines; ++i){
		stream << '\n';
	}

	if (flush){
		stream.flush();
	}
}


void CSdlTools::FeedLineHorizontally(QString& line, uint indents, char indentDelimiter)
{
	for (uint i = 0; i < indents; ++i){
		line += indentDelimiter;
	}
}


void CSdlTools::FeedStreamHorizontally(QTextStream& stream, uint indents, char indentDelimiter)
{
	for (uint i = 0; i < indents; ++i){
		stream << indentDelimiter;
	}
}


QString CSdlTools::GetCapitalizedValue(const QString& inputValue)
{
	if (inputValue.isEmpty()){
		return inputValue;
	}

	QString retVal = inputValue;
	retVal[0] = retVal[0].toUpper();

	return retVal;
}


QString CSdlTools::GetDecapitalizedValue(const QString& inputValue)
{
	if (inputValue.isEmpty()){
		return inputValue;
	}

	QString retVal = inputValue;
	retVal[0] = retVal[0].toLower();

	return retVal;
}


bool CSdlTools::IsTypeHasFundamentalTypes(const CSdlType& sdlType, QSet<QString>* foundTypesPtr)
{
	bool retVal = false;

	for(const CSdlField& sdlField: sdlType.GetFields()){
		bool isComplex = true;
		bool isArray = false;
		const QString convertedType = ConvertType(sdlField, nullptr, &isComplex, &isArray);
		isComplex = isComplex || isArray;

		if (foundTypesPtr != nullptr && !isComplex){
			foundTypesPtr->insert(convertedType);
		}

		retVal = retVal || !isComplex;
	}

	return retVal;
}


bool CSdlTools::IsTypeHasFundamentalTypes(const CSdlRequest& sdlRequest, QSet<QString>* foundTypesPtr)
{
	bool retVal = false;

	SdlFieldList fields;
	fields << sdlRequest.GetInputArguments();
	fields << sdlRequest.GetOutputArgument();

	for(const CSdlField& sdlField: fields){
		bool isComplex = true;
		bool isArray = false;
		const QString convertedType = ConvertType(sdlField, nullptr, &isComplex, &isArray);
		isComplex = isComplex || isArray;

		if (foundTypesPtr != nullptr && !isComplex){
			foundTypesPtr->insert(convertedType);
		}

		retVal = retVal || !isComplex;
	}

	return retVal;
}


bool CSdlTools::IsTypeHasNonFundamentalTypes(const CSdlType& sdlType, QSet<QString>* foundTypesPtr)
{
	bool retVal = false;

	for(const CSdlField& sdlField: sdlType.GetFields()){
		bool isComplex = false;
		bool isArray = false;
		ConvertType(sdlField, nullptr, &isComplex, &isArray);

		if (foundTypesPtr != nullptr && isComplex){
			foundTypesPtr->insert(ConvertType(sdlField.GetType()));
		}

		// add QList as non fundamental type (for includes)
		if (foundTypesPtr != nullptr && isArray){
			foundTypesPtr->insert(QStringLiteral("QList"));
		}

		retVal = retVal || isComplex;
	}

	return retVal;
}

bool CSdlTools::IsTypeHasNonFundamentalTypes(const CSdlRequest& sdlRequest, QSet<QString>* foundTypesPtr)
{
	bool retVal = false;

	SdlFieldList fields;
	fields << sdlRequest.GetInputArguments();
	fields << sdlRequest.GetOutputArgument();

	for(const CSdlField& sdlField: fields){
		bool isComplex = false;
		bool isArray = false;
		ConvertType(sdlField, nullptr, &isComplex, &isArray);

		if (foundTypesPtr != nullptr && isComplex){
			foundTypesPtr->insert(ConvertType(sdlField.GetType()));
		}

		// add QList as non fundamental type (for includes)
		if (foundTypesPtr != nullptr && isArray){
			foundTypesPtr->insert(QStringLiteral("QList"));
		}

		retVal = retVal || isComplex;
	}

	return retVal;
}

QString CSdlTools::GetFromVariantConversionString(const CSdlField& sdlField)
{
	const QString sdlType = sdlField.GetType();

	if (sdlType == QStringLiteral("Int") || sdlType == QStringLiteral("Integer")){
		return QStringLiteral("toInt()");
	}
	if (sdlType == QStringLiteral("Long")){
		return QStringLiteral("toLongLong()");
	}
	if (sdlType == QStringLiteral("LongLong")){
		return QStringLiteral("toLongLong()");
	}
	if (sdlType == QStringLiteral("Float")){
		return QStringLiteral("toFloat()");
	}
	if (sdlType == QStringLiteral("Double")){
		return QStringLiteral("toDouble()");
	}
	if (sdlType == QStringLiteral("String")){
		return QStringLiteral("toString()");
	}
	if (sdlType == QStringLiteral("Boolean") || sdlType == QStringLiteral("Bool")){
		return QStringLiteral("toBool()");
	}
	if (sdlType == QStringLiteral("ID")){
		return QStringLiteral("toByteArray()");
	}
	else if (sdlField.IsArray()){
		return QStringLiteral("toList()");
	}

	return "";
}


QString CSdlTools::GetFromVariantConversionStringExt(const CSdlField& sdlField, bool singleValue)
{
	const QString sdlType = sdlField.GetType();

	if (!singleValue && sdlField.IsArray()){
		return QStringLiteral("toList()");
	}

	if (sdlType == QStringLiteral("Int") || sdlType == QStringLiteral("Integer")){
		return QStringLiteral("toInt()");
	}
	if (sdlType == QStringLiteral("Long")){
		return QStringLiteral("toLongLong()");
	}
	if (sdlType == QStringLiteral("LongLong")){
		return QStringLiteral("toLongLong()");
	}
	if (sdlType == QStringLiteral("Float")){
		return QStringLiteral("toFloat()");
	}
	if (sdlType == QStringLiteral("Double")){
		return QStringLiteral("toDouble()");
	}
	if (sdlType == QStringLiteral("String")){
		return QStringLiteral("toString()");
	}
	if (sdlType == QStringLiteral("Boolean") || sdlType == QStringLiteral("Bool")){
		return QStringLiteral("toBool()");
	}
	if (sdlType == QStringLiteral("ID")){
		return QStringLiteral("toByteArray()");
	}

	return "";
}


QString CSdlTools::FromInternalMapCheckString(const CSdlField& sdlField)
{
	QString retVal = s_variantMapClassMemberName;
	retVal += QStringLiteral(".contains(");
	retVal += '"';
	retVal += sdlField.GetId();
	retVal += '"';
	retVal += ')';

	return retVal;
}


bool CSdlTools::GetSdlTypeForField(const CSdlField& sdlField, const SdlTypeList& typeList, CSdlType& sdlType)
{
	for (const CSdlType& type: typeList){
		if (type.GetName() == sdlField.GetType()){
			sdlType.SetFields(type.GetFields());
			sdlType.SetName(type.GetName());

			return true;
		}
	}

	return false;
}



void CSdlTools::AddSelfCheckRequiredValueCode(QTextStream& stream, const CSdlField& field, uint hIndents)
{
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("if (!") << FromInternalMapCheckString(field);
	stream << QStringLiteral("){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("return false;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents);
	stream << '}';
	FeedStream(stream, 1, false);
}


void CSdlTools::AddBeginSelfCheckNonRequiredValueCode(QTextStream& stream, const CSdlField& field, uint hIndents)
{
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("if (") << FromInternalMapCheckString(field);
	stream << QStringLiteral("){");
	FeedStream(stream, 1, false);
}


void CSdlTools::AddArrayInternalChecksFail(QTextStream& stream, const CSdlField& field, bool checkEmpty, uint hIndents)
{
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("if (!");
	stream << FromInternalMapCheckString(field);
	if (checkEmpty){
		stream << QStringLiteral(" || m_");
		stream << GetDecapitalizedValue(field.GetId());
		stream << QStringLiteral(".isEmpty()");
	}
	stream << QStringLiteral("){");
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("return false;\n\t}");
	FeedStream(stream, 1, false);
}


QString CSdlTools::GetNamespaceAcceptableString(const QString& originalText)
{
	QString retVal = originalText;

	static QRegularExpression nonWordRegexp("[^\\w\\:]");

	retVal.replace(nonWordRegexp, QStringLiteral("_"));

	return retVal;
}


QString CSdlTools::GetFileSystemAcceptableEntryPath(const QString& originalText)
{
	QString retVal = originalText;

	retVal.replace('*', '_');
	retVal.replace('?', '_');
	retVal.replace('\"', '_');
	retVal.replace('<', '_');
	retVal.replace('>', '_');
	retVal.replace('|', '_');

	while(retVal.endsWith('.')){
		retVal.chop(1);
	}

	return retVal;
}


QString CSdlTools::BuildNamespaceFromParams(const iprm::IParamsSet& schemaParams)
{
	QString retVal;

	iprm::TParamsPtr<iprm::ITextParam> namespaceParamPtr(&schemaParams, SdlCustomSchemaKeys::SchemaNamespace.toUtf8(), false);
	if (namespaceParamPtr.IsValid()){
		retVal += namespaceParamPtr->GetText();
	}

	iprm::TParamsPtr<iprm::ITextParam> nameParamPtr(&schemaParams, SdlCustomSchemaKeys::SchemaName.toUtf8(), false);
	if (nameParamPtr.IsValid()){
		if (!retVal.isEmpty()){
			retVal += QStringLiteral("::");
		}
		retVal += nameParamPtr->GetText();
	}

	iprm::TParamsPtr<iprm::ITextParam> versionNameParamPtr(&schemaParams, SdlCustomSchemaKeys::VersionName.toUtf8(), false);
	if (versionNameParamPtr.IsValid()){
		if (!retVal.isEmpty()){
			retVal += QStringLiteral("::V");
		}
		retVal += versionNameParamPtr->GetText();
	}

	return GetNamespaceAcceptableString(retVal);
}


QString CSdlTools::GetNamespaceFromParamsOrArguments(
			const SchemaParamsCompPtr& schemaParamsCompPtr,
			const ArgumentParserCompPtr& argumentParamsCompPtr)
{
	QString sdlNamespace;
	if (schemaParamsCompPtr.IsValid()){
		sdlNamespace = BuildNamespaceFromParams(*schemaParamsCompPtr);
		if (argumentParamsCompPtr.IsValid()){
			QString namespacePrefix =argumentParamsCompPtr->GetNamespacePrefix();
			if (!namespacePrefix.isEmpty()){
				while (!namespacePrefix.endsWith(QStringLiteral("::"))){
					namespacePrefix.append(':');
				}
				sdlNamespace.prepend(namespacePrefix);
			}
		}
	}

	// we should override nmespace from agrs if it porvided
	if (argumentParamsCompPtr.IsValid()){
		QString argNamespace = argumentParamsCompPtr->GetNamespace();
		if (!argNamespace.isEmpty()){
			sdlNamespace = argNamespace;
		}
	}

	Q_ASSERT(!sdlNamespace.isEmpty());

	return sdlNamespace;
}


QMap<QString, QString> CSdlTools::GetAutoJoinCppFilesSchema(const iprm::IParamsSet& schemaParams, const QString& baseDirPath, const QString defaultName)
{
	QMap<QString, QString> retVal;

	QString baseFilePath = baseDirPath;

	iprm::TParamsPtr<iprm::ITextParam> versionNameParamPtr(&schemaParams, SdlCustomSchemaKeys::VersionName.toUtf8(), false);
	if (versionNameParamPtr.IsValid()){
		if (!baseFilePath.endsWith('/')){
			baseFilePath += '/';
		}
		baseFilePath += versionNameParamPtr->GetText();
	}

	if (!baseFilePath.endsWith('/')){
		baseFilePath += '/';
	}
	baseFilePath += QStringLiteral("CPP/");

	iprm::TParamsPtr<iprm::ITextParam> nameParamPtr(&schemaParams, SdlCustomSchemaKeys::SchemaName.toUtf8(), false);
	if (nameParamPtr.IsValid()){
		baseFilePath += nameParamPtr->GetText();
	}
	else {
		baseFilePath += defaultName;
	}

	retVal.insert(ISdlProcessArgumentsParser::s_headerFileType, GetFileSystemAcceptableEntryPath(baseFilePath + QStringLiteral(".h")));
	retVal.insert(ISdlProcessArgumentsParser::s_sourceFileType, GetFileSystemAcceptableEntryPath(baseFilePath + QStringLiteral(".cpp")));

	return retVal;
}


QStringList CSdlTools::GetAutoJoinedCppFilePaths(const iprm::IParamsSet& schemaParams, const QString& baseDirPath, const QString defaultName)
{
	return GetAutoJoinCppFilesSchema(schemaParams, baseDirPath, defaultName).values();
}


QString CSdlTools::GetQmlModuleNameFromParamsOrArguments(
			const iprm::IParamsSet* schemaParamsPtr,
			const ISdlProcessArgumentsParser* argumentParamsPtr)
{
	QString retVal;

	if (schemaParamsPtr != nullptr){
		QString qrcFileName;
		iprm::TParamsPtr<iprm::ITextParam> namespaceParamPtr(schemaParamsPtr, SdlCustomSchemaKeys::SchemaNamespace.toUtf8(), false);
		if (namespaceParamPtr.IsValid()){
			qrcFileName += namespaceParamPtr->GetText();
		}

		iprm::TParamsPtr<iprm::ITextParam> nameParamPtr(&*schemaParamsPtr, SdlCustomSchemaKeys::SchemaName.toUtf8(), false);
		if (nameParamPtr.IsValid()){
			qrcFileName += nameParamPtr->GetText();
		}
		retVal += qrcFileName;
		retVal += QStringLiteral("Sdl");
	}

	// we should override it from nmespace from agrs if it porvided
	if (argumentParamsPtr != nullptr){
		QString argNamespace = argumentParamsPtr->GetNamespace();
		if (!argNamespace.isEmpty()){
			retVal = argNamespace;
		}
	}

	return retVal;
}


QString CSdlTools::GetQmlModuleNameFromParamsOrArguments(
			const SchemaParamsCompPtr& schemaParamsCompPtr,
			const ArgumentParserCompPtr& argumentParamsCompPtr)
{
	const iprm::IParamsSet* schemaParamsPtr = nullptr;
	if (schemaParamsCompPtr.IsValid()){
		schemaParamsPtr = schemaParamsCompPtr.GetPtr();
	}
	const ISdlProcessArgumentsParser* argumentParamsPtr = nullptr;
	if(argumentParamsCompPtr.IsValid()){
		argumentParamsPtr = argumentParamsCompPtr.GetPtr();
	}

	return GetQmlModuleNameFromParamsOrArguments(schemaParamsPtr, argumentParamsPtr);
}



QString CSdlTools::GetAutoDefinedQmlBasePath(const iprm::IParamsSet& schemaParams, const QString& baseDirPath)
{
	QString baseFilePath = baseDirPath;
	if (!baseFilePath.endsWith('/')){
		baseFilePath += '/';
	}

	iprm::TParamsPtr<iprm::ITextParam> versionNameParamPtr(&schemaParams, SdlCustomSchemaKeys::VersionName.toUtf8(), false);
	if (versionNameParamPtr.IsValid()){
		if (!baseFilePath.endsWith('/')){
			baseFilePath += '/';
		}
		baseFilePath += versionNameParamPtr->GetText();
	}

	if (!baseFilePath.endsWith('/')){
		baseFilePath += '/';
	}
	baseFilePath += QStringLiteral("QML/");

	const QString qmlModuleName = GetQmlModuleNameFromParamsOrArguments(&schemaParams, nullptr);
	baseFilePath += qmlModuleName;

	QString retVal;
	retVal = GetFileSystemAcceptableEntryPath(baseFilePath);

	return retVal;
}


QString CSdlTools::GetAutoDefinedQmlQrcFilePath(const iprm::IParamsSet& schemaParamsCompPtr, const QString& baseDirPath)
{
	QString baseFilePath = GetAutoDefinedQmlBasePath(schemaParamsCompPtr, baseDirPath);
	if (!baseFilePath.endsWith('/')){
		baseFilePath += '/';
	}

	QString qrcFileName = GetQmlModuleNameFromParamsOrArguments(&schemaParamsCompPtr, nullptr);

	QString retVal;
	retVal = GetFileSystemAcceptableEntryPath(baseFilePath + qrcFileName + QStringLiteral(".qrc"));

	return retVal;
}


void CSdlTools::GenerateListTempValueCode(QTextStream& stream, const CSdlField& sdlField, QString& tempVariableName, uint indents)
{
	if (!sdlField.IsArray()){
		// nothing todo
		return;
	}

	tempVariableName = QStringLiteral("temp") + GetCapitalizedValue(sdlField.GetId()) + QStringLiteral("List");

	FeedStreamHorizontally(stream, indents);
	stream << QStringLiteral("QVariantList ") << tempVariableName << ';';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, indents);
	stream << QStringLiteral("for (const auto& tempValue: std::as_const(m_");
	stream << GetDecapitalizedValue(sdlField.GetId());
	stream << QStringLiteral(")){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, indents + 1);
	stream << tempVariableName << QStringLiteral(" << QVariant::fromValue(tempValue);");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, indents);
	stream << '}';
	FeedStream(stream, 1, false);
}


void CSdlTools::PrintFiles(std::ostream& outStream, const QStringList& files, ISdlProcessArgumentsParser::GeneratorType projectCodeGenerator)
{
	for (const QString& file: files){
		outStream << file.toStdString();
		if (projectCodeGenerator == ISdlProcessArgumentsParser::GT_CMAKE){
			outStream << std::endl;
		}
		if (projectCodeGenerator == ISdlProcessArgumentsParser::GT_CMAKE_PIPE){
			outStream << ';';
		}
		else {
			outStream << ' ';
		}
	}

	outStream.flush();
}


} // namespace imtsdl
