// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtoas/CSecurity.h>


namespace imtoas
{


QList<QString> CSecurity::GetNames() const
{
	return m_names;
}


void CSecurity::SetNames(const QList<QString>& name)
{
	if (name != m_names){
		m_names = name;
	}
}


bool CSecurity::ReadFromJsonObject(CSecurity& object, const QJsonObject& jsonObject, const QJsonObject& globalObject)
{
	if (jsonObject.contains("name")){
		const QJsonArray nameArray = jsonObject["name"].toArray();
		qsizetype nameCount = nameArray.size();
		QList<QString> nameList;
		for (qsizetype nameIndex = 0; nameIndex < nameCount; ++nameIndex){
			nameList << nameArray[nameIndex].toString();
		}
		object.SetNames(nameList);
	}

	return true;
}


bool CSecurity::operator==(const CSecurity& other) const
{
	bool retVal = true;
	retVal = retVal && (m_names == other.m_names);

	return retVal;
}


} // namespace imtoas

