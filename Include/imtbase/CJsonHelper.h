#pragma once


// Qt includes
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>


namespace imtbase
{


class CJsonHelper
{
public:
	static bool CJsonHelper::GetBoolValue(const QJsonObject& object, const QString& key, bool &value);
	static bool CJsonHelper::GetDoubleValue(const QJsonObject& object, const QString& key, double &value);
	static bool CJsonHelper::GetStringValue(const QJsonObject& object, const QString& key, QString &value);
	static bool CJsonHelper::GetArrayValue(const QJsonObject& object, const QString& key, QJsonArray &value);
	static bool CJsonHelper::GetObjectValue(const QJsonObject& object, const QString& key, QJsonObject &value);
	static bool CJsonHelper::GetBoolValue(const QJsonArray& array, int index, bool &value);
	static bool CJsonHelper::GetDoubleValue(const QJsonArray& array, int index, double &value);
	static bool CJsonHelper::GetStringValue(const QJsonArray& array, int index, QString &value);
	static bool CJsonHelper::GetArrayValue(const QJsonArray& array, int index, QJsonArray &value);
	static bool CJsonHelper::GetObjectValue(const QJsonArray& array, int index, QJsonObject &value);
};


} // namespace imtbase
