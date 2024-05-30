#pragma once

// Qt includes
#include <QtCore/QString>
#include <QtCore/QTextStream>
#include <QtCore/QSet>

// imtsdl includes
#include <imtsdl/imtsdl.h>


namespace imtsdl
{


class CSdlTools
{
public:
	static const QString s_variantMapModificatorArgumentName;
	static const QString s_variantMapClassMemberName;

public:
	static QString ConvertType(const CSdlField& sdlField, bool* isCustomPtr = nullptr, bool* isComplexPtr = nullptr, bool* isArrayPtr = nullptr);
	static QString ConvertType(const QString& sdlTypeName, bool* isCustomPtr = nullptr, bool* isComplexPtr = nullptr);
	static void FeedStream(QTextStream& stream, uint lines = 1, bool flush = true);
	static void FeedLineHorizontally(QString& line, uint indents = 1, char indentDelimiter = '\t');
	static void FeedStreamHorizontally(QTextStream& stream, uint indents = 1, char indentDelimiter = '\t');
	static QString GetCapitalizedValue(const QString& inputValue);
	static QString GetDecapitalizedValue(const QString& inputValue);
	static bool IsTypeHasFundamentalTypes(const CSdlType& sdlType, QSet<QString>* foundTypesPtr =  nullptr);
	static bool IsTypeHasFundamentalTypes(const CSdlRequest& sdlRequest, QSet<QString>* foundTypesPtr =  nullptr);
	static bool IsTypeHasNonFundamentalTypes(const CSdlType& sdlType, QSet<QString>* foundTypesPtr =  nullptr);
	static bool IsTypeHasNonFundamentalTypes(const CSdlRequest& sdlRequest, QSet<QString>* foundTypesPtr =  nullptr);
	static QString GetFromVariantConversionString(const CSdlField& sdlField);
	static QString FromVariantMapAccessString(const CSdlField& sdlField);
	static bool GetSdlTypeForField(const CSdlField& sdlField, const SdlTypeList& typeList, CSdlType& sdlType);
	static void AddSelfCheckRequiredValueCode(QTextStream& stream, const CSdlField& field, uint hIndents = 1);
	static void AddBeginSelfCheckNonRequiredValueCode(QTextStream& stream, const CSdlField& field, uint hIndents = 1);
};


} // namespace imtsdl
