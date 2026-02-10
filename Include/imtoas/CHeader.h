// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonValue>
#include <QtCore/QString>

// ImtCore includes
#include <imtoas/CSchema.h>


namespace imtoas
{


class CHeader
{
public:
	[[nodiscard]] QString GetId() const;
	void SetId(const QString& id);

	[[nodiscard]] CSchema GetSchema() const;
	void SetSchema(const CSchema& schema);
	
	[[nodiscard]] QString GetDescription() const;
	void SetDescription(const QString& description);

	static bool ReadFromJsonObject(CHeader& object, const QJsonObject& jsonObject, const QJsonObject& globalObject);

	bool operator==(const CHeader& other) const;
	bool operator!=(const CHeader& other) const { return !(operator==(other)); }


private:
	QString m_id;
	CSchema m_schema;
	QString m_description;
};


} // namespace imtoas


Q_DECLARE_METATYPE(imtoas::CHeader);


