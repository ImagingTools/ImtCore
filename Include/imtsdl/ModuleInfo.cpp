#include <imtsdl/ModuleInfo.h>


// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonValue>


namespace imtsdl
{


// serialize methods of embedded class CSchemaParams

bool CSchemaParams::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (Name){
		jsonObject["Name"] = QJsonValue::fromVariant(*Name);
	}

	if (Namespace){
		jsonObject["Namespace"] = QJsonValue::fromVariant(*Namespace);
	}

	if (Version){
		jsonObject["Version"] = QJsonValue::fromVariant(*Version);
	}

	return true;
}


bool CSchemaParams::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("Name") && jsonObject["Name"].isString()){
		Name = jsonObject["Name"].toString();
	}

	if (jsonObject.contains("Namespace") && jsonObject["Namespace"].isString()){
		Namespace = jsonObject["Namespace"].toString();
	}

	if (jsonObject.contains("Version") && jsonObject["Version"].isString()){
		Version = jsonObject["Version"].toString();
	}

	return true;
}


// serialize methods of embedded class CEntity

bool CEntity::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!Name){
		return false;
	}
	jsonObject["Name"] = (*Name);

	if (TargerHeaderFilePath){
		jsonObject["TargerHeaderFilePath"] = (*TargerHeaderFilePath);
	}

	if (QmlModuleName){
		jsonObject["QmlModuleName"] = (*QmlModuleName);
	}

	return true;
}


bool CEntity::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("Name") || ! jsonObject["Name"].isString()){
		return false;
	}
	Name = jsonObject["Name"].toString();

	if (jsonObject.contains("TargerHeaderFilePath") && jsonObject["TargerHeaderFilePath"].isString()){
		TargerHeaderFilePath = jsonObject["TargerHeaderFilePath"].toString();
	}

	if (jsonObject.contains("QmlModuleName") && jsonObject["QmlModuleName"].isString()){
		QmlModuleName = jsonObject["QmlModuleName"].toString();
	}

	return true;
}


// serialize methods of embedded class CModule

bool CModule::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (Types){
		QJsonArray newTypesArray;
		for (qsizetype typesIndex = 0; typesIndex < Types->size(); ++typesIndex){
			QJsonObject newTypesJsonObject;
			if (!Types->at(typesIndex).WriteToJsonObject(newTypesJsonObject)){
				return false;
			}
			newTypesArray << newTypesJsonObject;
		}
		jsonObject["Types"] = newTypesArray;
	}

	if (Enums){
		QJsonArray newEnumsArray;
		for (qsizetype enumsIndex = 0; enumsIndex < Enums->size(); ++enumsIndex){
			QJsonObject newEnumsJsonObject;
			if (!Enums->at(enumsIndex).WriteToJsonObject(newEnumsJsonObject)){
				return false;
			}
			newEnumsArray << newEnumsJsonObject;
		}
		jsonObject["Enums"] = newEnumsArray;
	}

	if (Unions){
		QJsonArray newUnionsArray;
		for (qsizetype unionsIndex = 0; unionsIndex < Unions->size(); ++unionsIndex){
			QJsonObject newUnionsJsonObject;
			if (!Unions->at(unionsIndex).WriteToJsonObject(newUnionsJsonObject)){
				return false;
			}
			newUnionsArray << newUnionsJsonObject;
		}
		jsonObject["Unions"] = newUnionsArray;
	}

	if (!Params){
		return false;
	}
	QJsonObject paramsjsonObject;
	const bool isParamsAdded = Params->WriteToJsonObject(paramsjsonObject);
	if (!isParamsAdded){
		return false;
	}
	jsonObject["Params"] = paramsjsonObject;

	return true;
}


bool CModule::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("Types") && jsonObject["Types"].isArray()){
		const QJsonArray typesjsonArray = jsonObject["Types"].toArray();
		const qsizetype typesArrayCount = typesjsonArray.size();
		if (typesArrayCount <= 0){
			return false;
		}
		Types.emplace();

		for (qsizetype typesIndex = 0; typesIndex < typesArrayCount; ++typesIndex){
			CEntity tempType;
			if (!tempType.ReadFromJsonObject(typesjsonArray[typesIndex].toObject())){
				return false;
			}
			Types->append(tempType);
		}
	}

	if (jsonObject.contains("Enums") && jsonObject["Enums"].isArray()){
		const QJsonArray enumsJsonArray = jsonObject["Enums"].toArray();
		const qsizetype enumsArrayCount = enumsJsonArray.size();
		if (enumsArrayCount <= 0){
			return false;
		}
		Enums.emplace();
		for (qsizetype enumsIndex = 0; enumsIndex < enumsArrayCount; ++enumsIndex){
			CEntity tempEnum;
			if (!tempEnum.ReadFromJsonObject(enumsJsonArray[enumsIndex].toObject())){
				return false;
			}
			Enums->append(tempEnum);
		}
	}

	if (jsonObject.contains("Unions") && jsonObject["Unions"].isArray()){
		const QJsonArray unionsJsonArray = jsonObject["Unions"].toArray();
		const qsizetype unionsArrayCount = unionsJsonArray.size();
		if (unionsArrayCount <= 0){
			return false;
		}
		Unions.emplace();
		for (qsizetype unionsIndex = 0; unionsIndex < unionsArrayCount; ++unionsIndex){
			CEntity tempEnum;
			if (!tempEnum.ReadFromJsonObject(unionsJsonArray[unionsIndex].toObject())){
				return false;
			}
			Unions->append(tempEnum);
		}
	}

	if (!jsonObject.contains("Params") || ! jsonObject["Params"].isObject()){
		return false;
	}
	Params.emplace();
	const bool isParamsReaded = Params->ReadFromJsonObject(jsonObject["Params"].toObject());
	if (!isParamsReaded){
		return false;
	}

	return true;
}


} // namespace imtsdl




