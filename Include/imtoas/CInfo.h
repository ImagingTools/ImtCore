// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonValue>
#include <QtCore/QString>

// ImtCore includes
#include <imtoas/CContactInfo.h>
#include <imtoas/CLicense.h>


namespace imtoas
{


class CInfo
{
public:
	[[nodiscard]] QString GetTitle() const;
	void SetTitle(const QString& title);

	[[nodiscard]] QString GetSummary() const;
	void SetSummary(const QString& summary);

	[[nodiscard]] QString GetDescription() const;
	void SetDescription(const QString& description);

	[[nodiscard]] QString GetTermsOfService() const;
	void SetTermsOfService(const QString& termsOfService);
	
	[[nodiscard]] CContactInfo GetContact() const;
	void SetContact(const CContactInfo& contact);

	[[nodiscard]] CLicense GetLicense() const;
	void SetLicense(const CLicense& license);

	[[nodiscard]] QString GetVersion() const;
	void SetVersion(const QString& version);

	static bool ReadFromJsonObject(CInfo& object, const QJsonObject& jsonObject, const QJsonObject& globalObject);

	bool operator==(const CInfo& other) const;
	bool operator!=(const CInfo& other) const { return !(operator==(other)); }

private:
	QString m_title;
	QString m_summary;
	QString m_description;
	QString m_termsOfService;
	CContactInfo m_contact;
	CLicense m_license;
	QString m_version;
};


} // namespace imtoas


Q_DECLARE_METATYPE(imtoas::CInfo);


