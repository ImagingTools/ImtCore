// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CSqliteXPathExtractorComp.h>


namespace imtdb
{


// public methods

// reimplemented (ISqlXPathExtractor)

QString CSqliteXPathExtractorComp::ExtractXPath(
			const QString& jsonKey,
			const QString& fieldId,
			QMetaType::Type metaType,
			const QString& tableAlias) const
{
	QString jsonPath = QString("$.%1").arg(fieldId);
	QString fieldWithAlias = tableAlias.isEmpty()
								 ? QString(R"("%1")").arg(jsonKey)
								 : QString(R"(%1."%2")").arg(tableAlias, jsonKey);

	switch (metaType){
	case QMetaType::QString:
	case QMetaType::QByteArray:
		return QString(R"(json_extract(%1, '%2'))").arg(fieldWithAlias, jsonPath);

	case QMetaType::Int:
	case QMetaType::UInt:
	case QMetaType::Short:
	case QMetaType::UShort:
	case QMetaType::LongLong:
	case QMetaType::ULongLong:
		return QString(R"(CAST(json_extract(%1, '%2') AS INTEGER))").arg(fieldWithAlias, jsonPath);

	case QMetaType::Double:
	case QMetaType::Float:
		return QString(R"(CAST(json_extract(%1, '%2') AS REAL))").arg(fieldWithAlias, jsonPath);

	case QMetaType::Bool:
		return QString(R"(CAST(json_extract(%1, '%2') AS BOOLEAN))").arg(fieldWithAlias, jsonPath);

	case QMetaType::QJsonObject:
		return QString(R"(json_extract(%1, '%2'))").arg(fieldWithAlias, jsonPath);

	default:
		return QString();
	}
}


} // namespace imtdb


