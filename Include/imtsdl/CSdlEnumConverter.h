#pragma once

// Qt includes
#include <QtCore/QSet>
#include <QtCore/QTextStream>

// ACF includes
#include <icomp/TReferenceMember.h>

// ImtCore includes
#include <imtsdl/imtsdl.h>
#include <imtsdl/CSdlEnum.h>


namespace imtsdl
{


class CSdlEnumConverter
{
public:
	enum ConversionType
	{
		CT_AS_IS,
		CT_UPPER_CAMEL_CASE,	///< \brief aka Pascal \example UpperCamelCase
		CT_LOWER_CAMEL_CASE,	///< \example lowerCamelCase
		CT_UPPER_SNAKE_CASE,	///< \example UPPER_SNAKE_CASE
		CT_LOWER_SNAKE_CASE,	///< \example lower_snake_case
		CT_UPPER_KEBAB_CASE,	///< \example UPPER-KEBAB-CASE
		CT_LOWER_KEBAB_CASE		///< \example lower-kebab-case
	};

	// conversion methods
	[[nodiscard]] static QString ConvertEnumValueToStringEquivalent(const QString& enumValue, ConversionType conversionType = CT_AS_IS);

	// generate methods
	static void WriteConversionFromEnum(
				QTextStream& stream,
				const CSdlEnum& sdlEnum,
				const QString& sourceVariableName,
				const QString& targetVariableName,
				const QString& relatedNamespace,
				uint hIndents = 1);
	static void WriteConversionFromString(
				QTextStream& stream,
				const CSdlEnum& sdlEnum,
				const QString& sourceVariableName,
				const QString& targetVariableName,
				const QString& relatedNamespace,
				uint hIndents = 1);

private:
	[[nodiscard]] static QStringList SplitSingleStringIntoWords(const QString& sourceString);
	[[nodiscard]] static QString EncodeToUpperCamelCase(const QString& sourceString);
	[[nodiscard]] static QString EncodeToLowerCamelCase(const QString& sourceString);
	[[nodiscard]] static QString EncodeToUpperSnakeCase(const QString& sourceString);
	[[nodiscard]] static QString EncodeToLowerSnakeCase(const QString& sourceString);
	[[nodiscard]] static QString EncodeToUpperKebabCase(const QString& sourceString);
	[[nodiscard]] static QString EncodeToLowerKebabCase(const QString& sourceString);

};


} // namespace imtsdl
