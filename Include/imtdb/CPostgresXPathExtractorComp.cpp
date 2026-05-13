// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CPostgresXPathExtractorComp.h>


namespace imtdb
{


// public methods

// reimplemented (ISqlXPathExtractor)

QString CPostgresXPathExtractorComp::ExtractXPath(
			const QString& jsonKey,
			const QString& fieldId,
			QMetaType::Type metaType,
			const QString& tableAlias) const
{
	const QString prefix = tableAlias.isEmpty()
	? QString(R"("%1")").arg(jsonKey)
	: QString(R"(%0."%1")").arg(tableAlias, jsonKey);

	switch (metaType){
	case QMetaType::QString:
	case QMetaType::QByteArray:
		return QString(R"(%1->>'%2')").arg(prefix, fieldId);

	case QMetaType::Int:
	case QMetaType::UInt:
	case QMetaType::Short:
	case QMetaType::UShort:
		return QString(R"((%1->>'%2')::int)").arg(prefix, fieldId);

	case QMetaType::LongLong:
	case QMetaType::ULongLong:
		return QString(R"((%1->>'%2')::bigint)").arg(prefix, fieldId);

	case QMetaType::Double:
	case QMetaType::Float:
		return QString(R"((%1->>'%2')::float)").arg(prefix, fieldId);

	case QMetaType::Bool:
		return QString(R"((%1->>'%2')::boolean)").arg(prefix, fieldId);

	case QMetaType::QJsonObject:
	case QMetaType::QJsonValue:
	case QMetaType::QJsonArray:
		return QString(R"(%1->'%2')").arg(prefix, fieldId);

	default:
		return QString();
	}
}


} // namespace imtdb


