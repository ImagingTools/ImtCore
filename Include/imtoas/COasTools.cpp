#include <imtoas/COasTools.h>


// ImtCore includes
#include <imtoas/CReferenceObject.h>


namespace imtoas
{


bool COasTools::DeReferenceObject(
			const QJsonObject& jsonObject,
			const QJsonObject& lookUpjsonObject,
			QJsonObject& derefed,
			bool* isReferencePtr)
{
	CReferenceObject referenceObject;
	bool isReference = CReferenceObject::ReadFromJsonObject(referenceObject, jsonObject, lookUpjsonObject);
	if (isReferencePtr != nullptr){
		*isReferencePtr = isReference;
	}
	if (!isReference){
		return true;
	}

	QStringList referencePathParts = referenceObject.GetReference().split('/');
	QJsonObject foundObject;
	// remove first hash
	if (referencePathParts.constFirst() == '#'){
		referencePathParts.removeAt(0);
	}
	const QString firstValue = referencePathParts.constFirst();
	if (!firstValue.isEmpty()){
		if (!lookUpjsonObject.contains(firstValue)){
			return false;
		}
		QJsonValue jsonValue = lookUpjsonObject[firstValue];
		if (!jsonValue.isObject()){
			return false;
		}
		foundObject = jsonValue.toObject();
		referencePathParts.removeAt(0);
	}
	for (const QString& path: referencePathParts){
		if (!foundObject.contains(path)){
			return false;
		}
		QJsonValue jsonValue = foundObject[path];
		if (!jsonValue.isObject()){
			return false;
		}
		foundObject = jsonValue.toObject();
	}

	derefed = foundObject;

	return true;
}


QJsonObject COasTools::FindSecuritySchemeByName(const QString& schemeName, const QJsonObject& lookUpjsonObject)
{
	QJsonObject jsonObject = lookUpjsonObject[QStringLiteral("components")].toObject();
	jsonObject = jsonObject[QStringLiteral("securitySchemes")].toObject();
	for (QJsonObject::const_iterator schemeIter = jsonObject.constBegin(); schemeIter != jsonObject.constEnd(); ++schemeIter){
		if (schemeIter.key() == schemeName){
			return schemeIter->toObject();
		}
	}

	return QJsonObject();
}




} // namespace imtoas
