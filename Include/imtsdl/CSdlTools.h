#pragma once

// Qt includes
#include <QtCore/QSet>

// ACF includes
#include <icomp/TReferenceMember.h>

// ImtCore includes
#include <imtsdl/imtsdl.h>
#include <imtsdl/ISdlProcessArgumentsParser.h>


class QTextStream;

namespace iprm
{
class IParamsSet;
}


namespace imtsdl
{


class ISdlProcessArgumentsParser;


class CSdlTools
{
public:
	static const QString s_variantMapClassMemberName;

public:
	typedef icomp::TReferenceMember<iprm::IParamsSet> SchemaParamsCompPtr;
	typedef icomp::TReferenceMember<ISdlProcessArgumentsParser> ArgumentParserCompPtr;

	static QString ConvertType(const CSdlField& sdlField, bool* isCustomPtr = nullptr, bool* isComplexPtr = nullptr, bool* isArrayPtr = nullptr);
	static QString ConvertType(const QString& sdlTypeName, bool* isCustomPtr = nullptr, bool* isComplexPtr = nullptr);
	static QString QmlConvertType(const QString& sdlTypeName, bool* isCustomPtr = nullptr);
	static void FeedStream(QTextStream& stream, uint lines = 1, bool flush = true);
	static void FeedLineHorizontally(QString& line, uint indents = 1, char indentDelimiter = '\t');
	static void FeedStreamHorizontally(QTextStream& stream, uint indents = 1, char indentDelimiter = '\t');
	[[nodiscard]]static QString GetCapitalizedValue(const QString& inputValue);
	[[nodiscard]]static QString GetDecapitalizedValue(const QString& inputValue);
	static bool IsTypeHasFundamentalTypes(const CSdlType& sdlType, QSet<QString>* foundTypesPtr =  nullptr);
	static bool IsTypeHasFundamentalTypes(const CSdlRequest& sdlRequest, QSet<QString>* foundTypesPtr =  nullptr);
	static bool IsTypeHasNonFundamentalTypes(const CSdlType& sdlType, QSet<QString>* foundTypesPtr =  nullptr);
	static bool IsTypeHasNonFundamentalTypes(const CSdlRequest& sdlRequest, QSet<QString>* foundTypesPtr =  nullptr);

	Q_DECL_DEPRECATED_X("Use GetFromVariantConversionStringExt instead")
		[[nodiscard]]static QString GetFromVariantConversionString(const CSdlField& sdlField);

	[[nodiscard]]static QString GetFromVariantConversionStringExt(const CSdlField& sdlField, bool singleValue = false);
	[[nodiscard]]static QString FromInternalMapCheckString(const CSdlField& sdlField);
	static bool GetSdlTypeForField(const CSdlField& sdlField, const SdlTypeList& typeList, CSdlType& sdlType);
	static void AddSelfCheckRequiredValueCode(QTextStream& stream, const CSdlField& field, uint hIndents = 1);
	static void AddBeginSelfCheckNonRequiredValueCode(QTextStream& stream, const CSdlField& field, uint hIndents = 1);
	static void AddArrayInternalChecksFail(QTextStream& stream, const CSdlField& field, bool checkEmpty = false, uint hIndents = 1);
	[[nodiscard]]static QString GetNamespaceAcceptableString(const QString& originalText);
	[[nodiscard]]static QString GetFileSystemAcceptableEntryPath(const QString& originalText);
	[[nodiscard]]static QString BuildNamespaceFromComponents(const QString& schemaNamespace, const QString& schemaName, const QString& versionName);
	[[nodiscard]]static QString BuildNamespaceFromParams(const iprm::IParamsSet& schemaParams);
	[[nodiscard]]static QString GetNamespaceFromParamsOrArguments(
				const SchemaParamsCompPtr& schemaParamsCompPtr,
				const ArgumentParserCompPtr& argumentParamsCompPtr);

	[[nodiscard]]static QMap<QString/*type*/, QString/*path*/> GetAutoJoinCppFilesSchema(const iprm::IParamsSet& schemaParams, const QString& baseDirPath, const QString defaultName = QString());
	[[nodiscard]]static QStringList GetAutoJoinedCppFilePaths(const iprm::IParamsSet& schemaParams, const QString& baseDirPath, const QString defaultName = QString());
	[[nodiscard]]static QString GetQmlModuleNameFromParamsOrArguments(
				const iprm::IParamsSet* schemaParamsPtr,
				const ISdlProcessArgumentsParser* argumentParamsPtr);
	[[nodiscard]]static QString GetQmlModuleNameFromParamsOrArguments(
				const SchemaParamsCompPtr& schemaParamsCompPtr,
				const ArgumentParserCompPtr& argumentParamsCompPtr);

	[[nodiscard]]static QString GetAutoDefinedQmlBasePath(const iprm::IParamsSet& schemaParams, const QString& baseDirPath);
	[[nodiscard]]static QString GetAutoDefinedQmlQrcFilePath(const iprm::IParamsSet& schemaParamsCompPtr, const QString& baseDirPath);
	[[nodiscard]]static QString GetEscapedNamespace(const QString& aNamespace, const QString& relatedNamespace);

	static void GenerateListTempValueCode(QTextStream& stream, const CSdlField& sdlField, QString& tempVariableName, uint indents = 1);

	static void PrintFiles(std::ostream& outStream, const QStringList& files, ISdlProcessArgumentsParser::GeneratorType projectCodeGenerator = ISdlProcessArgumentsParser::GT_CMAKE);

};


} // namespace imtsdl
