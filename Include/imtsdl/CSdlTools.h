#pragma once

// Qt includes
#include <QtCore/QSet>

// ACF includes
#include <icomp/TReferenceMember.h>

// ImtCore includes
#include <imtsdl/imtsdl.h>
#include <imtsdl/CSdlEnum.h>
#include <imtsdl/ISdlProcessArgumentsParser.h>
#include <imtsdl/ISdlTypeListProvider.h>
#include <imtsdl/ISdlEnumListProvider.h>

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
	static const QString s_variantMapClassMemberName;

public:
	typedef icomp::TReferenceMember<iprm::IParamsSet> SchemaParamsCompPtr;
	typedef icomp::TReferenceMember<ISdlProcessArgumentsParser> ArgumentParserCompPtr;

	static void WrapTypeToList(QString& text);

	[[nodiscard]] static QString ConvertTypeWithNamespace(
				const CSdlField& sdlField,
				const QString& relatedNamespace,
				const ISdlTypeListProvider& listProvider,
				const ISdlEnumListProvider& enumProvider,
				bool* isCustomPtr = nullptr,
				bool* isComplexPtr = nullptr,
				bool* isArrayPtr = nullptr,
				bool* isEnumPtr = nullptr);

	[[nodiscard]] static QString OptListConvertTypeWithNamespace(
				const CSdlField& sdlField,
				const QString& relatedNamespace,
				const ISdlTypeListProvider& listProvider,
				const ISdlEnumListProvider& enumProvider,
				bool listWrap = false,
				bool* isCustomPtr = nullptr,
				bool* isComplexPtr = nullptr,
				bool* isArrayPtr = nullptr,
				bool* isEnumPtr = nullptr);

	static QString ConvertType(const CSdlField& sdlField, bool* isCustomPtr = nullptr, bool* isComplexPtr = nullptr, bool* isArrayPtr = nullptr);
	static QString ConvertType(const QString& sdlTypeName, bool* isCustomPtr = nullptr, bool* isComplexPtr = nullptr);
	static QString ConvertTypeOrEnum(const CSdlField& sdlField, const SdlEnumList& enumList, bool* isCustomPtr = nullptr, bool* isComplexPtr = nullptr, bool* isArrayPtr = nullptr, bool* isEnumPtr = nullptr);

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

	Q_DECL_DEPRECATED_X("Use GetFromVariantConversionStringExt instead")
		[[nodiscard]] static QString GetFromVariantConversionString(const CSdlField& sdlField);

	[[nodiscard]] static QString GetFromVariantConversionStringExt(const CSdlField& sdlField, bool singleValue = false);
	[[nodiscard]] static QString FromInternalMapCheckString(const CSdlField& sdlField);
	[[nodiscard]] static bool EnsureFieldHasValidType(const CSdlField& sdlField, const SdlTypeList& typeList, const SdlEnumList& enumList);
	[[nodiscard]] static bool GetSdlTypeForField(const CSdlField& sdlField, const SdlTypeList& typeList, CSdlType& sdlType);
	[[nodiscard]] static bool GetSdlEnumForField(const CSdlField& sdlField, const SdlEnumList& enumList, CSdlEnum& sdlEnum);
	[[nodiscard]] static std::shared_ptr<CSdlEntryBase> GetSdlTypeOrEnumForField(const CSdlField& sdlField, const SdlTypeList& typeList, const SdlEnumList& enumList);

	static void AddSelfCheckRequiredValueCode(QTextStream& stream, const CSdlField& field, uint hIndents = 1);
	static void AddBeginSelfCheckNonRequiredValueCode(QTextStream& stream, const CSdlField& field, uint hIndents = 1);
	static void AddArrayInternalChecksFail(QTextStream& stream, const CSdlField& field, bool checkEmpty = false, uint hIndents = 1);

	[[nodiscard]] static QString GetNamespaceAcceptableString(const QString& originalText);
	[[nodiscard]] static QString GetFileSystemAcceptableEntryPath(const QString& originalText);
	[[nodiscard]] static QString BuildNamespaceFromComponents(const QString& schemaNamespace, const QString& schemaName, const QString& versionName);
	[[nodiscard]] static QString BuildNamespaceFromParams(const iprm::IParamsSet& schemaParams);
	[[nodiscard]] static QString BuildQmlImportDeclarationFromParams(const iprm::IParamsSet& schemaParams, const QString& suffix);
	[[nodiscard]] static QString GetNamespaceFromParamsOrArguments(
				const SchemaParamsCompPtr& schemaParamsCompPtr,
				const ArgumentParserCompPtr& argumentParamsCompPtr);
	[[nodiscard]] static QString GetNamespaceFromParamsOrArguments(
				const iprm::IParamsSet* schemaParamsCompPtr,
				const ArgumentParserCompPtr& argumentParamsCompPtr);

	/// \sa imtsdlgen::ISdlProcessArgumentsParser::s_headerFileType imtsdlgen::ISdlProcessArgumentsParser::s_sourceFileType
	[[nodiscard]]static QMap<QString/*type*/, QString/*path*/> CalculateTargetCppFilesFromSchemaParams(const iprm::IParamsSet& schemaParams, const QString& baseDirPath, const QString defaultName = QString());
	static bool UpdateTypeInfo(CSdlEntryBase& sdlEntry, const iprm::IParamsSet* schemaParamsPtr, const imtsdl::ISdlProcessArgumentsParser* argumentParamsPtr);

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

	static void PrintFiles(std::ostream& outStream, const QStringList& files, imtsdl::ISdlProcessArgumentsParser::GeneratorType projectCodeGenerator = imtsdl::ISdlProcessArgumentsParser::GT_CMAKE);

	[[nodiscard]] static QString ResolveRelativeHeaderFileForType(const CSdlEntryBase& sdlEntry, const QStringList& lookupPaths);

	[[nodiscard]] static QString GetHeaderPathFromCache(const CSdlType& sdlType, const std::shared_ptr<QStringList>& cacheFilesListPtr);

	[[nodiscard]] static IncludeDirective CreateCxxDirective(const QString& path);
	[[nodiscard]] static IncludeDirective CreateQtDirective(const QString& path);
	[[nodiscard]] static IncludeDirective CreateImtDirective(const QString& path);
	[[nodiscard]] static IncludeDirective CreateCustomDirective(const QString& path);

};


} // namespace imtsdl
