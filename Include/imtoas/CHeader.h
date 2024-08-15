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
	[[nodiscard]] CSchema GetSchema() const;
	void SetSchema(const CSchema& schema);
	
	[[nodiscard]] QString GetDescription() const;
	void SetDescription(const QString& description);

	static bool ReadFromJsonObject(CHeader& object, const QJsonObject& jsonObject, const QJsonObject& globalObject);

	bool operator==(const CHeader& other) const;
	bool operator!=(const CHeader& other) const { return !(operator==(other)); }

private:
	CSchema m_schema;
	QString m_description;
};


} // namespace imtoas


Q_DECLARE_METATYPE(imtoas::CHeader);


