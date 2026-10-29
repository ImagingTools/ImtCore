// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CTagInfoRepresentation.h>


// Qt includes
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>


namespace imtservergql
{


// public static methods

bool CTagInfoRepresentation::FillTags(
			const imtbase::IObjectCollectionIterator& objectCollectionIterator,
			istd::TNullableValue<imtsdl::TElementList<sdl::V1_0::imtbase::CTagInfo>>& tags)
{
	const QVariant tagsValue = objectCollectionIterator.GetElementInfo(QByteArrayLiteral("Tags"));
	if (!tagsValue.isValid()){
		return false;
	}

	QList<sdl::V1_0::imtbase::CTagInfo> tagList;

	const QJsonArray tagsArray = QJsonDocument::fromJson(tagsValue.toString().toUtf8()).array();
	for (const QJsonValue& tagValue : tagsArray){
		const QJsonObject tagObject = tagValue.toObject();

		sdl::V1_0::imtbase::CTagInfo tagInfo;
		tagInfo.id = tagObject.value(QStringLiteral("id")).toString().toUtf8();
		tagInfo.name = tagObject.value(QStringLiteral("name")).toString();
		tagInfo.color = tagObject.value(QStringLiteral("color")).toString();

		// SQLite reports the flag as 0 or 1.
		const QJsonValue isSystemValue = tagObject.value(QStringLiteral("isSystem"));
		tagInfo.isSystem = isSystemValue.isBool() ? isSystemValue.toBool() : (isSystemValue.toInt() != 0);

		tagList << tagInfo;
	}

	tags.Emplace().FromList(tagList);

	return true;
}


} // namespace imtservergql


