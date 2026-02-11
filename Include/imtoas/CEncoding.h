// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonValue>
#include <QtCore/QString>
#include <QtCore/QList>

// ImtCore includes
#include <imtoas/CHeader.h>


namespace imtoas
{


class CEncoding
{
public:
	CEncoding();

	[[nodiscard]] QString GetId() const;
	void SetId(const QString& id);

	[[nodiscard]] QString GetContentType() const;
	void SetContentType(const QString& contentType);

	[[nodiscard]] QList<CHeader> GetHeaders() const;
	void SetHeaders(const QList<CHeader>& headers);

	[[nodiscard]] QString GetStyle() const;
	void SetStyle(const QString& style);

	[[nodiscard]] bool IsExplode() const;
	void SetExplode(bool explode = true);

	[[nodiscard]] bool IsAllowReserved() const;
	void SetAllowReserved(bool allowReserved = true);
	
	static bool ReadFromJsonObject(CEncoding& object, const QJsonObject& jsonObject, const QJsonObject& globalObject);

	bool operator==(const CEncoding& other) const;
	bool operator!=(const CEncoding& other) const { return !(operator==(other)); }

private:
	QString m_id;
	QString m_contentType;
	QList<CHeader> m_headers;
	QString m_style;
	bool m_isExplode;
	bool m_allowReserved;
};


} // namespace imtoas


Q_DECLARE_METATYPE(imtoas::CEncoding);


