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
	[[nodiscard]] CSchema GetSchema() const;
	void SetSchema(const CSchema& schema);

	[[nodiscard]] QList<CEncoding> GetEncodings() const;
	void SetEncodings(const QList<CEncoding>& encodings);

	[[nodiscard]] static bool ReadFromJsonObject(CMediaType& object, const QJsonObject& jsonObject);

	bool operator==(const CMediaType& other) const;
	bool operator!=(const CMediaType& other) const { return !(operator==(other)); }

private:
	CSchema m_schema;
	QList<CEncoding> m_encodings;
};


} // namespace imtoas


Q_DECLARE_METATYPE(imtoas::CMediaType);


