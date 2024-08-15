#pragma once


// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonValue>
#include <QtCore/QString>
#include <QtCore/QList>

// ImtCore includes
#include <imtoas/CServerVariable.h>


namespace imtoas
{


class CServer
{
public:
	[[nodiscard]] QString GetUrl() const;
	void SetUrl(const QString& url);

	[[nodiscard]] QString GetDescription() const;
	void SetDescription(const QString& description);

	[[nodiscard]] QList<CServerVariable> GetVariables() const;
	void SetVariables(const QList<CServerVariable>& variables);

	[[nodiscard]] static bool ReadFromJsonObject(CServer& object, const QJsonObject& jsonObject);

	bool operator==(const CServer& other) const;
	bool operator!=(const CServer& other) const { return !(operator==(other)); }

private:
	QString m_url;
	QString m_description;
	QList<CServerVariable> m_variables;
};


} // namespace imtoas


Q_DECLARE_METATYPE(imtoas::CServer);


