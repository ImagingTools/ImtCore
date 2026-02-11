// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtsdl/CSdlTools.h>


// STD includes
#include <ostream>

// Qt includes
#include <QtCore/QDirIterator>
#include <QtCore/QRegularExpression>

// ACF includes
#include <iprm/IParamsSet.h>
#include <iprm/ITextParam.h>
#include <iprm/TParamsPtr.h>
#include <ifile/IFileNameParam.h>

// ImtCore includes
#include <imtsdl/ISdlProcessArgumentsParser.h>
#include <imtsdl/CSdlField.h>
#include <imtsdl/CSdlType.h>
#include <imtsdl/CSdlRequest.h>
#include <imtsdl/CSdlEnum.h>


namespace imtsdl
{

//static helpers
static std::function<bool(const CSdlEntryBase&)> CreateFindByNamePredicate(const QString& name)
{
	return [name](const CSdlEntryBase& entry){
		return entry.GetName() == name;
	};
}


// public static variables
QString CSdlTools::s_sdlGlobalPrefix = QStringLiteral("sdl");


// public static methods


void CSdlTools::WrapTypeToList(QString& text)
{
	text.prepend(QStringLiteral("imtsdl::TElementList<"));
	text.append(">");
}


QString CSdlTools::ConvertTypeWithNamespace(
			const CSdlField& sdlField,
			const QString& relatedNamespace,
			const ISdlTypeListProvider& listProvider,
			const ISdlEnumListProvider& enumProvider,
			const ISdlUnionListProvider& unionProvider,
			bool* isCustomPtr,
			bool* isComplexPtr,
			bool* isArrayPtr,
			bool* isEnumPtr,
			bool* isUnionPtr)
{
	return OptListConvertTypeWithNamespace(
				sdlField,
				relatedNamespace,
				listProvider,
				enumProvider,
				unionProvider,
				true,
				isCustomPtr,
				isComplexPtr,
				isArrayPtr,
				isEnumPtr,
				isUnionPtr);
}


QString CSdlTools::OptListConvertTypeWithNamespace(
			const CSdlField& sdlField,
			const QString& relatedNamespace,
			const ISdlTypeListProvider& listProvider,
			const ISdlEnumListProvider& enumProvider,
			const ISdlUnionListProvider& unionProvider,
			bool listWrap,
			bool* isCustomPtr,
			bool* isComplexPtr,
			bool* isArrayPtr,
			bool* isEnumPtr,
			bool* isUnionPtr)
{
	bool _isCustom = false;
	QString retVal = ConvertTypeOrEnumOrUnion(sdlField, enumProvider.GetEnums(false), unionProvider.GetUnions(false), &_isCustom, isComplexPtr, isArrayPtr, isEnumPtr, isUnionPtr);
	if (isCustomPtr != nullptr){
		*isCustomPtr = _isCustom;
	}
	if (isArrayPtr != nullptr){
		*isArrayPtr = sdlField.IsArray();
	}

	if (!_isCustom){
		// we can define namespace only for custom types
		if (listWrap && sdlField.IsArray()){
			WrapTypeToList(retVal);
		}
		return retVal;
	}

	if (!relatedNamespace.isEmpty()){
		CSdlType typeForField;
		CSdlEnum enumForField;
		CSdlUnion unionForField;
		const bool isType = GetSdlTypeForField(sdlField, listProvider.GetSdlTypes(false), typeForField);
		[[maybe_unused]] const bool isEnum = GetSdlEnumForField(sdlField, enumProvider.GetEnums(false), enumForField);
		[[maybe_unused]] const bool isUnion = GetSdlUnionForField(sdlField, unionProvider.GetUnions(false), unionForField);

		Q_ASSERT(isType || isEnum || isUnion);

		QString typeNamespace;
		if (isType){
			typeNamespace = BuildNamespaceFromParams(typeForField.GetSchemaParams(), false);
		}
		else if (isEnum){
			typeNamespace = BuildNamespaceFromParams(enumForField.GetSchemaParams(), false);
		}
		else if (isUnion){
			typeNamespace = BuildNamespaceFromParams(unionForField.GetSchemaParams(), false);
		}
		if (typeNamespace != relatedNamespace){
			while (!typeNamespace.endsWith(QStringLiteral("::"))){
				typeNamespace.append(':');
			}
			retVal.prepend(typeNamespace);
			// use global namespace
			if (!retVal.startsWith(QStringLiteral("::")) && !isUnion && !isEnum){
				retVal.prepend(QStringLiteral("::"));
			}
		}
	}

	if (listWrap && sdlField.IsArray()){
		WrapTypeToList(retVal);
	}

	return retVal;
}


QString CSdlTools::ResolveRelativePath(const QString& targetPath, const QStringList& lookupPaths)
{
	if (targetPath.isEmpty() || lookupPaths.isEmpty()){
		return targetPath;
	}

	for (const QString& path: lookupPaths){
		QString cleanPath = QDir::cleanPath(path);
		QString cleanTargetPath = QDir::cleanPath(targetPath);
#ifdef Q_OS_WIN
		cleanTargetPath = cleanTargetPath.toLower();
		cleanPath = cleanPath.toLower();
#endif
		if (cleanTargetPath.startsWith(cleanPath)){
			QDir currentDir(path);

			return currentDir.relativeFilePath(cleanTargetPath);
		}
	}

	return targetPath;
}

QString CSdlTools::ConvertType(const CSdlField& sdlField, bool* isCustomPtr, bool* isComplexPtr, bool* isArrayPtr)
{
	return ConvertTypeOrEnum(sdlField, SdlEnumList(), isCustomPtr, isComplexPtr, isArrayPtr);
}


QString CSdlTools::ConvertType(const QString& sdlTypeName, bool* isCustomPtr, bool* isComplexPtr)
{
	// A signed 32‐bit integer
	if (sdlTypeName == QStringLiteral("Int") || sdlTypeName == QStringLiteral("Integer")){
		if (isCustomPtr != nullptr){
			*isCustomPtr = false;
		}
		if (isComplexPtr != nullptr){
			*isComplexPtr = false;
		}

		return QStringLiteral("int");
	}

	if (sdlTypeName == QStringLiteral("Long")){
		if (isCustomPtr != nullptr){
			*isCustomPtr = false;
		}
		if (isComplexPtr != nullptr){
			*isComplexPtr = false;
		}

		return QStringLiteral("long");
	}

	if (sdlTypeName == QStringLiteral("LongLong")){
		if (isCustomPtr != nullptr){
			*isCustomPtr = false;
		}
		if (isComplexPtr != nullptr){
			*isComplexPtr = false;
		}

		return QStringLiteral("qlonglong");
	}

	// A signed double-precision floating-point value
	if (sdlTypeName == QStringLiteral("Float")){
		if (isCustomPtr != nullptr){
			*isCustomPtr = false;
		}
		if (isComplexPtr != nullptr){
			*isComplexPtr = false;
		}

		return QStringLiteral("double");
	}

	// A signed double-precision floating-point value
	if (sdlTypeName == QStringLiteral("Double")){
		if (isCustomPtr != nullptr){
			*isCustomPtr = false;
		}
		if (isComplexPtr != nullptr){
			*isComplexPtr = false;
		}

		return QStringLiteral("double");
	}

	// A UTF‐8 character sequence
	if (sdlTypeName == QStringLiteral("String")){
		if (isCustomPtr != nullptr){
			*isCustomPtr = false;
		}
		if (isComplexPtr != nullptr){
			*isComplexPtr = true;
		}

		return QStringLiteral("QString");
	}

	// true or false
	if (sdlTypeName == QStringLiteral("Boolean") || sdlTypeName == QStringLiteral("Bool")){
		if (isCustomPtr != nullptr){
			*isCustomPtr = false;
		}
		if (isComplexPtr != nullptr){
			*isComplexPtr = false;
		}

		return QStringLiteral("bool");
	}

	// (serialized as a String): A unique identifier that's often used to refetch an object or as the key for a cache.
	if (sdlTypeName == QStringLiteral("ID")){
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

	return QStringLiteral("C") + sdlTypeName;
}


QString CSdlTools::ConvertTypeOrEnum(const CSdlField& sdlField, const SdlEnumList& enumList, bool* isCustomPtr, bool* isComplexPtr, bool* isArrayPtr, bool* isEnumPtr)
{
	return ConvertTypeOrEnumOrUnion(sdlField, enumList, SdlUnionList(), isCustomPtr, isComplexPtr, isArrayPtr, isEnumPtr);
}

QString CSdlTools::ConvertTypeOrEnumOrUnion(const CSdlField& sdlField,
	const SdlEnumList& enumList,
	const SdlUnionList& unionList,
	bool* isCustomPtr,
	bool* isComplexPtr,
	bool* isArrayPtr,
	bool* isEnumPtr,
	bool* isUnion)
{
	if (isEnumPtr != nullptr){
		*isEnumPtr = false;
	}

	if (isUnion != nullptr){
		*isUnion = false;
	}

	QString retVal;
	if (sdlField.IsArray()){
		if (isArrayPtr != nullptr){
			*isArrayPtr = true;
		}
	}
	else {
		if (isArrayPtr != nullptr){
			*isArrayPtr = false;
		}
	}

	bool isComplex = false;

	QString fieldTypeName = ConvertType(sdlField.GetType(), isCustomPtr, &isComplex);

	// maybe it is a enum
	if (isComplex && !enumList.isEmpty()){
		CSdlEnum foundEnum;
		const bool found = GetSdlEnumForField(sdlField, enumList, foundEnum);
		if (found){
			fieldTypeName = foundEnum.GetName();
			isComplex = false;
			if (isEnumPtr != nullptr){
				*isEnumPtr = true;
			}
		}
	}

	if (isComplex && !unionList.isEmpty()){
		CSdlUnion foundUnion;
		const bool found = GetSdlUnionForField(sdlField, unionList, foundUnion);
		if (found){
			fieldTypeName = foundUnion.GetName();
			isComplex = false;
			if (isUnion != nullptr){
				*isUnion = true;
			}
		}
	}

	retVal += fieldTypeName;

	if (isComplexPtr != nullptr){
		*isComplexPtr = isComplex;
	}

	return retVal;
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
	if (sdlField.IsArray()){
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


bool CSdlTools::EnsureFieldHasValidType(const CSdlField& sdlField, const SdlTypeList& typeList, const SdlEnumList& enumList, const SdlUnionList& unionList)
{
	CSdlType dummyType;
	CSdlEnum dummyEnum;
	CSdlUnion dummyUnion;

	return GetSdlTypeForField(sdlField, typeList, dummyType) || GetSdlEnumForField(sdlField, enumList, dummyEnum) || GetSdlUnionForField(sdlField, unionList, dummyUnion);
}


bool CSdlTools::GetSdlTypeForField(const CSdlField& sdlField, const SdlTypeList& typeList, CSdlType& sdlType)
{
	for (const CSdlType& type: typeList){
		if (type.GetName() == sdlField.GetType()){
			sdlType = type;

			return true;
		}
	}

	return false;
}


bool CSdlTools::GetSdlEnumForField(const CSdlField& sdlField, const SdlEnumList& enumList, CSdlEnum& sdlEnum)
{
	for (const CSdlEnum& enumElement: enumList){
		if (enumElement.GetName() == sdlField.GetType()){
			sdlEnum = enumElement;

			return true;
		}
	}

	return false;
}


bool CSdlTools::GetSdlUnionForField(const CSdlField& sdlField, const SdlUnionList& unionList, CSdlUnion& sdlUnion)
{
	for (const CSdlUnion& unionElement : unionList){
		if (unionElement.GetName() == sdlField.GetType()){
			sdlUnion = unionElement;

			return true;
		}
	}

	return false;
}


std::shared_ptr<CSdlEntryBase> CSdlTools::GetSdlTypeOrEnumOrUnionForField(
	const CSdlField& sdlField,
	const SdlTypeList& typeList,
	const SdlEnumList& enumList,
	const SdlUnionList& unionList)
{
	std::shared_ptr<CSdlEntryBase> retVal;

	std::shared_ptr<CSdlType> typePtr(new CSdlType);
	const bool isType = GetSdlTypeForField(sdlField, typeList, *typePtr);
	if (isType){
		retVal = typePtr;
	}
	else{
		std::shared_ptr<CSdlEnum> sdlEnumPtr(new CSdlEnum);
		const bool isEnum = GetSdlEnumForField(sdlField, enumList, *sdlEnumPtr);
		if (isEnum){
			retVal = sdlEnumPtr;
		}
		else{
			std::shared_ptr<CSdlUnion> sdlUnionPtr(new CSdlUnion);
			const bool isUnion = GetSdlUnionForField(sdlField, unionList, *sdlUnionPtr);
			if (isUnion){
				retVal = sdlUnionPtr;
			}
		}
	}

	return retVal;
}


std::shared_ptr<CSdlEntryBase> CSdlTools::FindEntryByName(
			const QString& entryName,
			const SdlTypeList& typeList,
			const SdlEnumList& enumList,
			const SdlUnionList& unionList)
{
	std::shared_ptr<CSdlEntryBase> retVal;

	// if type
	SdlTypeList::const_iterator typeFountIter =
		std::find_if(typeList.cbegin(), typeList.cend(), CreateFindByNamePredicate(entryName));

	if (typeFountIter != typeList.cend()){
		retVal.reset(new CSdlType(*typeFountIter));

		return retVal;
	}

	// if enum
	SdlEnumList::const_iterator enumFoundIter =
		std::find_if(enumList.cbegin(), enumList.cend(), CreateFindByNamePredicate(entryName));

	if (enumFoundIter != enumList.cend()){
		retVal.reset(new CSdlEnum(*enumFoundIter));

		return retVal;
	}

	// if union
	SdlUnionList::const_iterator unionFoundIter =
		std::find_if(unionList.cbegin(), unionList.cend(), CreateFindByNamePredicate(entryName));

	if (unionFoundIter != unionList.cend()){
		retVal.reset(new CSdlUnion(*unionFoundIter));

		return retVal;
	}

	return retVal;
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


QString CSdlTools::BuildNamespaceFromComponents(const QString& schemaNamespace, const QString& schemaName, const QString& versionName)
{
	QString retVal;
	if (!schemaNamespace.isEmpty()){
		retVal += schemaNamespace;
	}

	if (!schemaName.isEmpty()){
		if (!retVal.isEmpty()){
			retVal += QStringLiteral("::");
		}
		retVal += schemaName;
	}

	if (!versionName.isEmpty()){
		if (!retVal.isEmpty()){
			retVal += QStringLiteral("::V");
		}
		retVal += versionName;
	}

	return retVal;
}


QString CSdlTools::BuildNamespaceFromParams(const iprm::IParamsSet& schemaParams, bool addVersion, bool addPrefix)
{
	QString retVal;

	QString schemaNamespace;
	if (addPrefix && !s_sdlGlobalPrefix.isEmpty()){
		schemaNamespace = s_sdlGlobalPrefix + QStringLiteral("::");
	}
	iprm::TParamsPtr<iprm::ITextParam> namespaceParamPtr(&schemaParams, SdlCustomSchemaKeys::SchemaNamespace.toUtf8(), false);
	if (namespaceParamPtr.IsValid()){
		schemaNamespace += namespaceParamPtr->GetText();
	}

	QString schemaName;
	iprm::TParamsPtr<iprm::ITextParam> nameParamPtr(&schemaParams, SdlCustomSchemaKeys::SchemaName.toUtf8(), false);
	if (nameParamPtr.IsValid()){
		schemaName = nameParamPtr->GetText();
	}

	QString versionName;
	if (addVersion){
		iprm::TParamsPtr<iprm::ITextParam> versionNameParamPtr(&schemaParams, SdlCustomSchemaKeys::VersionName.toUtf8(), false);
		if (versionNameParamPtr.IsValid()){
			versionName = versionNameParamPtr->GetText();
		}
	}

	retVal = BuildNamespaceFromComponents(schemaNamespace, schemaName, versionName);

	return GetNamespaceAcceptableString(retVal);
}


QString CSdlTools::BuildQmlImportDeclarationFromParams(const iprm::IParamsSet& schemaParams, const QString& suffix, bool addVersion)
{
	QString retVal;

	QString schemaNamespace;
	iprm::TParamsPtr<iprm::ITextParam> namespaceParamPtr(&schemaParams, SdlCustomSchemaKeys::SchemaNamespace.toUtf8(), false);
	if (namespaceParamPtr.IsValid()){
		schemaNamespace = namespaceParamPtr->GetText();
	}

	QString schemaName;
	iprm::TParamsPtr<iprm::ITextParam> nameParamPtr(&schemaParams, SdlCustomSchemaKeys::SchemaName.toUtf8(), false);
	if (nameParamPtr.IsValid()){
		schemaName = nameParamPtr->GetText();
	}

	QString versionName;
	iprm::TParamsPtr<iprm::ITextParam> versionNameParamPtr(&schemaParams, SdlCustomSchemaKeys::VersionName.toUtf8(), false);
	if (versionNameParamPtr.IsValid()){
		versionName = versionNameParamPtr->GetText();
	}

	retVal = schemaNamespace + schemaName + suffix;

	if (addVersion){
		retVal += ' ' + versionName;
	}

	return retVal;
}


QString CSdlTools::GetNamespaceFromParamsOrArguments(
			const SchemaParamsCompPtr& schemaParamsCompPtr,
			const ArgumentParserCompPtr& argumentParamsCompPtr,
			bool addVersion)
{
	return GetNamespaceFromParamsOrArguments(
				(schemaParamsCompPtr.IsValid() ? schemaParamsCompPtr.GetPtr() : nullptr),
				(argumentParamsCompPtr.IsValid() ? argumentParamsCompPtr.GetPtr() : nullptr),
				addVersion);
}


QString CSdlTools::GetNamespaceFromParamsOrArguments(
			const iprm::IParamsSet* schemaParamsPtr,
			const ISdlProcessArgumentsParser* argumentParamsPtr,
			bool addVersion)
{
	QString sdlNamespace;
	if (schemaParamsPtr != nullptr){
		sdlNamespace = BuildNamespaceFromParams(*schemaParamsPtr, addVersion, true);
	}

	// we should override nmespace from agrs if it porvided
	if (argumentParamsPtr != nullptr){
		QString argNamespace = argumentParamsPtr->GetNamespace();
		if (!argNamespace.isEmpty()){
			sdlNamespace = argNamespace;
		}
	}

	Q_ASSERT(!sdlNamespace.isEmpty());

	return sdlNamespace;
}


QMap<QString, QString> CSdlTools::CalculateTargetCppFilesFromSchemaParams(const iprm::IParamsSet& schemaParams, const QString& baseDirPath, const QString& defaultName)
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

	retVal.insert(ISdlProcessArgumentsParser::s_headerFileType, QDir::cleanPath(GetFileSystemAcceptableEntryPath(baseFilePath + QStringLiteral(".h"))));
	retVal.insert(ISdlProcessArgumentsParser::s_sourceFileType, QDir::cleanPath(GetFileSystemAcceptableEntryPath(baseFilePath + QStringLiteral(".cpp"))));

	return retVal;
}


QString CSdlTools::ProcessTemplateString(const iprm::IParamsSet& schemaParams, const QString& templateString, const QString& outputDirPath)
{
	QString retVal = templateString;
	static const QRegularExpression outPathRegExp("^\\$\\(output-directory\\)");
	retVal.replace(outPathRegExp, outputDirPath);

	iprm::TParamsPtr<iprm::ITextParam> versionNameParamPtr(&schemaParams, SdlCustomSchemaKeys::VersionName.toUtf8(), false);
	if (versionNameParamPtr.IsValid()){
		static const QRegularExpression schemaVersionRegExp("\\$\\(schema\\.version\\)");
		const QString versionText = versionNameParamPtr->GetText();
		retVal.replace(schemaVersionRegExp, versionText);
	}

	iprm::TParamsPtr<iprm::ITextParam> nameParamPtr(&schemaParams, SdlCustomSchemaKeys::SchemaName.toUtf8(), false);
	if (nameParamPtr.IsValid()){
		static const QRegularExpression schemaNameRegExp("\\$\\(schema\\.name\\)");
		const QString nameText = nameParamPtr->GetText();
		retVal.replace(schemaNameRegExp, nameText);
	}

	iprm::TParamsPtr<iprm::ITextParam> namespaceParamPtr(&schemaParams, SdlCustomSchemaKeys::SchemaNamespace.toUtf8(), false);
	if (namespaceParamPtr.IsValid()){
		static const QRegularExpression schemaNamespaceRegExp("\\$\\(schema\\.namespace\\)");
		const QString namespaceText = namespaceParamPtr->GetText();
		retVal.replace(schemaNamespaceRegExp, namespaceText);
	}

	return retVal;
}


bool CSdlTools::UpdateTypeInfo(CSdlEntryBase& sdlEntry, const iprm::IParamsSet* schemaParamsPtr, const ISdlProcessArgumentsParser* argumentParserPtr)
{
	if (argumentParserPtr == nullptr){
		return false;
	}

	const QString defaultName = QFileInfo(argumentParserPtr->GetSchemaFilePath()).fileName();
	QMap<QString, QString> joinRules = CalculateTargetCppFilesFromSchemaParams(*schemaParamsPtr, argumentParserPtr->GetOutputDirectoryPath(), defaultName);
	
	if(joinRules.contains(ISdlProcessArgumentsParser::s_headerFileType)){
		sdlEntry.SetTargetHeaderFilePath(QDir::cleanPath(joinRules[ISdlProcessArgumentsParser::s_headerFileType]));
	}
	
	return true;
}


QMap<QString, QString> CSdlTools::CalculateTargetCppFilesFromSchemaParams(const iprm::IParamsSet& schemaParams, const ISdlProcessArgumentsParser& argumentParser, bool relativePath)
{
	if (!argumentParser.IsTemplateEnabled()){
		const QString defaultName = QFileInfo(argumentParser.GetSchemaFilePath()).fileName();
		QMap<QString, QString> joinRules = CalculateTargetCppFilesFromSchemaParams(schemaParams, argumentParser.GetOutputDirectoryPath(), defaultName);

		return joinRules;
	}

	QMap<QString, QString> retVal;
	QString baseFilePath;
	static const QRegularExpression outPathRegExp("^\\$\\(output-directory\\)\\/?");
	const QString templateInclidePath = argumentParser.GetTemplateIncludePath();
	if (!templateInclidePath.isEmpty()){
		baseFilePath = templateInclidePath;
	}
	else {
		baseFilePath = argumentParser.GetTemplateOutputPath();
	}
	if (!relativePath && !baseFilePath.contains(outPathRegExp)){
		baseFilePath.prepend(QStringLiteral("$(output-directory)/"));
	}

	baseFilePath+= QStringLiteral("/$(schema.name)");

	baseFilePath = ProcessTemplateString(schemaParams, baseFilePath, argumentParser.GetOutputDirectoryPath());

	if (relativePath && templateInclidePath.isEmpty()){
		// find if relative path, should be in subpath of output dir
		baseFilePath = ResolveRelativePath(baseFilePath, argumentParser.GetHeadersIncludePaths());
	}

	if (baseFilePath.isEmpty()){
		qWarning() << "Template is set, but string is not specified";

		return retVal;
	}

	retVal.insert(ISdlProcessArgumentsParser::s_headerFileType, QDir::cleanPath(GetFileSystemAcceptableEntryPath(baseFilePath + QStringLiteral(".h"))));
	retVal.insert(ISdlProcessArgumentsParser::s_sourceFileType, QDir::cleanPath(GetFileSystemAcceptableEntryPath(baseFilePath + QStringLiteral(".cpp"))));

	return retVal;
}


QString CSdlTools::GetCompleteOutputPath(const iprm::IParamsSet& schemaParams, const ISdlProcessArgumentsParser& argumentParser, bool cleanPath, bool cppPath)
{
	QString retVal;
	if (!argumentParser.IsTemplateEnabled()){
		retVal = argumentParser.GetOutputDirectoryPath();
		if (cleanPath){
			retVal = QDir::cleanPath(retVal);
		}

		return retVal;
	}

	/// \todo move to global
	static const QRegularExpression outPathRegExp("^\\$\\(output-directory\\)\\/?");
	if (cppPath){
		retVal = argumentParser.GetTemplateOutputPath();
	}
	else {
		retVal = argumentParser.GetTemplateQmlOutputPath();
	}
	if (!retVal.contains(outPathRegExp)){
		retVal.prepend(QStringLiteral("$(output-directory)/"));
	}

	retVal = ProcessTemplateString(schemaParams, retVal, argumentParser.GetOutputDirectoryPath());

	if (retVal.isEmpty()){
		qWarning() << "Template is set, but string is not specified";

		return QString();
	}

	if (cleanPath){
		retVal = QDir::cleanPath(retVal);
	}

	return retVal;
}


QString CSdlTools::GetCompleteOutputPath(const icomp::TReferenceMember<iprm::IParamsSet>& schemaParamsCompPtr, const ISdlProcessArgumentsParser& argumentParser, bool cleanPath, bool cppPath)
{
	if (!schemaParamsCompPtr.IsValid()){
		QString retVal;
		retVal = argumentParser.GetOutputDirectoryPath();
		if (cleanPath){
			retVal = QDir::cleanPath(retVal);
		}
	}

	return GetCompleteOutputPath(*schemaParamsCompPtr, argumentParser, cleanPath, cppPath);
}

QString CSdlTools::GetTempOutputPathFromParams(const iprm::IParamsSet* paramsPtr, const QString& fallbackPath)
{
	QString tempDirectoryPath = fallbackPath;

	iprm::TParamsPtr<ifile::IFileNameParam> tempDirParamPtr(paramsPtr, imtsdl::ProcessorParamKeys::TempDirPath, fallbackPath.isNull());
	if (tempDirParamPtr.IsValid()){
		const QString paramsPath = tempDirParamPtr->GetPath();
		if (!paramsPath.isEmpty()){
			tempDirectoryPath = tempDirParamPtr->GetPath();
		}
	}

	return tempDirectoryPath;
}


QStringList CSdlTools::GetAutoJoinedCppFilePaths(const iprm::IParamsSet& schemaParams, const ISdlProcessArgumentsParser& argParser)
{
	return CalculateTargetCppFilesFromSchemaParams(schemaParams, argParser).values();
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
	retVal = QDir::cleanPath(retVal);

	return retVal;
}


QString CSdlTools::GetEscapedNamespace(const QString& aNamespace, const QString& relatedNamespace)
{
	static QRegularExpression imtbaseNamespaceRegExp("\\s*(?:\\:\\:){0,}\\s*imtbase\\s*(?:\\:\\:){0,}\\s*");
	if (aNamespace == QStringLiteral("imtbase") && (relatedNamespace.isEmpty() || imtbaseNamespaceRegExp.match(relatedNamespace).hasMatch())){
		return (QStringLiteral("::") + aNamespace);
	}

	static QRegularExpression imtgqlNamespaceRegExp("\\s*(?:\\:\\:){0,}\\s*imtgql\\s*(?:\\:\\:){0,}\\s*");
	if(aNamespace == QStringLiteral("imtgql") && (relatedNamespace.isEmpty() || imtgqlNamespaceRegExp.match(relatedNamespace).hasMatch())){
		return (QStringLiteral("::") + aNamespace);
	}

	return aNamespace;
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


void CSdlTools::PrintFiles(const QString& filePath, const QStringList& files, const iprm::IOptionsManager& depsList)
{
	if (filePath.isEmpty()){
		return;
	}

	QFile depFile(filePath);
	const bool isOpen = depFile.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text);
	if (!isOpen){
		qCritical() << "Can't open dep file" << filePath;
	}

	QTextStream fStream(&depFile);

	for(QString targetPath: files){
		targetPath.replace(' ', "\\ ");
		fStream << targetPath;
		fStream << ' ';
	}

	fStream << ':' << ' ';

	QSet<QString> cumulatedFiles;
	int optionsCount = depsList.GetOptionsCount();
	for (int i = 0; i < optionsCount; ++i){
		cumulatedFiles << depsList.GetOptionName(i);
	}

	for(QString depPath: cumulatedFiles){
		depPath.replace(' ', "\\ ");
		fStream << depPath;
		fStream << ' ';
	}

	fStream << Qt::endl;
}


void CSdlTools::PrintFiles(std::ostream& outStream, const QStringList& files, ISdlProcessArgumentsParser::GeneratorType projectCodeGenerator)
{
	if (projectCodeGenerator == ISdlProcessArgumentsParser::GT_DEP_FILE){
		return;
	}

	char glue = ' ';
	if (projectCodeGenerator == ISdlProcessArgumentsParser::GT_CMAKE){
		glue = '\n';
	}
	if (projectCodeGenerator == ISdlProcessArgumentsParser::GT_CMAKE_PIPE){
		glue = ';';
	}

	QString outputString = files.join(glue);

	// remove empty elements
	outputString = outputString.remove(QString("%1%1").arg(glue));
	while (outputString.endsWith(glue)){
		outputString.chop(1);
	}

	outStream << outputString.toStdString();

	outStream.flush();
}


QString CSdlTools::GetTypeVersion(const CSdlEntryBase& sdlType)
{
	QString typeVersionName;
	const iprm::IParamsSet& schemaParams = sdlType.GetSchemaParams();
	iprm::TParamsPtr<iprm::ITextParam> versionNameParamPtr(&schemaParams, SdlCustomSchemaKeys::VersionName.toUtf8(), false);
	if (versionNameParamPtr.IsValid()){
		typeVersionName = versionNameParamPtr->GetText();
	}

	return typeVersionName;
}


IncludeDirective CSdlTools::CreateCxxDirective(const QString& path)
{
	IncludeDirective retVal;
	retVal.path = path;
	retVal.priority = P_C;
	retVal.remark = QStringLiteral("C/C++ includes");
	retVal.type = T_HEADER;

	return retVal;
}


IncludeDirective CSdlTools::CreateAcfDirective(const QString& path)
{
	IncludeDirective retVal;
	retVal.path = path;
	retVal.priority = P_ACF;
	retVal.remark = QStringLiteral("ACF includes");
	retVal.type = T_HEADER;

	return retVal;
}


IncludeDirective CSdlTools::CreateImtDirective(const QString& path)
{
	IncludeDirective retVal;
	retVal.path = path;
	retVal.priority = P_IMT;
	retVal.remark = QStringLiteral("ImtCore includes");
	retVal.type = T_HEADER;

	return retVal;
}


IncludeDirective CSdlTools::CreateQtDirective(const QString& path)
{
	IncludeDirective retVal;
	retVal.path = path;
	retVal.priority = P_QT;
	retVal.remark = QStringLiteral("Qt includes");
	retVal.type = T_HEADER;

	return retVal;
}


IncludeDirective CSdlTools::CreateCustomDirective(const QString& path)
{
	IncludeDirective retVal;
	retVal.path = path;
	retVal.priority = P_CUSTOM;
	retVal.remark = QStringLiteral("custom types includes");
	retVal.type = T_HEADER;

	return retVal;
}


} // namespace imtsdl
