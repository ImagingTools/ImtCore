#include <imtdb/CPostgresXPathExtractorComp.h>


namespace imtdb
{


// public methods

// reimplemented (ISqlXPathExtractor)

QString CPostgresXPathExtractorComp::ExtractXPath(
			const QString& jsonKey,
			const QString& key,
			QMetaType::Type metaType,
			const QString& tableAlias) const
{
	const QString prefix = tableAlias.isEmpty()
	? QString(R"("%1")").arg(jsonKey)
	: QString(R"(%0."%1")").arg(tableAlias, jsonKey);
	
	switch (metaType) {
	case QMetaType::QString:
	case QMetaType::QByteArray:
		return QString(R"(%1->>'%2')").arg(prefix, key);
		
	case QMetaType::Int:
	case QMetaType::UInt:
	case QMetaType::Short:
	case QMetaType::UShort:
		return QString(R"((%1->>'%2')::int)").arg(prefix, key);
		
	case QMetaType::LongLong:
	case QMetaType::ULongLong:
		return QString(R"((%1->>'%2')::bigint)").arg(prefix, key);
		
	case QMetaType::Double:
	case QMetaType::Float:
		return QString(R"((%1->>'%2')::float)").arg(prefix, key);
		
	case QMetaType::Bool:
		return QString(R"((%1->>'%2')::boolean)").arg(prefix, key);
		
	case QMetaType::QJsonObject:
		return QString(R"(%1->'%2')").arg(prefix, key);
		
	default:
		return QString();
	}
}


} // namespace imtdb


