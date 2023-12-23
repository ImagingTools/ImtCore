#include <imtbase/CJsonHelper.h>


namespace imtbase
{


bool CJsonHelper::GetBoolValue(const QJsonObject& object, const QString& key, bool &value)
{
	if (object.contains(key)){
		QJsonValue jsonValue = object[key];
		if (jsonValue.isBool()){
			value = jsonValue.toBool();
			return true;
		}
	}

	return false;
}

bool CJsonHelper::GetIntValue(const QJsonObject& object, const QString& key, int& value)
{
	if (object.contains(key)){
		QJsonValue jsonValue = object[key];
		if (jsonValue.isDouble()){
			value = jsonValue.toInt();
			return true;
		}
	}

	return false;
}


bool CJsonHelper::GetDoubleValue(const QJsonObject& object, const QString& key, double &value)
{
	if (object.contains(key)){
		QJsonValue jsonValue = object[key];
		if (jsonValue.isDouble()){
			value = jsonValue.toDouble();
			return true;
		}
	}

	return false;
}


bool CJsonHelper::GetStringValue(const QJsonObject& object, const QString& key, QString &value)
{
	if (object.contains(key)){
		QJsonValue jsonValue = object[key];
		if (jsonValue.isString()){
			value = jsonValue.toString();
			return true;
		}
	}

	return false;
}


bool CJsonHelper::GetArrayValue(const QJsonObject& object, const QString& key, QJsonArray &value)
{
	if (object.contains(key)){
		QJsonValue jsonValue = object[key];
		if (jsonValue.isArray()){
			value = jsonValue.toArray();
			return true;
		}
	}

	return false;
}


bool CJsonHelper::GetObjectValue(const QJsonObject& object, const QString& key, QJsonObject &value)
{
	if (object.contains(key)){
		QJsonValue jsonValue = object[key];
		if (jsonValue.isObject()){
			value = jsonValue.toObject();
			return true;
		}
	}

	return false;
}

bool CJsonHelper::GetStringListValue(const QJsonObject& object, const QString& key, QStringList& value)
{
	if (object.contains(key)){
		QJsonValue jsonValue = object[key];
		if (jsonValue.isArray()){
			QJsonArray jsonArray = jsonValue.toArray();
			for(const QJsonValue& arrayValue: ::std::as_const(jsonArray)){
				if(!arrayValue.isString()) {
					return false;
				}
				value << arrayValue.toString();
			}
		}
	}
	return false;
}

bool CJsonHelper::GetVariantListValue(const QJsonObject& object, const QString& key, QVariantList& value)
{
	if (object.contains(key)){
		QJsonValue jsonValue = object[key];
		if (jsonValue.isArray()){
			value = jsonValue.toArray().toVariantList();
			return true;
		}
	}
	return false;
}


bool CJsonHelper::GetBoolValue(const QJsonArray& array, int index, bool &value)
{
	if (index >= 0 && index < array.count()){
		QJsonValue jsonValue = array[index];
		if (jsonValue.isBool()){
			value = jsonValue.toBool();
			return true;
		}
	}

	return false;
}


bool CJsonHelper::GetDoubleValue(const QJsonArray& array, int index, double &value)
{
	if (index >= 0 && index < array.count()){
		QJsonValue jsonValue = array[index];
		if (jsonValue.isDouble()){
			value = jsonValue.toDouble();
			return true;
		}
	}

	return false;
}


bool CJsonHelper::GetStringValue(const QJsonArray& array, int index, QString &value)
{
	if (index >= 0 && index < array.count()){
		QJsonValue jsonValue = array[index];
		if (jsonValue.isString()){
			value = jsonValue.toString();
			return true;
		}
	}

	return false;
}


bool CJsonHelper::GetArrayValue(const QJsonArray& array, int index, QJsonArray &value)
{
	if (index >= 0 && index < array.count()){
		QJsonValue jsonValue = array[index];
		if (jsonValue.isArray()){
			value = jsonValue.toArray();
			return true;
		}
	}

	return false;
}


bool CJsonHelper::GetObjectValue(const QJsonArray& array, int index, QJsonObject &value)
{
	if (index >= 0 && index < array.count()){
		QJsonValue jsonValue = array[index];
		if (jsonValue.isObject()){
			value = jsonValue.toObject();
			return true;
		}
	}

	return false;
}


} // namespace imtbase


