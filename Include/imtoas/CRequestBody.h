// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonValue>
#include <QtCore/QString>
#include <QtCore/QList>

// ImtCore includes
#include <imtoas/CMediaType.h>


namespace imtoas
{


class CRequestBody
{
public:
	CRequestBody();

	[[nodiscard]] QString GetDescription() const;
	void SetDescription(const QString& description);

	[[nodiscard]] QList<CMediaType> GetContent() const;
	void SetContent(const QList<CMediaType>& content);

	[[nodiscard]] bool IsRequired() const;
	void SetRequired(bool required = true);

	[[nodiscard]] static bool ReadFromJsonObject(CRequestBody& object, const QJsonObject& jsonObject, const QJsonObject& globalObject);

	bool operator==(const CRequestBody& other) const;
	bool operator!=(const CRequestBody& other) const { return !(operator==(other)); }

	[[nodiscard]] QString GetId() const;
	void SetId(const QString& id);

private:
	QString m_id;
	QString m_description;
	QList<CMediaType> m_content;
	bool m_isRequired;
};


} // namespace imtoas


Q_DECLARE_METATYPE(imtoas::CRequestBody);


