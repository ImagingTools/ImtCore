#pragma once

// Qt includes
#include <QtCore/QSet>

// ACF includes
#include <icomp/TReferenceMember.h>

// ImtCore includes
#include <imtsdl/imtsdl.h>


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
	static const QString s_variantMapModificatorArgumentName;
	static const QString s_variantMapClassMemberName;

public:
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
	[[nodiscard]]static QString GetFromVariantConversionString(const CSdlField& sdlField);
	[[nodiscard]]static QString FromVariantMapAccessString(const CSdlField& sdlField);
	static bool GetSdlTypeForField(const CSdlField& sdlField, const SdlTypeList& typeList, CSdlType& sdlType);
	static void AddSelfCheckRequiredValueCode(QTextStream& stream, const CSdlField& field, uint hIndents = 1);
	static void AddBeginSelfCheckNonRequiredValueCode(QTextStream& stream, const CSdlField& field, uint hIndents = 1);
	static void AddArrayInternalChecksFail(QTextStream& stream, const CSdlField& field, bool checkEmpty = false, uint hIndents = 1);
	[[nodiscard]]static QString GetNamespaceAcceptableString(const QString& originalText);
	[[nodiscard]]static QString GetFileSystemAcceptableEntryPath(const QString& originalText);
	[[nodiscard]]static QString BuildNamespaceFromParams(const iprm::IParamsSet& namespaceParams);

	[[nodiscard]]static QString GetNamespaceFromParamsOrArguments(
				const icomp::TReferenceMember<iprm::IParamsSet>& schemaParamsCompPtr,
				const icomp::TReferenceMember<ISdlProcessArgumentsParser>& argumentParamsCompPtr);

};


} // namespace imtsdl
