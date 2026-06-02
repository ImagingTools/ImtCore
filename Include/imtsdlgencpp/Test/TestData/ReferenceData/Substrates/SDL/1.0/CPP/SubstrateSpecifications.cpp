#include "SubstrateSpecifications.h"
#include "SubstrateSpecifications_fwd.h"


namespace sdl::V1_0::V1_0
{


QByteArray CInputId::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CInputId::operator==(const CInputId& other) const
{
	return 
				id == other.id;
}


bool CInputId::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "InputId", modelIndex);

	if (!id){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "id").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("id", *id, modelIndex);


	return true;
}


bool CInputId::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant idData = model.GetData("id", modelIndex);
	if (idData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "id").toLocal8Bit().constData();)

		return false;
	}
	id = idData.toByteArray();

	return true;
}


bool CInputId::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant idData = model.GetData("id", modelIndex);
	if (!idData.isNull()){
		id = idData.toByteArray();
	}

	return true;
}


bool CInputId::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!id){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "id").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("id", QVariant(*id));

	gqlObject.InsertParam("__typename", QVariant("InputId"));

	return true;
}


bool CInputId::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("id") || (gqlObject["id"].userType() != QMetaType::QString && gqlObject["id"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "id").toLocal8Bit().constData();)

		return false;
	}
	id = gqlObject["id"].toByteArray();

	return true;
}


bool CInputId::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("id") && (gqlObject["id"].userType() == QMetaType::QString || gqlObject["id"].userType() == QMetaType::QByteArray)){
		id = gqlObject["id"].toByteArray();
	}

	return true;
}


bool CInputId::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!id){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "id").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["id"] = QString::fromUtf8(*id);

	jsonObject["__typename"] = "InputId";

	return true;
}


bool CInputId::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("id") || ! jsonObject["id"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "id").toLocal8Bit().constData();)

		return false;
	}
	id = jsonObject["id"].toString().toUtf8();

	return true;
}


bool CInputId::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("id") && jsonObject["id"].isString()){
		id = jsonObject["id"].toString().toUtf8();
	}

	return true;
}


// serialize methods

QByteArray CCollectionViewParamsTest::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CCollectionViewParamsTest::operator==(const CCollectionViewParamsTest& other) const
{
	return 
				count == other.count &&
				offset == other.offset;
}


bool CCollectionViewParamsTest::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "CollectionViewParamsTest", modelIndex);

	if (count){
		model.SetData("count", *count, modelIndex);
	}

	if (offset){
		model.SetData("offset", *offset, modelIndex);
	}


	return true;
}


bool CCollectionViewParamsTest::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant countData = model.GetData("count", modelIndex);
	if (!countData.isNull()){
		count = countData.toInt();
	}

	QVariant offsetData = model.GetData("offset", modelIndex);
	if (!offsetData.isNull()){
		offset = offsetData.toInt();
	}

	return true;
}


bool CCollectionViewParamsTest::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant countData = model.GetData("count", modelIndex);
	if (!countData.isNull()){
		count = countData.toInt();
	}

	QVariant offsetData = model.GetData("offset", modelIndex);
	if (!offsetData.isNull()){
		offset = offsetData.toInt();
	}

	return true;
}


bool CCollectionViewParamsTest::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (count){
		gqlObject.InsertParam("count", QVariant(*count));
	}

	if (offset){
		gqlObject.InsertParam("offset", QVariant(*offset));
	}

	gqlObject.InsertParam("__typename", QVariant("CollectionViewParamsTest"));

	return true;
}


bool CCollectionViewParamsTest::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("count") && (gqlObject["count"].userType() == QMetaType::Int || gqlObject["count"].userType() == QMetaType::UInt || gqlObject["count"].userType() == QMetaType::LongLong || gqlObject["count"].userType() == QMetaType::ULongLong || gqlObject["count"].userType() == QMetaType::Long || gqlObject["count"].userType() == QMetaType::Short || gqlObject["count"].userType() == QMetaType::ULong || gqlObject["count"].userType() == QMetaType::UShort || gqlObject["count"].userType() == QMetaType::UChar)){
		count = gqlObject["count"].toInt();
	}

	if (gqlObject.ContainsParam("offset") && (gqlObject["offset"].userType() == QMetaType::Int || gqlObject["offset"].userType() == QMetaType::UInt || gqlObject["offset"].userType() == QMetaType::LongLong || gqlObject["offset"].userType() == QMetaType::ULongLong || gqlObject["offset"].userType() == QMetaType::Long || gqlObject["offset"].userType() == QMetaType::Short || gqlObject["offset"].userType() == QMetaType::ULong || gqlObject["offset"].userType() == QMetaType::UShort || gqlObject["offset"].userType() == QMetaType::UChar)){
		offset = gqlObject["offset"].toInt();
	}

	return true;
}


bool CCollectionViewParamsTest::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("count") && (gqlObject["count"].userType() == QMetaType::Int || gqlObject["count"].userType() == QMetaType::UInt || gqlObject["count"].userType() == QMetaType::LongLong || gqlObject["count"].userType() == QMetaType::ULongLong || gqlObject["count"].userType() == QMetaType::Long || gqlObject["count"].userType() == QMetaType::Short || gqlObject["count"].userType() == QMetaType::ULong || gqlObject["count"].userType() == QMetaType::UShort || gqlObject["count"].userType() == QMetaType::UChar)){
		count = gqlObject["count"].toInt();
	}

	if (gqlObject.ContainsParam("offset") && (gqlObject["offset"].userType() == QMetaType::Int || gqlObject["offset"].userType() == QMetaType::UInt || gqlObject["offset"].userType() == QMetaType::LongLong || gqlObject["offset"].userType() == QMetaType::ULongLong || gqlObject["offset"].userType() == QMetaType::Long || gqlObject["offset"].userType() == QMetaType::Short || gqlObject["offset"].userType() == QMetaType::ULong || gqlObject["offset"].userType() == QMetaType::UShort || gqlObject["offset"].userType() == QMetaType::UChar)){
		offset = gqlObject["offset"].toInt();
	}

	return true;
}


bool CCollectionViewParamsTest::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (count){
		jsonObject["count"] = QJsonValue::fromVariant(*count);
	}

	if (offset){
		jsonObject["offset"] = QJsonValue::fromVariant(*offset);
	}

	jsonObject["__typename"] = "CollectionViewParamsTest";

	return true;
}


bool CCollectionViewParamsTest::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("count") && jsonObject["count"].isDouble()){
		count = jsonObject["count"].toInt();
	}

	if (jsonObject.contains("offset") && jsonObject["offset"].isDouble()){
		offset = jsonObject["offset"].toInt();
	}

	return true;
}


bool CCollectionViewParamsTest::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("count") && jsonObject["count"].isDouble()){
		count = jsonObject["count"].toInt();
	}

	if (jsonObject.contains("offset") && jsonObject["offset"].isDouble()){
		offset = jsonObject["offset"].toInt();
	}

	return true;
}


// serialize methods

QByteArray CSubstrateSpecificationDocumentTypeIds::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CSubstrateSpecificationDocumentTypeIds::operator==(const CSubstrateSpecificationDocumentTypeIds& other) const
{
	return 
				PaperID == other.PaperID &&
				CardboardID == other.CardboardID &&
				FilmID == other.FilmID;
}


bool CSubstrateSpecificationDocumentTypeIds::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "SubstrateSpecificationDocumentTypeIds", modelIndex);

	if (PaperID){
		model.SetData("PaperID", *PaperID, modelIndex);
	}

	if (CardboardID){
		model.SetData("CardboardID", *CardboardID, modelIndex);
	}

	if (FilmID){
		model.SetData("FilmID", *FilmID, modelIndex);
	}


	return true;
}


bool CSubstrateSpecificationDocumentTypeIds::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant paperIDData = model.GetData("PaperID", modelIndex);
	if (!paperIDData.isNull()){
		PaperID = paperIDData.toByteArray();
	}

	QVariant cardboardIDData = model.GetData("CardboardID", modelIndex);
	if (!cardboardIDData.isNull()){
		CardboardID = cardboardIDData.toByteArray();
	}

	QVariant filmIDData = model.GetData("FilmID", modelIndex);
	if (!filmIDData.isNull()){
		FilmID = filmIDData.toByteArray();
	}

	return true;
}


bool CSubstrateSpecificationDocumentTypeIds::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant paperIDData = model.GetData("PaperID", modelIndex);
	if (!paperIDData.isNull()){
		PaperID = paperIDData.toByteArray();
	}

	QVariant cardboardIDData = model.GetData("CardboardID", modelIndex);
	if (!cardboardIDData.isNull()){
		CardboardID = cardboardIDData.toByteArray();
	}

	QVariant filmIDData = model.GetData("FilmID", modelIndex);
	if (!filmIDData.isNull()){
		FilmID = filmIDData.toByteArray();
	}

	return true;
}


bool CSubstrateSpecificationDocumentTypeIds::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (PaperID){
		gqlObject.InsertParam("PaperID", QVariant(*PaperID));
	}

	if (CardboardID){
		gqlObject.InsertParam("CardboardID", QVariant(*CardboardID));
	}

	if (FilmID){
		gqlObject.InsertParam("FilmID", QVariant(*FilmID));
	}

	gqlObject.InsertParam("__typename", QVariant("SubstrateSpecificationDocumentTypeIds"));

	return true;
}


bool CSubstrateSpecificationDocumentTypeIds::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("PaperID") && (gqlObject["PaperID"].userType() == QMetaType::QString || gqlObject["PaperID"].userType() == QMetaType::QByteArray)){
		PaperID = gqlObject["PaperID"].toByteArray();
	}

	if (gqlObject.ContainsParam("CardboardID") && (gqlObject["CardboardID"].userType() == QMetaType::QString || gqlObject["CardboardID"].userType() == QMetaType::QByteArray)){
		CardboardID = gqlObject["CardboardID"].toByteArray();
	}

	if (gqlObject.ContainsParam("FilmID") && (gqlObject["FilmID"].userType() == QMetaType::QString || gqlObject["FilmID"].userType() == QMetaType::QByteArray)){
		FilmID = gqlObject["FilmID"].toByteArray();
	}

	return true;
}


bool CSubstrateSpecificationDocumentTypeIds::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("PaperID") && (gqlObject["PaperID"].userType() == QMetaType::QString || gqlObject["PaperID"].userType() == QMetaType::QByteArray)){
		PaperID = gqlObject["PaperID"].toByteArray();
	}

	if (gqlObject.ContainsParam("CardboardID") && (gqlObject["CardboardID"].userType() == QMetaType::QString || gqlObject["CardboardID"].userType() == QMetaType::QByteArray)){
		CardboardID = gqlObject["CardboardID"].toByteArray();
	}

	if (gqlObject.ContainsParam("FilmID") && (gqlObject["FilmID"].userType() == QMetaType::QString || gqlObject["FilmID"].userType() == QMetaType::QByteArray)){
		FilmID = gqlObject["FilmID"].toByteArray();
	}

	return true;
}


bool CSubstrateSpecificationDocumentTypeIds::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (PaperID){
		jsonObject["PaperID"] = QString::fromUtf8(*PaperID);
	}

	if (CardboardID){
		jsonObject["CardboardID"] = QString::fromUtf8(*CardboardID);
	}

	if (FilmID){
		jsonObject["FilmID"] = QString::fromUtf8(*FilmID);
	}

	jsonObject["__typename"] = "SubstrateSpecificationDocumentTypeIds";

	return true;
}


bool CSubstrateSpecificationDocumentTypeIds::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("PaperID") && jsonObject["PaperID"].isString()){
		PaperID = jsonObject["PaperID"].toString().toUtf8();
	}

	if (jsonObject.contains("CardboardID") && jsonObject["CardboardID"].isString()){
		CardboardID = jsonObject["CardboardID"].toString().toUtf8();
	}

	if (jsonObject.contains("FilmID") && jsonObject["FilmID"].isString()){
		FilmID = jsonObject["FilmID"].toString().toUtf8();
	}

	return true;
}


bool CSubstrateSpecificationDocumentTypeIds::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("PaperID") && jsonObject["PaperID"].isString()){
		PaperID = jsonObject["PaperID"].toString().toUtf8();
	}

	if (jsonObject.contains("CardboardID") && jsonObject["CardboardID"].isString()){
		CardboardID = jsonObject["CardboardID"].toString().toUtf8();
	}

	if (jsonObject.contains("FilmID") && jsonObject["FilmID"].isString()){
		FilmID = jsonObject["FilmID"].toString().toUtf8();
	}

	return true;
}


// serialize methods

QByteArray CSubstrateSpecificationListInput::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CSubstrateSpecificationListInput::operator==(const CSubstrateSpecificationListInput& other) const
{
	return 
				viewParams == other.viewParams;
}


bool CSubstrateSpecificationListInput::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "SubstrateSpecificationListInput", modelIndex);


	if (viewParams){
		::imtbase::CTreeItemModel* viewParamsNewModelPtr = model.AddTreeModel("viewParams", modelIndex);
		const bool isViewParamsAdded = viewParams->WriteToModel(*viewParamsNewModelPtr, 0);
		if (!isViewParamsAdded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "viewParams").toLocal8Bit().constData();)

			return false;
		}

	}

	return true;
}


bool CSubstrateSpecificationListInput::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	::imtbase::CTreeItemModel* viewParamsDataModelPtr = model.GetTreeItemModel("viewParams", modelIndex);
	if (viewParamsDataModelPtr != nullptr){
		viewParams = CCollectionViewParamsTest();
		const bool isViewParamsRead = viewParams->ReadFromModel(*viewParamsDataModelPtr, modelIndex);
		if (!isViewParamsRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "viewParams").toLocal8Bit().constData();)

			return false;
		}
	}

	return true;
}


bool CSubstrateSpecificationListInput::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	::imtbase::CTreeItemModel* viewParamsDataModelPtr = model.GetTreeItemModel("viewParams", modelIndex);
	if (viewParamsDataModelPtr != nullptr){
		viewParams = CCollectionViewParamsTest();
		const bool isViewParamsRead = viewParams->ReadFromModel(*viewParamsDataModelPtr, modelIndex);
		if (!isViewParamsRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "viewParams").toLocal8Bit().constData();)

			return false;
		}
	}

	return true;
}


bool CSubstrateSpecificationListInput::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (viewParams){
		::imtgql::CGqlParamObject viewParamsGqlObject;
		const bool isViewParamsAdded = viewParams->WriteToGraphQlObject(viewParamsGqlObject);
		if (!isViewParamsAdded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "viewParams").toLocal8Bit().constData();)

			return false;
		}
		gqlObject.InsertParam("viewParams", viewParamsGqlObject);
	}

	gqlObject.InsertParam("__typename", QVariant("SubstrateSpecificationListInput"));

	return true;
}


bool CSubstrateSpecificationListInput::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("viewParams") && (gqlObject.GetParamArgumentObjectPtr("viewParams") != nullptr)){
		viewParams = CCollectionViewParamsTest();
		const bool isViewParamsRead = viewParams->ReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("viewParams"));
		if (!isViewParamsRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "viewParams").toLocal8Bit().constData();)

			return false;
		}
	}

	return true;
}


bool CSubstrateSpecificationListInput::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("viewParams") && (gqlObject.GetParamArgumentObjectPtr("viewParams") != nullptr)){
		viewParams = CCollectionViewParamsTest();
		const bool isViewParamsRead = viewParams->OptReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("viewParams"));
		if (!isViewParamsRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "viewParams").toLocal8Bit().constData();)

			return false;
		}
	}

	return true;
}


bool CSubstrateSpecificationListInput::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (viewParams){
		QJsonObject viewParamsJsonObject;
		const bool isViewParamsAdded = viewParams->WriteToJsonObject(viewParamsJsonObject);
		if (!isViewParamsAdded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "viewParams").toLocal8Bit().constData();)

			return false;
		}
		jsonObject["viewParams"] = viewParamsJsonObject;
	}

	jsonObject["__typename"] = "SubstrateSpecificationListInput";

	return true;
}


bool CSubstrateSpecificationListInput::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("viewParams") && jsonObject["viewParams"].isObject()){
		viewParams = CCollectionViewParamsTest();
		const bool isViewParamsRead = viewParams->ReadFromJsonObject(jsonObject["viewParams"].toObject());
		if (!isViewParamsRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "viewParams").toLocal8Bit().constData();)

			return false;
		}
	}

	return true;
}


bool CSubstrateSpecificationListInput::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("viewParams") && jsonObject["viewParams"].isObject()){
		viewParams = CCollectionViewParamsTest();
		const bool isViewParamsRead = viewParams->OptReadFromJsonObject(jsonObject["viewParams"].toObject());
		if (!isViewParamsRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "viewParams").toLocal8Bit().constData();)

			return false;
		}
	}

	return true;
}


// serialize methods

QByteArray CSubstrateSpecificationListItem::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CSubstrateSpecificationListItem::operator==(const CSubstrateSpecificationListItem& other) const
{
	return 
				id == other.id &&
				typeId == other.typeId &&
				name == other.name &&
				materialId == other.materialId &&
				materialName == other.materialName &&
				added == other.added &&
				timeStamp == other.timeStamp &&
				colorPreview == other.colorPreview;
}


bool CSubstrateSpecificationListItem::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "SubstrateSpecificationListItem", modelIndex);

	if (!id){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "id").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("id", *id, modelIndex);

	if (!typeId){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "typeId").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("typeId", *typeId, modelIndex);

	if (!name){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "name").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("name", *name, modelIndex);

	if (materialId){
		model.SetData("materialId", *materialId, modelIndex);
	}

	if (materialName){
		model.SetData("materialName", *materialName, modelIndex);
	}

	if (added){
		model.SetData("added", *added, modelIndex);
	}

	if (timeStamp){
		model.SetData("timeStamp", *timeStamp, modelIndex);
	}

	if (colorPreview){
		model.SetData("colorPreview", *colorPreview, modelIndex);
	}


	return true;
}


bool CSubstrateSpecificationListItem::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant idData = model.GetData("id", modelIndex);
	if (idData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "id").toLocal8Bit().constData();)

		return false;
	}
	id = idData.toByteArray();

	QVariant typeIdData = model.GetData("typeId", modelIndex);
	if (typeIdData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "typeId").toLocal8Bit().constData();)

		return false;
	}
	typeId = typeIdData.toByteArray();

	QVariant nameData = model.GetData("name", modelIndex);
	if (nameData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "name").toLocal8Bit().constData();)

		return false;
	}
	name = nameData.toString();

	QVariant materialIdData = model.GetData("materialId", modelIndex);
	if (!materialIdData.isNull()){
		materialId = materialIdData.toByteArray();
	}

	QVariant materialNameData = model.GetData("materialName", modelIndex);
	if (!materialNameData.isNull()){
		materialName = materialNameData.toString();
	}

	QVariant addedData = model.GetData("added", modelIndex);
	if (!addedData.isNull()){
		added = addedData.toString();
	}

	QVariant timeStampData = model.GetData("timeStamp", modelIndex);
	if (!timeStampData.isNull()){
		timeStamp = timeStampData.toString();
	}

	QVariant colorPreviewData = model.GetData("colorPreview", modelIndex);
	if (!colorPreviewData.isNull()){
		colorPreview = colorPreviewData.toString();
	}

	return true;
}


bool CSubstrateSpecificationListItem::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant idData = model.GetData("id", modelIndex);
	if (!idData.isNull()){
		id = idData.toByteArray();
	}

	QVariant typeIdData = model.GetData("typeId", modelIndex);
	if (!typeIdData.isNull()){
		typeId = typeIdData.toByteArray();
	}

	QVariant nameData = model.GetData("name", modelIndex);
	if (!nameData.isNull()){
		name = nameData.toString();
	}

	QVariant materialIdData = model.GetData("materialId", modelIndex);
	if (!materialIdData.isNull()){
		materialId = materialIdData.toByteArray();
	}

	QVariant materialNameData = model.GetData("materialName", modelIndex);
	if (!materialNameData.isNull()){
		materialName = materialNameData.toString();
	}

	QVariant addedData = model.GetData("added", modelIndex);
	if (!addedData.isNull()){
		added = addedData.toString();
	}

	QVariant timeStampData = model.GetData("timeStamp", modelIndex);
	if (!timeStampData.isNull()){
		timeStamp = timeStampData.toString();
	}

	QVariant colorPreviewData = model.GetData("colorPreview", modelIndex);
	if (!colorPreviewData.isNull()){
		colorPreview = colorPreviewData.toString();
	}

	return true;
}


bool CSubstrateSpecificationListItem::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!id){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "id").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("id", QVariant(*id));

	if (!typeId){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "typeId").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("typeId", QVariant(*typeId));

	if (!name){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "name").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("name", QVariant(*name));

	if (materialId){
		gqlObject.InsertParam("materialId", QVariant(*materialId));
	}

	if (materialName){
		gqlObject.InsertParam("materialName", QVariant(*materialName));
	}

	if (added){
		gqlObject.InsertParam("added", QVariant(*added));
	}

	if (timeStamp){
		gqlObject.InsertParam("timeStamp", QVariant(*timeStamp));
	}

	if (colorPreview){
		gqlObject.InsertParam("colorPreview", QVariant(*colorPreview));
	}

	gqlObject.InsertParam("__typename", QVariant("SubstrateSpecificationListItem"));

	return true;
}


bool CSubstrateSpecificationListItem::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("id") || (gqlObject["id"].userType() != QMetaType::QString && gqlObject["id"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "id").toLocal8Bit().constData();)

		return false;
	}
	id = gqlObject["id"].toByteArray();

	if (!gqlObject.ContainsParam("typeId") || (gqlObject["typeId"].userType() != QMetaType::QString && gqlObject["typeId"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "typeId").toLocal8Bit().constData();)

		return false;
	}
	typeId = gqlObject["typeId"].toByteArray();

	if (!gqlObject.ContainsParam("name") || (gqlObject["name"].userType() != QMetaType::QString && gqlObject["name"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "name").toLocal8Bit().constData();)

		return false;
	}
	name = gqlObject["name"].toString();

	if (gqlObject.ContainsParam("materialId") && (gqlObject["materialId"].userType() == QMetaType::QString || gqlObject["materialId"].userType() == QMetaType::QByteArray)){
		materialId = gqlObject["materialId"].toByteArray();
	}

	if (gqlObject.ContainsParam("materialName") && (gqlObject["materialName"].userType() == QMetaType::QString || gqlObject["materialName"].userType() == QMetaType::QByteArray)){
		materialName = gqlObject["materialName"].toString();
	}

	if (gqlObject.ContainsParam("added") && (gqlObject["added"].userType() == QMetaType::QString || gqlObject["added"].userType() == QMetaType::QByteArray)){
		added = gqlObject["added"].toString();
	}

	if (gqlObject.ContainsParam("timeStamp") && (gqlObject["timeStamp"].userType() == QMetaType::QString || gqlObject["timeStamp"].userType() == QMetaType::QByteArray)){
		timeStamp = gqlObject["timeStamp"].toString();
	}

	if (gqlObject.ContainsParam("colorPreview") && (gqlObject["colorPreview"].userType() == QMetaType::QString || gqlObject["colorPreview"].userType() == QMetaType::QByteArray)){
		colorPreview = gqlObject["colorPreview"].toString();
	}

	return true;
}


bool CSubstrateSpecificationListItem::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("id") && (gqlObject["id"].userType() == QMetaType::QString || gqlObject["id"].userType() == QMetaType::QByteArray)){
		id = gqlObject["id"].toByteArray();
	}

	if (gqlObject.ContainsParam("typeId") && (gqlObject["typeId"].userType() == QMetaType::QString || gqlObject["typeId"].userType() == QMetaType::QByteArray)){
		typeId = gqlObject["typeId"].toByteArray();
	}

	if (gqlObject.ContainsParam("name") && (gqlObject["name"].userType() == QMetaType::QString || gqlObject["name"].userType() == QMetaType::QByteArray)){
		name = gqlObject["name"].toString();
	}

	if (gqlObject.ContainsParam("materialId") && (gqlObject["materialId"].userType() == QMetaType::QString || gqlObject["materialId"].userType() == QMetaType::QByteArray)){
		materialId = gqlObject["materialId"].toByteArray();
	}

	if (gqlObject.ContainsParam("materialName") && (gqlObject["materialName"].userType() == QMetaType::QString || gqlObject["materialName"].userType() == QMetaType::QByteArray)){
		materialName = gqlObject["materialName"].toString();
	}

	if (gqlObject.ContainsParam("added") && (gqlObject["added"].userType() == QMetaType::QString || gqlObject["added"].userType() == QMetaType::QByteArray)){
		added = gqlObject["added"].toString();
	}

	if (gqlObject.ContainsParam("timeStamp") && (gqlObject["timeStamp"].userType() == QMetaType::QString || gqlObject["timeStamp"].userType() == QMetaType::QByteArray)){
		timeStamp = gqlObject["timeStamp"].toString();
	}

	if (gqlObject.ContainsParam("colorPreview") && (gqlObject["colorPreview"].userType() == QMetaType::QString || gqlObject["colorPreview"].userType() == QMetaType::QByteArray)){
		colorPreview = gqlObject["colorPreview"].toString();
	}

	return true;
}


bool CSubstrateSpecificationListItem::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!id){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "id").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["id"] = QString::fromUtf8(*id);

	if (!typeId){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "typeId").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["typeId"] = QString::fromUtf8(*typeId);

	if (!name){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "name").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["name"] = QJsonValue::fromVariant(*name);

	if (materialId){
		jsonObject["materialId"] = QString::fromUtf8(*materialId);
	}

	if (materialName){
		jsonObject["materialName"] = QJsonValue::fromVariant(*materialName);
	}

	if (added){
		jsonObject["added"] = QJsonValue::fromVariant(*added);
	}

	if (timeStamp){
		jsonObject["timeStamp"] = QJsonValue::fromVariant(*timeStamp);
	}

	if (colorPreview){
		jsonObject["colorPreview"] = QJsonValue::fromVariant(*colorPreview);
	}

	jsonObject["__typename"] = "SubstrateSpecificationListItem";

	return true;
}


bool CSubstrateSpecificationListItem::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("id") || ! jsonObject["id"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "id").toLocal8Bit().constData();)

		return false;
	}
	id = jsonObject["id"].toString().toUtf8();

	if (!jsonObject.contains("typeId") || ! jsonObject["typeId"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "typeId").toLocal8Bit().constData();)

		return false;
	}
	typeId = jsonObject["typeId"].toString().toUtf8();

	if (!jsonObject.contains("name") || ! jsonObject["name"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "name").toLocal8Bit().constData();)

		return false;
	}
	name = jsonObject["name"].toString();

	if (jsonObject.contains("materialId") && jsonObject["materialId"].isString()){
		materialId = jsonObject["materialId"].toString().toUtf8();
	}

	if (jsonObject.contains("materialName") && jsonObject["materialName"].isString()){
		materialName = jsonObject["materialName"].toString();
	}

	if (jsonObject.contains("added") && jsonObject["added"].isString()){
		added = jsonObject["added"].toString();
	}

	if (jsonObject.contains("timeStamp") && jsonObject["timeStamp"].isString()){
		timeStamp = jsonObject["timeStamp"].toString();
	}

	if (jsonObject.contains("colorPreview") && jsonObject["colorPreview"].isString()){
		colorPreview = jsonObject["colorPreview"].toString();
	}

	return true;
}


bool CSubstrateSpecificationListItem::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("id") && jsonObject["id"].isString()){
		id = jsonObject["id"].toString().toUtf8();
	}

	if (jsonObject.contains("typeId") && jsonObject["typeId"].isString()){
		typeId = jsonObject["typeId"].toString().toUtf8();
	}

	if (jsonObject.contains("name") && jsonObject["name"].isString()){
		name = jsonObject["name"].toString();
	}

	if (jsonObject.contains("materialId") && jsonObject["materialId"].isString()){
		materialId = jsonObject["materialId"].toString().toUtf8();
	}

	if (jsonObject.contains("materialName") && jsonObject["materialName"].isString()){
		materialName = jsonObject["materialName"].toString();
	}

	if (jsonObject.contains("added") && jsonObject["added"].isString()){
		added = jsonObject["added"].toString();
	}

	if (jsonObject.contains("timeStamp") && jsonObject["timeStamp"].isString()){
		timeStamp = jsonObject["timeStamp"].toString();
	}

	if (jsonObject.contains("colorPreview") && jsonObject["colorPreview"].isString()){
		colorPreview = jsonObject["colorPreview"].toString();
	}

	return true;
}


// serialize methods

QByteArray CSubstrateSpecificationListResponse::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CSubstrateSpecificationListResponse::operator==(const CSubstrateSpecificationListResponse& other) const
{
	return 
				items == other.items;
}


bool CSubstrateSpecificationListResponse::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "SubstrateSpecificationListResponse", modelIndex);

	if (items){
		::imtbase::CTreeItemModel* newItemsModelPtr = model.AddTreeModel("items", modelIndex);
		newItemsModelPtr->setIsArray(true);
		for (qsizetype itemsIndex = 0; itemsIndex < items->size(); ++itemsIndex){
			newItemsModelPtr->InsertNewItem();
			if (!(items->at(itemsIndex)->WriteToModel(*newItemsModelPtr, itemsIndex))){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "items").toLocal8Bit().constData();)

				return false;
			}
		}
	}

	return true;
}


bool CSubstrateSpecificationListResponse::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	::imtbase::CTreeItemModel* itemsModel = model.GetTreeItemModel("items", modelIndex);
	if (itemsModel != nullptr){
		int itemsCount = itemsModel->GetItemsCount();
		imtsdl::TElementList<CSubstrateSpecificationListItem> itemsList;
		for (int itemsIndex = 0; itemsIndex < itemsCount; ++itemsIndex){
			CSubstrateSpecificationListItem t_items;
			if (!t_items.ReadFromModel(*itemsModel, itemsIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "items").toLocal8Bit().constData();)

				return false;
			}
			itemsList << t_items;
		}
		items = itemsList;

	}

	return true;
}


bool CSubstrateSpecificationListResponse::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	::imtbase::CTreeItemModel* itemsModel = model.GetTreeItemModel("items", modelIndex);
	if (itemsModel != nullptr){
		int itemsCount = itemsModel->GetItemsCount();
		imtsdl::TElementList<CSubstrateSpecificationListItem> itemsList;
		for (int itemsIndex = 0; itemsIndex < itemsCount; ++itemsIndex){
			CSubstrateSpecificationListItem t_items;
			if (!t_items.OptReadFromModel(*itemsModel, itemsIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "items").toLocal8Bit().constData();)

				return false;
			}
			itemsList << t_items;
		}
		items = itemsList;

	}

	return true;
}


bool CSubstrateSpecificationListResponse::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (items){
		QList<::imtgql::CGqlParamObject> itemsDataObjectList;
		for (qsizetype itemsIndex = 0; itemsIndex < items->size(); ++itemsIndex){
			::imtgql::CGqlParamObject newItemsGqlObject;
			if (!items->at(itemsIndex)->WriteToGraphQlObject(newItemsGqlObject)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "items").toLocal8Bit().constData();)

				return false;
			}
			itemsDataObjectList << newItemsGqlObject;
		}
		gqlObject.InsertParam("items", itemsDataObjectList);
	}

	gqlObject.InsertParam("__typename", QVariant("SubstrateSpecificationListResponse"));

	return true;
}


bool CSubstrateSpecificationListResponse::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("items")){
		items.emplace();
	}
	if (gqlObject.ContainsParam("items") && (gqlObject.GetObjectsCount("items") > 0)){
		const qsizetype itemsElementsCount = gqlObject.GetObjectsCount("items");
		items = imtsdl::TElementList<CSubstrateSpecificationListItem>();
		for (qsizetype itemsIndex = 0; itemsIndex < itemsElementsCount; ++itemsIndex){
			const ::imtgql::CGqlParamObject* itemsDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("items", itemsIndex);
			if (itemsDataObjectPtr == nullptr){
				qDebug() << "invalid type" << itemsDataObjectPtr;
				return false;
			}
			CSubstrateSpecificationListItem tempItems;
			if (!tempItems.ReadFromGraphQlObject(*itemsDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "items").toLocal8Bit().constData();)

				return false;
			}
			items->append(tempItems);
		}
	}

	return true;
}


bool CSubstrateSpecificationListResponse::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("items")){
		items.emplace();
	}
	if (gqlObject.ContainsParam("items") && (gqlObject.GetObjectsCount("items") > 0)){
		const qsizetype itemsElementsCount = gqlObject.GetObjectsCount("items");
		items = imtsdl::TElementList<CSubstrateSpecificationListItem>();
		for (qsizetype itemsIndex = 0; itemsIndex < itemsElementsCount; ++itemsIndex){
			const ::imtgql::CGqlParamObject* itemsDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("items", itemsIndex);
			if (itemsDataObjectPtr == nullptr){
				qDebug() << "invalid type" << itemsDataObjectPtr;
				return false;
			}
			CSubstrateSpecificationListItem tempItems;
			if (!tempItems.OptReadFromGraphQlObject(*itemsDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "items").toLocal8Bit().constData();)

				return false;
			}
			items->append(tempItems);
		}
	}

	return true;
}


bool CSubstrateSpecificationListResponse::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (items){
		QJsonArray newItemsArray;
		for (qsizetype itemsIndex = 0; itemsIndex < items->size(); ++itemsIndex){
			QJsonObject newItemsJsonObject;
			if (!items->at(itemsIndex)->WriteToJsonObject(newItemsJsonObject)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "items").toLocal8Bit().constData();)

				return false;
			}
			newItemsArray << newItemsJsonObject;
		}
		jsonObject["items"] = newItemsArray;
	}

	jsonObject["__typename"] = "SubstrateSpecificationListResponse";

	return true;
}


bool CSubstrateSpecificationListResponse::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("items") && jsonObject["items"].isArray()){
		const QJsonArray itemsJsonArray = jsonObject["items"].toArray();
		const qsizetype itemsArrayCount = itemsJsonArray.size();
		items = imtsdl::TElementList<CSubstrateSpecificationListItem>();
		for (qsizetype itemsIndex = 0; itemsIndex < itemsArrayCount; ++itemsIndex){
			CSubstrateSpecificationListItem tempItems;
			if (!tempItems.ReadFromJsonObject(itemsJsonArray[itemsIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "items").toLocal8Bit().constData();)

				return false;
			}
			items->append(tempItems);
		}
	}

	return true;
}


bool CSubstrateSpecificationListResponse::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("items") && jsonObject["items"].isArray()){
		const QJsonArray itemsJsonArray = jsonObject["items"].toArray();
		const qsizetype itemsArrayCount = itemsJsonArray.size();
		items = imtsdl::TElementList<CSubstrateSpecificationListItem>();
		for (qsizetype itemsIndex = 0; itemsIndex < itemsArrayCount; ++itemsIndex){
			CSubstrateSpecificationListItem tempItems;
			if (!tempItems.OptReadFromJsonObject(itemsJsonArray[itemsIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "items").toLocal8Bit().constData();)

				return false;
			}
			items->append(tempItems);
		}
	}

	return true;
}


// serialize methods

QByteArray CCardboardSubstrateSpecification::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CCardboardSubstrateSpecification::operator==(const CCardboardSubstrateSpecification& other) const
{
	return 
				color == other.color &&
				grammage.has_value() == other.grammage.has_value() &&
				((grammage.has_value() && other.grammage.has_value()) ?
					qFuzzyCompare(*grammage, *other.grammage) : true);
}


bool CCardboardSubstrateSpecification::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "CardboardSubstrateSpecification", modelIndex);

	if (color){
		model.SetData("color", *color, modelIndex);
	}

	if (grammage){
		model.SetData("grammage", *grammage, modelIndex);
	}


	return true;
}


bool CCardboardSubstrateSpecification::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant colorData = model.GetData("color", modelIndex);
	if (!colorData.isNull()){
		color = colorData.toString();
	}

	QVariant grammageData = model.GetData("grammage", modelIndex);
	if (!grammageData.isNull()){
		grammage = grammageData.toDouble();
	}

	return true;
}


bool CCardboardSubstrateSpecification::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant colorData = model.GetData("color", modelIndex);
	if (!colorData.isNull()){
		color = colorData.toString();
	}

	QVariant grammageData = model.GetData("grammage", modelIndex);
	if (!grammageData.isNull()){
		grammage = grammageData.toDouble();
	}

	return true;
}


bool CCardboardSubstrateSpecification::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (color){
		gqlObject.InsertParam("color", QVariant(*color));
	}

	if (grammage){
		gqlObject.InsertParam("grammage", QVariant(*grammage));
	}

	gqlObject.InsertParam("__typename", QVariant("CardboardSubstrateSpecification"));

	return true;
}


bool CCardboardSubstrateSpecification::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("color") && (gqlObject["color"].userType() == QMetaType::QString || gqlObject["color"].userType() == QMetaType::QByteArray)){
		color = gqlObject["color"].toString();
	}

	if (gqlObject.ContainsParam("grammage") && (gqlObject["grammage"].userType() == QMetaType::Float || gqlObject["grammage"].userType() == QMetaType::Double || gqlObject["grammage"].userType() == QMetaType::Int || gqlObject["grammage"].userType() == QMetaType::UInt || gqlObject["grammage"].userType() == QMetaType::LongLong || gqlObject["grammage"].userType() == QMetaType::ULongLong || gqlObject["grammage"].userType() == QMetaType::Long || gqlObject["grammage"].userType() == QMetaType::Short || gqlObject["grammage"].userType() == QMetaType::ULong || gqlObject["grammage"].userType() == QMetaType::UShort || gqlObject["grammage"].userType() == QMetaType::UChar)){
		grammage = gqlObject["grammage"].toDouble();
	}

	return true;
}


bool CCardboardSubstrateSpecification::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("color") && (gqlObject["color"].userType() == QMetaType::QString || gqlObject["color"].userType() == QMetaType::QByteArray)){
		color = gqlObject["color"].toString();
	}

	if (gqlObject.ContainsParam("grammage") && (gqlObject["grammage"].userType() == QMetaType::Float || gqlObject["grammage"].userType() == QMetaType::Double || gqlObject["grammage"].userType() == QMetaType::Int || gqlObject["grammage"].userType() == QMetaType::UInt || gqlObject["grammage"].userType() == QMetaType::LongLong || gqlObject["grammage"].userType() == QMetaType::ULongLong || gqlObject["grammage"].userType() == QMetaType::Long || gqlObject["grammage"].userType() == QMetaType::Short || gqlObject["grammage"].userType() == QMetaType::ULong || gqlObject["grammage"].userType() == QMetaType::UShort || gqlObject["grammage"].userType() == QMetaType::UChar)){
		grammage = gqlObject["grammage"].toDouble();
	}

	return true;
}


bool CCardboardSubstrateSpecification::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (color){
		jsonObject["color"] = QJsonValue::fromVariant(*color);
	}

	if (grammage){
		jsonObject["grammage"] = QJsonValue::fromVariant(*grammage);
	}

	jsonObject["__typename"] = "CardboardSubstrateSpecification";

	return true;
}


bool CCardboardSubstrateSpecification::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("color") && jsonObject["color"].isString()){
		color = jsonObject["color"].toString();
	}

	if (jsonObject.contains("grammage") && jsonObject["grammage"].isDouble()){
		grammage = jsonObject["grammage"].toDouble();
	}

	return true;
}


bool CCardboardSubstrateSpecification::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("color") && jsonObject["color"].isString()){
		color = jsonObject["color"].toString();
	}

	if (jsonObject.contains("grammage") && jsonObject["grammage"].isDouble()){
		grammage = jsonObject["grammage"].toDouble();
	}

	return true;
}


// serialize methods

QByteArray CPaperSubstrateSpecification::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CPaperSubstrateSpecification::operator==(const CPaperSubstrateSpecification& other) const
{
	return 
				grammage.has_value() == other.grammage.has_value() &&
				((grammage.has_value() && other.grammage.has_value()) ?
					qFuzzyCompare(*grammage, *other.grammage) : true) &&
				color == other.color &&
				coatType == other.coatType;
}


bool CPaperSubstrateSpecification::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "PaperSubstrateSpecification", modelIndex);

	if (grammage){
		model.SetData("grammage", *grammage, modelIndex);
	}

	if (color){
		model.SetData("color", *color, modelIndex);
	}

	if (coatType){
		model.SetData("coatType", *coatType, modelIndex);
	}


	return true;
}


bool CPaperSubstrateSpecification::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant grammageData = model.GetData("grammage", modelIndex);
	if (!grammageData.isNull()){
		grammage = grammageData.toDouble();
	}

	QVariant colorData = model.GetData("color", modelIndex);
	if (!colorData.isNull()){
		color = colorData.toString();
	}

	QVariant coatTypeData = model.GetData("coatType", modelIndex);
	if (!coatTypeData.isNull()){
		coatType = coatTypeData.toString();
	}

	return true;
}


bool CPaperSubstrateSpecification::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant grammageData = model.GetData("grammage", modelIndex);
	if (!grammageData.isNull()){
		grammage = grammageData.toDouble();
	}

	QVariant colorData = model.GetData("color", modelIndex);
	if (!colorData.isNull()){
		color = colorData.toString();
	}

	QVariant coatTypeData = model.GetData("coatType", modelIndex);
	if (!coatTypeData.isNull()){
		coatType = coatTypeData.toString();
	}

	return true;
}


bool CPaperSubstrateSpecification::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (grammage){
		gqlObject.InsertParam("grammage", QVariant(*grammage));
	}

	if (color){
		gqlObject.InsertParam("color", QVariant(*color));
	}

	if (coatType){
		gqlObject.InsertParam("coatType", QVariant(*coatType));
	}

	gqlObject.InsertParam("__typename", QVariant("PaperSubstrateSpecification"));

	return true;
}


bool CPaperSubstrateSpecification::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("grammage") && (gqlObject["grammage"].userType() == QMetaType::Float || gqlObject["grammage"].userType() == QMetaType::Double || gqlObject["grammage"].userType() == QMetaType::Int || gqlObject["grammage"].userType() == QMetaType::UInt || gqlObject["grammage"].userType() == QMetaType::LongLong || gqlObject["grammage"].userType() == QMetaType::ULongLong || gqlObject["grammage"].userType() == QMetaType::Long || gqlObject["grammage"].userType() == QMetaType::Short || gqlObject["grammage"].userType() == QMetaType::ULong || gqlObject["grammage"].userType() == QMetaType::UShort || gqlObject["grammage"].userType() == QMetaType::UChar)){
		grammage = gqlObject["grammage"].toDouble();
	}

	if (gqlObject.ContainsParam("color") && (gqlObject["color"].userType() == QMetaType::QString || gqlObject["color"].userType() == QMetaType::QByteArray)){
		color = gqlObject["color"].toString();
	}

	if (gqlObject.ContainsParam("coatType") && (gqlObject["coatType"].userType() == QMetaType::QString || gqlObject["coatType"].userType() == QMetaType::QByteArray)){
		coatType = gqlObject["coatType"].toString();
	}

	return true;
}


bool CPaperSubstrateSpecification::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("grammage") && (gqlObject["grammage"].userType() == QMetaType::Float || gqlObject["grammage"].userType() == QMetaType::Double || gqlObject["grammage"].userType() == QMetaType::Int || gqlObject["grammage"].userType() == QMetaType::UInt || gqlObject["grammage"].userType() == QMetaType::LongLong || gqlObject["grammage"].userType() == QMetaType::ULongLong || gqlObject["grammage"].userType() == QMetaType::Long || gqlObject["grammage"].userType() == QMetaType::Short || gqlObject["grammage"].userType() == QMetaType::ULong || gqlObject["grammage"].userType() == QMetaType::UShort || gqlObject["grammage"].userType() == QMetaType::UChar)){
		grammage = gqlObject["grammage"].toDouble();
	}

	if (gqlObject.ContainsParam("color") && (gqlObject["color"].userType() == QMetaType::QString || gqlObject["color"].userType() == QMetaType::QByteArray)){
		color = gqlObject["color"].toString();
	}

	if (gqlObject.ContainsParam("coatType") && (gqlObject["coatType"].userType() == QMetaType::QString || gqlObject["coatType"].userType() == QMetaType::QByteArray)){
		coatType = gqlObject["coatType"].toString();
	}

	return true;
}


bool CPaperSubstrateSpecification::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (grammage){
		jsonObject["grammage"] = QJsonValue::fromVariant(*grammage);
	}

	if (color){
		jsonObject["color"] = QJsonValue::fromVariant(*color);
	}

	if (coatType){
		jsonObject["coatType"] = QJsonValue::fromVariant(*coatType);
	}

	jsonObject["__typename"] = "PaperSubstrateSpecification";

	return true;
}


bool CPaperSubstrateSpecification::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("grammage") && jsonObject["grammage"].isDouble()){
		grammage = jsonObject["grammage"].toDouble();
	}

	if (jsonObject.contains("color") && jsonObject["color"].isString()){
		color = jsonObject["color"].toString();
	}

	if (jsonObject.contains("coatType") && jsonObject["coatType"].isString()){
		coatType = jsonObject["coatType"].toString();
	}

	return true;
}


bool CPaperSubstrateSpecification::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("grammage") && jsonObject["grammage"].isDouble()){
		grammage = jsonObject["grammage"].toDouble();
	}

	if (jsonObject.contains("color") && jsonObject["color"].isString()){
		color = jsonObject["color"].toString();
	}

	if (jsonObject.contains("coatType") && jsonObject["coatType"].isString()){
		coatType = jsonObject["coatType"].toString();
	}

	return true;
}


// serialize methods

QByteArray CFilmSubstrateSpecification::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CFilmSubstrateSpecification::operator==(const CFilmSubstrateSpecification& other) const
{
	return 
				transparency.has_value() == other.transparency.has_value() &&
				((transparency.has_value() && other.transparency.has_value()) ?
					qFuzzyCompare(*transparency, *other.transparency) : true) &&
				color == other.color &&
				material == other.material;
}


bool CFilmSubstrateSpecification::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "FilmSubstrateSpecification", modelIndex);

	if (transparency){
		model.SetData("transparency", *transparency, modelIndex);
	}

	if (color){
		model.SetData("color", *color, modelIndex);
	}

	if (material){
		model.SetData("material", *material, modelIndex);
	}


	return true;
}


bool CFilmSubstrateSpecification::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant transparencyData = model.GetData("transparency", modelIndex);
	if (!transparencyData.isNull()){
		transparency = transparencyData.toDouble();
	}

	QVariant colorData = model.GetData("color", modelIndex);
	if (!colorData.isNull()){
		color = colorData.toString();
	}

	QVariant materialData = model.GetData("material", modelIndex);
	if (!materialData.isNull()){
		material = materialData.toString();
	}

	return true;
}


bool CFilmSubstrateSpecification::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant transparencyData = model.GetData("transparency", modelIndex);
	if (!transparencyData.isNull()){
		transparency = transparencyData.toDouble();
	}

	QVariant colorData = model.GetData("color", modelIndex);
	if (!colorData.isNull()){
		color = colorData.toString();
	}

	QVariant materialData = model.GetData("material", modelIndex);
	if (!materialData.isNull()){
		material = materialData.toString();
	}

	return true;
}


bool CFilmSubstrateSpecification::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (transparency){
		gqlObject.InsertParam("transparency", QVariant(*transparency));
	}

	if (color){
		gqlObject.InsertParam("color", QVariant(*color));
	}

	if (material){
		gqlObject.InsertParam("material", QVariant(*material));
	}

	gqlObject.InsertParam("__typename", QVariant("FilmSubstrateSpecification"));

	return true;
}


bool CFilmSubstrateSpecification::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("transparency") && (gqlObject["transparency"].userType() == QMetaType::Float || gqlObject["transparency"].userType() == QMetaType::Double || gqlObject["transparency"].userType() == QMetaType::Int || gqlObject["transparency"].userType() == QMetaType::UInt || gqlObject["transparency"].userType() == QMetaType::LongLong || gqlObject["transparency"].userType() == QMetaType::ULongLong || gqlObject["transparency"].userType() == QMetaType::Long || gqlObject["transparency"].userType() == QMetaType::Short || gqlObject["transparency"].userType() == QMetaType::ULong || gqlObject["transparency"].userType() == QMetaType::UShort || gqlObject["transparency"].userType() == QMetaType::UChar)){
		transparency = gqlObject["transparency"].toDouble();
	}

	if (gqlObject.ContainsParam("color") && (gqlObject["color"].userType() == QMetaType::QString || gqlObject["color"].userType() == QMetaType::QByteArray)){
		color = gqlObject["color"].toString();
	}

	if (gqlObject.ContainsParam("material") && (gqlObject["material"].userType() == QMetaType::QString || gqlObject["material"].userType() == QMetaType::QByteArray)){
		material = gqlObject["material"].toString();
	}

	return true;
}


bool CFilmSubstrateSpecification::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("transparency") && (gqlObject["transparency"].userType() == QMetaType::Float || gqlObject["transparency"].userType() == QMetaType::Double || gqlObject["transparency"].userType() == QMetaType::Int || gqlObject["transparency"].userType() == QMetaType::UInt || gqlObject["transparency"].userType() == QMetaType::LongLong || gqlObject["transparency"].userType() == QMetaType::ULongLong || gqlObject["transparency"].userType() == QMetaType::Long || gqlObject["transparency"].userType() == QMetaType::Short || gqlObject["transparency"].userType() == QMetaType::ULong || gqlObject["transparency"].userType() == QMetaType::UShort || gqlObject["transparency"].userType() == QMetaType::UChar)){
		transparency = gqlObject["transparency"].toDouble();
	}

	if (gqlObject.ContainsParam("color") && (gqlObject["color"].userType() == QMetaType::QString || gqlObject["color"].userType() == QMetaType::QByteArray)){
		color = gqlObject["color"].toString();
	}

	if (gqlObject.ContainsParam("material") && (gqlObject["material"].userType() == QMetaType::QString || gqlObject["material"].userType() == QMetaType::QByteArray)){
		material = gqlObject["material"].toString();
	}

	return true;
}


bool CFilmSubstrateSpecification::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (transparency){
		jsonObject["transparency"] = QJsonValue::fromVariant(*transparency);
	}

	if (color){
		jsonObject["color"] = QJsonValue::fromVariant(*color);
	}

	if (material){
		jsonObject["material"] = QJsonValue::fromVariant(*material);
	}

	jsonObject["__typename"] = "FilmSubstrateSpecification";

	return true;
}


bool CFilmSubstrateSpecification::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("transparency") && jsonObject["transparency"].isDouble()){
		transparency = jsonObject["transparency"].toDouble();
	}

	if (jsonObject.contains("color") && jsonObject["color"].isString()){
		color = jsonObject["color"].toString();
	}

	if (jsonObject.contains("material") && jsonObject["material"].isString()){
		material = jsonObject["material"].toString();
	}

	return true;
}


bool CFilmSubstrateSpecification::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("transparency") && jsonObject["transparency"].isDouble()){
		transparency = jsonObject["transparency"].toDouble();
	}

	if (jsonObject.contains("color") && jsonObject["color"].isString()){
		color = jsonObject["color"].toString();
	}

	if (jsonObject.contains("material") && jsonObject["material"].isString()){
		material = jsonObject["material"].toString();
	}

	return true;
}


// serialize methods

QByteArray CSubstrateSpecificationInput::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CSubstrateSpecificationInput::operator==(const CSubstrateSpecificationInput& other) const
{
	return 
				id == other.id &&
				name == other.name &&
				description == other.description &&
				item == other.item &&
				typeId == other.typeId;
}


bool CSubstrateSpecificationInput::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "SubstrateSpecificationInput", modelIndex);

	if (!id){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "id").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("id", *id, modelIndex);

	if (name){
		model.SetData("name", *name, modelIndex);
	}

	if (description){
		model.SetData("description", *description, modelIndex);
	}

	if (!item){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "item").toLocal8Bit().constData();)

		return false;
	}
	QVariant itemVariantValue;
		if (const CCardboardSubstrateSpecification* cardboardSubstrateSpecificationVal = std::get_if<CCardboardSubstrateSpecification>(item.GetPtr())){
			if (!cardboardSubstrateSpecificationVal->WriteToModel(*(model.AddTreeModel("item", modelIndex)), 0)){
				return false;
			}
			if(model.GetTreeItemModel("item", modelIndex) != nullptr){
				model.GetTreeItemModel("item", modelIndex)->SetData("__typename", "CardboardSubstrateSpecification", 0);
			}
		}
		else if (const CPaperSubstrateSpecification* paperSubstrateSpecificationVal = std::get_if<CPaperSubstrateSpecification>(item.GetPtr())){
			if (!paperSubstrateSpecificationVal->WriteToModel(*(model.AddTreeModel("item", modelIndex)), 0)){
				return false;
			}
			if(model.GetTreeItemModel("item", modelIndex) != nullptr){
				model.GetTreeItemModel("item", modelIndex)->SetData("__typename", "PaperSubstrateSpecification", 0);
			}
		}
		else if (const CFilmSubstrateSpecification* filmSubstrateSpecificationVal = std::get_if<CFilmSubstrateSpecification>(item.GetPtr())){
			if (!filmSubstrateSpecificationVal->WriteToModel(*(model.AddTreeModel("item", modelIndex)), 0)){
				return false;
			}
			if(model.GetTreeItemModel("item", modelIndex) != nullptr){
				model.GetTreeItemModel("item", modelIndex)->SetData("__typename", "FilmSubstrateSpecification", 0);
			}
		}


	if (!typeId){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "typeId").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("typeId", *typeId, modelIndex);


	return true;
}


bool CSubstrateSpecificationInput::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant idData = model.GetData("id", modelIndex);
	if (idData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "id").toLocal8Bit().constData();)

		return false;
	}
	id = idData.toByteArray();

	QVariant nameData = model.GetData("name", modelIndex);
	if (!nameData.isNull()){
		name = nameData.toString();
	}

	QVariant descriptionData = model.GetData("description", modelIndex);
	if (!descriptionData.isNull()){
		description = descriptionData.toString();
	}

	QVariant itemData = model.GetData("item", modelIndex);
	if (itemData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "item").toLocal8Bit().constData();)

		return false;
	}
	QString itemTypename = model.GetTreeItemModel("item", modelIndex)->GetData("__typename").toString();
	if (itemTypename == "CardboardSubstrateSpecification") {
		CCardboardSubstrateSpecification itemConvert;
		const bool isitemRead = itemConvert.ReadFromModel(*model.GetTreeItemModel("item", modelIndex)); 
		if (!isitemRead){
			return false;
		}
		item = SubstrateSpecificationDocument(itemConvert);
	}
	else if (itemTypename == "PaperSubstrateSpecification") {
		CPaperSubstrateSpecification itemConvert;
		const bool isitemRead = itemConvert.ReadFromModel(*model.GetTreeItemModel("item", modelIndex)); 
		if (!isitemRead){
			return false;
		}
		item = SubstrateSpecificationDocument(itemConvert);
	}
	else if (itemTypename == "FilmSubstrateSpecification") {
		CFilmSubstrateSpecification itemConvert;
		const bool isitemRead = itemConvert.ReadFromModel(*model.GetTreeItemModel("item", modelIndex)); 
		if (!isitemRead){
			return false;
		}
		item = SubstrateSpecificationDocument(itemConvert);
	}

	QVariant typeIdData = model.GetData("typeId", modelIndex);
	if (typeIdData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "typeId").toLocal8Bit().constData();)

		return false;
	}
	typeId = typeIdData.toByteArray();

	return true;
}


bool CSubstrateSpecificationInput::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant idData = model.GetData("id", modelIndex);
	if (!idData.isNull()){
		id = idData.toByteArray();
	}

	QVariant nameData = model.GetData("name", modelIndex);
	if (!nameData.isNull()){
		name = nameData.toString();
	}

	QVariant descriptionData = model.GetData("description", modelIndex);
	if (!descriptionData.isNull()){
		description = descriptionData.toString();
	}

	QVariant itemData = model.GetData("item", modelIndex);
	if (!itemData.isNull()){
		QString itemTypename = model.GetTreeItemModel("item", modelIndex)->GetData("__typename").toString();
		if (itemTypename == "CardboardSubstrateSpecification") {
			CCardboardSubstrateSpecification itemConvert;
			const bool isitemRead = itemConvert.ReadFromModel(*model.GetTreeItemModel("item", modelIndex)); 
			if (!isitemRead){
				return false;
			}
			item = SubstrateSpecificationDocument(itemConvert);
		}
		else if (itemTypename == "PaperSubstrateSpecification") {
			CPaperSubstrateSpecification itemConvert;
			const bool isitemRead = itemConvert.ReadFromModel(*model.GetTreeItemModel("item", modelIndex)); 
			if (!isitemRead){
				return false;
			}
			item = SubstrateSpecificationDocument(itemConvert);
		}
		else if (itemTypename == "FilmSubstrateSpecification") {
			CFilmSubstrateSpecification itemConvert;
			const bool isitemRead = itemConvert.ReadFromModel(*model.GetTreeItemModel("item", modelIndex)); 
			if (!isitemRead){
				return false;
			}
			item = SubstrateSpecificationDocument(itemConvert);
		}
	}

	QVariant typeIdData = model.GetData("typeId", modelIndex);
	if (!typeIdData.isNull()){
		typeId = typeIdData.toByteArray();
	}

	return true;
}


bool CSubstrateSpecificationInput::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!id){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "id").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("id", QVariant(*id));

	if (name){
		gqlObject.InsertParam("name", QVariant(*name));
	}

	if (description){
		gqlObject.InsertParam("description", QVariant(*description));
	}

	if (!item){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "item").toLocal8Bit().constData();)

		return false;
	}
	::imtgql::CGqlParamObject itemDataObject;
	if (const CCardboardSubstrateSpecification* cardboardSubstrateSpecificationVal = std::get_if<CCardboardSubstrateSpecification>(item.GetPtr())){
		if (!cardboardSubstrateSpecificationVal->WriteToGraphQlObject(itemDataObject)){
			return false;
		}
		itemDataObject.InsertParam("__typename", QVariant("CardboardSubstrateSpecification"));
	}
	else if (const CPaperSubstrateSpecification* paperSubstrateSpecificationVal = std::get_if<CPaperSubstrateSpecification>(item.GetPtr())){
		if (!paperSubstrateSpecificationVal->WriteToGraphQlObject(itemDataObject)){
			return false;
		}
		itemDataObject.InsertParam("__typename", QVariant("PaperSubstrateSpecification"));
	}
	else if (const CFilmSubstrateSpecification* filmSubstrateSpecificationVal = std::get_if<CFilmSubstrateSpecification>(item.GetPtr())){
		if (!filmSubstrateSpecificationVal->WriteToGraphQlObject(itemDataObject)){
			return false;
		}
		itemDataObject.InsertParam("__typename", QVariant("FilmSubstrateSpecification"));
	}
	gqlObject.InsertParam("item", itemDataObject);

	if (!typeId){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "typeId").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("typeId", QVariant(*typeId));

	gqlObject.InsertParam("__typename", QVariant("SubstrateSpecificationInput"));

	return true;
}


bool CSubstrateSpecificationInput::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("id") || (gqlObject["id"].userType() != QMetaType::QString && gqlObject["id"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "id").toLocal8Bit().constData();)

		return false;
	}
	id = gqlObject["id"].toByteArray();

	if (gqlObject.ContainsParam("name") && (gqlObject["name"].userType() == QMetaType::QString || gqlObject["name"].userType() == QMetaType::QByteArray)){
		name = gqlObject["name"].toString();
	}

	if (gqlObject.ContainsParam("description") && (gqlObject["description"].userType() == QMetaType::QString || gqlObject["description"].userType() == QMetaType::QByteArray)){
		description = gqlObject["description"].toString();
	}

	if (!gqlObject.ContainsParam("item")){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "item").toLocal8Bit().constData();)

		return false;
	}
	const ::imtgql::CGqlParamObject* itemDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("item");
	if (!itemDataObjectPtr->ContainsParam("__typename")){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "item").toLocal8Bit().constData();)

		return false;
	}
	QString itemTypename = itemDataObjectPtr->GetParamArgumentValue("__typename").toString();
	if (itemTypename == "CardboardSubstrateSpecification") {
		CCardboardSubstrateSpecification itemConvert;
		const bool isItemRead = itemConvert.ReadFromGraphQlObject(*itemDataObjectPtr);
		if (!isItemRead){
			return false;
		}
		item = SubstrateSpecificationDocument(itemConvert);
	}
	else if (itemTypename == "PaperSubstrateSpecification") {
		CPaperSubstrateSpecification itemConvert;
		const bool isItemRead = itemConvert.ReadFromGraphQlObject(*itemDataObjectPtr);
		if (!isItemRead){
			return false;
		}
		item = SubstrateSpecificationDocument(itemConvert);
	}
	else if (itemTypename == "FilmSubstrateSpecification") {
		CFilmSubstrateSpecification itemConvert;
		const bool isItemRead = itemConvert.ReadFromGraphQlObject(*itemDataObjectPtr);
		if (!isItemRead){
			return false;
		}
		item = SubstrateSpecificationDocument(itemConvert);
	}

	if (!gqlObject.ContainsParam("typeId") || (gqlObject["typeId"].userType() != QMetaType::QString && gqlObject["typeId"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "typeId").toLocal8Bit().constData();)

		return false;
	}
	typeId = gqlObject["typeId"].toByteArray();

	return true;
}


bool CSubstrateSpecificationInput::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("id") && (gqlObject["id"].userType() == QMetaType::QString || gqlObject["id"].userType() == QMetaType::QByteArray)){
		id = gqlObject["id"].toByteArray();
	}

	if (gqlObject.ContainsParam("name") && (gqlObject["name"].userType() == QMetaType::QString || gqlObject["name"].userType() == QMetaType::QByteArray)){
		name = gqlObject["name"].toString();
	}

	if (gqlObject.ContainsParam("description") && (gqlObject["description"].userType() == QMetaType::QString || gqlObject["description"].userType() == QMetaType::QByteArray)){
		description = gqlObject["description"].toString();
	}

	if (gqlObject.ContainsParam("item")){
		const ::imtgql::CGqlParamObject* itemDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("item");
		if (!itemDataObjectPtr->ContainsParam("__typename")){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "item").toLocal8Bit().constData();)

			return false;
		}
		QString itemTypename = itemDataObjectPtr->GetParamArgumentValue("__typename").toString();
		if (itemTypename == "CardboardSubstrateSpecification") {
			CCardboardSubstrateSpecification itemConvert;
			const bool isItemRead = itemConvert.ReadFromGraphQlObject(*itemDataObjectPtr);
			if (!isItemRead){
				return false;
			}
			item = SubstrateSpecificationDocument(itemConvert);
		}
		else if (itemTypename == "PaperSubstrateSpecification") {
			CPaperSubstrateSpecification itemConvert;
			const bool isItemRead = itemConvert.ReadFromGraphQlObject(*itemDataObjectPtr);
			if (!isItemRead){
				return false;
			}
			item = SubstrateSpecificationDocument(itemConvert);
		}
		else if (itemTypename == "FilmSubstrateSpecification") {
			CFilmSubstrateSpecification itemConvert;
			const bool isItemRead = itemConvert.ReadFromGraphQlObject(*itemDataObjectPtr);
			if (!isItemRead){
				return false;
			}
			item = SubstrateSpecificationDocument(itemConvert);
		}
	}

	if (gqlObject.ContainsParam("typeId") && (gqlObject["typeId"].userType() == QMetaType::QString || gqlObject["typeId"].userType() == QMetaType::QByteArray)){
		typeId = gqlObject["typeId"].toByteArray();
	}

	return true;
}


bool CSubstrateSpecificationInput::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!id){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "id").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["id"] = QString::fromUtf8(*id);

	if (name){
		jsonObject["name"] = QJsonValue::fromVariant(*name);
	}

	if (description){
		jsonObject["description"] = QJsonValue::fromVariant(*description);
	}

	if (!item){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "item").toLocal8Bit().constData();)

		return false;
	}
	if (const CCardboardSubstrateSpecification* cardboardSubstrateSpecificationVal = std::get_if<CCardboardSubstrateSpecification>(item.GetPtr())){
		QJsonObject itemJsonObject;
		const bool isitemAdded = cardboardSubstrateSpecificationVal->WriteToJsonObject(itemJsonObject);
		if (!isitemAdded){
			return false;
		}
		itemJsonObject["__typename"] = "CardboardSubstrateSpecification";
		jsonObject["item"] = itemJsonObject;
	}
	else if (const CPaperSubstrateSpecification* paperSubstrateSpecificationVal = std::get_if<CPaperSubstrateSpecification>(item.GetPtr())){
		QJsonObject itemJsonObject;
		const bool isitemAdded = paperSubstrateSpecificationVal->WriteToJsonObject(itemJsonObject);
		if (!isitemAdded){
			return false;
		}
		itemJsonObject["__typename"] = "PaperSubstrateSpecification";
		jsonObject["item"] = itemJsonObject;
	}
	else if (const CFilmSubstrateSpecification* filmSubstrateSpecificationVal = std::get_if<CFilmSubstrateSpecification>(item.GetPtr())){
		QJsonObject itemJsonObject;
		const bool isitemAdded = filmSubstrateSpecificationVal->WriteToJsonObject(itemJsonObject);
		if (!isitemAdded){
			return false;
		}
		itemJsonObject["__typename"] = "FilmSubstrateSpecification";
		jsonObject["item"] = itemJsonObject;
	}


	if (!typeId){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "typeId").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["typeId"] = QString::fromUtf8(*typeId);

	jsonObject["__typename"] = "SubstrateSpecificationInput";

	return true;
}


bool CSubstrateSpecificationInput::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("id") || ! jsonObject["id"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "id").toLocal8Bit().constData();)

		return false;
	}
	id = jsonObject["id"].toString().toUtf8();

	if (jsonObject.contains("name") && jsonObject["name"].isString()){
		name = jsonObject["name"].toString();
	}

	if (jsonObject.contains("description") && jsonObject["description"].isString()){
		description = jsonObject["description"].toString();
	}

	if (!jsonObject.contains("item")){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "item").toLocal8Bit().constData();)

		return false;
	}
	const QJsonObject itemObject = jsonObject.value("item").toObject();
	if (!jsonObject.value("item").isObject() || !itemObject.contains("__typename")){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "item").toLocal8Bit().constData();)

		return false;
	}
	QString itemTypename = itemObject.value("__typename").toString();
	if (itemTypename == "CardboardSubstrateSpecification") {
		CCardboardSubstrateSpecification itemConvert;
		const bool isitemRead = itemConvert.ReadFromJsonObject(jsonObject["item"].toObject());
		if (!isitemRead){
			return false;
		}
		item = SubstrateSpecificationDocument(itemConvert);
	}
	else if (itemTypename == "PaperSubstrateSpecification") {
		CPaperSubstrateSpecification itemConvert;
		const bool isitemRead = itemConvert.ReadFromJsonObject(jsonObject["item"].toObject());
		if (!isitemRead){
			return false;
		}
		item = SubstrateSpecificationDocument(itemConvert);
	}
	else if (itemTypename == "FilmSubstrateSpecification") {
		CFilmSubstrateSpecification itemConvert;
		const bool isitemRead = itemConvert.ReadFromJsonObject(jsonObject["item"].toObject());
		if (!isitemRead){
			return false;
		}
		item = SubstrateSpecificationDocument(itemConvert);
	}

	if (!jsonObject.contains("typeId") || ! jsonObject["typeId"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "typeId").toLocal8Bit().constData();)

		return false;
	}
	typeId = jsonObject["typeId"].toString().toUtf8();

	return true;
}


bool CSubstrateSpecificationInput::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("id") && jsonObject["id"].isString()){
		id = jsonObject["id"].toString().toUtf8();
	}

	if (jsonObject.contains("name") && jsonObject["name"].isString()){
		name = jsonObject["name"].toString();
	}

	if (jsonObject.contains("description") && jsonObject["description"].isString()){
		description = jsonObject["description"].toString();
	}

	if (jsonObject.contains("item")){
		const QJsonObject itemObject = jsonObject.value("item").toObject();
		if (!jsonObject.value("item").isObject() || !itemObject.contains("__typename")){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "item").toLocal8Bit().constData();)

			return false;
		}
		QString itemTypename = itemObject.value("__typename").toString();
		if (itemTypename == "CardboardSubstrateSpecification") {
			CCardboardSubstrateSpecification itemConvert;
			const bool isitemRead = itemConvert.ReadFromJsonObject(jsonObject["item"].toObject());
			if (!isitemRead){
				return false;
			}
			item = SubstrateSpecificationDocument(itemConvert);
		}
		else if (itemTypename == "PaperSubstrateSpecification") {
			CPaperSubstrateSpecification itemConvert;
			const bool isitemRead = itemConvert.ReadFromJsonObject(jsonObject["item"].toObject());
			if (!isitemRead){
				return false;
			}
			item = SubstrateSpecificationDocument(itemConvert);
		}
		else if (itemTypename == "FilmSubstrateSpecification") {
			CFilmSubstrateSpecification itemConvert;
			const bool isitemRead = itemConvert.ReadFromJsonObject(jsonObject["item"].toObject());
			if (!isitemRead){
				return false;
			}
			item = SubstrateSpecificationDocument(itemConvert);
		}
	}

	if (jsonObject.contains("typeId") && jsonObject["typeId"].isString()){
		typeId = jsonObject["typeId"].toString().toUtf8();
	}

	return true;
}


// serialize methods

QByteArray CSubstrateSpecificationDocumentMetaInfo::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CSubstrateSpecificationDocumentMetaInfo::operator==(const CSubstrateSpecificationDocumentMetaInfo& other) const
{
	return 
				materialId == other.materialId &&
				lab == other.lab;
}


bool CSubstrateSpecificationDocumentMetaInfo::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "SubstrateSpecificationDocumentMetaInfo", modelIndex);

	if (materialId){
		model.SetData("materialId", *materialId, modelIndex);
	}

	if (lab){
		model.SetData("lab", *lab, modelIndex);
	}


	return true;
}


bool CSubstrateSpecificationDocumentMetaInfo::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant materialIdData = model.GetData("materialId", modelIndex);
	if (!materialIdData.isNull()){
		materialId = materialIdData.toByteArray();
	}

	QVariant labData = model.GetData("lab", modelIndex);
	if (!labData.isNull()){
		lab = labData.toString();
	}

	return true;
}


bool CSubstrateSpecificationDocumentMetaInfo::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant materialIdData = model.GetData("materialId", modelIndex);
	if (!materialIdData.isNull()){
		materialId = materialIdData.toByteArray();
	}

	QVariant labData = model.GetData("lab", modelIndex);
	if (!labData.isNull()){
		lab = labData.toString();
	}

	return true;
}


bool CSubstrateSpecificationDocumentMetaInfo::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (materialId){
		gqlObject.InsertParam("materialId", QVariant(*materialId));
	}

	if (lab){
		gqlObject.InsertParam("lab", QVariant(*lab));
	}

	gqlObject.InsertParam("__typename", QVariant("SubstrateSpecificationDocumentMetaInfo"));

	return true;
}


bool CSubstrateSpecificationDocumentMetaInfo::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("materialId") && (gqlObject["materialId"].userType() == QMetaType::QString || gqlObject["materialId"].userType() == QMetaType::QByteArray)){
		materialId = gqlObject["materialId"].toByteArray();
	}

	if (gqlObject.ContainsParam("lab") && (gqlObject["lab"].userType() == QMetaType::QString || gqlObject["lab"].userType() == QMetaType::QByteArray)){
		lab = gqlObject["lab"].toString();
	}

	return true;
}


bool CSubstrateSpecificationDocumentMetaInfo::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("materialId") && (gqlObject["materialId"].userType() == QMetaType::QString || gqlObject["materialId"].userType() == QMetaType::QByteArray)){
		materialId = gqlObject["materialId"].toByteArray();
	}

	if (gqlObject.ContainsParam("lab") && (gqlObject["lab"].userType() == QMetaType::QString || gqlObject["lab"].userType() == QMetaType::QByteArray)){
		lab = gqlObject["lab"].toString();
	}

	return true;
}


bool CSubstrateSpecificationDocumentMetaInfo::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (materialId){
		jsonObject["materialId"] = QString::fromUtf8(*materialId);
	}

	if (lab){
		jsonObject["lab"] = QJsonValue::fromVariant(*lab);
	}

	jsonObject["__typename"] = "SubstrateSpecificationDocumentMetaInfo";

	return true;
}


bool CSubstrateSpecificationDocumentMetaInfo::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("materialId") && jsonObject["materialId"].isString()){
		materialId = jsonObject["materialId"].toString().toUtf8();
	}

	if (jsonObject.contains("lab") && jsonObject["lab"].isString()){
		lab = jsonObject["lab"].toString();
	}

	return true;
}


bool CSubstrateSpecificationDocumentMetaInfo::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("materialId") && jsonObject["materialId"].isString()){
		materialId = jsonObject["materialId"].toString().toUtf8();
	}

	if (jsonObject.contains("lab") && jsonObject["lab"].isString()){
		lab = jsonObject["lab"].toString();
	}

	return true;
}


// serialize methods

CInputIdObject::CInputIdObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CInputIdObject::idChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CInputIdObject::GetId()
{
	return CInputId::id.value();
}


void CInputIdObject::SetId(const QVariant& v)
{

	CInputId::id = v.value<QString>().toUtf8();
	idChanged();
}


bool CInputIdObject::hasId()
{
	 return CInputId::id.HasValue();
}


QString CInputIdObject::toJson() const
{
	QJsonObject jsonObject;
	bool res = WriteToJsonObject(jsonObject);
	if (res){
		QJsonDocument document;
		document.setObject(jsonObject);
		return document.toJson(QJsonDocument::Compact);
	}

	return QString();
}


bool CInputIdObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CInputIdObject::fromObject(const QJsonObject& jsonObject)
{
	beginChanges();
	bool res = ReadFromJsonObject(jsonObject);
	if (res){
		QVariantList changelist;
		modelChanged(changelist);
	}

	endChanges();

	finished();

	return res;
}


QString CInputIdObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CInputIdObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CInputIdObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "id")){
		return "id";
	}

	return propertyName;
}





bool CInputIdObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CInputIdObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CInputIdObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CInputIdObjectList::append(sdl::V1_0::V1_0::CInputIdObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::V1_0::CInputIdObjectList* sdl::V1_0::V1_0::CInputIdObjectList::copyMe()
{
	sdl::V1_0::V1_0::CInputIdObjectList* retVal = new sdl::V1_0::V1_0::CInputIdObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::V1_0::CInputIdObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::V1_0::CInputIdObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::V1_0::CInputIdObjectList::addElement(sdl::V1_0::V1_0::CInputIdObject* item)
{
	append(item);
}


void sdl::V1_0::V1_0::CInputIdObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::V1_0::CInputIdObjectList::isEqualWithModel(sdl::V1_0::V1_0::CInputIdObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::V1_0::CInputIdObjectList::insert(int index, sdl::V1_0::V1_0::CInputIdObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::V1_0::CInputIdObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::V1_0::CInputIdObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::V1_0::CInputIdObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::V1_0::CInputIdObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_id"){
		return QVariant::fromValue(at(index)->id.value());
	}

	return QVariant();
}
CCollectionViewParamsTestObject::CCollectionViewParamsTestObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CCollectionViewParamsTestObject::countChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCollectionViewParamsTestObject::offsetChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CCollectionViewParamsTestObject::GetCount()
{
	return CCollectionViewParamsTest::count.value();
}


void CCollectionViewParamsTestObject::SetCount(const QVariant& v)
{

	CCollectionViewParamsTest::count = v.value<int>();
	countChanged();
}


bool CCollectionViewParamsTestObject::hasCount()
{
	 return CCollectionViewParamsTest::count.HasValue();
}


QVariant CCollectionViewParamsTestObject::GetOffset()
{
	return CCollectionViewParamsTest::offset.value();
}


void CCollectionViewParamsTestObject::SetOffset(const QVariant& v)
{

	CCollectionViewParamsTest::offset = v.value<int>();
	offsetChanged();
}


bool CCollectionViewParamsTestObject::hasOffset()
{
	 return CCollectionViewParamsTest::offset.HasValue();
}


QString CCollectionViewParamsTestObject::toJson() const
{
	QJsonObject jsonObject;
	bool res = WriteToJsonObject(jsonObject);
	if (res){
		QJsonDocument document;
		document.setObject(jsonObject);
		return document.toJson(QJsonDocument::Compact);
	}

	return QString();
}


bool CCollectionViewParamsTestObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CCollectionViewParamsTestObject::fromObject(const QJsonObject& jsonObject)
{
	beginChanges();
	bool res = ReadFromJsonObject(jsonObject);
	if (res){
		QVariantList changelist;
		modelChanged(changelist);
	}

	endChanges();

	finished();

	return res;
}


QString CCollectionViewParamsTestObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CCollectionViewParamsTestObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CCollectionViewParamsTestObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "count")){
		return "count";
	}
	if (propertyName == (QString("m_") + "offset")){
		return "offset";
	}

	return propertyName;
}





bool CCollectionViewParamsTestObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CCollectionViewParamsTestObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CCollectionViewParamsTestObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CCollectionViewParamsTestObjectList::append(sdl::V1_0::V1_0::CCollectionViewParamsTestObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::V1_0::CCollectionViewParamsTestObjectList* sdl::V1_0::V1_0::CCollectionViewParamsTestObjectList::copyMe()
{
	sdl::V1_0::V1_0::CCollectionViewParamsTestObjectList* retVal = new sdl::V1_0::V1_0::CCollectionViewParamsTestObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::V1_0::CCollectionViewParamsTestObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::V1_0::CCollectionViewParamsTestObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::V1_0::CCollectionViewParamsTestObjectList::addElement(sdl::V1_0::V1_0::CCollectionViewParamsTestObject* item)
{
	append(item);
}


void sdl::V1_0::V1_0::CCollectionViewParamsTestObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::V1_0::CCollectionViewParamsTestObjectList::isEqualWithModel(sdl::V1_0::V1_0::CCollectionViewParamsTestObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::V1_0::CCollectionViewParamsTestObjectList::insert(int index, sdl::V1_0::V1_0::CCollectionViewParamsTestObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::V1_0::CCollectionViewParamsTestObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::V1_0::CCollectionViewParamsTestObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::V1_0::CCollectionViewParamsTestObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::V1_0::CCollectionViewParamsTestObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_count"){
		return QVariant::fromValue(at(index)->count.value());
	}
	if (nameId == "m_offset"){
		return QVariant::fromValue(at(index)->offset.value());
	}

	return QVariant();
}
CSubstrateSpecificationDocumentTypeIdsObject::CSubstrateSpecificationDocumentTypeIdsObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CSubstrateSpecificationDocumentTypeIdsObject::paperIDChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CSubstrateSpecificationDocumentTypeIdsObject::cardboardIDChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CSubstrateSpecificationDocumentTypeIdsObject::filmIDChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CSubstrateSpecificationDocumentTypeIdsObject::GetPaperID()
{
	return CSubstrateSpecificationDocumentTypeIds::PaperID.value();
}


void CSubstrateSpecificationDocumentTypeIdsObject::SetPaperID(const QVariant& v)
{

	CSubstrateSpecificationDocumentTypeIds::PaperID = v.value<QString>().toUtf8();
	paperIDChanged();
}


bool CSubstrateSpecificationDocumentTypeIdsObject::hasPaperID()
{
	 return CSubstrateSpecificationDocumentTypeIds::PaperID.HasValue();
}


QVariant CSubstrateSpecificationDocumentTypeIdsObject::GetCardboardID()
{
	return CSubstrateSpecificationDocumentTypeIds::CardboardID.value();
}


void CSubstrateSpecificationDocumentTypeIdsObject::SetCardboardID(const QVariant& v)
{

	CSubstrateSpecificationDocumentTypeIds::CardboardID = v.value<QString>().toUtf8();
	cardboardIDChanged();
}


bool CSubstrateSpecificationDocumentTypeIdsObject::hasCardboardID()
{
	 return CSubstrateSpecificationDocumentTypeIds::CardboardID.HasValue();
}


QVariant CSubstrateSpecificationDocumentTypeIdsObject::GetFilmID()
{
	return CSubstrateSpecificationDocumentTypeIds::FilmID.value();
}


void CSubstrateSpecificationDocumentTypeIdsObject::SetFilmID(const QVariant& v)
{

	CSubstrateSpecificationDocumentTypeIds::FilmID = v.value<QString>().toUtf8();
	filmIDChanged();
}


bool CSubstrateSpecificationDocumentTypeIdsObject::hasFilmID()
{
	 return CSubstrateSpecificationDocumentTypeIds::FilmID.HasValue();
}


QString CSubstrateSpecificationDocumentTypeIdsObject::toJson() const
{
	QJsonObject jsonObject;
	bool res = WriteToJsonObject(jsonObject);
	if (res){
		QJsonDocument document;
		document.setObject(jsonObject);
		return document.toJson(QJsonDocument::Compact);
	}

	return QString();
}


bool CSubstrateSpecificationDocumentTypeIdsObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CSubstrateSpecificationDocumentTypeIdsObject::fromObject(const QJsonObject& jsonObject)
{
	beginChanges();
	bool res = ReadFromJsonObject(jsonObject);
	if (res){
		QVariantList changelist;
		modelChanged(changelist);
	}

	endChanges();

	finished();

	return res;
}


QString CSubstrateSpecificationDocumentTypeIdsObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CSubstrateSpecificationDocumentTypeIdsObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CSubstrateSpecificationDocumentTypeIdsObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "paperID")){
		return "PaperID";
	}
	if (propertyName == (QString("m_") + "cardboardID")){
		return "CardboardID";
	}
	if (propertyName == (QString("m_") + "filmID")){
		return "FilmID";
	}

	return propertyName;
}





bool CSubstrateSpecificationDocumentTypeIdsObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CSubstrateSpecificationDocumentTypeIdsObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CSubstrateSpecificationDocumentTypeIdsObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CSubstrateSpecificationDocumentTypeIdsObjectList::append(sdl::V1_0::V1_0::CSubstrateSpecificationDocumentTypeIdsObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::V1_0::CSubstrateSpecificationDocumentTypeIdsObjectList* sdl::V1_0::V1_0::CSubstrateSpecificationDocumentTypeIdsObjectList::copyMe()
{
	sdl::V1_0::V1_0::CSubstrateSpecificationDocumentTypeIdsObjectList* retVal = new sdl::V1_0::V1_0::CSubstrateSpecificationDocumentTypeIdsObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::V1_0::CSubstrateSpecificationDocumentTypeIdsObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::V1_0::CSubstrateSpecificationDocumentTypeIdsObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::V1_0::CSubstrateSpecificationDocumentTypeIdsObjectList::addElement(sdl::V1_0::V1_0::CSubstrateSpecificationDocumentTypeIdsObject* item)
{
	append(item);
}


void sdl::V1_0::V1_0::CSubstrateSpecificationDocumentTypeIdsObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::V1_0::CSubstrateSpecificationDocumentTypeIdsObjectList::isEqualWithModel(sdl::V1_0::V1_0::CSubstrateSpecificationDocumentTypeIdsObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::V1_0::CSubstrateSpecificationDocumentTypeIdsObjectList::insert(int index, sdl::V1_0::V1_0::CSubstrateSpecificationDocumentTypeIdsObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::V1_0::CSubstrateSpecificationDocumentTypeIdsObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::V1_0::CSubstrateSpecificationDocumentTypeIdsObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::V1_0::CSubstrateSpecificationDocumentTypeIdsObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::V1_0::CSubstrateSpecificationDocumentTypeIdsObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_paperID"){
		return QVariant::fromValue(at(index)->PaperID.value());
	}
	if (nameId == "m_cardboardID"){
		return QVariant::fromValue(at(index)->CardboardID.value());
	}
	if (nameId == "m_filmID"){
		return QVariant::fromValue(at(index)->FilmID.value());
	}

	return QVariant();
}
CSubstrateSpecificationListInputObject::CSubstrateSpecificationListInputObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CSubstrateSpecificationListInputObject::viewParamsChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CSubstrateSpecificationListInputObject::GetViewParams()
{
	if (!m_viewParamsQObjectPtr.isValid()){
		m_viewParamsQObjectPtr = CreateObject("viewParams");
		auto itemPtr = m_viewParamsQObjectPtr.value<sdl::V1_0::V1_0::CCollectionViewParamsTestObject*>();
		if (itemPtr != nullptr) {
			itemPtr->CCollectionViewParamsTest::count = viewParams->CCollectionViewParamsTest::count;
			itemPtr->CCollectionViewParamsTest::offset = viewParams->CCollectionViewParamsTest::offset;
		}
	}
	return m_viewParamsQObjectPtr;
}


void CSubstrateSpecificationListInputObject::SetViewParams(const QVariant& v)
{

	if (v.isValid()){
		sdl::V1_0::V1_0::CCollectionViewParamsTestObject* itemPtr = v.value<sdl::V1_0::V1_0::CCollectionViewParamsTestObject*>();
		if (itemPtr != nullptr)  viewParams = static_cast<const decltype(viewParams)::value_type&>(*itemPtr);
	}
	else {
		CSubstrateSpecificationListInput::viewParams = nullptr;
	}
	m_viewParamsQObjectPtr = v;

	viewParamsChanged();
}


bool CSubstrateSpecificationListInputObject::hasViewParams()
{
	 return CSubstrateSpecificationListInput::viewParams.HasValue();
}


void CSubstrateSpecificationListInputObject::emplaceViewParams()
{
	ResetViewParams();
	CSubstrateSpecificationListInput::viewParams.emplace();
}


void CSubstrateSpecificationListInputObject::ResetViewParams()
{
	auto* itemPtr = m_viewParamsQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_viewParamsQObjectPtr = QVariant();
}


QString CSubstrateSpecificationListInputObject::toJson() const
{
	QJsonObject jsonObject;
	bool res = WriteToJsonObject(jsonObject);
	if (res){
		QJsonDocument document;
		document.setObject(jsonObject);
		return document.toJson(QJsonDocument::Compact);
	}

	return QString();
}


bool CSubstrateSpecificationListInputObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CSubstrateSpecificationListInputObject::fromObject(const QJsonObject& jsonObject)
{
	beginChanges();
	bool res = ReadFromJsonObject(jsonObject);
	if (res){
		QVariantList changelist;
		modelChanged(changelist);
	}

	endChanges();

	finished();

	return res;
}


QString CSubstrateSpecificationListInputObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CSubstrateSpecificationListInputObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	if (key == "viewParams"){
		return QVariant::fromValue(new sdl::V1_0::V1_0::CCollectionViewParamsTestObject(this));
	}
	return QVariant();
}


QString CSubstrateSpecificationListInputObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "viewParams")){
		return "viewParams";
	}

	return propertyName;
}





bool CSubstrateSpecificationListInputObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CSubstrateSpecificationListInputObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CSubstrateSpecificationListInputObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CSubstrateSpecificationListInputObjectList::append(sdl::V1_0::V1_0::CSubstrateSpecificationListInputObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::V1_0::CSubstrateSpecificationListInputObjectList* sdl::V1_0::V1_0::CSubstrateSpecificationListInputObjectList::copyMe()
{
	sdl::V1_0::V1_0::CSubstrateSpecificationListInputObjectList* retVal = new sdl::V1_0::V1_0::CSubstrateSpecificationListInputObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::V1_0::CSubstrateSpecificationListInputObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::V1_0::CSubstrateSpecificationListInputObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::V1_0::CSubstrateSpecificationListInputObjectList::addElement(sdl::V1_0::V1_0::CSubstrateSpecificationListInputObject* item)
{
	append(item);
}


void sdl::V1_0::V1_0::CSubstrateSpecificationListInputObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::V1_0::CSubstrateSpecificationListInputObjectList::isEqualWithModel(sdl::V1_0::V1_0::CSubstrateSpecificationListInputObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::V1_0::CSubstrateSpecificationListInputObjectList::insert(int index, sdl::V1_0::V1_0::CSubstrateSpecificationListInputObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::V1_0::CSubstrateSpecificationListInputObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::V1_0::CSubstrateSpecificationListInputObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::V1_0::CSubstrateSpecificationListInputObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::V1_0::CSubstrateSpecificationListInputObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_viewParams"){
		return itemPtr->GetViewParams();
	}

	return QVariant();
}
CSubstrateSpecificationListItemObject::CSubstrateSpecificationListItemObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CSubstrateSpecificationListItemObject::idChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CSubstrateSpecificationListItemObject::typeIdChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CSubstrateSpecificationListItemObject::nameChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CSubstrateSpecificationListItemObject::materialIdChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CSubstrateSpecificationListItemObject::materialNameChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CSubstrateSpecificationListItemObject::addedChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CSubstrateSpecificationListItemObject::timeStampChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CSubstrateSpecificationListItemObject::colorPreviewChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CSubstrateSpecificationListItemObject::GetId()
{
	return CSubstrateSpecificationListItem::id.value();
}


void CSubstrateSpecificationListItemObject::SetId(const QVariant& v)
{

	CSubstrateSpecificationListItem::id = v.value<QString>().toUtf8();
	idChanged();
}


bool CSubstrateSpecificationListItemObject::hasId()
{
	 return CSubstrateSpecificationListItem::id.HasValue();
}


QVariant CSubstrateSpecificationListItemObject::GetTypeId()
{
	return CSubstrateSpecificationListItem::typeId.value();
}


void CSubstrateSpecificationListItemObject::SetTypeId(const QVariant& v)
{

	CSubstrateSpecificationListItem::typeId = v.value<QString>().toUtf8();
	typeIdChanged();
}


bool CSubstrateSpecificationListItemObject::hasTypeId()
{
	 return CSubstrateSpecificationListItem::typeId.HasValue();
}


QVariant CSubstrateSpecificationListItemObject::GetName()
{
	return CSubstrateSpecificationListItem::name.value();
}


void CSubstrateSpecificationListItemObject::SetName(const QVariant& v)
{

	CSubstrateSpecificationListItem::name = v.value<QString>();
	nameChanged();
}


bool CSubstrateSpecificationListItemObject::hasName()
{
	 return CSubstrateSpecificationListItem::name.HasValue();
}


QVariant CSubstrateSpecificationListItemObject::GetMaterialId()
{
	return CSubstrateSpecificationListItem::materialId.value();
}


void CSubstrateSpecificationListItemObject::SetMaterialId(const QVariant& v)
{

	CSubstrateSpecificationListItem::materialId = v.value<QString>().toUtf8();
	materialIdChanged();
}


bool CSubstrateSpecificationListItemObject::hasMaterialId()
{
	 return CSubstrateSpecificationListItem::materialId.HasValue();
}


QVariant CSubstrateSpecificationListItemObject::GetMaterialName()
{
	return CSubstrateSpecificationListItem::materialName.value();
}


void CSubstrateSpecificationListItemObject::SetMaterialName(const QVariant& v)
{

	CSubstrateSpecificationListItem::materialName = v.value<QString>();
	materialNameChanged();
}


bool CSubstrateSpecificationListItemObject::hasMaterialName()
{
	 return CSubstrateSpecificationListItem::materialName.HasValue();
}


QVariant CSubstrateSpecificationListItemObject::GetAdded()
{
	return CSubstrateSpecificationListItem::added.value();
}


void CSubstrateSpecificationListItemObject::SetAdded(const QVariant& v)
{

	CSubstrateSpecificationListItem::added = v.value<QString>();
	addedChanged();
}


bool CSubstrateSpecificationListItemObject::hasAdded()
{
	 return CSubstrateSpecificationListItem::added.HasValue();
}


QVariant CSubstrateSpecificationListItemObject::GetTimeStamp()
{
	return CSubstrateSpecificationListItem::timeStamp.value();
}


void CSubstrateSpecificationListItemObject::SetTimeStamp(const QVariant& v)
{

	CSubstrateSpecificationListItem::timeStamp = v.value<QString>();
	timeStampChanged();
}


bool CSubstrateSpecificationListItemObject::hasTimeStamp()
{
	 return CSubstrateSpecificationListItem::timeStamp.HasValue();
}


QVariant CSubstrateSpecificationListItemObject::GetColorPreview()
{
	return CSubstrateSpecificationListItem::colorPreview.value();
}


void CSubstrateSpecificationListItemObject::SetColorPreview(const QVariant& v)
{

	CSubstrateSpecificationListItem::colorPreview = v.value<QString>();
	colorPreviewChanged();
}


bool CSubstrateSpecificationListItemObject::hasColorPreview()
{
	 return CSubstrateSpecificationListItem::colorPreview.HasValue();
}


QString CSubstrateSpecificationListItemObject::toJson() const
{
	QJsonObject jsonObject;
	bool res = WriteToJsonObject(jsonObject);
	if (res){
		QJsonDocument document;
		document.setObject(jsonObject);
		return document.toJson(QJsonDocument::Compact);
	}

	return QString();
}


bool CSubstrateSpecificationListItemObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CSubstrateSpecificationListItemObject::fromObject(const QJsonObject& jsonObject)
{
	beginChanges();
	bool res = ReadFromJsonObject(jsonObject);
	if (res){
		QVariantList changelist;
		modelChanged(changelist);
	}

	endChanges();

	finished();

	return res;
}


QString CSubstrateSpecificationListItemObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CSubstrateSpecificationListItemObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CSubstrateSpecificationListItemObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "id")){
		return "id";
	}
	if (propertyName == (QString("m_") + "typeId")){
		return "typeId";
	}
	if (propertyName == (QString("m_") + "name")){
		return "name";
	}
	if (propertyName == (QString("m_") + "materialId")){
		return "materialId";
	}
	if (propertyName == (QString("m_") + "materialName")){
		return "materialName";
	}
	if (propertyName == (QString("m_") + "added")){
		return "added";
	}
	if (propertyName == (QString("m_") + "timeStamp")){
		return "timeStamp";
	}
	if (propertyName == (QString("m_") + "colorPreview")){
		return "colorPreview";
	}

	return propertyName;
}





bool CSubstrateSpecificationListItemObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CSubstrateSpecificationListItemObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CSubstrateSpecificationListItemObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CSubstrateSpecificationListItemObjectList::append(sdl::V1_0::V1_0::CSubstrateSpecificationListItemObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::V1_0::CSubstrateSpecificationListItemObjectList* sdl::V1_0::V1_0::CSubstrateSpecificationListItemObjectList::copyMe()
{
	sdl::V1_0::V1_0::CSubstrateSpecificationListItemObjectList* retVal = new sdl::V1_0::V1_0::CSubstrateSpecificationListItemObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::V1_0::CSubstrateSpecificationListItemObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::V1_0::CSubstrateSpecificationListItemObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::V1_0::CSubstrateSpecificationListItemObjectList::addElement(sdl::V1_0::V1_0::CSubstrateSpecificationListItemObject* item)
{
	append(item);
}


void sdl::V1_0::V1_0::CSubstrateSpecificationListItemObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::V1_0::CSubstrateSpecificationListItemObjectList::isEqualWithModel(sdl::V1_0::V1_0::CSubstrateSpecificationListItemObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::V1_0::CSubstrateSpecificationListItemObjectList::insert(int index, sdl::V1_0::V1_0::CSubstrateSpecificationListItemObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::V1_0::CSubstrateSpecificationListItemObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::V1_0::CSubstrateSpecificationListItemObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::V1_0::CSubstrateSpecificationListItemObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::V1_0::CSubstrateSpecificationListItemObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_id"){
		return QVariant::fromValue(at(index)->id.value());
	}
	if (nameId == "m_typeId"){
		return QVariant::fromValue(at(index)->typeId.value());
	}
	if (nameId == "m_name"){
		return QVariant::fromValue(at(index)->name.value());
	}
	if (nameId == "m_materialId"){
		return QVariant::fromValue(at(index)->materialId.value());
	}
	if (nameId == "m_materialName"){
		return QVariant::fromValue(at(index)->materialName.value());
	}
	if (nameId == "m_added"){
		return QVariant::fromValue(at(index)->added.value());
	}
	if (nameId == "m_timeStamp"){
		return QVariant::fromValue(at(index)->timeStamp.value());
	}
	if (nameId == "m_colorPreview"){
		return QVariant::fromValue(at(index)->colorPreview.value());
	}

	return QVariant();
}
CSubstrateSpecificationListResponseObject::CSubstrateSpecificationListResponseObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CSubstrateSpecificationListResponseObject::itemsChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CSubstrateSpecificationListResponseObject::GetItems()
{
	if (!m_itemsQObjectPtr.isValid()){
		m_itemsQObjectPtr = CreateObject("items");
		auto itemPtr = m_itemsQObjectPtr.value<sdl::V1_0::V1_0::CSubstrateSpecificationListItemObjectList*>();
		if (itemPtr != nullptr) itemPtr->items = items;
	}
	return m_itemsQObjectPtr;
}


void CSubstrateSpecificationListResponseObject::SetItems(const QVariant& v)
{

	if (v.isValid()){
		sdl::V1_0::V1_0::CSubstrateSpecificationListItemObjectList* itemPtr = v.value<sdl::V1_0::V1_0::CSubstrateSpecificationListItemObjectList*>();
		if (itemPtr != nullptr)  items = static_cast<const decltype(items)::value_type&>(*itemPtr);
	}
	else {
		CSubstrateSpecificationListResponse::items = nullptr;
	}
	m_itemsQObjectPtr = v;

	itemsChanged();
}


bool CSubstrateSpecificationListResponseObject::hasItems()
{
	 return CSubstrateSpecificationListResponse::items.HasValue();
}


void CSubstrateSpecificationListResponseObject::emplaceItems()
{
	ResetItems();
	CSubstrateSpecificationListResponse::items.emplace();
}


void CSubstrateSpecificationListResponseObject::ResetItems()
{
	auto* itemPtr = m_itemsQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_itemsQObjectPtr = QVariant();
}


QVariant CSubstrateSpecificationListResponseObject::createItemsArrayElement(const QVariant& v)
{
	Q_UNUSED(v);
	return QVariant::fromValue(new sdl::V1_0::V1_0::CSubstrateSpecificationListItemObject());
}


QString CSubstrateSpecificationListResponseObject::toJson() const
{
	QJsonObject jsonObject;
	bool res = WriteToJsonObject(jsonObject);
	if (res){
		QJsonDocument document;
		document.setObject(jsonObject);
		return document.toJson(QJsonDocument::Compact);
	}

	return QString();
}


bool CSubstrateSpecificationListResponseObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CSubstrateSpecificationListResponseObject::fromObject(const QJsonObject& jsonObject)
{
	beginChanges();
	bool res = ReadFromJsonObject(jsonObject);
	if (res){
		QVariantList changelist;
		modelChanged(changelist);
	}

	endChanges();

	finished();

	return res;
}


QString CSubstrateSpecificationListResponseObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CSubstrateSpecificationListResponseObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	if (key == "items"){
		return QVariant::fromValue(new sdl::V1_0::V1_0::CSubstrateSpecificationListItemObjectList(this));
	}
	return QVariant();
}


QString CSubstrateSpecificationListResponseObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "items")){
		return "items";
	}

	return propertyName;
}





bool CSubstrateSpecificationListResponseObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CSubstrateSpecificationListResponseObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CSubstrateSpecificationListResponseObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CSubstrateSpecificationListResponseObjectList::append(sdl::V1_0::V1_0::CSubstrateSpecificationListResponseObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::V1_0::CSubstrateSpecificationListResponseObjectList* sdl::V1_0::V1_0::CSubstrateSpecificationListResponseObjectList::copyMe()
{
	sdl::V1_0::V1_0::CSubstrateSpecificationListResponseObjectList* retVal = new sdl::V1_0::V1_0::CSubstrateSpecificationListResponseObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::V1_0::CSubstrateSpecificationListResponseObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::V1_0::CSubstrateSpecificationListResponseObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::V1_0::CSubstrateSpecificationListResponseObjectList::addElement(sdl::V1_0::V1_0::CSubstrateSpecificationListResponseObject* item)
{
	append(item);
}


void sdl::V1_0::V1_0::CSubstrateSpecificationListResponseObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::V1_0::CSubstrateSpecificationListResponseObjectList::isEqualWithModel(sdl::V1_0::V1_0::CSubstrateSpecificationListResponseObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::V1_0::CSubstrateSpecificationListResponseObjectList::insert(int index, sdl::V1_0::V1_0::CSubstrateSpecificationListResponseObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::V1_0::CSubstrateSpecificationListResponseObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::V1_0::CSubstrateSpecificationListResponseObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::V1_0::CSubstrateSpecificationListResponseObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::V1_0::CSubstrateSpecificationListResponseObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_items"){
		return itemPtr->GetItems();
	}

	return QVariant();
}
CCardboardSubstrateSpecificationObject::CCardboardSubstrateSpecificationObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CCardboardSubstrateSpecificationObject::colorChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCardboardSubstrateSpecificationObject::grammageChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CCardboardSubstrateSpecificationObject::GetColor()
{
	return CCardboardSubstrateSpecification::color.value();
}


void CCardboardSubstrateSpecificationObject::SetColor(const QVariant& v)
{

	CCardboardSubstrateSpecification::color = v.value<QString>();
	colorChanged();
}


bool CCardboardSubstrateSpecificationObject::hasColor()
{
	 return CCardboardSubstrateSpecification::color.HasValue();
}


QVariant CCardboardSubstrateSpecificationObject::GetGrammage()
{
	return CCardboardSubstrateSpecification::grammage.value();
}


void CCardboardSubstrateSpecificationObject::SetGrammage(const QVariant& v)
{

	CCardboardSubstrateSpecification::grammage = v.value<double>();
	grammageChanged();
}


bool CCardboardSubstrateSpecificationObject::hasGrammage()
{
	 return CCardboardSubstrateSpecification::grammage.HasValue();
}


QString CCardboardSubstrateSpecificationObject::toJson() const
{
	QJsonObject jsonObject;
	bool res = WriteToJsonObject(jsonObject);
	if (res){
		QJsonDocument document;
		document.setObject(jsonObject);
		return document.toJson(QJsonDocument::Compact);
	}

	return QString();
}


bool CCardboardSubstrateSpecificationObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CCardboardSubstrateSpecificationObject::fromObject(const QJsonObject& jsonObject)
{
	beginChanges();
	bool res = ReadFromJsonObject(jsonObject);
	if (res){
		QVariantList changelist;
		modelChanged(changelist);
	}

	endChanges();

	finished();

	return res;
}


QString CCardboardSubstrateSpecificationObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CCardboardSubstrateSpecificationObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CCardboardSubstrateSpecificationObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "color")){
		return "color";
	}
	if (propertyName == (QString("m_") + "grammage")){
		return "grammage";
	}

	return propertyName;
}





bool CCardboardSubstrateSpecificationObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CCardboardSubstrateSpecificationObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CCardboardSubstrateSpecificationObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CCardboardSubstrateSpecificationObjectList::append(sdl::V1_0::V1_0::CCardboardSubstrateSpecificationObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::V1_0::CCardboardSubstrateSpecificationObjectList* sdl::V1_0::V1_0::CCardboardSubstrateSpecificationObjectList::copyMe()
{
	sdl::V1_0::V1_0::CCardboardSubstrateSpecificationObjectList* retVal = new sdl::V1_0::V1_0::CCardboardSubstrateSpecificationObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::V1_0::CCardboardSubstrateSpecificationObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::V1_0::CCardboardSubstrateSpecificationObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::V1_0::CCardboardSubstrateSpecificationObjectList::addElement(sdl::V1_0::V1_0::CCardboardSubstrateSpecificationObject* item)
{
	append(item);
}


void sdl::V1_0::V1_0::CCardboardSubstrateSpecificationObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::V1_0::CCardboardSubstrateSpecificationObjectList::isEqualWithModel(sdl::V1_0::V1_0::CCardboardSubstrateSpecificationObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::V1_0::CCardboardSubstrateSpecificationObjectList::insert(int index, sdl::V1_0::V1_0::CCardboardSubstrateSpecificationObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::V1_0::CCardboardSubstrateSpecificationObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::V1_0::CCardboardSubstrateSpecificationObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::V1_0::CCardboardSubstrateSpecificationObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::V1_0::CCardboardSubstrateSpecificationObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_color"){
		return QVariant::fromValue(at(index)->color.value());
	}
	if (nameId == "m_grammage"){
		return QVariant::fromValue(at(index)->grammage.value());
	}

	return QVariant();
}
CPaperSubstrateSpecificationObject::CPaperSubstrateSpecificationObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CPaperSubstrateSpecificationObject::grammageChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CPaperSubstrateSpecificationObject::colorChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CPaperSubstrateSpecificationObject::coatTypeChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CPaperSubstrateSpecificationObject::GetGrammage()
{
	return CPaperSubstrateSpecification::grammage.value();
}


void CPaperSubstrateSpecificationObject::SetGrammage(const QVariant& v)
{

	CPaperSubstrateSpecification::grammage = v.value<double>();
	grammageChanged();
}


bool CPaperSubstrateSpecificationObject::hasGrammage()
{
	 return CPaperSubstrateSpecification::grammage.HasValue();
}


QVariant CPaperSubstrateSpecificationObject::GetColor()
{
	return CPaperSubstrateSpecification::color.value();
}


void CPaperSubstrateSpecificationObject::SetColor(const QVariant& v)
{

	CPaperSubstrateSpecification::color = v.value<QString>();
	colorChanged();
}


bool CPaperSubstrateSpecificationObject::hasColor()
{
	 return CPaperSubstrateSpecification::color.HasValue();
}


QVariant CPaperSubstrateSpecificationObject::GetCoatType()
{
	return CPaperSubstrateSpecification::coatType.value();
}


void CPaperSubstrateSpecificationObject::SetCoatType(const QVariant& v)
{

	CPaperSubstrateSpecification::coatType = v.value<QString>();
	coatTypeChanged();
}


bool CPaperSubstrateSpecificationObject::hasCoatType()
{
	 return CPaperSubstrateSpecification::coatType.HasValue();
}


QString CPaperSubstrateSpecificationObject::toJson() const
{
	QJsonObject jsonObject;
	bool res = WriteToJsonObject(jsonObject);
	if (res){
		QJsonDocument document;
		document.setObject(jsonObject);
		return document.toJson(QJsonDocument::Compact);
	}

	return QString();
}


bool CPaperSubstrateSpecificationObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CPaperSubstrateSpecificationObject::fromObject(const QJsonObject& jsonObject)
{
	beginChanges();
	bool res = ReadFromJsonObject(jsonObject);
	if (res){
		QVariantList changelist;
		modelChanged(changelist);
	}

	endChanges();

	finished();

	return res;
}


QString CPaperSubstrateSpecificationObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CPaperSubstrateSpecificationObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CPaperSubstrateSpecificationObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "grammage")){
		return "grammage";
	}
	if (propertyName == (QString("m_") + "color")){
		return "color";
	}
	if (propertyName == (QString("m_") + "coatType")){
		return "coatType";
	}

	return propertyName;
}





bool CPaperSubstrateSpecificationObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CPaperSubstrateSpecificationObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CPaperSubstrateSpecificationObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CPaperSubstrateSpecificationObjectList::append(sdl::V1_0::V1_0::CPaperSubstrateSpecificationObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::V1_0::CPaperSubstrateSpecificationObjectList* sdl::V1_0::V1_0::CPaperSubstrateSpecificationObjectList::copyMe()
{
	sdl::V1_0::V1_0::CPaperSubstrateSpecificationObjectList* retVal = new sdl::V1_0::V1_0::CPaperSubstrateSpecificationObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::V1_0::CPaperSubstrateSpecificationObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::V1_0::CPaperSubstrateSpecificationObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::V1_0::CPaperSubstrateSpecificationObjectList::addElement(sdl::V1_0::V1_0::CPaperSubstrateSpecificationObject* item)
{
	append(item);
}


void sdl::V1_0::V1_0::CPaperSubstrateSpecificationObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::V1_0::CPaperSubstrateSpecificationObjectList::isEqualWithModel(sdl::V1_0::V1_0::CPaperSubstrateSpecificationObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::V1_0::CPaperSubstrateSpecificationObjectList::insert(int index, sdl::V1_0::V1_0::CPaperSubstrateSpecificationObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::V1_0::CPaperSubstrateSpecificationObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::V1_0::CPaperSubstrateSpecificationObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::V1_0::CPaperSubstrateSpecificationObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::V1_0::CPaperSubstrateSpecificationObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_grammage"){
		return QVariant::fromValue(at(index)->grammage.value());
	}
	if (nameId == "m_color"){
		return QVariant::fromValue(at(index)->color.value());
	}
	if (nameId == "m_coatType"){
		return QVariant::fromValue(at(index)->coatType.value());
	}

	return QVariant();
}
CFilmSubstrateSpecificationObject::CFilmSubstrateSpecificationObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CFilmSubstrateSpecificationObject::transparencyChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CFilmSubstrateSpecificationObject::colorChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CFilmSubstrateSpecificationObject::materialChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CFilmSubstrateSpecificationObject::GetTransparency()
{
	return CFilmSubstrateSpecification::transparency.value();
}


void CFilmSubstrateSpecificationObject::SetTransparency(const QVariant& v)
{

	CFilmSubstrateSpecification::transparency = v.value<double>();
	transparencyChanged();
}


bool CFilmSubstrateSpecificationObject::hasTransparency()
{
	 return CFilmSubstrateSpecification::transparency.HasValue();
}


QVariant CFilmSubstrateSpecificationObject::GetColor()
{
	return CFilmSubstrateSpecification::color.value();
}


void CFilmSubstrateSpecificationObject::SetColor(const QVariant& v)
{

	CFilmSubstrateSpecification::color = v.value<QString>();
	colorChanged();
}


bool CFilmSubstrateSpecificationObject::hasColor()
{
	 return CFilmSubstrateSpecification::color.HasValue();
}


QVariant CFilmSubstrateSpecificationObject::GetMaterial()
{
	return CFilmSubstrateSpecification::material.value();
}


void CFilmSubstrateSpecificationObject::SetMaterial(const QVariant& v)
{

	CFilmSubstrateSpecification::material = v.value<QString>();
	materialChanged();
}


bool CFilmSubstrateSpecificationObject::hasMaterial()
{
	 return CFilmSubstrateSpecification::material.HasValue();
}


QString CFilmSubstrateSpecificationObject::toJson() const
{
	QJsonObject jsonObject;
	bool res = WriteToJsonObject(jsonObject);
	if (res){
		QJsonDocument document;
		document.setObject(jsonObject);
		return document.toJson(QJsonDocument::Compact);
	}

	return QString();
}


bool CFilmSubstrateSpecificationObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CFilmSubstrateSpecificationObject::fromObject(const QJsonObject& jsonObject)
{
	beginChanges();
	bool res = ReadFromJsonObject(jsonObject);
	if (res){
		QVariantList changelist;
		modelChanged(changelist);
	}

	endChanges();

	finished();

	return res;
}


QString CFilmSubstrateSpecificationObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CFilmSubstrateSpecificationObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CFilmSubstrateSpecificationObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "transparency")){
		return "transparency";
	}
	if (propertyName == (QString("m_") + "color")){
		return "color";
	}
	if (propertyName == (QString("m_") + "material")){
		return "material";
	}

	return propertyName;
}





bool CFilmSubstrateSpecificationObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CFilmSubstrateSpecificationObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CFilmSubstrateSpecificationObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CFilmSubstrateSpecificationObjectList::append(sdl::V1_0::V1_0::CFilmSubstrateSpecificationObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::V1_0::CFilmSubstrateSpecificationObjectList* sdl::V1_0::V1_0::CFilmSubstrateSpecificationObjectList::copyMe()
{
	sdl::V1_0::V1_0::CFilmSubstrateSpecificationObjectList* retVal = new sdl::V1_0::V1_0::CFilmSubstrateSpecificationObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::V1_0::CFilmSubstrateSpecificationObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::V1_0::CFilmSubstrateSpecificationObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::V1_0::CFilmSubstrateSpecificationObjectList::addElement(sdl::V1_0::V1_0::CFilmSubstrateSpecificationObject* item)
{
	append(item);
}


void sdl::V1_0::V1_0::CFilmSubstrateSpecificationObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::V1_0::CFilmSubstrateSpecificationObjectList::isEqualWithModel(sdl::V1_0::V1_0::CFilmSubstrateSpecificationObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::V1_0::CFilmSubstrateSpecificationObjectList::insert(int index, sdl::V1_0::V1_0::CFilmSubstrateSpecificationObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::V1_0::CFilmSubstrateSpecificationObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::V1_0::CFilmSubstrateSpecificationObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::V1_0::CFilmSubstrateSpecificationObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::V1_0::CFilmSubstrateSpecificationObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_transparency"){
		return QVariant::fromValue(at(index)->transparency.value());
	}
	if (nameId == "m_color"){
		return QVariant::fromValue(at(index)->color.value());
	}
	if (nameId == "m_material"){
		return QVariant::fromValue(at(index)->material.value());
	}

	return QVariant();
}
CSubstrateSpecificationInputObject::CSubstrateSpecificationInputObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CSubstrateSpecificationInputObject::idChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CSubstrateSpecificationInputObject::nameChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CSubstrateSpecificationInputObject::descriptionChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CSubstrateSpecificationInputObject::itemChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CSubstrateSpecificationInputObject::typeIdChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CSubstrateSpecificationInputObject::GetId()
{
	return CSubstrateSpecificationInput::id.value();
}


void CSubstrateSpecificationInputObject::SetId(const QVariant& v)
{

	CSubstrateSpecificationInput::id = v.value<QString>().toUtf8();
	idChanged();
}


bool CSubstrateSpecificationInputObject::hasId()
{
	 return CSubstrateSpecificationInput::id.HasValue();
}


QVariant CSubstrateSpecificationInputObject::GetName()
{
	return CSubstrateSpecificationInput::name.value();
}


void CSubstrateSpecificationInputObject::SetName(const QVariant& v)
{

	CSubstrateSpecificationInput::name = v.value<QString>();
	nameChanged();
}


bool CSubstrateSpecificationInputObject::hasName()
{
	 return CSubstrateSpecificationInput::name.HasValue();
}


QVariant CSubstrateSpecificationInputObject::GetDescription()
{
	return CSubstrateSpecificationInput::description.value();
}


void CSubstrateSpecificationInputObject::SetDescription(const QVariant& v)
{

	CSubstrateSpecificationInput::description = v.value<QString>();
	descriptionChanged();
}


bool CSubstrateSpecificationInputObject::hasDescription()
{
	 return CSubstrateSpecificationInput::description.HasValue();
}


QVariant CSubstrateSpecificationInputObject::GetItem()
{
	if (m_itemQObjectPtr.isValid()){
		if (const CCardboardSubstrateSpecification* val = std::get_if<CCardboardSubstrateSpecification>((CSubstrateSpecificationInput::item).GetPtr())){
			CCardboardSubstrateSpecificationObject *newObjectPtr = new CCardboardSubstrateSpecificationObject(this);
			static_cast<CCardboardSubstrateSpecification&>(*newObjectPtr) = *val;
			m_itemQObjectPtr = QVariant::fromValue(newObjectPtr);
		}
		if (const CPaperSubstrateSpecification* val = std::get_if<CPaperSubstrateSpecification>((CSubstrateSpecificationInput::item).GetPtr())){
			CPaperSubstrateSpecificationObject *newObjectPtr = new CPaperSubstrateSpecificationObject(this);
			static_cast<CPaperSubstrateSpecification&>(*newObjectPtr) = *val;
			m_itemQObjectPtr = QVariant::fromValue(newObjectPtr);
		}
		if (const CFilmSubstrateSpecification* val = std::get_if<CFilmSubstrateSpecification>((CSubstrateSpecificationInput::item).GetPtr())){
			CFilmSubstrateSpecificationObject *newObjectPtr = new CFilmSubstrateSpecificationObject(this);
			static_cast<CFilmSubstrateSpecification&>(*newObjectPtr) = *val;
			m_itemQObjectPtr = QVariant::fromValue(newObjectPtr);
		}
	}
	return m_itemQObjectPtr;
}


void CSubstrateSpecificationInputObject::SetItem(const QVariant& v)
{

	if (v.isValid()){
		if (const CCardboardSubstrateSpecificationObject* val = v.value<const CCardboardSubstrateSpecificationObject*>()){
			*item = *val;
		}
		if (const CPaperSubstrateSpecificationObject* val = v.value<const CPaperSubstrateSpecificationObject*>()){
			*item = *val;
		}
		if (const CFilmSubstrateSpecificationObject* val = v.value<const CFilmSubstrateSpecificationObject*>()){
			*item = *val;
		}
	}
	else {
		CSubstrateSpecificationInput::item = nullptr;
	}
	m_itemQObjectPtr = v;

	itemChanged();
}


bool CSubstrateSpecificationInputObject::hasItem()
{
	 return CSubstrateSpecificationInput::item.HasValue();
}


void CSubstrateSpecificationInputObject::emplaceItem()
{
	ResetItem();
	CSubstrateSpecificationInput::item.emplace();
}


void CSubstrateSpecificationInputObject::ResetItem()
{
	auto* itemPtr = m_itemQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_itemQObjectPtr = QVariant();
}


QVariant CSubstrateSpecificationInputObject::GetTypeId()
{
	return CSubstrateSpecificationInput::typeId.value();
}


void CSubstrateSpecificationInputObject::SetTypeId(const QVariant& v)
{

	CSubstrateSpecificationInput::typeId = v.value<QString>().toUtf8();
	typeIdChanged();
}


bool CSubstrateSpecificationInputObject::hasTypeId()
{
	 return CSubstrateSpecificationInput::typeId.HasValue();
}


QString CSubstrateSpecificationInputObject::toJson() const
{
	QJsonObject jsonObject;
	bool res = WriteToJsonObject(jsonObject);
	if (res){
		QJsonDocument document;
		document.setObject(jsonObject);
		return document.toJson(QJsonDocument::Compact);
	}

	return QString();
}


bool CSubstrateSpecificationInputObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CSubstrateSpecificationInputObject::fromObject(const QJsonObject& jsonObject)
{
	beginChanges();
	bool res = ReadFromJsonObject(jsonObject);
	if (res){
		QVariantList changelist;
		modelChanged(changelist);
	}

	endChanges();

	finished();

	return res;
}


QString CSubstrateSpecificationInputObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CSubstrateSpecificationInputObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CSubstrateSpecificationInputObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "id")){
		return "id";
	}
	if (propertyName == (QString("m_") + "name")){
		return "name";
	}
	if (propertyName == (QString("m_") + "description")){
		return "description";
	}
	if (propertyName == (QString("m_") + "item")){
		return "item";
	}
	if (propertyName == (QString("m_") + "typeId")){
		return "typeId";
	}

	return propertyName;
}





bool CSubstrateSpecificationInputObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CSubstrateSpecificationInputObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CSubstrateSpecificationInputObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CSubstrateSpecificationInputObjectList::append(sdl::V1_0::V1_0::CSubstrateSpecificationInputObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::V1_0::CSubstrateSpecificationInputObjectList* sdl::V1_0::V1_0::CSubstrateSpecificationInputObjectList::copyMe()
{
	sdl::V1_0::V1_0::CSubstrateSpecificationInputObjectList* retVal = new sdl::V1_0::V1_0::CSubstrateSpecificationInputObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::V1_0::CSubstrateSpecificationInputObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::V1_0::CSubstrateSpecificationInputObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::V1_0::CSubstrateSpecificationInputObjectList::addElement(sdl::V1_0::V1_0::CSubstrateSpecificationInputObject* item)
{
	append(item);
}


void sdl::V1_0::V1_0::CSubstrateSpecificationInputObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::V1_0::CSubstrateSpecificationInputObjectList::isEqualWithModel(sdl::V1_0::V1_0::CSubstrateSpecificationInputObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::V1_0::CSubstrateSpecificationInputObjectList::insert(int index, sdl::V1_0::V1_0::CSubstrateSpecificationInputObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::V1_0::CSubstrateSpecificationInputObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::V1_0::CSubstrateSpecificationInputObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::V1_0::CSubstrateSpecificationInputObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::V1_0::CSubstrateSpecificationInputObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_id"){
		return QVariant::fromValue(at(index)->id.value());
	}
	if (nameId == "m_name"){
		return QVariant::fromValue(at(index)->name.value());
	}
	if (nameId == "m_description"){
		return QVariant::fromValue(at(index)->description.value());
	}
	if (nameId == "m_item"){
		return itemPtr->GetItem();
	}
	if (nameId == "m_typeId"){
		return QVariant::fromValue(at(index)->typeId.value());
	}

	return QVariant();
}
CSubstrateSpecificationDocumentMetaInfoObject::CSubstrateSpecificationDocumentMetaInfoObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CSubstrateSpecificationDocumentMetaInfoObject::materialIdChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CSubstrateSpecificationDocumentMetaInfoObject::labChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CSubstrateSpecificationDocumentMetaInfoObject::GetMaterialId()
{
	return CSubstrateSpecificationDocumentMetaInfo::materialId.value();
}


void CSubstrateSpecificationDocumentMetaInfoObject::SetMaterialId(const QVariant& v)
{

	CSubstrateSpecificationDocumentMetaInfo::materialId = v.value<QString>().toUtf8();
	materialIdChanged();
}


bool CSubstrateSpecificationDocumentMetaInfoObject::hasMaterialId()
{
	 return CSubstrateSpecificationDocumentMetaInfo::materialId.HasValue();
}


QVariant CSubstrateSpecificationDocumentMetaInfoObject::GetLab()
{
	return CSubstrateSpecificationDocumentMetaInfo::lab.value();
}


void CSubstrateSpecificationDocumentMetaInfoObject::SetLab(const QVariant& v)
{

	CSubstrateSpecificationDocumentMetaInfo::lab = v.value<QString>();
	labChanged();
}


bool CSubstrateSpecificationDocumentMetaInfoObject::hasLab()
{
	 return CSubstrateSpecificationDocumentMetaInfo::lab.HasValue();
}


QString CSubstrateSpecificationDocumentMetaInfoObject::toJson() const
{
	QJsonObject jsonObject;
	bool res = WriteToJsonObject(jsonObject);
	if (res){
		QJsonDocument document;
		document.setObject(jsonObject);
		return document.toJson(QJsonDocument::Compact);
	}

	return QString();
}


bool CSubstrateSpecificationDocumentMetaInfoObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CSubstrateSpecificationDocumentMetaInfoObject::fromObject(const QJsonObject& jsonObject)
{
	beginChanges();
	bool res = ReadFromJsonObject(jsonObject);
	if (res){
		QVariantList changelist;
		modelChanged(changelist);
	}

	endChanges();

	finished();

	return res;
}


QString CSubstrateSpecificationDocumentMetaInfoObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CSubstrateSpecificationDocumentMetaInfoObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CSubstrateSpecificationDocumentMetaInfoObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "materialId")){
		return "materialId";
	}
	if (propertyName == (QString("m_") + "lab")){
		return "lab";
	}

	return propertyName;
}





bool CSubstrateSpecificationDocumentMetaInfoObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CSubstrateSpecificationDocumentMetaInfoObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CSubstrateSpecificationDocumentMetaInfoObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CSubstrateSpecificationDocumentMetaInfoObjectList::append(sdl::V1_0::V1_0::CSubstrateSpecificationDocumentMetaInfoObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::V1_0::CSubstrateSpecificationDocumentMetaInfoObjectList* sdl::V1_0::V1_0::CSubstrateSpecificationDocumentMetaInfoObjectList::copyMe()
{
	sdl::V1_0::V1_0::CSubstrateSpecificationDocumentMetaInfoObjectList* retVal = new sdl::V1_0::V1_0::CSubstrateSpecificationDocumentMetaInfoObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::V1_0::CSubstrateSpecificationDocumentMetaInfoObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::V1_0::CSubstrateSpecificationDocumentMetaInfoObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::V1_0::CSubstrateSpecificationDocumentMetaInfoObjectList::addElement(sdl::V1_0::V1_0::CSubstrateSpecificationDocumentMetaInfoObject* item)
{
	append(item);
}


void sdl::V1_0::V1_0::CSubstrateSpecificationDocumentMetaInfoObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::V1_0::CSubstrateSpecificationDocumentMetaInfoObjectList::isEqualWithModel(sdl::V1_0::V1_0::CSubstrateSpecificationDocumentMetaInfoObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::V1_0::CSubstrateSpecificationDocumentMetaInfoObjectList::insert(int index, sdl::V1_0::V1_0::CSubstrateSpecificationDocumentMetaInfoObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::V1_0::CSubstrateSpecificationDocumentMetaInfoObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::V1_0::CSubstrateSpecificationDocumentMetaInfoObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::V1_0::CSubstrateSpecificationDocumentMetaInfoObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::V1_0::CSubstrateSpecificationDocumentMetaInfoObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_materialId"){
		return QVariant::fromValue(at(index)->materialId.value());
	}
	if (nameId == "m_lab"){
		return QVariant::fromValue(at(index)->lab.value());
	}

	return QVariant();
}



QVariant CSubstrateSpecificationDocumentObjectList::GetOrCreateCachedObject(int index) const
{
	QVariant retVal;
	if (this->m_objectDataTypeMap.contains(index)){
		retVal = this->m_objectDataTypeMap[index];
	}
	else{
		if (const CCardboardSubstrateSpecification* val = std::get_if<CCardboardSubstrateSpecification>(this->at(index).GetPtr())){
			CCardboardSubstrateSpecificationObject *newObjectPtr = new CCardboardSubstrateSpecificationObject();
			static_cast<CCardboardSubstrateSpecification&>(*newObjectPtr) = *val;
			retVal = QVariant::fromValue(newObjectPtr);
		}

		if (const CPaperSubstrateSpecification* val = std::get_if<CPaperSubstrateSpecification>(this->at(index).GetPtr())){
			CPaperSubstrateSpecificationObject *newObjectPtr = new CPaperSubstrateSpecificationObject();
			static_cast<CPaperSubstrateSpecification&>(*newObjectPtr) = *val;
			retVal = QVariant::fromValue(newObjectPtr);
		}

		if (const CFilmSubstrateSpecification* val = std::get_if<CFilmSubstrateSpecification>(this->at(index).GetPtr())){
			CFilmSubstrateSpecificationObject *newObjectPtr = new CFilmSubstrateSpecificationObject();
			static_cast<CFilmSubstrateSpecification&>(*newObjectPtr) = *val;
			retVal = QVariant::fromValue(newObjectPtr);
		}

		this->m_objectDataTypeMap.insert(index, retVal);
	}
	return retVal;
}


bool CSubstrateSpecificationDocumentObjectList::containsKey(const QString& nameId, int /*index*/)
{
	return nameId == "item";
}


int CSubstrateSpecificationDocumentObjectList::getItemsCount()
{
	return rowCount();
}


QVariantMap CSubstrateSpecificationDocumentObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CSubstrateSpecificationDocumentObjectList::append(QVariant item)
{
	beginInsertRows(QModelIndex(), count(), 0);

	if (item.canConvert<CCardboardSubstrateSpecificationObject*>()){
		CCardboardSubstrateSpecificationObject* val = item.value<CCardboardSubstrateSpecificationObject*>();
		CCardboardSubstrateSpecification* newItemPtr = dynamic_cast<CCardboardSubstrateSpecification*>(val);
		append(istd::TNullableValue<SubstrateSpecificationDocument>(*newItemPtr));
	}

	if (item.canConvert<CPaperSubstrateSpecificationObject*>()){
		CPaperSubstrateSpecificationObject* val = item.value<CPaperSubstrateSpecificationObject*>();
		CPaperSubstrateSpecification* newItemPtr = dynamic_cast<CPaperSubstrateSpecification*>(val);
		append(istd::TNullableValue<SubstrateSpecificationDocument>(*newItemPtr));
	}

	if (item.canConvert<CFilmSubstrateSpecificationObject*>()){
		CFilmSubstrateSpecificationObject* val = item.value<CFilmSubstrateSpecificationObject*>();
		CFilmSubstrateSpecification* newItemPtr = dynamic_cast<CFilmSubstrateSpecification*>(val);
		append(istd::TNullableValue<SubstrateSpecificationDocument>(*newItemPtr));
	}

	ClearCache();
	endInsertRows();
}


sdl::V1_0::V1_0::CSubstrateSpecificationDocumentObjectList* CSubstrateSpecificationDocumentObjectList::copyMe()
{
	sdl::V1_0::V1_0::CSubstrateSpecificationDocumentObjectList* objectListPtr = new sdl::V1_0::V1_0::CSubstrateSpecificationDocumentObjectList();

	for (int i = 0; i < this->rowCount(); i++){
		QVariant item = this->getData("item", i);

		if (item.canConvert<CCardboardSubstrateSpecificationObject*>()){
			CCardboardSubstrateSpecificationObject* val = item.value<CCardboardSubstrateSpecificationObject*>();
			objectListPtr->addElement(QVariant::fromValue(val->copyMe()));
		}

		if (item.canConvert<CPaperSubstrateSpecificationObject*>()){
			CPaperSubstrateSpecificationObject* val = item.value<CPaperSubstrateSpecificationObject*>();
			objectListPtr->addElement(QVariant::fromValue(val->copyMe()));
		}

		if (item.canConvert<CFilmSubstrateSpecificationObject*>()){
			CFilmSubstrateSpecificationObject* val = item.value<CFilmSubstrateSpecificationObject*>();
			objectListPtr->addElement(QVariant::fromValue(val->copyMe()));
		}
	}
	return objectListPtr;
}


QString CSubstrateSpecificationDocumentObjectList::toJson()
{
	QString retVal = QStringLiteral("[");

	for (int i = 0; i < this->rowCount(); i++){
		if (i > 0 && i < this->rowCount() - 1){
			retVal += QStringLiteral(", ");
		}

		QVariant item = this->getData("item", i);

		if (item.canConvert<CCardboardSubstrateSpecificationObject*>()){
			CCardboardSubstrateSpecificationObject* val = item.value<CCardboardSubstrateSpecificationObject*>();
			retVal += val->toJson();
		}

		if (item.canConvert<CPaperSubstrateSpecificationObject*>()){
			CPaperSubstrateSpecificationObject* val = item.value<CPaperSubstrateSpecificationObject*>();
			retVal += val->toJson();
		}

		if (item.canConvert<CFilmSubstrateSpecificationObject*>()){
			CFilmSubstrateSpecificationObject* val = item.value<CFilmSubstrateSpecificationObject*>();
			retVal += val->toJson();
		}
	}

	retVal += QStringLiteral("]");

	return retVal;
}


QString CSubstrateSpecificationDocumentObjectList::toGraphQL()
{
	QString retVal = QStringLiteral("[");

	for (int i = 0; i < this->rowCount(); i++){
		if (i > 0 && i < this->rowCount() - 1){
			retVal += QStringLiteral(", ");
		}

		QVariant item = this->getData("item", i);

		if (item.canConvert<CCardboardSubstrateSpecificationObject*>()){
			CCardboardSubstrateSpecificationObject* val = item.value<CCardboardSubstrateSpecificationObject*>();
			retVal += val->toGraphQL();
		}

		if (item.canConvert<CPaperSubstrateSpecificationObject*>()){
			CPaperSubstrateSpecificationObject* val = item.value<CPaperSubstrateSpecificationObject*>();
			retVal += val->toGraphQL();
		}

		if (item.canConvert<CFilmSubstrateSpecificationObject*>()){
			CFilmSubstrateSpecificationObject* val = item.value<CFilmSubstrateSpecificationObject*>();
			retVal += val->toGraphQL();
		}
	}

	retVal += QStringLiteral("]");

	return retVal;
}


void CSubstrateSpecificationDocumentObjectList::addElement(QVariant item)
{
	append(item);
}

void CSubstrateSpecificationDocumentObjectList::removeElement(int index)
{
	remove(index);
}

bool CSubstrateSpecificationDocumentObjectList::isEqualWithModel(sdl::V1_0::V1_0::CSubstrateSpecificationDocumentObjectList* otherModelPtr)
{
	if (otherModelPtr == nullptr){
		return false;
	}

	if (this == otherModelPtr){
		return false;
	}

	if (this->rowCount() != otherModelPtr->rowCount()){
		return false;
	}

	for (int i = 0; i < this->rowCount(); i++){
		QVariant selfItem = this->getData("item", i);
		QVariant otherItem = otherModelPtr->getData("item", i);
		if (selfItem != otherItem){
			return false;
		}

	}

	return true;
}


void CSubstrateSpecificationDocumentObjectList::insert(int index, QVariant item)
{
	if (index < 0 || index > size()) return;
	beginInsertRows(QModelIndex(), count(), 0);

	if (item.canConvert<CCardboardSubstrateSpecificationObject*>()){
		CCardboardSubstrateSpecificationObject* val = item.value<CCardboardSubstrateSpecificationObject*>();
		CCardboardSubstrateSpecification* newItemPtr = dynamic_cast<CCardboardSubstrateSpecification*>(val);
		append(istd::TNullableValue<SubstrateSpecificationDocument>(*newItemPtr));
	}

	if (item.canConvert<CPaperSubstrateSpecificationObject*>()){
		CPaperSubstrateSpecificationObject* val = item.value<CPaperSubstrateSpecificationObject*>();
		CPaperSubstrateSpecification* newItemPtr = dynamic_cast<CPaperSubstrateSpecification*>(val);
		append(istd::TNullableValue<SubstrateSpecificationDocument>(*newItemPtr));
	}

	if (item.canConvert<CFilmSubstrateSpecificationObject*>()){
		CFilmSubstrateSpecificationObject* val = item.value<CFilmSubstrateSpecificationObject*>();
		CFilmSubstrateSpecification* newItemPtr = dynamic_cast<CFilmSubstrateSpecification*>(val);
		append(istd::TNullableValue<SubstrateSpecificationDocument>(*newItemPtr));
	}

	ClearCache();
	endInsertRows();
}


void CSubstrateSpecificationDocumentObjectList::remove(int index)
{
	BaseClass::remove(index);
}


void CSubstrateSpecificationDocumentObjectList::clear()
{
	BaseClass::clear();
}


QVariant CSubstrateSpecificationDocumentObjectList::getData(const QString& nameId, int index)
{
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		QVariant retVal = GetOrCreateCachedObject(index);
		return retVal;
	}
	return QVariant();
}


QByteArray CGetSubstrateSpecificationListGqlRequest::GetCommandId()
{
	return QByteArrayLiteral("GetSubstrateSpecificationList");
}


bool CGetSubstrateSpecificationListGqlRequest::SetupGqlRequest(::imtgql::CGqlRequest& gqlRequest, const GetSubstrateSpecificationListRequestArguments& requestArguments, const GetSubstrateSpecificationListRequestInfo& /*requestInfo*/)
{
	gqlRequest.SetCommandId(GetCommandId());

	// writting input arguments
	::imtgql::CGqlParamObject inputDataObject;
	if (!requestArguments.input->WriteToGraphQlObject(inputDataObject)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to create GQL Object").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

		return false;
	}
	gqlRequest.AddParam("input", inputDataObject);

	return true;
}


CGetSubstrateSpecificationListGqlRequest::CGetSubstrateSpecificationListGqlRequest(const ::imtgql::CGqlRequest& gqlRequest, bool optRead)
	: m_isValid(true)
{
	m_gqlContextPtr = gqlRequest.GetRequestContext();

	const QByteArray protocolVersion = gqlRequest.GetProtocolVersion();

	// reading input arguments
	const ::imtgql::CGqlParamObject* inputDataObjectPtr = gqlRequest.GetParamObject("input");
	if (inputDataObjectPtr != nullptr){
		if (!protocolVersion.isEmpty()){
			if (protocolVersion == "1.0"){
				bool isInputRead;
				if (optRead){
					isInputRead = m_requestedArguments.input.emplace().OptReadFromGraphQlObject(*inputDataObjectPtr, CSubstrateSpecificationListInput::PV_1_0);
				}
				else {
					isInputRead = m_requestedArguments.input.emplace().ReadFromGraphQlObject(*inputDataObjectPtr, CSubstrateSpecificationListInput::PV_1_0);
				}
				m_isValid = isInputRead;
				if (!isInputRead){
					return;
				}
			}
			else {
				qWarning() << QString("Bad request. Version %1 is not supported").arg(qPrintable(protocolVersion));
				m_isValid = false;

				return;
			}
		}
		else {
			bool isInputRead;
			if (optRead){
				isInputRead = m_requestedArguments.input.emplace().OptReadFromGraphQlObject(*inputDataObjectPtr);
			}
			else {
				isInputRead = m_requestedArguments.input.emplace().ReadFromGraphQlObject(*inputDataObjectPtr);
			}
			m_isValid = isInputRead;
			if (!isInputRead){
				return;
			}
		}
	}

	// reading requested fields
	const imtgql::CGqlFieldObject* requestedFieldsObjectPtr = nullptr;
	if (!gqlRequest.GetFields().GetFieldIds().isEmpty()){
		requestedFieldsObjectPtr = gqlRequest.GetFields().GetFieldArgumentObjectPtr(gqlRequest.GetFields().GetFieldIds().constFirst());
		if (requestedFieldsObjectPtr != nullptr){
			const QByteArrayList requestedIds = requestedFieldsObjectPtr->GetFieldIds();
			if (!requestedIds.isEmpty()){
				m_requestInfo.isItemsRequested = requestedIds.contains("items");
				const imtgql::CGqlFieldObject* itemsRequestedFieldsPtr = requestedFieldsObjectPtr->GetFieldArgumentObjectPtr("items");
				if (itemsRequestedFieldsPtr != nullptr){
					const QByteArrayList itemsRequestedIds = itemsRequestedFieldsPtr->GetFieldIds();
					if (!itemsRequestedIds.isEmpty()){
						m_requestInfo.items.isIdRequested = true;
						m_requestInfo.items.isTypeIdRequested = true;
						m_requestInfo.items.isNameRequested = true;
						m_requestInfo.items.isMaterialIdRequested = itemsRequestedIds.contains("materialId");
						m_requestInfo.items.isMaterialNameRequested = itemsRequestedIds.contains("materialName");
						m_requestInfo.items.isAddedRequested = itemsRequestedIds.contains("added");
						m_requestInfo.items.isTimeStampRequested = itemsRequestedIds.contains("timeStamp");
						m_requestInfo.items.isColorPreviewRequested = itemsRequestedIds.contains("colorPreview");
					}
				}
			}
		}
	}
}


bool CGetSubstrateSpecificationListGqlRequest::IsValid() const
{
	return m_isValid;
}


const ::imtgql::IGqlContext* CGetSubstrateSpecificationListGqlRequest::GetRequestContext() const
{
	return m_gqlContextPtr;
}


const GetSubstrateSpecificationListRequestArguments& CGetSubstrateSpecificationListGqlRequest::GetRequestedArguments() const
{
	return m_requestedArguments;
}


GetSubstrateSpecificationListRequestInfo CGetSubstrateSpecificationListGqlRequest::GetRequestInfo() const
{
	return m_requestInfo;
}


QByteArray CGetSubstrateSpecificationGqlRequest::GetCommandId()
{
	return QByteArrayLiteral("GetSubstrateSpecification");
}


bool CGetSubstrateSpecificationGqlRequest::SetupGqlRequest(::imtgql::CGqlRequest& gqlRequest, const GetSubstrateSpecificationRequestArguments& requestArguments, const GetSubstrateSpecificationRequestInfo& /*requestInfo*/)
{
	gqlRequest.SetCommandId(GetCommandId());

	// writting input arguments
	::imtgql::CGqlParamObject inputDataObject;
	if (!requestArguments.input->WriteToGraphQlObject(inputDataObject)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to create GQL Object").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

		return false;
	}
	gqlRequest.AddParam("input", inputDataObject);

	return true;
}


CGetSubstrateSpecificationGqlRequest::CGetSubstrateSpecificationGqlRequest(const ::imtgql::CGqlRequest& gqlRequest, bool optRead)
	: m_isValid(true)
{
	m_gqlContextPtr = gqlRequest.GetRequestContext();

	const QByteArray protocolVersion = gqlRequest.GetProtocolVersion();

	// reading input arguments
	const ::imtgql::CGqlParamObject* inputDataObjectPtr = gqlRequest.GetParamObject("input");
	if (inputDataObjectPtr != nullptr){
		if (!protocolVersion.isEmpty()){
			if (protocolVersion == "1.0"){
				bool isInputRead;
				if (optRead){
					isInputRead = m_requestedArguments.input.emplace().OptReadFromGraphQlObject(*inputDataObjectPtr, CInputId::PV_1_0);
				}
				else {
					isInputRead = m_requestedArguments.input.emplace().ReadFromGraphQlObject(*inputDataObjectPtr, CInputId::PV_1_0);
				}
				m_isValid = isInputRead;
				if (!isInputRead){
					return;
				}
			}
			else {
				qWarning() << QString("Bad request. Version %1 is not supported").arg(qPrintable(protocolVersion));
				m_isValid = false;

				return;
			}
		}
		else {
			bool isInputRead;
			if (optRead){
				isInputRead = m_requestedArguments.input.emplace().OptReadFromGraphQlObject(*inputDataObjectPtr);
			}
			else {
				isInputRead = m_requestedArguments.input.emplace().ReadFromGraphQlObject(*inputDataObjectPtr);
			}
			m_isValid = isInputRead;
			if (!isInputRead){
				return;
			}
		}
	}

	// reading requested fields
	const imtgql::CGqlFieldObject* requestedFieldsObjectPtr = nullptr;
	if (!gqlRequest.GetFields().GetFieldIds().isEmpty()){
		requestedFieldsObjectPtr = gqlRequest.GetFields().GetFieldArgumentObjectPtr(gqlRequest.GetFields().GetFieldIds().constFirst());
		if (requestedFieldsObjectPtr != nullptr){
			const QByteArrayList requestedIds = requestedFieldsObjectPtr->GetFieldIds();
		}
	}
}


bool CGetSubstrateSpecificationGqlRequest::IsValid() const
{
	return m_isValid;
}


const ::imtgql::IGqlContext* CGetSubstrateSpecificationGqlRequest::GetRequestContext() const
{
	return m_gqlContextPtr;
}


const GetSubstrateSpecificationRequestArguments& CGetSubstrateSpecificationGqlRequest::GetRequestedArguments() const
{
	return m_requestedArguments;
}


GetSubstrateSpecificationRequestInfo CGetSubstrateSpecificationGqlRequest::GetRequestInfo() const
{
	return m_requestInfo;
}


QByteArray CGetOptionsListGqlRequest::GetCommandId()
{
	return QByteArrayLiteral("GetOptionsList");
}


bool CGetOptionsListGqlRequest::SetupGqlRequest(::imtgql::CGqlRequest& gqlRequest, const GetOptionsListRequestArguments& requestArguments, const GetOptionsListRequestInfo& /*requestInfo*/)
{
	gqlRequest.SetCommandId(GetCommandId());

	// writting input arguments
	::imtgql::CGqlParamObject inputDataObject;
	if (!requestArguments.input->WriteToGraphQlObject(inputDataObject)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to create GQL Object").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

		return false;
	}
	gqlRequest.AddParam("input", inputDataObject);

	return true;
}


CGetOptionsListGqlRequest::CGetOptionsListGqlRequest(const ::imtgql::CGqlRequest& gqlRequest, bool optRead)
	: m_isValid(true)
{
	m_gqlContextPtr = gqlRequest.GetRequestContext();

	const QByteArray protocolVersion = gqlRequest.GetProtocolVersion();

	// reading input arguments
	const ::imtgql::CGqlParamObject* inputDataObjectPtr = gqlRequest.GetParamObject("input");
	if (inputDataObjectPtr != nullptr){
		if (!protocolVersion.isEmpty()){
			if (protocolVersion == "1.0"){
				bool isInputRead;
				if (optRead){
					isInputRead = m_requestedArguments.input.emplace().OptReadFromGraphQlObject(*inputDataObjectPtr, CCollectionViewParamsTest::PV_1_0);
				}
				else {
					isInputRead = m_requestedArguments.input.emplace().ReadFromGraphQlObject(*inputDataObjectPtr, CCollectionViewParamsTest::PV_1_0);
				}
				m_isValid = isInputRead;
				if (!isInputRead){
					return;
				}
			}
			else {
				qWarning() << QString("Bad request. Version %1 is not supported").arg(qPrintable(protocolVersion));
				m_isValid = false;

				return;
			}
		}
		else {
			bool isInputRead;
			if (optRead){
				isInputRead = m_requestedArguments.input.emplace().OptReadFromGraphQlObject(*inputDataObjectPtr);
			}
			else {
				isInputRead = m_requestedArguments.input.emplace().ReadFromGraphQlObject(*inputDataObjectPtr);
			}
			m_isValid = isInputRead;
			if (!isInputRead){
				return;
			}
		}
	}

	// reading requested fields
	const imtgql::CGqlFieldObject* requestedFieldsObjectPtr = nullptr;
	if (!gqlRequest.GetFields().GetFieldIds().isEmpty()){
		requestedFieldsObjectPtr = gqlRequest.GetFields().GetFieldArgumentObjectPtr(gqlRequest.GetFields().GetFieldIds().constFirst());
		if (requestedFieldsObjectPtr != nullptr){
			const QByteArrayList requestedIds = requestedFieldsObjectPtr->GetFieldIds();
			if (!requestedIds.isEmpty()){
				m_requestInfo.isTotalCountRequested = requestedIds.contains("totalCount");
				m_requestInfo.isOffsetRequested = requestedIds.contains("offset");
				m_requestInfo.isOptionsRequested = requestedIds.contains("options");
				const imtgql::CGqlFieldObject* optionsRequestedFieldsPtr = requestedFieldsObjectPtr->GetFieldArgumentObjectPtr("options");
				if (optionsRequestedFieldsPtr != nullptr){
					const QByteArrayList optionsRequestedIds = optionsRequestedFieldsPtr->GetFieldIds();
					if (!optionsRequestedIds.isEmpty()){
						m_requestInfo.options.isIdRequested = optionsRequestedIds.contains("id");
						m_requestInfo.options.isNameRequested = optionsRequestedIds.contains("name");
						m_requestInfo.options.isDescriptionRequested = optionsRequestedIds.contains("description");
						m_requestInfo.options.isEnabledRequested = optionsRequestedIds.contains("enabled");
					}
				}
			}
		}
	}
}


bool CGetOptionsListGqlRequest::IsValid() const
{
	return m_isValid;
}


const ::imtgql::IGqlContext* CGetOptionsListGqlRequest::GetRequestContext() const
{
	return m_gqlContextPtr;
}


const GetOptionsListRequestArguments& CGetOptionsListGqlRequest::GetRequestedArguments() const
{
	return m_requestedArguments;
}


GetOptionsListRequestInfo CGetOptionsListGqlRequest::GetRequestInfo() const
{
	return m_requestInfo;
}


QByteArray CInsertSubstrateSpecificationGqlRequest::GetCommandId()
{
	return QByteArrayLiteral("InsertSubstrateSpecification");
}


bool CInsertSubstrateSpecificationGqlRequest::SetupGqlRequest(::imtgql::CGqlRequest& gqlRequest, const InsertSubstrateSpecificationRequestArguments& requestArguments, const InsertSubstrateSpecificationRequestInfo& /*requestInfo*/)
{
	gqlRequest.SetCommandId(GetCommandId());

	// writting input arguments
	::imtgql::CGqlParamObject inputDataObject;
	if (!requestArguments.input->WriteToGraphQlObject(inputDataObject)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to create GQL Object").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

		return false;
	}
	gqlRequest.AddParam("input", inputDataObject);

	return true;
}


CInsertSubstrateSpecificationGqlRequest::CInsertSubstrateSpecificationGqlRequest(const ::imtgql::CGqlRequest& gqlRequest, bool optRead)
	: m_isValid(true)
{
	m_gqlContextPtr = gqlRequest.GetRequestContext();

	const QByteArray protocolVersion = gqlRequest.GetProtocolVersion();

	// reading input arguments
	const ::imtgql::CGqlParamObject* inputDataObjectPtr = gqlRequest.GetParamObject("input");
	if (inputDataObjectPtr != nullptr){
		if (!protocolVersion.isEmpty()){
			if (protocolVersion == "1.0"){
				bool isInputRead;
				if (optRead){
					isInputRead = m_requestedArguments.input.emplace().OptReadFromGraphQlObject(*inputDataObjectPtr, CSubstrateSpecificationInput::PV_1_0);
				}
				else {
					isInputRead = m_requestedArguments.input.emplace().ReadFromGraphQlObject(*inputDataObjectPtr, CSubstrateSpecificationInput::PV_1_0);
				}
				m_isValid = isInputRead;
				if (!isInputRead){
					return;
				}
			}
			else {
				qWarning() << QString("Bad request. Version %1 is not supported").arg(qPrintable(protocolVersion));
				m_isValid = false;

				return;
			}
		}
		else {
			bool isInputRead;
			if (optRead){
				isInputRead = m_requestedArguments.input.emplace().OptReadFromGraphQlObject(*inputDataObjectPtr);
			}
			else {
				isInputRead = m_requestedArguments.input.emplace().ReadFromGraphQlObject(*inputDataObjectPtr);
			}
			m_isValid = isInputRead;
			if (!isInputRead){
				return;
			}
		}
	}

	// reading requested fields
	const imtgql::CGqlFieldObject* requestedFieldsObjectPtr = nullptr;
	if (!gqlRequest.GetFields().GetFieldIds().isEmpty()){
		requestedFieldsObjectPtr = gqlRequest.GetFields().GetFieldArgumentObjectPtr(gqlRequest.GetFields().GetFieldIds().constFirst());
		if (requestedFieldsObjectPtr != nullptr){
			const QByteArrayList requestedIds = requestedFieldsObjectPtr->GetFieldIds();
			if (!requestedIds.isEmpty()){
				m_requestInfo.isIdRequested = true;
			}
		}
	}
}


bool CInsertSubstrateSpecificationGqlRequest::IsValid() const
{
	return m_isValid;
}


const ::imtgql::IGqlContext* CInsertSubstrateSpecificationGqlRequest::GetRequestContext() const
{
	return m_gqlContextPtr;
}


const InsertSubstrateSpecificationRequestArguments& CInsertSubstrateSpecificationGqlRequest::GetRequestedArguments() const
{
	return m_requestedArguments;
}


InsertSubstrateSpecificationRequestInfo CInsertSubstrateSpecificationGqlRequest::GetRequestInfo() const
{
	return m_requestInfo;
}


QByteArray CUpdateSubstrateSpecificationGqlRequest::GetCommandId()
{
	return QByteArrayLiteral("UpdateSubstrateSpecification");
}


bool CUpdateSubstrateSpecificationGqlRequest::SetupGqlRequest(::imtgql::CGqlRequest& gqlRequest, const UpdateSubstrateSpecificationRequestArguments& requestArguments, const UpdateSubstrateSpecificationRequestInfo& /*requestInfo*/)
{
	gqlRequest.SetCommandId(GetCommandId());

	// writting input arguments
	::imtgql::CGqlParamObject inputDataObject;
	if (!requestArguments.input->WriteToGraphQlObject(inputDataObject)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to create GQL Object").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

		return false;
	}
	gqlRequest.AddParam("input", inputDataObject);

	return true;
}


CUpdateSubstrateSpecificationGqlRequest::CUpdateSubstrateSpecificationGqlRequest(const ::imtgql::CGqlRequest& gqlRequest, bool optRead)
	: m_isValid(true)
{
	m_gqlContextPtr = gqlRequest.GetRequestContext();

	const QByteArray protocolVersion = gqlRequest.GetProtocolVersion();

	// reading input arguments
	const ::imtgql::CGqlParamObject* inputDataObjectPtr = gqlRequest.GetParamObject("input");
	if (inputDataObjectPtr != nullptr){
		if (!protocolVersion.isEmpty()){
			if (protocolVersion == "1.0"){
				bool isInputRead;
				if (optRead){
					isInputRead = m_requestedArguments.input.emplace().OptReadFromGraphQlObject(*inputDataObjectPtr, CSubstrateSpecificationInput::PV_1_0);
				}
				else {
					isInputRead = m_requestedArguments.input.emplace().ReadFromGraphQlObject(*inputDataObjectPtr, CSubstrateSpecificationInput::PV_1_0);
				}
				m_isValid = isInputRead;
				if (!isInputRead){
					return;
				}
			}
			else {
				qWarning() << QString("Bad request. Version %1 is not supported").arg(qPrintable(protocolVersion));
				m_isValid = false;

				return;
			}
		}
		else {
			bool isInputRead;
			if (optRead){
				isInputRead = m_requestedArguments.input.emplace().OptReadFromGraphQlObject(*inputDataObjectPtr);
			}
			else {
				isInputRead = m_requestedArguments.input.emplace().ReadFromGraphQlObject(*inputDataObjectPtr);
			}
			m_isValid = isInputRead;
			if (!isInputRead){
				return;
			}
		}
	}

	// reading requested fields
	const imtgql::CGqlFieldObject* requestedFieldsObjectPtr = nullptr;
	if (!gqlRequest.GetFields().GetFieldIds().isEmpty()){
		requestedFieldsObjectPtr = gqlRequest.GetFields().GetFieldArgumentObjectPtr(gqlRequest.GetFields().GetFieldIds().constFirst());
		if (requestedFieldsObjectPtr != nullptr){
			const QByteArrayList requestedIds = requestedFieldsObjectPtr->GetFieldIds();
			if (!requestedIds.isEmpty()){
				m_requestInfo.isIdRequested = true;
			}
		}
	}
}


bool CUpdateSubstrateSpecificationGqlRequest::IsValid() const
{
	return m_isValid;
}


const ::imtgql::IGqlContext* CUpdateSubstrateSpecificationGqlRequest::GetRequestContext() const
{
	return m_gqlContextPtr;
}


const UpdateSubstrateSpecificationRequestArguments& CUpdateSubstrateSpecificationGqlRequest::GetRequestedArguments() const
{
	return m_requestedArguments;
}


UpdateSubstrateSpecificationRequestInfo CUpdateSubstrateSpecificationGqlRequest::GetRequestInfo() const
{
	return m_requestInfo;
}
QMap<int, QByteArray> CSubstrateSpecificationCollectionControllerCompBase::GetSupportedCommandIds() const
{
	static QMap<int, QByteArray> retVal = {
		std::make_pair(OT_LIST, CGetSubstrateSpecificationListGqlRequest::GetCommandId()),
		std::make_pair(OT_GET, CGetSubstrateSpecificationGqlRequest::GetCommandId()),
		std::make_pair(OT_NEW, CInsertSubstrateSpecificationGqlRequest::GetCommandId()),
		std::make_pair(OT_UPDATE, CUpdateSubstrateSpecificationGqlRequest::GetCommandId()),
	};

	return retVal;
}


bool CSubstrateSpecificationCollectionControllerCompBase::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	const QByteArray commandId = gqlRequest.GetCommandId();
	if (commandId == CGetSubstrateSpecificationListGqlRequest::GetCommandId()){
		return true;
	}
	else if (commandId == CGetOptionsListGqlRequest::GetCommandId()){
		return true;
	}
	else if (commandId == CGetSubstrateSpecificationGqlRequest::GetCommandId()){
		return true;
	}
	else if (commandId == CInsertSubstrateSpecificationGqlRequest::GetCommandId()){
		return true;
	}
	else if (commandId == CUpdateSubstrateSpecificationGqlRequest::GetCommandId()){
		return true;
	}

	return BaseClass::IsRequestSupported(gqlRequest);
}


bool CSubstrateSpecificationCollectionControllerCompBase::GetOperationFromRequest(const ::imtgql::CGqlRequest& gqlRequest, ::imtgql::CGqlParamObject& gqlObject, QString& errorMessage, int& operationType) const
{
	const QByteArray commandId = gqlRequest.GetCommandId();

	if (commandId == CGetSubstrateSpecificationListGqlRequest::GetCommandId()){
		operationType =  OT_LIST;

		return true;
	}

	if (commandId == CGetOptionsListGqlRequest::GetCommandId()){
		return true;
	}

	if (commandId == CGetSubstrateSpecificationGqlRequest::GetCommandId()){
		operationType =  OT_GET;

		return true;
	}

	if (commandId == CInsertSubstrateSpecificationGqlRequest::GetCommandId()){
		operationType =  OT_NEW;

		return true;
	}

	if (commandId == CUpdateSubstrateSpecificationGqlRequest::GetCommandId()){
		operationType =  OT_UPDATE;

		return true;
	}

	return BaseClass::GetOperationFromRequest(gqlRequest, gqlObject, errorMessage, operationType);
}


bool CSubstrateSpecificationCollectionControllerCompBase::CreateRepresentationFromObject(const istd::IChangeable& data, const QByteArray& objectTypeId, const ::imtgql::CGqlRequest& gqlRequest, ::imtbase::CTreeItemModel& dataModel, QString& errorMessage) const
{
	Q_UNUSED(objectTypeId);

	const QByteArray commandId = gqlRequest.GetCommandId();

	//GetSubstrateSpecification
	if (commandId == CGetSubstrateSpecificationGqlRequest::GetCommandId()){
		CGetSubstrateSpecificationGqlRequest getSubstrateSpecificationGqlRequest(gqlRequest, false);
		SubstrateSpecificationDocument representationObject;
		const bool isRepresentationCreated = CreateRepresentationFromObject(data, getSubstrateSpecificationGqlRequest, representationObject, errorMessage);
		if (!isRepresentationCreated){
	I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to create representation").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

			return false;
		}

		if (const CCardboardSubstrateSpecification* cardboardSubstrateSpecificationVal = std::get_if<CCardboardSubstrateSpecification>(&representationObject)){
			if (!cardboardSubstrateSpecificationVal->WriteToModel(dataModel)){
				return false;
			}
			dataModel.SetData("__typename", "CardboardSubstrateSpecification", 0);
		}
		else if (const CPaperSubstrateSpecification* paperSubstrateSpecificationVal = std::get_if<CPaperSubstrateSpecification>(&representationObject)){
			if (!paperSubstrateSpecificationVal->WriteToModel(dataModel)){
				return false;
			}
			dataModel.SetData("__typename", "PaperSubstrateSpecification", 0);
		}
		else if (const CFilmSubstrateSpecification* filmSubstrateSpecificationVal = std::get_if<CFilmSubstrateSpecification>(&representationObject)){
			if (!filmSubstrateSpecificationVal->WriteToModel(dataModel)){
				return false;
			}
			dataModel.SetData("__typename", "FilmSubstrateSpecification", 0);
		}

		return true;
	}

	errorMessage = QString("Bad request. Unexpected command-ID: '%1'").arg(qPrintable(commandId));

	SendErrorMessage(0, errorMessage);

	return false;
}


istd::IChangeableUniquePtr CSubstrateSpecificationCollectionControllerCompBase::CreateObjectFromRequest(const ::imtgql::CGqlRequest& gqlRequest, QByteArray& newObjectId, QString& errorMessage) const
{
	const QByteArray commandId = gqlRequest.GetCommandId();

	//InsertSubstrateSpecification
	if (commandId == CInsertSubstrateSpecificationGqlRequest::GetCommandId()){
		CInsertSubstrateSpecificationGqlRequest insertSubstrateSpecificationGqlRequest(gqlRequest, false);
		if (!insertSubstrateSpecificationGqlRequest.IsValid()){
			errorMessage = QString("Bad request. Unexpected request for command-ID: '%1'").arg(qPrintable(commandId));
			SendErrorMessage(0, errorMessage);

			return nullptr;
		}

		return CreateObjectFromRepresentation(*insertSubstrateSpecificationGqlRequest.GetRequestedArguments().input.item, newObjectId, errorMessage);
	}

	errorMessage = QString("Bad request. Unexpected command-ID: '%1'").arg(qPrintable(commandId));
	SendErrorMessage(0, errorMessage);

	return nullptr;
}


bool CSubstrateSpecificationCollectionControllerCompBase::UpdateObjectFromRequest(const imtgql::CGqlRequest& gqlRequest, istd::IChangeable& object, QString& errorMessage) const
{
	const QByteArray commandId = gqlRequest.GetCommandId();

	//UpdateSubstrateSpecification
	if (commandId == CUpdateSubstrateSpecificationGqlRequest::GetCommandId()){
		CUpdateSubstrateSpecificationGqlRequest updateSubstrateSpecificationGqlRequest(gqlRequest, true);
		return UpdateObjectFromRepresentationRequest(gqlRequest, updateSubstrateSpecificationGqlRequest, object, errorMessage);
	}

	errorMessage = QString("Bad request. Unexpected command-ID: '%1'").arg(qPrintable(commandId));
	SendErrorMessage(0, errorMessage);

	return false;
}


::imtservergql::CObjectCollectionControllerCompBase::GqlItemSetupContext CSubstrateSpecificationCollectionControllerCompBase::CreateGqlItemSetupContext(const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	const QByteArray commandId = gqlRequest.GetCommandId();
	auto setupContext = std::make_shared<CGetSubstrateSpecificationListGqlRequest>(gqlRequest, false);
	if (!setupContext->IsValid()){
		errorMessage = QString("Bad request. Unexpected request for command-ID: '%1'").arg(qPrintable(commandId));
		SendErrorMessage(0, errorMessage);

		return {};
	}

	return setupContext;
}


bool CSubstrateSpecificationCollectionControllerCompBase::SetupGqlItemWithContext(const ::imtgql::CGqlRequest& gqlRequest, const ::imtservergql::CObjectCollectionControllerCompBase::GqlItemSetupContext& setupContext, ::imtbase::CTreeItemModel& dataModel, int itemIndex,const ::imtbase::IObjectCollectionIterator* objectCollectionIterator, QString& errorMessage) const
{
	Q_UNUSED(gqlRequest);

	if (objectCollectionIterator == nullptr){
I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to create object iterator.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

		return false;
	}

	const auto typedSetupContext = std::static_pointer_cast<const CGetSubstrateSpecificationListGqlRequest>(setupContext);
	if (!typedSetupContext){
		errorMessage = QString("Internal error. Invalid request setup context for command-ID: '%1'").arg(qPrintable(CGetSubstrateSpecificationListGqlRequest::GetCommandId()));
		SendErrorMessage(0, errorMessage);

		return false;
	}

	//GetSubstrateSpecificationList
	CSubstrateSpecificationListItem representationObject;
	const bool isRepresentationCreated = CreateRepresentationFromObject(*objectCollectionIterator, *typedSetupContext, representationObject, errorMessage);
	if (!isRepresentationCreated){
	I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to create representation").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

		return false;
	}

	const bool isRepresentationWritten = representationObject.WriteToModel(dataModel, itemIndex);
	if (!isRepresentationWritten){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to Write TreeModel").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

		return false;
	}

	return true;
}


::imtbase::CTreeItemModel* CSubstrateSpecificationCollectionControllerCompBase::CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	const QString commandId = gqlRequest.GetCommandId();
	istd::TDelPtr<::imtbase::CTreeItemModel> modelPtr(new ::imtbase::CTreeItemModel);
	::imtbase::CTreeItemModel* dataModelPtr = modelPtr->AddTreeModel("data");

	// GetOptionsList
	if (commandId == CGetOptionsListGqlRequest::GetCommandId()){
		CGetOptionsListGqlRequest getOptionsListGqlRequest(gqlRequest, false);
		if (!getOptionsListGqlRequest.IsValid()){
			errorMessage = QString("Bad request. Unexpected request for command-ID: '%1'").arg(qPrintable(commandId));
			SendErrorMessage(0, errorMessage);

			return nullptr;
		}

		sdl::V1_0::imtbase::COptionsList replyPayload = OnGetOptionsList(getOptionsListGqlRequest, errorMessage);
		if (!errorMessage.isEmpty()){
			SendErrorMessage(0, QString("The derived call [OnGetOptionsList] returned an error: %1").arg(errorMessage));

			return nullptr;
		}

		const bool isModelCreated = replyPayload.WriteToModel(*dataModelPtr);
		if (!isModelCreated){
			errorMessage = QString("Internal error. Unable to create response for command-ID: '%1'").arg(qPrintable(commandId));
			SendCriticalMessage(0, errorMessage);

			return nullptr;
		}

		return modelPtr.PopPtr();
	}

	return BaseClass::CreateInternalResponse(gqlRequest, errorMessage);
}


bool CSubstrateSpecificationsGqlHandlerCompBase::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	const QByteArray commandId = gqlRequest.GetCommandId();
	if (commandId == CGetSubstrateSpecificationListGqlRequest::GetCommandId()){
		return true;
	}
	else if (commandId == CGetSubstrateSpecificationGqlRequest::GetCommandId()){
		return true;
	}
	else if (commandId == CGetOptionsListGqlRequest::GetCommandId()){
		return true;
	}
	else if (commandId == CInsertSubstrateSpecificationGqlRequest::GetCommandId()){
		return true;
	}
	else if (commandId == CUpdateSubstrateSpecificationGqlRequest::GetCommandId()){
		return true;
	}

	return BaseClass::IsRequestSupported(gqlRequest);
}


::imtbase::CTreeItemModel* CSubstrateSpecificationsGqlHandlerCompBase::CreateInternalResponse(const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	const QByteArray commandId = gqlRequest.GetCommandId();
	istd::TDelPtr<::imtbase::CTreeItemModel> modelPtr(new ::imtbase::CTreeItemModel);
	::imtbase::CTreeItemModel* dataModelPtr = modelPtr->AddTreeModel("data");

	// GetSubstrateSpecificationList
	if (commandId == CGetSubstrateSpecificationListGqlRequest::GetCommandId()){
		CGetSubstrateSpecificationListGqlRequest getSubstrateSpecificationListGqlRequest(gqlRequest, false);
		if (!getSubstrateSpecificationListGqlRequest.IsValid()){
			errorMessage = QString("Bad request. Unexpected request for command-ID: '%1'").arg(qPrintable(commandId));
			SendErrorMessage(0, errorMessage);

			return nullptr;
		}

		CSubstrateSpecificationListResponse replyPayload = OnGetSubstrateSpecificationList(getSubstrateSpecificationListGqlRequest, gqlRequest, errorMessage);
		if (!errorMessage.isEmpty()){
			SendErrorMessage(0, QString("The derived call [OnGetSubstrateSpecificationList] returned an error: %1").arg(errorMessage));

			return nullptr;
		}

		const bool isModelCreated = replyPayload.WriteToModel(*dataModelPtr);
		if (!isModelCreated){
			errorMessage = QString("Internal error. Unable to create response for command-ID: '%1'").arg(qPrintable(commandId));
			SendCriticalMessage(0, errorMessage);

			return nullptr;
		}

		return modelPtr.PopPtr();
	}

	// GetSubstrateSpecification
	if (commandId == CGetSubstrateSpecificationGqlRequest::GetCommandId()){
		CGetSubstrateSpecificationGqlRequest getSubstrateSpecificationGqlRequest(gqlRequest, false);
		if (!getSubstrateSpecificationGqlRequest.IsValid()){
			errorMessage = QString("Bad request. Unexpected request for command-ID: '%1'").arg(qPrintable(commandId));
			SendErrorMessage(0, errorMessage);

			return nullptr;
		}

		SubstrateSpecificationDocument replyPayload = OnGetSubstrateSpecification(getSubstrateSpecificationGqlRequest, gqlRequest, errorMessage);
		if (!errorMessage.isEmpty()){
			SendErrorMessage(0, QString("The derived call [OnGetSubstrateSpecification] returned an error: %1").arg(errorMessage));

			return nullptr;
		}

		if (const CCardboardSubstrateSpecification* cardboardSubstrateSpecificationVal = std::get_if<CCardboardSubstrateSpecification>(&replyPayload)){
			if (!cardboardSubstrateSpecificationVal->WriteToModel(*dataModelPtr)){
				return nullptr;
			}
			if(dataModelPtr != nullptr){
				dataModelPtr->SetData("__typename", "CardboardSubstrateSpecification", 0);
			}
		}
		else if (const CPaperSubstrateSpecification* paperSubstrateSpecificationVal = std::get_if<CPaperSubstrateSpecification>(&replyPayload)){
			if (!paperSubstrateSpecificationVal->WriteToModel(*dataModelPtr)){
				return nullptr;
			}
			if(dataModelPtr != nullptr){
				dataModelPtr->SetData("__typename", "PaperSubstrateSpecification", 0);
			}
		}
		else if (const CFilmSubstrateSpecification* filmSubstrateSpecificationVal = std::get_if<CFilmSubstrateSpecification>(&replyPayload)){
			if (!filmSubstrateSpecificationVal->WriteToModel(*dataModelPtr)){
				return nullptr;
			}
			if(dataModelPtr != nullptr){
				dataModelPtr->SetData("__typename", "FilmSubstrateSpecification", 0);
			}
		}
		return modelPtr.PopPtr();
	}

	// GetOptionsList
	if (commandId == CGetOptionsListGqlRequest::GetCommandId()){
		CGetOptionsListGqlRequest getOptionsListGqlRequest(gqlRequest, false);
		if (!getOptionsListGqlRequest.IsValid()){
			errorMessage = QString("Bad request. Unexpected request for command-ID: '%1'").arg(qPrintable(commandId));
			SendErrorMessage(0, errorMessage);

			return nullptr;
		}

		sdl::V1_0::imtbase::COptionsList replyPayload = OnGetOptionsList(getOptionsListGqlRequest, gqlRequest, errorMessage);
		if (!errorMessage.isEmpty()){
			SendErrorMessage(0, QString("The derived call [OnGetOptionsList] returned an error: %1").arg(errorMessage));

			return nullptr;
		}

		const bool isModelCreated = replyPayload.WriteToModel(*dataModelPtr);
		if (!isModelCreated){
			errorMessage = QString("Internal error. Unable to create response for command-ID: '%1'").arg(qPrintable(commandId));
			SendCriticalMessage(0, errorMessage);

			return nullptr;
		}

		return modelPtr.PopPtr();
	}

	// InsertSubstrateSpecification
	if (commandId == CInsertSubstrateSpecificationGqlRequest::GetCommandId()){
		CInsertSubstrateSpecificationGqlRequest insertSubstrateSpecificationGqlRequest(gqlRequest, false);
		if (!insertSubstrateSpecificationGqlRequest.IsValid()){
			errorMessage = QString("Bad request. Unexpected request for command-ID: '%1'").arg(qPrintable(commandId));
			SendErrorMessage(0, errorMessage);

			return nullptr;
		}

		CInputId replyPayload = OnInsertSubstrateSpecification(insertSubstrateSpecificationGqlRequest, gqlRequest, errorMessage);
		if (!errorMessage.isEmpty()){
			SendErrorMessage(0, QString("The derived call [OnInsertSubstrateSpecification] returned an error: %1").arg(errorMessage));

			return nullptr;
		}

		const bool isModelCreated = replyPayload.WriteToModel(*dataModelPtr);
		if (!isModelCreated){
			errorMessage = QString("Internal error. Unable to create response for command-ID: '%1'").arg(qPrintable(commandId));
			SendCriticalMessage(0, errorMessage);

			return nullptr;
		}

		return modelPtr.PopPtr();
	}

	// UpdateSubstrateSpecification
	if (commandId == CUpdateSubstrateSpecificationGqlRequest::GetCommandId()){
		CUpdateSubstrateSpecificationGqlRequest updateSubstrateSpecificationGqlRequest(gqlRequest, false);
		if (!updateSubstrateSpecificationGqlRequest.IsValid()){
			errorMessage = QString("Bad request. Unexpected request for command-ID: '%1'").arg(qPrintable(commandId));
			SendErrorMessage(0, errorMessage);

			return nullptr;
		}

		CInputId replyPayload = OnUpdateSubstrateSpecification(updateSubstrateSpecificationGqlRequest, gqlRequest, errorMessage);
		if (!errorMessage.isEmpty()){
			SendErrorMessage(0, QString("The derived call [OnUpdateSubstrateSpecification] returned an error: %1").arg(errorMessage));

			return nullptr;
		}

		const bool isModelCreated = replyPayload.WriteToModel(*dataModelPtr);
		if (!isModelCreated){
			errorMessage = QString("Internal error. Unable to create response for command-ID: '%1'").arg(qPrintable(commandId));
			SendCriticalMessage(0, errorMessage);

			return nullptr;
		}

		return modelPtr.PopPtr();
	}

	errorMessage = QString("Bad request. Unexpected command-ID: '%1'").arg(qPrintable(commandId));
	SendErrorMessage(0, errorMessage);

	return nullptr;
}


} // namespace sdl::V1_0::V1_0
