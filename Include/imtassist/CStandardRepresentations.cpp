// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtassist/CStandardRepresentations.h>


namespace imtassist
{


const QByteArray CStandardRepresentations::s_selectableListType = QByteArrayLiteral("imtassist.selectableList");


CParameterSchema CStandardRepresentations::CreateSelectableListSchema()
{
	std::shared_ptr<CParameterSchema> itemSchemaPtr = std::make_shared<CParameterSchema>(s_selectableListType + ".item", SELECTABLE_LIST_VERSION);
	itemSchemaPtr->AddId("id", true);
	itemSchemaPtr->AddString("label", true, 500);
	itemSchemaPtr->AddString("description", false, 1000);

	CParameterSchema retVal(s_selectableListType, SELECTABLE_LIST_VERSION);
	retVal.AddString("title", false, 200);
	retVal.AddEnum("selectionMode", false, QStringList() << "single" << "multiple");
	retVal.AddStringPattern("selectionKind", false, 64, QStringLiteral("^[A-Za-z0-9._-]{1,64}$"));
	retVal.AddObjectList("items", true, itemSchemaPtr, SELECTABLE_LIST_MAX_ITEMS);

	return retVal;
}


QJsonObject CStandardRepresentations::CreateSelectableListItem(const QByteArray& id, const QString& label, const QString& description)
{
	QJsonObject retVal;
	retVal["id"] = QString::fromUtf8(id);
	retVal["label"] = label;
	if (!description.isEmpty()){
		retVal["description"] = description;
	}

	return retVal;
}


QJsonObject CStandardRepresentations::CreateSelectableListData(const QJsonArray& items, const QString& title, bool isMultiSelection)
{
	QJsonObject retVal;
	retVal["items"] = items;
	retVal["selectionMode"] = isMultiSelection ? QStringLiteral("multiple") : QStringLiteral("single");
	if (!title.isEmpty()){
		retVal["title"] = title;
	}

	return retVal;
}


} // namespace imtassist

