// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonValue>
#include <QtCore/QString>


namespace imtoas
{


class CReferenceObject
{
public:
	[[nodiscard]] QString GetReference() const;
	void SetReference(const QString& reference);

	[[nodiscard]] QString GetSummary() const;
	void SetSummary(const QString& summary);

	[[nodiscard]] QString GetDescription() const;
	void SetDescription(const QString& description);

	[[nodiscard]] static bool ReadFromJsonObject(CReferenceObject& object, const QJsonObject& jsonObject, const QJsonObject& globalObject);

	bool operator==(const CReferenceObject& other) const;
	bool operator!=(const CReferenceObject& other) const { return !(operator==(other)); }

private:
	QString m_reference;
	QString m_summary;
	QString m_description;
};


} // namespace imtoas


Q_DECLARE_METATYPE(imtoas::CReferenceObject);


