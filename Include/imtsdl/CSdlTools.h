#pragma once

// Qt includes
#include <QtCore/QSet>

// ACF includes
#include <icomp/TReferenceMember.h>
#include <iprm/IOptionsManager.h>

// ImtCore includes
#include <imtsdl/imtsdl.h>
#include <imtsdl/CSdlEnum.h>
#include <imtsdl/CSdlUnion.h>
#include <imtsdl/ISdlProcessArgumentsParser.h>
#include <imtsdl/ISdlTypeListProvider.h>
#include <imtsdl/ISdlEnumListProvider.h>
#include <imtsdl/ISdlUnionListProvider.h>

class QTextStream;

namespace iprm
{
class IParamsSet;
}


namespace imtsdl
{


class ISdlProcessArgumentsParser;


/// \todo reorder methods and specify usage
class CSdlTools
{
public:
	// static variables
	/// \todo remove it. \deprecated
	static const QString s_variantMapClassMemberName;
	/**
		\brief Defines an SDL scheme namespace prefix.
		\warning If a change is required, it MUST ONLY be made BEFORE the process of parsing the scheme.
	*/
	static QString s_sdlGlobalPrefix;

public:
	typedef icomp::TReferenceMember<iprm::IParamsSet> SchemaParamsCompPtr;
	typedef icomp::TReferenceMember<ISdlProcessArgumentsParser> ArgumentParserCompPtr;

	static void WrapTypeToList(QString& text);

	[[nodiscard]] static QString ConvertTypeWithNamespace(
				const CSdlField& sdlField,
				const QString& relatedNamespace,
				const ISdlTypeListProvider& listProvider,
				const ISdlEnumListProvider& enumProvider,
				const ISdlUnionListProvider& unionProvider,
				bool* isCustomPtr = nullptr,
				bool* isComplexPtr = nullptr,
				bool* isArrayPtr = nullptr,
				bool* isEnumPtr = nullptr,
				bool* isUnionPtr = nullptr);

	[[nodiscard]] static QString OptListConvertTypeWithNamespace(
				const CSdlField& sdlField,
				const QString& relatedNamespace,
				const ISdlTypeListProvider& listProvider,
				const ISdlEnumListProvider& enumProvider,
				const ISdlUnionListProvider& unionProvider,
				bool listWrap = false,
				bool* isCustomPtr = nullptr,
				bool* isComplexPtr = nullptr,
				bool* isArrayPtr = nullptr,
				bool* isEnumPtr = nullptr,
				bool* isUnionPtr = nullptr);

	static QString ConvertType(const CSdlField& sdlField, bool* isCustomPtr = nullptr, bool* isComplexPtr = nullptr, bool* isArrayPtr = nullptr);
	static QString ConvertType(const QString& sdlTypeName, bool* isCustomPtr = nullptr, bool* isComplexPtr = nullptr);
	static QString ConvertTypeOrEnum(const CSdlField& sdlField, const SdlEnumList& enumList, bool* isCustomPtr = nullptr, bool* isComplexPtr = nullptr, bool* isArrayPtr = nullptr, bool* isEnumPtr = nullptr);
	static QString ConvertTypeOrEnumOrUnion(const CSdlField& sdlField, const SdlEnumList& enumList, const SdlUnionList& unionList, bool* isCustomPtr = nullptr, bool* isComplexPtr = nullptr, bool* isArrayPtr = nullptr, bool* isEnumPtr = nullptr, bool* isUnion = nullptr);

	static QString QmlConvertType(const QString& sdlTypeName, bool* isCustomPtr = nullptr);

	static void FeedStream(QTextStream& stream, uint lines = 1, bool flush = true);
	static void FeedLineHorizontally(QString& line, uint indents = 1, char indentDelimiter = '\t');
	static void FeedStreamHorizontally(QTextStream& stream, uint indents = 1, char indentDelimiter = '\t');

	[[nodiscard]] static QString GetCapitalizedValue(const QString& inputValue);
	[[nodiscard]] static QString GetDecapitalizedValue(const QString& inputValue);
	[[nodiscard]] static bool IsTypeHasFundamentalTypes(const CSdlType& sdlType, QSet<QString>* foundTypesPtr =  nullptr);
	[[nodiscard]] static bool IsTypeHasFundamentalTypes(const CSdlRequest& sdlRequest, QSet<QString>* foundTypesPtr =  nullptr);
	[[nodiscard]] static bool IsTypeHasNonFundamentalTypes(const CSdlType& sdlType, QSet<QString>* foundTypesPtr =  nullptr);
	[[nodiscard]] static bool IsTypeHasNonFundamentalTypes(const CSdlRequest& sdlRequest, QSet<QString>* foundTypesPtr =  nullptr);

	/// \deprecated \todo REMOVE IT! (later))
	[[nodiscard]] static QString GetFromVariantConversionString(const CSdlField& sdlField);

	[[nodiscard]] static QString GetFromVariantConversionStringExt(const CSdlField& sdlField, bool singleValue = false);
	[[nodiscard]] static QString FromInternalMapCheckString(const CSdlField& sdlField);
	[[nodiscard]] static bool EnsureFieldHasValidType(const CSdlField& sdlField, const SdlTypeList& typeList, const SdlEnumList& enumList, const SdlUnionList& unionList);
	[[nodiscard]] static bool GetSdlTypeForField(const CSdlField& sdlField, const SdlTypeList& typeList, CSdlType& sdlType);
	[[nodiscard]] static bool GetSdlEnumForField(const CSdlField& sdlField, const SdlEnumList& enumList, CSdlEnum& sdlEnum);
	[[nodiscard]] static bool GetSdlUnionForField(const CSdlField& sdlField, const SdlUnionList& unionList, CSdlUnion& sdlEnum);
	[[nodiscard]] static std::shared_ptr<CSdlEntryBase> GetSdlTypeOrEnumOrUnionForField(const CSdlField& sdlField, const SdlTypeList& typeList, const SdlEnumList& enumList, const SdlUnionList& unionList);

	static void AddSelfCheckRequiredValueCode(QTextStream& stream, const CSdlField& field, uint hIndents = 1);
	static void AddBeginSelfCheckNonRequiredValueCode(QTextStream& stream, const CSdlField& field, uint hIndents = 1);
	static void AddArrayInternalChecksFail(QTextStream& stream, const CSdlField& field, bool checkEmpty = false, uint hIndents = 1);

	[[nodiscard]] static QString GetNamespaceAcceptableString(const QString& originalText);
	[[nodiscard]] static QString GetFileSystemAcceptableEntryPath(const QString& originalText);
	[[nodiscard]] static QString BuildNamespaceFromComponents(const QString& schemaNamespace, const QString& schemaName, const QString& versionName);
	[[nodiscard]] static QString BuildNamespaceFromParams(const iprm::IParamsSet& schemaParams, bool addVersion = true, bool addPrefix = false);
	[[nodiscard]] static QString BuildQmlImportDeclarationFromParams(const iprm::IParamsSet& schemaParams, const QString& suffix, bool addVersion = true);
	[[nodiscard]] static QString GetNamespaceFromParamsOrArguments(
				const SchemaParamsCompPtr& schemaParamsCompPtr,
				const ArgumentParserCompPtr& argumentParamsCompPtr, bool addVersion = true);
	[[nodiscard]] static QString GetNamespaceFromParamsOrArguments(
				const iprm::IParamsSet* schemaParamsCompPtr,
				const ArgumentParserCompPtr& argumentParamsCompPtr, bool addVersion = true);

	/// \sa imtsdlgen::ISdlProcessArgumentsParser::s_headerFileType imtsdlgen::ISdlProcessArgumentsParser::s_sourceFileType
	[[nodiscard]] static QMap<QString/*type*/, QString/*path*/> CalculateTargetCppFilesFromSchemaParams(const iprm::IParamsSet& schemaParams, const QString& baseDirPath, const QString defaultName = QString());
	[[nodiscard]] static bool UpdateTypeInfo(CSdlEntryBase& sdlEntry, const iprm::IParamsSet* schemaParamsPtr, const imtsdl::ISdlProcessArgumentsParser* argumentParamsPtr);

	[[nodiscard]] static bool FindHeaderForEntry(
				CSdlEntryBase& sdlEntry,
				const QStringList& icludePaths,
				const QString& outputDirPath = QString(),
				const QString& currentSchemaPath = QString());

	[[nodiscard]] static QStringList GetAutoJoinedCppFilePaths(const iprm::IParamsSet& schemaParams, const QString& baseDirPath, const QString defaultName = QString());
	[[nodiscard]] static QString GetQmlModuleNameFromParamsOrArguments(
				const iprm::IParamsSet* schemaParamsPtr,
				const imtsdl::ISdlProcessArgumentsParser* argumentParamsPtr);
	[[nodiscard]] static QString GetQmlModuleNameFromParamsOrArguments(
				const SchemaParamsCompPtr& schemaParamsCompPtr,
				const ArgumentParserCompPtr& argumentParamsCompPtr);

	[[nodiscard]] static QString GetAutoDefinedQmlBasePath(const iprm::IParamsSet& schemaParams, const QString& baseDirPath);
	[[nodiscard]] static QString GetAutoDefinedQmlQrcFilePath(const iprm::IParamsSet& schemaParamsCompPtr, const QString& baseDirPath);

	[[nodiscard]] static QString GetEscapedNamespace(const QString& aNamespace, const QString& relatedNamespace);

	static void GenerateListTempValueCode(QTextStream& stream, const CSdlField& sdlField, QString& tempVariableName, uint indents = 1);

	static void PrintFiles(const QString& filePath, const QStringList& files, const iprm::IOptionsManager& depsList);
	static void PrintFiles(std::ostream& outStream, const QStringList& files, imtsdl::ISdlProcessArgumentsParser::GeneratorType projectCodeGenerator = imtsdl::ISdlProcessArgumentsParser::GT_CMAKE);

	/// \todo remove \param rawLookup - V1 fallback use ONLY for V1 generation!!!!!
	[[nodiscard]] static QString ResolveRelativeHeaderFileForType(const CSdlEntryBase& sdlEntry, const QStringList& lookupPaths, bool rawLookup = true);
	[[nodiscard]] static QString GetTypeVersion(const CSdlEntryBase& sdlType);

	[[nodiscard]] static IncludeDirective CreateCxxDirective(const QString& path);
	[[nodiscard]] static IncludeDirective CreateQtDirective(const QString& path);
	[[nodiscard]] static IncludeDirective CreateImtDirective(const QString& path);
	[[nodiscard]] static IncludeDirective CreateCustomDirective(const QString& path);
};


} // namespace imtsdl
