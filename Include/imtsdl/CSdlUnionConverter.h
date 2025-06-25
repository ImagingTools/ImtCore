#pragma once


// Qt includes
#include <QtCore/QSet>
#include <QtCore/QTextStream>

// ACF includes
#include <icomp/TReferenceMember.h>

// ImtCore includes
#include <imtsdl/imtsdl.h>
#include <imtsdl/CSdlUnion.h>
#include <imtsdlgenv2/CSdlGenTools.h>


namespace imtsdl
{


class CSdlUnionConverter: protected CSdlTools
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

	// generate methods
	static void WriteConversionFromUnion(
				QTextStream& stream,
				const CSdlUnion& sdlUnion,
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
				const QString returnOnFail = QStringLiteral("false"));
	static void WriteUnionConversionFromData(
				QTextStream& stream,
				const CSdlUnion& sdlUnion,
				const QString& sourceVariableName,
				const QString& targetVariableName,
				const QString& relatedNamespace,
				const QString& modelIndex,
				const imtsdl::ISdlTypeListProvider& listProvider,
				const imtsdl::ISdlEnumListProvider& enumlistProvider,
				const imtsdl::ISdlUnionListProvider& unionlistProvider,
				uint hIndents = 1,
				const ConversionType& conversionType = CT_MODEL_SCALAR,
				const QString& targetName = QString());
};


} // namespace imtsdl
