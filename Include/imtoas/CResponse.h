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
#include <imtoas/CHeader.h>


namespace imtoas
{


class CResponse
{
public:
	[[nodiscard]] QString GetId() const;
	void SetId(const QString& id);

	[[nodiscard]] QString GetDescription() const;
	void SetDescription(const QString& description);

	[[nodiscard]] QList<CHeader> GetHeaders() const;
	void SetHeaders(const QList<CHeader>& headers);

	[[nodiscard]] QList<CMediaType> GetContentList() const;
	void SetContentList(const QList<CMediaType>& contentList);

	[[nodiscard]] static bool ReadFromJsonObject(CResponse& object, const QJsonObject& jsonObject, const QJsonObject& globalObject);

	bool operator==(const CResponse& other) const;
	bool operator!=(const CResponse& other) const { return !(operator==(other)); }


private:
	QString m_id;
	QString m_description;
	QList<CHeader> m_headers;
	QList<CMediaType> m_contentList;
};


} // namespace imtoas


Q_DECLARE_METATYPE(imtoas::CResponse);


