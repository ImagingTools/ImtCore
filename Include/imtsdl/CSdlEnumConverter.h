// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/TReferenceMember.h>

// ImtCore includes
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

	// static variables
	/**
		\brief Defines a default type to conversion.
		\warning If a change is required, it MUST ONLY be made during the process of parsing the scheme.
	*/
	static ConversionType s_defaultConversionType;

	// conversion methods
	[[nodiscard]] static QString ConvertEnumValueToStringEquivalent(const QString& enumValue, ConversionType conversionType = s_defaultConversionType);

	// generate methods
	static void WriteConversionFromEnum(
				QTextStream& stream,
				const CSdlEnum& sdlEnum,
				const QString& sourceVariableName,
				const QString& targetVariableName,
				const QString& relatedNamespace,
				uint hIndents = 1);
	static void WriteEnumConversionFromString(
				QTextStream& stream,
				const CSdlEnum& sdlEnum,
				const QString& sourceVariableName,
				const QString& targetVariableName,
				const QString& relatedNamespace,
				uint hIndents = 1);
};


} // namespace imtsdl
