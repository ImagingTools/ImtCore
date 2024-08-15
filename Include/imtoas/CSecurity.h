#pragma once


// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonValue>
#include <QtCore/QString>
#include <QtCore/QList>


namespace imtoas
{


class CSecurity
{
public:
	[[nodiscard]] QList<QString> GetNames() const;
	void SetNames(const QList<QString>& name);

	[[nodiscard]] static bool ReadFromJsonObject(CSecurity& object, const QJsonObject& jsonObject, const QJsonObject& globalObject);

	bool operator==(const CSecurity& other) const;
	bool operator!=(const CSecurity& other) const { return !(operator==(other)); }

private:
	QList<QString> m_names;
};


} // namespace imtoas


Q_DECLARE_METATYPE(imtoas::CSecurity);


