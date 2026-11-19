// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/TReferenceMember.h>

// ImtCore includes
#include <imtsdl/imtsdl.h>
#include <imtsdl/ISdlTypeListProvider.h>
#include <imtsdl/ISdlEnumListProvider.h>
#include <imtsdl/ISdlUnionListProvider.h>
#include <imtsdl/CSdlUnion.h>


namespace imtsdlgencpp
{


/// \todo refactor this use class and it's properties instead of arguments
class CSdlUnionConverter
{

public:
	enum ConversionType
	{
		/// \todo rename it to 'CT_MODEL_SINGLE'
		CT_MODEL_SCALAR = 2,
		CT_MODEL_ARRAY = 3,
		CT_GQL_SCALAR = 8,
		CT_GQL_ARRAY = 12,
		CT_JSON_SCALAR = 32,
		CT_JSON_ARRAY = 48
	};

	/**
		Creates C++ conditions which select the scalar alternative of a union for a QVariant (\c variantExpression).
		\param scalarTypes	pairs of the SDL type and the converted C++ type of the scalar alternatives in declaration order.
		\param isJsonSource	the value is read from JSON, see the note below.
		\return pairs of the converted C++ type and the condition in the order in which they must be checked (as an 'if/else if' chain).
		At first every alternative is matched by its native metatype only, so e.g. a \c long value is never narrowed to an \c Int alternative.
		Values whose metatype matches no alternative (e.g. \c Short, \c UInt or integers read from JSON) are matched afterwards:
		integral alternatives (in declaration order) accept any integral number and any integral-valued floating number which fits into their range,
		floating alternatives accept any number.
		\note JSON has no separate integer type, so its numbers have an ambiguous type. For JSON sources integral-valued floating numbers
		are not matched by floating alternatives at first, i.e. they are read as integers if the union has an integral alternative which can hold the value.
	*/
	static QList<QPair<QString, QString>> GetScalarUnionMatchConditions(
				const QList<QPair<QString, QString>>& scalarTypes,
				const QString& variantExpression,
				bool isJsonSource);

	// generate methods
	static void WriteConversionFromUnion(
				QTextStream& stream,
				const imtsdl::CSdlUnion& sdlUnion,
				const QString& sourceVariableName,
				const QString& targetVariableName,
				const QString& relatedNamespace,
				const QString& targetName,
				const QString& modelIndex,
				const imtsdl::ISdlTypeListProvider& listProvider,
				const imtsdl::ISdlEnumListProvider& enumlistProvider,
				const imtsdl::ISdlUnionListProvider& unionlistProvider,
				uint hIndents = 1,
				const ConversionType& conversionType = CT_MODEL_SCALAR,
				const QString& addCommand = QString(),
				const QString& customModelTarget = QString(),
				const QString& returnOnFail = QStringLiteral("false"));
	static void WriteUnionConversionFromData(
				QTextStream& stream,
				const imtsdl::CSdlUnion& sdlUnion,
				const QString& sourceVariableName,
				const QString& targetVariableName,
				const QString& relatedNamespace,
				const QString& modelIndex,
				const imtsdl::ISdlTypeListProvider& listProvider,
				const imtsdl::ISdlEnumListProvider& enumlistProvider,
				const imtsdl::ISdlUnionListProvider& unionlistProvider,
				uint hIndents = 1,
				const ConversionType& conversionType = CT_MODEL_SCALAR,
				const QString& targetName = QString(),
				const QString& arraySourceVariableName = QString());
};


} // namespace imtsdlgencpp
