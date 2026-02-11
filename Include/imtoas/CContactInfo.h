// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonValue>
#include <QtCore/QString>


namespace imtoas
{


class CContactInfo
{
public:
	[[nodiscard]] QString GetName() const;
	void SetName(const QString& name);

	[[nodiscard]] QString GetUrl() const;
	void SetUrl(const QString& url);

	[[nodiscard]] QString GetEmail() const;
	void SetEmail(const QString& email);

	static bool ReadFromJsonObject(CContactInfo& object, const QJsonObject& jsonObject, const QJsonObject& globalObject);

	bool operator==(const CContactInfo& other) const;
	bool operator!=(const CContactInfo& other) const { return !(operator==(other)); }

private:
	QString m_name;
	QString m_url;
	QString m_email;
};


} // namespace imtoas


Q_DECLARE_METATYPE(imtoas::CContactInfo);


