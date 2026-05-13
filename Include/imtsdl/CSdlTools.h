// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QSet>

// ACF includes
#include <icomp/TReferenceMember.h>
#include <iprm/IOptionsManager.h>

// ImtCore includes
#include <imtsdl/CSdlUnion.h>
#include <imtsdl/CSdlRequest.h>
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

	[[nodiscard]] static QString ResolveRelativePath(const QString& path, const QStringList& lookupPaths);

	static QString ConvertType(const CSdlField& sdlField, bool* isCustomPtr = nullptr, bool* isComplexPtr = nullptr, bool* isArrayPtr = nullptr);
	static QString ConvertType(const QString& sdlTypeName, bool* isCustomPtr = nullptr, bool* isComplexPtr = nullptr);
	static QString ConvertTypeOrEnum(const CSdlField& sdlField, const SdlEnumList& enumList, bool* isCustomPtr = nullptr, bool* isComplexPtr = nullptr, bool* isArrayPtr = nullptr, bool* isEnumPtr = nullptr);
	/// \todo rename it to 'ConvertEntryType' and make a structure with result; remove \c isArrayPtr
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
	[[nodiscard]] static QString GetFromVariantConversionString(const CSdlField& sdlField);

	[[nodiscard]] static QString GetFromVariantConversionStringExt(const CSdlField& sdlField, bool singleValue = false);
	[[nodiscard]] static bool EnsureFieldHasValidType(const CSdlField& sdlField, const SdlTypeList& typeList, const SdlEnumList& enumList, const SdlUnionList& unionList);
	[[nodiscard]] static bool GetSdlTypeForField(const CSdlField& sdlField, const SdlTypeList& typeList, CSdlType& sdlType);
	[[nodiscard]] static bool GetSdlEnumForField(const CSdlField& sdlField, const SdlEnumList& enumList, CSdlEnum& sdlEnum);
	[[nodiscard]] static bool GetSdlUnionForField(const CSdlField& sdlField, const SdlUnionList& unionList, CSdlUnion& sdlUnion);
	/// \todo rename it to 'GetSdlEntryFromField'
	[[nodiscard]] static std::shared_ptr<CSdlEntryBase> GetSdlTypeOrEnumOrUnionForField(const CSdlField& sdlField, const SdlTypeList& typeList, const SdlEnumList& enumList, const SdlUnionList& unionList);

	[[nodiscard]] static std::shared_ptr<CSdlEntryBase> FindEntryByName(const QString& entryName, const SdlTypeList& typeList, const SdlEnumList& enumList = SdlEnumList(), const SdlUnionList& unionList = SdlUnionList());

	[[nodiscard]] static QString GetNamespaceAcceptableString(const QString& originalText);
	[[nodiscard]] static QString GetFileSystemAcceptableEntryPath(const QString& originalText);
	[[nodiscard]] static QString BuildNamespaceFromComponents(const QString& schemaNamespace, const QString& schemaName, const QString& versionName);
	[[nodiscard]] static QString BuildNamespaceFromParams(const iprm::IParamsSet& schemaParams, bool addVersion = true, bool addPrefix = false);
	[[nodiscard]] static QString BuildQmlImportDeclarationFromParams(const iprm::IParamsSet& schemaParams, const QString& suffix, bool addVersion = true);
	[[nodiscard]] static QString GetNamespaceFromParamsOrArguments(
				const SchemaParamsCompPtr& schemaParamsCompPtr,
				const ArgumentParserCompPtr& argumentParamsCompPtr, bool addVersion = true);
	[[nodiscard]] static QString GetNamespaceFromParamsOrArguments(
				const iprm::IParamsSet* schemaParamsPtr,
				const ISdlProcessArgumentsParser* argumentParamsPtr, bool addVersion = true);

	/// \sa imtsdl::ISdlProcessArgumentsParser::s_headerFileType imtsdl::ISdlProcessArgumentsParser::s_sourceFileType
	[[nodiscard]] static QMap<QString/*type*/, QString/*path*/> CalculateTargetCppFilesFromSchemaParams(const iprm::IParamsSet& schemaParams, const ISdlProcessArgumentsParser& argumentParser, bool relativePath = false);

	/// resolves a complete output path from schema params and template(if enabled) if template is not enabled, returns \c argumentParser.GetOutputDirectoryPath() \note returns clean path \sa QDir::cleanPath
	[[nodiscard]] static QString GetCompleteOutputPath(const iprm::IParamsSet& schemaParams, const ISdlProcessArgumentsParser& argumentParser, bool cleanPath, bool cppPath);
	[[nodiscard]] static QString GetCompleteOutputPath(const icomp::TReferenceMember<iprm::IParamsSet>& schemaParamsCompPtr, const ISdlProcessArgumentsParser& argumentParser, bool cleanPath, bool cppPath);

	/// extracts a temp path from \c paramsPtr ant returns it on successfull, returns \c fallbackPath otherwise
	[[nodiscard]] static QString GetTempOutputPathFromParams(const iprm::IParamsSet* paramsPtr, const QString& fallbackPath = QString());

	[[nodiscard]] static QString ProcessTemplateString(const iprm::IParamsSet& schemaParams, const QString& templateString, const QString& outputDirPath);
	[[nodiscard]] static bool UpdateTypeInfo(CSdlEntryBase& sdlEntry, const iprm::IParamsSet* schemaParamsPtr, const imtsdl::ISdlProcessArgumentsParser* argumentParserPtr);

	[[nodiscard]] static QStringList GetAutoJoinedCppFilePaths(const iprm::IParamsSet& schemaParams, const ISdlProcessArgumentsParser& argParser);
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

	[[nodiscard]] static QString GetTypeVersion(const CSdlEntryBase& sdlType);

	[[nodiscard]] static IncludeDirective CreateCxxDirective(const QString& path);
	[[nodiscard]] static IncludeDirective CreateQtDirective(const QString& path);
	[[nodiscard]] static IncludeDirective CreateAcfDirective(const QString& path);
	[[nodiscard]] static IncludeDirective CreateImtDirective(const QString& path);
	[[nodiscard]] static IncludeDirective CreateCustomDirective(const QString& path);

private:
	[[nodiscard]] static QMap<QString/*type*/, QString/*path*/> CalculateTargetCppFilesFromSchemaParams(const iprm::IParamsSet& schemaParams, const QString& baseDirPath, const QString& defaultName = QString());

};


} // namespace imtsdl
