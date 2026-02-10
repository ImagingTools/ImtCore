// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonValue>
#include <QtCore/QList>

// ImtCore includes
#include <imtoas/CSchema.h>
#include <imtoas/CEncoding.h>


namespace imtoas
{


class CMediaType
{
public:
	[[nodiscard]]QString GetId() const;
	void SetId(const QString& id);

	[[nodiscard]] CSchema GetSchema() const;
	void SetSchema(const CSchema& schema);

	[[nodiscard]] QList<CEncoding> GetEncodings() const;
	void SetEncodings(const QList<CEncoding>& encodings);

	[[nodiscard]] static bool ReadFromJsonObject(CMediaType& object, const QJsonObject& jsonObject, const QJsonObject& globalObject);

	bool operator==(const CMediaType& other) const;
	bool operator!=(const CMediaType& other) const { return !(operator==(other)); }


private:
	QString m_id;
	CSchema m_schema;
	QList<CEncoding> m_encodings;
};


} // namespace imtoas


Q_DECLARE_METATYPE(imtoas::CMediaType);


