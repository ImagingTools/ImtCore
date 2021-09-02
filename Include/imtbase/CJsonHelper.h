#pragma once


// Qt includes
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>


namespace imtbase
{


class CJsonHelper
{
public:
	static bool GetBoolValue(const QJsonObject& object, const QString& key, bool &value);
	static bool GetDoubleValue(const QJsonObject& object, const QString& key, double &value);
	static bool GetStringValue(const QJsonObject& object, const QString& key, QString &value);
	static bool GetArrayValue(const QJsonObject& object, const QString& key, QJsonArray &value);
	static bool GetObjectValue(const QJsonObject& object, const QString& key, QJsonObject &value);
	static bool GetStringListValue(const QJsonObject& object, const QString& key, QStringList &value);
	static bool GetVariantListValue(const QJsonObject& object, const QString& key, QVariantList &value);
	static bool GetBoolValue(const QJsonArray& array, int index, bool &value);
	static bool GetDoubleValue(const QJsonArray& array, int index, double &value);
	static bool GetStringValue(const QJsonArray& array, int index, QString &value);
	static bool GetArrayValue(const QJsonArray& array, int index, QJsonArray &value);
	static bool GetObjectValue(const QJsonArray& array, int index, QJsonObject &value);
};


} // namespace imtbase
