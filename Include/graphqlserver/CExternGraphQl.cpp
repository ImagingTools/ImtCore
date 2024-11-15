#include <graphqlserver/CExternGraphQl.h>

// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>


namespace graphqlserver
{


void CExternGraphQl::CreateParams(const imtgql::CGqlObject& gqlObject, QJsonObject& result) const
{
	QByteArrayList fieldIds = gqlObject.GetFieldIds();
	for (const QByteArray& fieldId: fieldIds){
		QVariant value = gqlObject.GetFieldArgumentValue(fieldId);
		if (gqlObject.IsObject(fieldId)){
			QJsonObject jsonObject;
			const imtgql::CGqlObject* gqlObjectPtr = gqlObject.GetFieldArgumentObjectPtr(fieldId);
			if (gqlObjectPtr != nullptr){
				CreateParams(*gqlObjectPtr, jsonObject);
				result.insert(fieldId, jsonObject);
			}
		}
		else if (gqlObject.IsObjectList(fieldId)){
			QJsonArray jsonArray;
			QList<const imtgql::CGqlObject*> gqlObjectPtrList = gqlObject.GetFieldArgumentObjectPtrList(fieldId);
			for (int index = 0; index < gqlObjectPtrList.count(); index++){
				const imtgql::CGqlObject* gqlObjectPtr = gqlObject.GetFieldArgumentObjectPtr(fieldId, index);
				if (gqlObjectPtr != nullptr){
					QJsonObject jsonObject;
					CreateParams(*gqlObjectPtr, jsonObject);
					jsonArray.append(jsonObject);
				}
			}
			result.insert(fieldId, jsonArray);

		}
		else{
			if (value.canConvert<QStringList>()){
				QStringList stringList = value.value<QStringList>();
				QJsonArray jsonArray;
				for (int i = 0; i < stringList.count(); i++){
					jsonArray.append(stringList.value(i));
				}
				result.insert(fieldId, jsonArray);
			}
			else if (value.canConvert<QList<QString>>()){
				QList<QString> stringList = value.value<QList<QString>>();
				QJsonArray jsonArray;
				for (int i = 0; i < stringList.count(); i++){
					jsonArray.append(stringList.value(i));
				}
				result.insert(fieldId, jsonArray);
			}
			else{
				result.insert(fieldId, value.toString());
			}
		}
	}
}


void CExternGraphQl::CreateResultKeys(const imtgql::CGqlObject& gqlObject, graphqlserver::ResultKeys& resultKeys) const
{
	QByteArrayList fieldIds = gqlObject.GetFieldIds();
	for (const QByteArray& fieldId: fieldIds){
		if (gqlObject.IsObject(fieldId)){
			ResultKeys childKeys;
			const imtgql::CGqlObject* gqlObjectPtr = gqlObject.GetFieldArgumentObjectPtr(fieldId);
			if (gqlObjectPtr != nullptr){
				CreateResultKeys(*gqlObjectPtr, childKeys);
				resultKeys.InsertChild(fieldId, childKeys);
			}
		}
		else{
			QVariant value = gqlObject.GetFieldArgumentValue(fieldId);
			resultKeys.AppendSimpleKey(value.toByteArray());
		}
	}
}


} // namespace graphqlserver


