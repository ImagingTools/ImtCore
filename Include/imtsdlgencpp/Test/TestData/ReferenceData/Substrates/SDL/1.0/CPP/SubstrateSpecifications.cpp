#include "SubstrateSpecifications.h"


namespace sdl::substrate::SubstrateSpecifications
{


QByteArray CInputId::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CInputId::V1_0::operator==(const V1_0& other) const
{
	return 
				id == other.id;
}


bool CInputId::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "InputId", modelIndex);

	if (!id){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "id").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("id", *id, modelIndex);


	return true;
}


bool CInputId::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant idData = model.GetData("id", modelIndex);
	if (idData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "id").toLocal8Bit().constData();)

		return false;
	}
	id = idData.toByteArray();

	return true;
}


bool CInputId::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant idData = model.GetData("id", modelIndex);
	if (!idData.isNull()){
		id = idData.toByteArray();
	}

	return true;
}


bool CInputId::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!id){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "id").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("id", QVariant(*id));

	gqlObject.InsertParam("__typename", QVariant("InputId"));

	return true;
}


bool CInputId::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("id") || (gqlObject["id"].userType() != QMetaType::QString && gqlObject["id"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "id").toLocal8Bit().constData();)

		return false;
	}
	id = gqlObject["id"].toByteArray();

	return true;
}


bool CInputId::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("id") && (gqlObject["id"].userType() == QMetaType::QString || gqlObject["id"].userType() == QMetaType::QByteArray)){
		id = gqlObject["id"].toByteArray();
	}

	return true;
}


bool CInputId::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!id){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "id").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["id"] = QJsonValue::fromVariant(*id);

	jsonObject["__typename"] = "InputId";

	return true;
}


bool CInputId::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("id") || ! jsonObject["id"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "id").toLocal8Bit().constData();)

		return false;
	}
	id = jsonObject["id"].toString().toUtf8();

	return true;
}


bool CInputId::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("id") && jsonObject["id"].isString()){
		id = jsonObject["id"].toString().toUtf8();
	}

	return true;
}


// serialize methods

bool CInputId::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CInputId::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CInputId::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CInputId::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CInputId::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CInputId::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CInputId::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CInputId::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CInputId::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}




QByteArray CCollectionViewParamsTest::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CCollectionViewParamsTest::V1_0::operator==(const V1_0& other) const
{
	return 
				count == other.count &&
				offset == other.offset;
}


bool CCollectionViewParamsTest::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
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


bool CCollectionViewParamsTest::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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


bool CCollectionViewParamsTest::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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


bool CCollectionViewParamsTest::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
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


bool CCollectionViewParamsTest::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("count") && (gqlObject["count"].userType() == QMetaType::Int || gqlObject["count"].userType() == QMetaType::UInt || gqlObject["count"].userType() == QMetaType::LongLong || gqlObject["count"].userType() == QMetaType::ULongLong || gqlObject["count"].userType() == QMetaType::Long || gqlObject["count"].userType() == QMetaType::Short || gqlObject["count"].userType() == QMetaType::ULong || gqlObject["count"].userType() == QMetaType::UShort || gqlObject["count"].userType() == QMetaType::UChar)){
		count = gqlObject["count"].toInt();
	}

	if (gqlObject.ContainsParam("offset") && (gqlObject["offset"].userType() == QMetaType::Int || gqlObject["offset"].userType() == QMetaType::UInt || gqlObject["offset"].userType() == QMetaType::LongLong || gqlObject["offset"].userType() == QMetaType::ULongLong || gqlObject["offset"].userType() == QMetaType::Long || gqlObject["offset"].userType() == QMetaType::Short || gqlObject["offset"].userType() == QMetaType::ULong || gqlObject["offset"].userType() == QMetaType::UShort || gqlObject["offset"].userType() == QMetaType::UChar)){
		offset = gqlObject["offset"].toInt();
	}

	return true;
}


bool CCollectionViewParamsTest::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("count") && (gqlObject["count"].userType() == QMetaType::Int || gqlObject["count"].userType() == QMetaType::UInt || gqlObject["count"].userType() == QMetaType::LongLong || gqlObject["count"].userType() == QMetaType::ULongLong || gqlObject["count"].userType() == QMetaType::Long || gqlObject["count"].userType() == QMetaType::Short || gqlObject["count"].userType() == QMetaType::ULong || gqlObject["count"].userType() == QMetaType::UShort || gqlObject["count"].userType() == QMetaType::UChar)){
		count = gqlObject["count"].toInt();
	}

	if (gqlObject.ContainsParam("offset") && (gqlObject["offset"].userType() == QMetaType::Int || gqlObject["offset"].userType() == QMetaType::UInt || gqlObject["offset"].userType() == QMetaType::LongLong || gqlObject["offset"].userType() == QMetaType::ULongLong || gqlObject["offset"].userType() == QMetaType::Long || gqlObject["offset"].userType() == QMetaType::Short || gqlObject["offset"].userType() == QMetaType::ULong || gqlObject["offset"].userType() == QMetaType::UShort || gqlObject["offset"].userType() == QMetaType::UChar)){
		offset = gqlObject["offset"].toInt();
	}

	return true;
}


bool CCollectionViewParamsTest::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
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


bool CCollectionViewParamsTest::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("count") && jsonObject["count"].isDouble()){
		count = jsonObject["count"].toInt();
	}

	if (jsonObject.contains("offset") && jsonObject["offset"].isDouble()){
		offset = jsonObject["offset"].toInt();
	}

	return true;
}


bool CCollectionViewParamsTest::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
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

bool CCollectionViewParamsTest::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CCollectionViewParamsTest::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CCollectionViewParamsTest::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CCollectionViewParamsTest::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CCollectionViewParamsTest::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CCollectionViewParamsTest::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CCollectionViewParamsTest::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CCollectionViewParamsTest::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CCollectionViewParamsTest::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}




QByteArray CSubstrateSpecificationDocumentTypeIds::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CSubstrateSpecificationDocumentTypeIds::V1_0::operator==(const V1_0& other) const
{
	return 
				PaperID == other.PaperID &&
				CardboardID == other.CardboardID &&
				FilmID == other.FilmID;
}


bool CSubstrateSpecificationDocumentTypeIds::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
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


bool CSubstrateSpecificationDocumentTypeIds::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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


bool CSubstrateSpecificationDocumentTypeIds::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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


bool CSubstrateSpecificationDocumentTypeIds::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
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


bool CSubstrateSpecificationDocumentTypeIds::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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


bool CSubstrateSpecificationDocumentTypeIds::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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


bool CSubstrateSpecificationDocumentTypeIds::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (PaperID){
		jsonObject["PaperID"] = QJsonValue::fromVariant(*PaperID);
	}

	if (CardboardID){
		jsonObject["CardboardID"] = QJsonValue::fromVariant(*CardboardID);
	}

	if (FilmID){
		jsonObject["FilmID"] = QJsonValue::fromVariant(*FilmID);
	}

	jsonObject["__typename"] = "SubstrateSpecificationDocumentTypeIds";

	return true;
}


bool CSubstrateSpecificationDocumentTypeIds::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
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


bool CSubstrateSpecificationDocumentTypeIds::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
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

bool CSubstrateSpecificationDocumentTypeIds::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CSubstrateSpecificationDocumentTypeIds::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CSubstrateSpecificationDocumentTypeIds::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CSubstrateSpecificationDocumentTypeIds::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CSubstrateSpecificationDocumentTypeIds::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CSubstrateSpecificationDocumentTypeIds::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CSubstrateSpecificationDocumentTypeIds::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CSubstrateSpecificationDocumentTypeIds::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CSubstrateSpecificationDocumentTypeIds::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}




QByteArray CSubstrateSpecificationListInput::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CSubstrateSpecificationListInput::V1_0::operator==(const V1_0& other) const
{
	return 
				viewParams == other.viewParams;
}


bool CSubstrateSpecificationListInput::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
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


bool CSubstrateSpecificationListInput::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	::imtbase::CTreeItemModel* viewParamsDataModelPtr = model.GetTreeItemModel("viewParams", modelIndex);
	if (viewParamsDataModelPtr != nullptr){
		viewParams = CCollectionViewParamsTest::V1_0();
		const bool isViewParamsRead = viewParams->ReadFromModel(*viewParamsDataModelPtr, modelIndex);
		if (!isViewParamsRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "viewParams").toLocal8Bit().constData();)

			return false;
		}
	}

	return true;
}


bool CSubstrateSpecificationListInput::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	::imtbase::CTreeItemModel* viewParamsDataModelPtr = model.GetTreeItemModel("viewParams", modelIndex);
	if (viewParamsDataModelPtr != nullptr){
		viewParams = CCollectionViewParamsTest::V1_0();
		const bool isViewParamsRead = viewParams->ReadFromModel(*viewParamsDataModelPtr, modelIndex);
		if (!isViewParamsRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "viewParams").toLocal8Bit().constData();)

			return false;
		}
	}

	return true;
}


bool CSubstrateSpecificationListInput::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
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


bool CSubstrateSpecificationListInput::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("viewParams") && (gqlObject.GetParamArgumentObjectPtr("viewParams") != nullptr)){
		viewParams = CCollectionViewParamsTest::V1_0();
		const bool isViewParamsRead = viewParams->ReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("viewParams"));
		if (!isViewParamsRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "viewParams").toLocal8Bit().constData();)

			return false;
		}
	}

	return true;
}


bool CSubstrateSpecificationListInput::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("viewParams") && (gqlObject.GetParamArgumentObjectPtr("viewParams") != nullptr)){
		viewParams = CCollectionViewParamsTest::V1_0();
		const bool isViewParamsRead = viewParams->OptReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("viewParams"));
		if (!isViewParamsRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "viewParams").toLocal8Bit().constData();)

			return false;
		}
	}

	return true;
}


bool CSubstrateSpecificationListInput::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
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


bool CSubstrateSpecificationListInput::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("viewParams") && jsonObject["viewParams"].isObject()){
		viewParams = CCollectionViewParamsTest::V1_0();
		const bool isViewParamsRead = viewParams->ReadFromJsonObject(jsonObject["viewParams"].toObject());
		if (!isViewParamsRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "viewParams").toLocal8Bit().constData();)

			return false;
		}
	}

	return true;
}


bool CSubstrateSpecificationListInput::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("viewParams") && jsonObject["viewParams"].isObject()){
		viewParams = CCollectionViewParamsTest::V1_0();
		const bool isViewParamsRead = viewParams->OptReadFromJsonObject(jsonObject["viewParams"].toObject());
		if (!isViewParamsRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "viewParams").toLocal8Bit().constData();)

			return false;
		}
	}

	return true;
}


// serialize methods

bool CSubstrateSpecificationListInput::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CSubstrateSpecificationListInput::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CSubstrateSpecificationListInput::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CSubstrateSpecificationListInput::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CSubstrateSpecificationListInput::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CSubstrateSpecificationListInput::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CSubstrateSpecificationListInput::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CSubstrateSpecificationListInput::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CSubstrateSpecificationListInput::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}




QByteArray CSubstrateSpecificationListItem::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CSubstrateSpecificationListItem::V1_0::operator==(const V1_0& other) const
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


bool CSubstrateSpecificationListItem::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
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


bool CSubstrateSpecificationListItem::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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


bool CSubstrateSpecificationListItem::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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


bool CSubstrateSpecificationListItem::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
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


bool CSubstrateSpecificationListItem::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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


bool CSubstrateSpecificationListItem::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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


bool CSubstrateSpecificationListItem::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!id){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "id").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["id"] = QJsonValue::fromVariant(*id);

	if (!typeId){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "typeId").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["typeId"] = QJsonValue::fromVariant(*typeId);

	if (!name){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "name").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["name"] = QJsonValue::fromVariant(*name);

	if (materialId){
		jsonObject["materialId"] = QJsonValue::fromVariant(*materialId);
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


bool CSubstrateSpecificationListItem::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
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


bool CSubstrateSpecificationListItem::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
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

bool CSubstrateSpecificationListItem::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CSubstrateSpecificationListItem::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CSubstrateSpecificationListItem::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CSubstrateSpecificationListItem::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CSubstrateSpecificationListItem::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CSubstrateSpecificationListItem::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CSubstrateSpecificationListItem::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CSubstrateSpecificationListItem::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CSubstrateSpecificationListItem::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}




QByteArray CSubstrateSpecificationListResponse::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CSubstrateSpecificationListResponse::V1_0::operator==(const V1_0& other) const
{
	return 
				items == other.items;
}


bool CSubstrateSpecificationListResponse::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
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


bool CSubstrateSpecificationListResponse::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	::imtbase::CTreeItemModel* itemsModel = model.GetTreeItemModel("items", modelIndex);
	if (itemsModel != nullptr){
		int itemsCount = itemsModel->GetItemsCount();
		imtsdl::TElementList<CSubstrateSpecificationListItem::V1_0> itemsList;
		for (int itemsIndex = 0; itemsIndex < itemsCount; ++itemsIndex){
			CSubstrateSpecificationListItem::V1_0 t_items;
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


bool CSubstrateSpecificationListResponse::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	::imtbase::CTreeItemModel* itemsModel = model.GetTreeItemModel("items", modelIndex);
	if (itemsModel != nullptr){
		int itemsCount = itemsModel->GetItemsCount();
		imtsdl::TElementList<CSubstrateSpecificationListItem::V1_0> itemsList;
		for (int itemsIndex = 0; itemsIndex < itemsCount; ++itemsIndex){
			CSubstrateSpecificationListItem::V1_0 t_items;
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


bool CSubstrateSpecificationListResponse::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
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


bool CSubstrateSpecificationListResponse::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("items")){
		items.emplace();
	}
	if (gqlObject.ContainsParam("items") && (gqlObject.GetObjectsCount("items") > 0)){
		const qsizetype itemsElementsCount = gqlObject.GetObjectsCount("items");
		items = imtsdl::TElementList<CSubstrateSpecificationListItem::V1_0>();
		for (qsizetype itemsIndex = 0; itemsIndex < itemsElementsCount; ++itemsIndex){
			const ::imtgql::CGqlParamObject* itemsDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("items", itemsIndex);
			if (itemsDataObjectPtr == nullptr){
				qDebug() << "invalid type" << itemsDataObjectPtr;
				return false;
			}
			CSubstrateSpecificationListItem::V1_0 tempItems;
			if (!tempItems.ReadFromGraphQlObject(*itemsDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "items").toLocal8Bit().constData();)

				return false;
			}
			items->append(tempItems);
		}
	}

	return true;
}


bool CSubstrateSpecificationListResponse::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("items")){
		items.emplace();
	}
	if (gqlObject.ContainsParam("items") && (gqlObject.GetObjectsCount("items") > 0)){
		const qsizetype itemsElementsCount = gqlObject.GetObjectsCount("items");
		items = imtsdl::TElementList<CSubstrateSpecificationListItem::V1_0>();
		for (qsizetype itemsIndex = 0; itemsIndex < itemsElementsCount; ++itemsIndex){
			const ::imtgql::CGqlParamObject* itemsDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("items", itemsIndex);
			if (itemsDataObjectPtr == nullptr){
				qDebug() << "invalid type" << itemsDataObjectPtr;
				return false;
			}
			CSubstrateSpecificationListItem::V1_0 tempItems;
			if (!tempItems.OptReadFromGraphQlObject(*itemsDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "items").toLocal8Bit().constData();)

				return false;
			}
			items->append(tempItems);
		}
	}

	return true;
}


bool CSubstrateSpecificationListResponse::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
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


bool CSubstrateSpecificationListResponse::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("items") && jsonObject["items"].isArray()){
		const QJsonArray itemsJsonArray = jsonObject["items"].toArray();
		const qsizetype itemsArrayCount = itemsJsonArray.size();
		items = imtsdl::TElementList<CSubstrateSpecificationListItem::V1_0>();
		for (qsizetype itemsIndex = 0; itemsIndex < itemsArrayCount; ++itemsIndex){
			CSubstrateSpecificationListItem::V1_0 tempItems;
			if (!tempItems.ReadFromJsonObject(itemsJsonArray[itemsIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "items").toLocal8Bit().constData();)

				return false;
			}
			items->append(tempItems);
		}
	}

	return true;
}


bool CSubstrateSpecificationListResponse::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("items") && jsonObject["items"].isArray()){
		const QJsonArray itemsJsonArray = jsonObject["items"].toArray();
		const qsizetype itemsArrayCount = itemsJsonArray.size();
		items = imtsdl::TElementList<CSubstrateSpecificationListItem::V1_0>();
		for (qsizetype itemsIndex = 0; itemsIndex < itemsArrayCount; ++itemsIndex){
			CSubstrateSpecificationListItem::V1_0 tempItems;
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

bool CSubstrateSpecificationListResponse::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CSubstrateSpecificationListResponse::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CSubstrateSpecificationListResponse::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CSubstrateSpecificationListResponse::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CSubstrateSpecificationListResponse::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CSubstrateSpecificationListResponse::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CSubstrateSpecificationListResponse::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CSubstrateSpecificationListResponse::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CSubstrateSpecificationListResponse::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}




QByteArray CCardboardSubstrateSpecification::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CCardboardSubstrateSpecification::V1_0::operator==(const V1_0& other) const
{
	return 
				color == other.color &&
				grammage.has_value() == other.grammage.has_value() &&
				((grammage.has_value() && other.grammage.has_value()) ?
					qFuzzyCompare(*grammage, *other.grammage) : true);
}


bool CCardboardSubstrateSpecification::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
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


bool CCardboardSubstrateSpecification::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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


bool CCardboardSubstrateSpecification::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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


bool CCardboardSubstrateSpecification::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
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


bool CCardboardSubstrateSpecification::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("color") && (gqlObject["color"].userType() == QMetaType::QString || gqlObject["color"].userType() == QMetaType::QByteArray)){
		color = gqlObject["color"].toString();
	}

	if (gqlObject.ContainsParam("grammage") && (gqlObject["grammage"].userType() == QMetaType::Float || gqlObject["grammage"].userType() == QMetaType::Double || gqlObject["grammage"].userType() == QMetaType::Int || gqlObject["grammage"].userType() == QMetaType::UInt || gqlObject["grammage"].userType() == QMetaType::LongLong || gqlObject["grammage"].userType() == QMetaType::ULongLong || gqlObject["grammage"].userType() == QMetaType::Long || gqlObject["grammage"].userType() == QMetaType::Short || gqlObject["grammage"].userType() == QMetaType::ULong || gqlObject["grammage"].userType() == QMetaType::UShort || gqlObject["grammage"].userType() == QMetaType::UChar)){
		grammage = gqlObject["grammage"].toDouble();
	}

	return true;
}


bool CCardboardSubstrateSpecification::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("color") && (gqlObject["color"].userType() == QMetaType::QString || gqlObject["color"].userType() == QMetaType::QByteArray)){
		color = gqlObject["color"].toString();
	}

	if (gqlObject.ContainsParam("grammage") && (gqlObject["grammage"].userType() == QMetaType::Float || gqlObject["grammage"].userType() == QMetaType::Double || gqlObject["grammage"].userType() == QMetaType::Int || gqlObject["grammage"].userType() == QMetaType::UInt || gqlObject["grammage"].userType() == QMetaType::LongLong || gqlObject["grammage"].userType() == QMetaType::ULongLong || gqlObject["grammage"].userType() == QMetaType::Long || gqlObject["grammage"].userType() == QMetaType::Short || gqlObject["grammage"].userType() == QMetaType::ULong || gqlObject["grammage"].userType() == QMetaType::UShort || gqlObject["grammage"].userType() == QMetaType::UChar)){
		grammage = gqlObject["grammage"].toDouble();
	}

	return true;
}


bool CCardboardSubstrateSpecification::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
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


bool CCardboardSubstrateSpecification::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("color") && jsonObject["color"].isString()){
		color = jsonObject["color"].toString();
	}

	if (jsonObject.contains("grammage") && jsonObject["grammage"].isDouble()){
		grammage = jsonObject["grammage"].toDouble();
	}

	return true;
}


bool CCardboardSubstrateSpecification::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
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

bool CCardboardSubstrateSpecification::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CCardboardSubstrateSpecification::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CCardboardSubstrateSpecification::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CCardboardSubstrateSpecification::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CCardboardSubstrateSpecification::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CCardboardSubstrateSpecification::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CCardboardSubstrateSpecification::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CCardboardSubstrateSpecification::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CCardboardSubstrateSpecification::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}




QByteArray CPaperSubstrateSpecification::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CPaperSubstrateSpecification::V1_0::operator==(const V1_0& other) const
{
	return 
				grammage.has_value() == other.grammage.has_value() &&
				((grammage.has_value() && other.grammage.has_value()) ?
					qFuzzyCompare(*grammage, *other.grammage) : true) &&
				color == other.color &&
				coatType == other.coatType;
}


bool CPaperSubstrateSpecification::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
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


bool CPaperSubstrateSpecification::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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


bool CPaperSubstrateSpecification::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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


bool CPaperSubstrateSpecification::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
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


bool CPaperSubstrateSpecification::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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


bool CPaperSubstrateSpecification::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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


bool CPaperSubstrateSpecification::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
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


bool CPaperSubstrateSpecification::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
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


bool CPaperSubstrateSpecification::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
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

bool CPaperSubstrateSpecification::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CPaperSubstrateSpecification::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CPaperSubstrateSpecification::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CPaperSubstrateSpecification::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CPaperSubstrateSpecification::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CPaperSubstrateSpecification::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CPaperSubstrateSpecification::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CPaperSubstrateSpecification::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CPaperSubstrateSpecification::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}




QByteArray CFilmSubstrateSpecification::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CFilmSubstrateSpecification::V1_0::operator==(const V1_0& other) const
{
	return 
				transparency.has_value() == other.transparency.has_value() &&
				((transparency.has_value() && other.transparency.has_value()) ?
					qFuzzyCompare(*transparency, *other.transparency) : true) &&
				color == other.color &&
				material == other.material;
}


bool CFilmSubstrateSpecification::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
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


bool CFilmSubstrateSpecification::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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


bool CFilmSubstrateSpecification::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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


bool CFilmSubstrateSpecification::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
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


bool CFilmSubstrateSpecification::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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


bool CFilmSubstrateSpecification::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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


bool CFilmSubstrateSpecification::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
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


bool CFilmSubstrateSpecification::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
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


bool CFilmSubstrateSpecification::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
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

bool CFilmSubstrateSpecification::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CFilmSubstrateSpecification::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CFilmSubstrateSpecification::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CFilmSubstrateSpecification::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CFilmSubstrateSpecification::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CFilmSubstrateSpecification::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CFilmSubstrateSpecification::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CFilmSubstrateSpecification::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CFilmSubstrateSpecification::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}




QByteArray CSubstrateSpecificationInput::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CSubstrateSpecificationInput::V1_0::operator==(const V1_0& other) const
{
	return 
				id == other.id &&
				name == other.name &&
				description == other.description &&
				item == other.item &&
				typeId == other.typeId;
}


bool CSubstrateSpecificationInput::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
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
		if (const CCardboardSubstrateSpecification* val = std::get_if<CCardboardSubstrateSpecification>(item.GetPtr())){
			if (!val->WriteToModel(*(model.AddTreeModel("item", modelIndex)), 0)){
				return false;
			}
			if(model.GetTreeItemModel("item", modelIndex) != nullptr){
				model.GetTreeItemModel("item", modelIndex)->SetData("__typename", "CardboardSubstrateSpecification", 0);
			}
		}
		else if (const CPaperSubstrateSpecification* val = std::get_if<CPaperSubstrateSpecification>(item.GetPtr())){
			if (!val->WriteToModel(*(model.AddTreeModel("item", modelIndex)), 0)){
				return false;
			}
			if(model.GetTreeItemModel("item", modelIndex) != nullptr){
				model.GetTreeItemModel("item", modelIndex)->SetData("__typename", "PaperSubstrateSpecification", 0);
			}
		}
		else if (const CFilmSubstrateSpecification* val = std::get_if<CFilmSubstrateSpecification>(item.GetPtr())){
			if (!val->WriteToModel(*(model.AddTreeModel("item", modelIndex)), 0)){
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


bool CSubstrateSpecificationInput::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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
	QString itemTypename = itemData.value<::imtbase::CTreeItemModel*>()->GetData("__typename").toString();
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


bool CSubstrateSpecificationInput::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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
		QString itemTypename = itemData.value<::imtbase::CTreeItemModel*>()->GetData("__typename").toString();
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


bool CSubstrateSpecificationInput::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
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
	if (const CCardboardSubstrateSpecification* val = std::get_if<CCardboardSubstrateSpecification>(item.GetPtr())){
		if (!val->WriteToGraphQlObject(itemDataObject)){
			return false;
		}
		itemDataObject.InsertParam("__typename", QVariant("CardboardSubstrateSpecification"));
	}
	else if (const CPaperSubstrateSpecification* val = std::get_if<CPaperSubstrateSpecification>(item.GetPtr())){
		if (!val->WriteToGraphQlObject(itemDataObject)){
			return false;
		}
		itemDataObject.InsertParam("__typename", QVariant("PaperSubstrateSpecification"));
	}
	else if (const CFilmSubstrateSpecification* val = std::get_if<CFilmSubstrateSpecification>(item.GetPtr())){
		if (!val->WriteToGraphQlObject(itemDataObject)){
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


bool CSubstrateSpecificationInput::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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


bool CSubstrateSpecificationInput::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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


bool CSubstrateSpecificationInput::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!id){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "id").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["id"] = QJsonValue::fromVariant(*id);

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
	if (const CCardboardSubstrateSpecification* val = std::get_if<CCardboardSubstrateSpecification>(item.GetPtr())){
		QJsonObject itemJsonObject;
		const bool isitemAdded = val->WriteToJsonObject(itemJsonObject);
		if (!isitemAdded){
			return false;
		}
		itemJsonObject["__typename"] = "CardboardSubstrateSpecification";
		jsonObject["item"] = itemJsonObject;
	}
	else if (const CPaperSubstrateSpecification* val = std::get_if<CPaperSubstrateSpecification>(item.GetPtr())){
		QJsonObject itemJsonObject;
		const bool isitemAdded = val->WriteToJsonObject(itemJsonObject);
		if (!isitemAdded){
			return false;
		}
		itemJsonObject["__typename"] = "PaperSubstrateSpecification";
		jsonObject["item"] = itemJsonObject;
	}
	else if (const CFilmSubstrateSpecification* val = std::get_if<CFilmSubstrateSpecification>(item.GetPtr())){
		QJsonObject itemJsonObject;
		const bool isitemAdded = val->WriteToJsonObject(itemJsonObject);
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
	jsonObject["typeId"] = QJsonValue::fromVariant(*typeId);

	jsonObject["__typename"] = "SubstrateSpecificationInput";

	return true;
}


bool CSubstrateSpecificationInput::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
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


bool CSubstrateSpecificationInput::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
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

bool CSubstrateSpecificationInput::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CSubstrateSpecificationInput::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CSubstrateSpecificationInput::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CSubstrateSpecificationInput::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CSubstrateSpecificationInput::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CSubstrateSpecificationInput::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CSubstrateSpecificationInput::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CSubstrateSpecificationInput::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CSubstrateSpecificationInput::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}




QByteArray CSubstrateSpecificationDocumentMetaInfo::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CSubstrateSpecificationDocumentMetaInfo::V1_0::operator==(const V1_0& other) const
{
	return 
				materialId == other.materialId &&
				lab == other.lab;
}


bool CSubstrateSpecificationDocumentMetaInfo::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
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


bool CSubstrateSpecificationDocumentMetaInfo::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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


bool CSubstrateSpecificationDocumentMetaInfo::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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


bool CSubstrateSpecificationDocumentMetaInfo::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
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


bool CSubstrateSpecificationDocumentMetaInfo::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("materialId") && (gqlObject["materialId"].userType() == QMetaType::QString || gqlObject["materialId"].userType() == QMetaType::QByteArray)){
		materialId = gqlObject["materialId"].toByteArray();
	}

	if (gqlObject.ContainsParam("lab") && (gqlObject["lab"].userType() == QMetaType::QString || gqlObject["lab"].userType() == QMetaType::QByteArray)){
		lab = gqlObject["lab"].toString();
	}

	return true;
}


bool CSubstrateSpecificationDocumentMetaInfo::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("materialId") && (gqlObject["materialId"].userType() == QMetaType::QString || gqlObject["materialId"].userType() == QMetaType::QByteArray)){
		materialId = gqlObject["materialId"].toByteArray();
	}

	if (gqlObject.ContainsParam("lab") && (gqlObject["lab"].userType() == QMetaType::QString || gqlObject["lab"].userType() == QMetaType::QByteArray)){
		lab = gqlObject["lab"].toString();
	}

	return true;
}


bool CSubstrateSpecificationDocumentMetaInfo::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (materialId){
		jsonObject["materialId"] = QJsonValue::fromVariant(*materialId);
	}

	if (lab){
		jsonObject["lab"] = QJsonValue::fromVariant(*lab);
	}

	jsonObject["__typename"] = "SubstrateSpecificationDocumentMetaInfo";

	return true;
}


bool CSubstrateSpecificationDocumentMetaInfo::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("materialId") && jsonObject["materialId"].isString()){
		materialId = jsonObject["materialId"].toString().toUtf8();
	}

	if (jsonObject.contains("lab") && jsonObject["lab"].isString()){
		lab = jsonObject["lab"].toString();
	}

	return true;
}


bool CSubstrateSpecificationDocumentMetaInfo::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
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

bool CSubstrateSpecificationDocumentMetaInfo::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CSubstrateSpecificationDocumentMetaInfo::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CSubstrateSpecificationDocumentMetaInfo::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CSubstrateSpecificationDocumentMetaInfo::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CSubstrateSpecificationDocumentMetaInfo::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CSubstrateSpecificationDocumentMetaInfo::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CSubstrateSpecificationDocumentMetaInfo::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CSubstrateSpecificationDocumentMetaInfo::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CSubstrateSpecificationDocumentMetaInfo::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}




CInputIdObject::CInputIdObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &CInputIdObject::idChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CInputIdObject::GetId()
{
	if (Version_1_0 && Version_1_0->id){
		return Version_1_0->id.value();
	}

	return QVariant();
}


void CInputIdObject::SetId(const QVariant& v)
{
	Version_1_0->id = v.value<QString>().toUtf8();
	idChanged();
}


bool CInputIdObject::hasId()
{
	 return Version_1_0->id.HasValue();
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


void CInputIdObjectList::append(sdl::substrate::SubstrateSpecifications::CInputIdObject* item)
{
	BaseClass::append(item);
}


sdl::substrate::SubstrateSpecifications::CInputIdObjectList* sdl::substrate::SubstrateSpecifications::CInputIdObjectList::copyMe()
{
	sdl::substrate::SubstrateSpecifications::CInputIdObjectList* retVal = new sdl::substrate::SubstrateSpecifications::CInputIdObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::substrate::SubstrateSpecifications::CInputIdObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::substrate::SubstrateSpecifications::CInputIdObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::substrate::SubstrateSpecifications::CInputIdObjectList::addElement(sdl::substrate::SubstrateSpecifications::CInputIdObject* item)
{
	append(item);
}


void sdl::substrate::SubstrateSpecifications::CInputIdObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::substrate::SubstrateSpecifications::CInputIdObjectList::isEqualWithModel(sdl::substrate::SubstrateSpecifications::CInputIdObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::substrate::SubstrateSpecifications::CInputIdObjectList::insert(int index, sdl::substrate::SubstrateSpecifications::CInputIdObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::substrate::SubstrateSpecifications::CInputIdObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::substrate::SubstrateSpecifications::CInputIdObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::substrate::SubstrateSpecifications::CInputIdObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::substrate::SubstrateSpecifications::CInputIdObject*>();
	if (itemPtr == nullptr) return QVariant();
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
		if (nameId == "m_id"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index)->id.value());
		}
	return QVariant();
}
CCollectionViewParamsTestObject::CCollectionViewParamsTestObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &CCollectionViewParamsTestObject::countChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCollectionViewParamsTestObject::offsetChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CCollectionViewParamsTestObject::GetCount()
{
	if (Version_1_0 && Version_1_0->count){
		return Version_1_0->count.value();
	}

	return QVariant();
}


void CCollectionViewParamsTestObject::SetCount(const QVariant& v)
{
	Version_1_0->count = v.value<int>();
	countChanged();
}


bool CCollectionViewParamsTestObject::hasCount()
{
	 return Version_1_0->count.HasValue();
}


QVariant CCollectionViewParamsTestObject::GetOffset()
{
	if (Version_1_0 && Version_1_0->offset){
		return Version_1_0->offset.value();
	}

	return QVariant();
}


void CCollectionViewParamsTestObject::SetOffset(const QVariant& v)
{
	Version_1_0->offset = v.value<int>();
	offsetChanged();
}


bool CCollectionViewParamsTestObject::hasOffset()
{
	 return Version_1_0->offset.HasValue();
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


void CCollectionViewParamsTestObjectList::append(sdl::substrate::SubstrateSpecifications::CCollectionViewParamsTestObject* item)
{
	BaseClass::append(item);
}


sdl::substrate::SubstrateSpecifications::CCollectionViewParamsTestObjectList* sdl::substrate::SubstrateSpecifications::CCollectionViewParamsTestObjectList::copyMe()
{
	sdl::substrate::SubstrateSpecifications::CCollectionViewParamsTestObjectList* retVal = new sdl::substrate::SubstrateSpecifications::CCollectionViewParamsTestObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::substrate::SubstrateSpecifications::CCollectionViewParamsTestObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::substrate::SubstrateSpecifications::CCollectionViewParamsTestObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::substrate::SubstrateSpecifications::CCollectionViewParamsTestObjectList::addElement(sdl::substrate::SubstrateSpecifications::CCollectionViewParamsTestObject* item)
{
	append(item);
}


void sdl::substrate::SubstrateSpecifications::CCollectionViewParamsTestObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::substrate::SubstrateSpecifications::CCollectionViewParamsTestObjectList::isEqualWithModel(sdl::substrate::SubstrateSpecifications::CCollectionViewParamsTestObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::substrate::SubstrateSpecifications::CCollectionViewParamsTestObjectList::insert(int index, sdl::substrate::SubstrateSpecifications::CCollectionViewParamsTestObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::substrate::SubstrateSpecifications::CCollectionViewParamsTestObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::substrate::SubstrateSpecifications::CCollectionViewParamsTestObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::substrate::SubstrateSpecifications::CCollectionViewParamsTestObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::substrate::SubstrateSpecifications::CCollectionViewParamsTestObject*>();
	if (itemPtr == nullptr) return QVariant();
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
		if (nameId == "m_count"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index)->count.value());
		}
		if (nameId == "m_offset"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index)->offset.value());
		}
	return QVariant();
}
CSubstrateSpecificationDocumentTypeIdsObject::CSubstrateSpecificationDocumentTypeIdsObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &CSubstrateSpecificationDocumentTypeIdsObject::paperIDChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CSubstrateSpecificationDocumentTypeIdsObject::cardboardIDChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CSubstrateSpecificationDocumentTypeIdsObject::filmIDChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CSubstrateSpecificationDocumentTypeIdsObject::GetPaperID()
{
	if (Version_1_0 && Version_1_0->PaperID){
		return Version_1_0->PaperID.value();
	}

	return QVariant();
}


void CSubstrateSpecificationDocumentTypeIdsObject::SetPaperID(const QVariant& v)
{
	Version_1_0->PaperID = v.value<QString>().toUtf8();
	paperIDChanged();
}


bool CSubstrateSpecificationDocumentTypeIdsObject::hasPaperID()
{
	 return Version_1_0->PaperID.HasValue();
}


QVariant CSubstrateSpecificationDocumentTypeIdsObject::GetCardboardID()
{
	if (Version_1_0 && Version_1_0->CardboardID){
		return Version_1_0->CardboardID.value();
	}

	return QVariant();
}


void CSubstrateSpecificationDocumentTypeIdsObject::SetCardboardID(const QVariant& v)
{
	Version_1_0->CardboardID = v.value<QString>().toUtf8();
	cardboardIDChanged();
}


bool CSubstrateSpecificationDocumentTypeIdsObject::hasCardboardID()
{
	 return Version_1_0->CardboardID.HasValue();
}


QVariant CSubstrateSpecificationDocumentTypeIdsObject::GetFilmID()
{
	if (Version_1_0 && Version_1_0->FilmID){
		return Version_1_0->FilmID.value();
	}

	return QVariant();
}


void CSubstrateSpecificationDocumentTypeIdsObject::SetFilmID(const QVariant& v)
{
	Version_1_0->FilmID = v.value<QString>().toUtf8();
	filmIDChanged();
}


bool CSubstrateSpecificationDocumentTypeIdsObject::hasFilmID()
{
	 return Version_1_0->FilmID.HasValue();
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


void CSubstrateSpecificationDocumentTypeIdsObjectList::append(sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentTypeIdsObject* item)
{
	BaseClass::append(item);
}


sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentTypeIdsObjectList* sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentTypeIdsObjectList::copyMe()
{
	sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentTypeIdsObjectList* retVal = new sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentTypeIdsObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentTypeIdsObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentTypeIdsObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentTypeIdsObjectList::addElement(sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentTypeIdsObject* item)
{
	append(item);
}


void sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentTypeIdsObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentTypeIdsObjectList::isEqualWithModel(sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentTypeIdsObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentTypeIdsObjectList::insert(int index, sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentTypeIdsObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentTypeIdsObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentTypeIdsObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentTypeIdsObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentTypeIdsObject*>();
	if (itemPtr == nullptr) return QVariant();
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
		if (nameId == "m_paperID"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index)->PaperID.value());
		}
		if (nameId == "m_cardboardID"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index)->CardboardID.value());
		}
		if (nameId == "m_filmID"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index)->FilmID.value());
		}
	return QVariant();
}
CSubstrateSpecificationListInputObject::CSubstrateSpecificationListInputObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &CSubstrateSpecificationListInputObject::viewParamsChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CSubstrateSpecificationListInputObject::GetViewParams()
{
	if (Version_1_0 && Version_1_0->viewParams){
		if (!m_viewParamsQObjectPtr.isValid()){
			m_viewParamsQObjectPtr = CreateObject("viewParams");
			auto itemPtr = m_viewParamsQObjectPtr.value<sdl::substrate::SubstrateSpecifications::CCollectionViewParamsTestObject*>();
			if (itemPtr != nullptr) itemPtr->Version_1_0 = Version_1_0->viewParams;
		}
		return m_viewParamsQObjectPtr;
	}

	return QVariant();
}


void CSubstrateSpecificationListInputObject::SetViewParams(const QVariant& v)
{
	if (v.isValid()){
		sdl::substrate::SubstrateSpecifications::CCollectionViewParamsTestObject* itemPtr = v.value<sdl::substrate::SubstrateSpecifications::CCollectionViewParamsTestObject*>();
		if (itemPtr != nullptr)  Version_1_0->viewParams = itemPtr->Version_1_0;
	}
	else {
		Version_1_0->viewParams = nullptr;
	}
	m_viewParamsQObjectPtr = v;

	viewParamsChanged();
}


bool CSubstrateSpecificationListInputObject::hasViewParams()
{
	 return Version_1_0->viewParams.HasValue();
}


void CSubstrateSpecificationListInputObject::emplaceViewParams()
{
	Version_1_0->viewParams.emplace();

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
		return QVariant::fromValue(new sdl::substrate::SubstrateSpecifications::CCollectionViewParamsTestObject(this));
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


void CSubstrateSpecificationListInputObjectList::append(sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListInputObject* item)
{
	BaseClass::append(item);
}


sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListInputObjectList* sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListInputObjectList::copyMe()
{
	sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListInputObjectList* retVal = new sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListInputObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListInputObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListInputObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListInputObjectList::addElement(sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListInputObject* item)
{
	append(item);
}


void sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListInputObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListInputObjectList::isEqualWithModel(sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListInputObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListInputObjectList::insert(int index, sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListInputObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListInputObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListInputObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListInputObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListInputObject*>();
	if (itemPtr == nullptr) return QVariant();
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
		if (nameId == "m_viewParams"){
			return itemPtr->GetViewParams();
		}
	return QVariant();
}
CSubstrateSpecificationListItemObject::CSubstrateSpecificationListItemObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

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
	if (Version_1_0 && Version_1_0->id){
		return Version_1_0->id.value();
	}

	return QVariant();
}


void CSubstrateSpecificationListItemObject::SetId(const QVariant& v)
{
	Version_1_0->id = v.value<QString>().toUtf8();
	idChanged();
}


bool CSubstrateSpecificationListItemObject::hasId()
{
	 return Version_1_0->id.HasValue();
}


QVariant CSubstrateSpecificationListItemObject::GetTypeId()
{
	if (Version_1_0 && Version_1_0->typeId){
		return Version_1_0->typeId.value();
	}

	return QVariant();
}


void CSubstrateSpecificationListItemObject::SetTypeId(const QVariant& v)
{
	Version_1_0->typeId = v.value<QString>().toUtf8();
	typeIdChanged();
}


bool CSubstrateSpecificationListItemObject::hasTypeId()
{
	 return Version_1_0->typeId.HasValue();
}


QVariant CSubstrateSpecificationListItemObject::GetName()
{
	if (Version_1_0 && Version_1_0->name){
		return Version_1_0->name.value();
	}

	return QVariant();
}


void CSubstrateSpecificationListItemObject::SetName(const QVariant& v)
{
	Version_1_0->name = v.value<QString>();
	nameChanged();
}


bool CSubstrateSpecificationListItemObject::hasName()
{
	 return Version_1_0->name.HasValue();
}


QVariant CSubstrateSpecificationListItemObject::GetMaterialId()
{
	if (Version_1_0 && Version_1_0->materialId){
		return Version_1_0->materialId.value();
	}

	return QVariant();
}


void CSubstrateSpecificationListItemObject::SetMaterialId(const QVariant& v)
{
	Version_1_0->materialId = v.value<QString>().toUtf8();
	materialIdChanged();
}


bool CSubstrateSpecificationListItemObject::hasMaterialId()
{
	 return Version_1_0->materialId.HasValue();
}


QVariant CSubstrateSpecificationListItemObject::GetMaterialName()
{
	if (Version_1_0 && Version_1_0->materialName){
		return Version_1_0->materialName.value();
	}

	return QVariant();
}


void CSubstrateSpecificationListItemObject::SetMaterialName(const QVariant& v)
{
	Version_1_0->materialName = v.value<QString>();
	materialNameChanged();
}


bool CSubstrateSpecificationListItemObject::hasMaterialName()
{
	 return Version_1_0->materialName.HasValue();
}


QVariant CSubstrateSpecificationListItemObject::GetAdded()
{
	if (Version_1_0 && Version_1_0->added){
		return Version_1_0->added.value();
	}

	return QVariant();
}


void CSubstrateSpecificationListItemObject::SetAdded(const QVariant& v)
{
	Version_1_0->added = v.value<QString>();
	addedChanged();
}


bool CSubstrateSpecificationListItemObject::hasAdded()
{
	 return Version_1_0->added.HasValue();
}


QVariant CSubstrateSpecificationListItemObject::GetTimeStamp()
{
	if (Version_1_0 && Version_1_0->timeStamp){
		return Version_1_0->timeStamp.value();
	}

	return QVariant();
}


void CSubstrateSpecificationListItemObject::SetTimeStamp(const QVariant& v)
{
	Version_1_0->timeStamp = v.value<QString>();
	timeStampChanged();
}


bool CSubstrateSpecificationListItemObject::hasTimeStamp()
{
	 return Version_1_0->timeStamp.HasValue();
}


QVariant CSubstrateSpecificationListItemObject::GetColorPreview()
{
	if (Version_1_0 && Version_1_0->colorPreview){
		return Version_1_0->colorPreview.value();
	}

	return QVariant();
}


void CSubstrateSpecificationListItemObject::SetColorPreview(const QVariant& v)
{
	Version_1_0->colorPreview = v.value<QString>();
	colorPreviewChanged();
}


bool CSubstrateSpecificationListItemObject::hasColorPreview()
{
	 return Version_1_0->colorPreview.HasValue();
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


void CSubstrateSpecificationListItemObjectList::append(sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListItemObject* item)
{
	BaseClass::append(item);
}


sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListItemObjectList* sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListItemObjectList::copyMe()
{
	sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListItemObjectList* retVal = new sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListItemObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListItemObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListItemObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListItemObjectList::addElement(sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListItemObject* item)
{
	append(item);
}


void sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListItemObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListItemObjectList::isEqualWithModel(sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListItemObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListItemObjectList::insert(int index, sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListItemObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListItemObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListItemObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListItemObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListItemObject*>();
	if (itemPtr == nullptr) return QVariant();
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
		if (nameId == "m_id"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index)->id.value());
		}
		if (nameId == "m_typeId"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index)->typeId.value());
		}
		if (nameId == "m_name"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index)->name.value());
		}
		if (nameId == "m_materialId"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index)->materialId.value());
		}
		if (nameId == "m_materialName"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index)->materialName.value());
		}
		if (nameId == "m_added"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index)->added.value());
		}
		if (nameId == "m_timeStamp"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index)->timeStamp.value());
		}
		if (nameId == "m_colorPreview"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index)->colorPreview.value());
		}
	return QVariant();
}
CSubstrateSpecificationListResponseObject::CSubstrateSpecificationListResponseObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &CSubstrateSpecificationListResponseObject::itemsChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CSubstrateSpecificationListResponseObject::GetItems()
{
	if (Version_1_0 && Version_1_0->items){
		if (!m_itemsQObjectPtr.isValid()){
			m_itemsQObjectPtr = CreateObject("items");
			auto itemPtr = m_itemsQObjectPtr.value<sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListItemObjectList*>();
			if (itemPtr != nullptr) itemPtr->Version_1_0 = Version_1_0->items;
		}
		return m_itemsQObjectPtr;
	}

	return QVariant();
}


void CSubstrateSpecificationListResponseObject::SetItems(const QVariant& v)
{
	if (v.isValid()){
		sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListItemObjectList* itemPtr = v.value<sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListItemObjectList*>();
		if (itemPtr != nullptr)  Version_1_0->items = itemPtr->Version_1_0;
	}
	else {
		Version_1_0->items = nullptr;
	}
	m_itemsQObjectPtr = v;

	itemsChanged();
}


bool CSubstrateSpecificationListResponseObject::hasItems()
{
	 return Version_1_0->items.HasValue();
}


void CSubstrateSpecificationListResponseObject::emplaceItems()
{
	Version_1_0->items.emplace();

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
	return QVariant::fromValue(new sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListItemObject());
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
		return QVariant::fromValue(new sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListItemObjectList(this));
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


void CSubstrateSpecificationListResponseObjectList::append(sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListResponseObject* item)
{
	BaseClass::append(item);
}


sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListResponseObjectList* sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListResponseObjectList::copyMe()
{
	sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListResponseObjectList* retVal = new sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListResponseObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListResponseObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListResponseObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListResponseObjectList::addElement(sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListResponseObject* item)
{
	append(item);
}


void sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListResponseObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListResponseObjectList::isEqualWithModel(sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListResponseObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListResponseObjectList::insert(int index, sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListResponseObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListResponseObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListResponseObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListResponseObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListResponseObject*>();
	if (itemPtr == nullptr) return QVariant();
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
		if (nameId == "m_items"){
			return itemPtr->GetItems();
		}
	return QVariant();
}
CCardboardSubstrateSpecificationObject::CCardboardSubstrateSpecificationObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &CCardboardSubstrateSpecificationObject::colorChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCardboardSubstrateSpecificationObject::grammageChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CCardboardSubstrateSpecificationObject::GetColor()
{
	if (Version_1_0 && Version_1_0->color){
		return Version_1_0->color.value();
	}

	return QVariant();
}


void CCardboardSubstrateSpecificationObject::SetColor(const QVariant& v)
{
	Version_1_0->color = v.value<QString>();
	colorChanged();
}


bool CCardboardSubstrateSpecificationObject::hasColor()
{
	 return Version_1_0->color.HasValue();
}


QVariant CCardboardSubstrateSpecificationObject::GetGrammage()
{
	if (Version_1_0 && Version_1_0->grammage){
		return Version_1_0->grammage.value();
	}

	return QVariant();
}


void CCardboardSubstrateSpecificationObject::SetGrammage(const QVariant& v)
{
	Version_1_0->grammage = v.value<double>();
	grammageChanged();
}


bool CCardboardSubstrateSpecificationObject::hasGrammage()
{
	 return Version_1_0->grammage.HasValue();
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


void CCardboardSubstrateSpecificationObjectList::append(sdl::substrate::SubstrateSpecifications::CCardboardSubstrateSpecificationObject* item)
{
	BaseClass::append(item);
}


sdl::substrate::SubstrateSpecifications::CCardboardSubstrateSpecificationObjectList* sdl::substrate::SubstrateSpecifications::CCardboardSubstrateSpecificationObjectList::copyMe()
{
	sdl::substrate::SubstrateSpecifications::CCardboardSubstrateSpecificationObjectList* retVal = new sdl::substrate::SubstrateSpecifications::CCardboardSubstrateSpecificationObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::substrate::SubstrateSpecifications::CCardboardSubstrateSpecificationObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::substrate::SubstrateSpecifications::CCardboardSubstrateSpecificationObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::substrate::SubstrateSpecifications::CCardboardSubstrateSpecificationObjectList::addElement(sdl::substrate::SubstrateSpecifications::CCardboardSubstrateSpecificationObject* item)
{
	append(item);
}


void sdl::substrate::SubstrateSpecifications::CCardboardSubstrateSpecificationObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::substrate::SubstrateSpecifications::CCardboardSubstrateSpecificationObjectList::isEqualWithModel(sdl::substrate::SubstrateSpecifications::CCardboardSubstrateSpecificationObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::substrate::SubstrateSpecifications::CCardboardSubstrateSpecificationObjectList::insert(int index, sdl::substrate::SubstrateSpecifications::CCardboardSubstrateSpecificationObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::substrate::SubstrateSpecifications::CCardboardSubstrateSpecificationObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::substrate::SubstrateSpecifications::CCardboardSubstrateSpecificationObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::substrate::SubstrateSpecifications::CCardboardSubstrateSpecificationObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::substrate::SubstrateSpecifications::CCardboardSubstrateSpecificationObject*>();
	if (itemPtr == nullptr) return QVariant();
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
		if (nameId == "m_color"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index)->color.value());
		}
		if (nameId == "m_grammage"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index)->grammage.value());
		}
	return QVariant();
}
CPaperSubstrateSpecificationObject::CPaperSubstrateSpecificationObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &CPaperSubstrateSpecificationObject::grammageChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CPaperSubstrateSpecificationObject::colorChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CPaperSubstrateSpecificationObject::coatTypeChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CPaperSubstrateSpecificationObject::GetGrammage()
{
	if (Version_1_0 && Version_1_0->grammage){
		return Version_1_0->grammage.value();
	}

	return QVariant();
}


void CPaperSubstrateSpecificationObject::SetGrammage(const QVariant& v)
{
	Version_1_0->grammage = v.value<double>();
	grammageChanged();
}


bool CPaperSubstrateSpecificationObject::hasGrammage()
{
	 return Version_1_0->grammage.HasValue();
}


QVariant CPaperSubstrateSpecificationObject::GetColor()
{
	if (Version_1_0 && Version_1_0->color){
		return Version_1_0->color.value();
	}

	return QVariant();
}


void CPaperSubstrateSpecificationObject::SetColor(const QVariant& v)
{
	Version_1_0->color = v.value<QString>();
	colorChanged();
}


bool CPaperSubstrateSpecificationObject::hasColor()
{
	 return Version_1_0->color.HasValue();
}


QVariant CPaperSubstrateSpecificationObject::GetCoatType()
{
	if (Version_1_0 && Version_1_0->coatType){
		return Version_1_0->coatType.value();
	}

	return QVariant();
}


void CPaperSubstrateSpecificationObject::SetCoatType(const QVariant& v)
{
	Version_1_0->coatType = v.value<QString>();
	coatTypeChanged();
}


bool CPaperSubstrateSpecificationObject::hasCoatType()
{
	 return Version_1_0->coatType.HasValue();
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


void CPaperSubstrateSpecificationObjectList::append(sdl::substrate::SubstrateSpecifications::CPaperSubstrateSpecificationObject* item)
{
	BaseClass::append(item);
}


sdl::substrate::SubstrateSpecifications::CPaperSubstrateSpecificationObjectList* sdl::substrate::SubstrateSpecifications::CPaperSubstrateSpecificationObjectList::copyMe()
{
	sdl::substrate::SubstrateSpecifications::CPaperSubstrateSpecificationObjectList* retVal = new sdl::substrate::SubstrateSpecifications::CPaperSubstrateSpecificationObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::substrate::SubstrateSpecifications::CPaperSubstrateSpecificationObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::substrate::SubstrateSpecifications::CPaperSubstrateSpecificationObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::substrate::SubstrateSpecifications::CPaperSubstrateSpecificationObjectList::addElement(sdl::substrate::SubstrateSpecifications::CPaperSubstrateSpecificationObject* item)
{
	append(item);
}


void sdl::substrate::SubstrateSpecifications::CPaperSubstrateSpecificationObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::substrate::SubstrateSpecifications::CPaperSubstrateSpecificationObjectList::isEqualWithModel(sdl::substrate::SubstrateSpecifications::CPaperSubstrateSpecificationObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::substrate::SubstrateSpecifications::CPaperSubstrateSpecificationObjectList::insert(int index, sdl::substrate::SubstrateSpecifications::CPaperSubstrateSpecificationObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::substrate::SubstrateSpecifications::CPaperSubstrateSpecificationObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::substrate::SubstrateSpecifications::CPaperSubstrateSpecificationObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::substrate::SubstrateSpecifications::CPaperSubstrateSpecificationObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::substrate::SubstrateSpecifications::CPaperSubstrateSpecificationObject*>();
	if (itemPtr == nullptr) return QVariant();
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
		if (nameId == "m_grammage"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index)->grammage.value());
		}
		if (nameId == "m_color"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index)->color.value());
		}
		if (nameId == "m_coatType"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index)->coatType.value());
		}
	return QVariant();
}
CFilmSubstrateSpecificationObject::CFilmSubstrateSpecificationObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &CFilmSubstrateSpecificationObject::transparencyChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CFilmSubstrateSpecificationObject::colorChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CFilmSubstrateSpecificationObject::materialChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CFilmSubstrateSpecificationObject::GetTransparency()
{
	if (Version_1_0 && Version_1_0->transparency){
		return Version_1_0->transparency.value();
	}

	return QVariant();
}


void CFilmSubstrateSpecificationObject::SetTransparency(const QVariant& v)
{
	Version_1_0->transparency = v.value<double>();
	transparencyChanged();
}


bool CFilmSubstrateSpecificationObject::hasTransparency()
{
	 return Version_1_0->transparency.HasValue();
}


QVariant CFilmSubstrateSpecificationObject::GetColor()
{
	if (Version_1_0 && Version_1_0->color){
		return Version_1_0->color.value();
	}

	return QVariant();
}


void CFilmSubstrateSpecificationObject::SetColor(const QVariant& v)
{
	Version_1_0->color = v.value<QString>();
	colorChanged();
}


bool CFilmSubstrateSpecificationObject::hasColor()
{
	 return Version_1_0->color.HasValue();
}


QVariant CFilmSubstrateSpecificationObject::GetMaterial()
{
	if (Version_1_0 && Version_1_0->material){
		return Version_1_0->material.value();
	}

	return QVariant();
}


void CFilmSubstrateSpecificationObject::SetMaterial(const QVariant& v)
{
	Version_1_0->material = v.value<QString>();
	materialChanged();
}


bool CFilmSubstrateSpecificationObject::hasMaterial()
{
	 return Version_1_0->material.HasValue();
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


void CFilmSubstrateSpecificationObjectList::append(sdl::substrate::SubstrateSpecifications::CFilmSubstrateSpecificationObject* item)
{
	BaseClass::append(item);
}


sdl::substrate::SubstrateSpecifications::CFilmSubstrateSpecificationObjectList* sdl::substrate::SubstrateSpecifications::CFilmSubstrateSpecificationObjectList::copyMe()
{
	sdl::substrate::SubstrateSpecifications::CFilmSubstrateSpecificationObjectList* retVal = new sdl::substrate::SubstrateSpecifications::CFilmSubstrateSpecificationObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::substrate::SubstrateSpecifications::CFilmSubstrateSpecificationObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::substrate::SubstrateSpecifications::CFilmSubstrateSpecificationObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::substrate::SubstrateSpecifications::CFilmSubstrateSpecificationObjectList::addElement(sdl::substrate::SubstrateSpecifications::CFilmSubstrateSpecificationObject* item)
{
	append(item);
}


void sdl::substrate::SubstrateSpecifications::CFilmSubstrateSpecificationObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::substrate::SubstrateSpecifications::CFilmSubstrateSpecificationObjectList::isEqualWithModel(sdl::substrate::SubstrateSpecifications::CFilmSubstrateSpecificationObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::substrate::SubstrateSpecifications::CFilmSubstrateSpecificationObjectList::insert(int index, sdl::substrate::SubstrateSpecifications::CFilmSubstrateSpecificationObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::substrate::SubstrateSpecifications::CFilmSubstrateSpecificationObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::substrate::SubstrateSpecifications::CFilmSubstrateSpecificationObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::substrate::SubstrateSpecifications::CFilmSubstrateSpecificationObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::substrate::SubstrateSpecifications::CFilmSubstrateSpecificationObject*>();
	if (itemPtr == nullptr) return QVariant();
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
		if (nameId == "m_transparency"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index)->transparency.value());
		}
		if (nameId == "m_color"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index)->color.value());
		}
		if (nameId == "m_material"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index)->material.value());
		}
	return QVariant();
}
CSubstrateSpecificationInputObject::CSubstrateSpecificationInputObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &CSubstrateSpecificationInputObject::idChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CSubstrateSpecificationInputObject::nameChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CSubstrateSpecificationInputObject::descriptionChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CSubstrateSpecificationInputObject::itemChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CSubstrateSpecificationInputObject::typeIdChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CSubstrateSpecificationInputObject::GetId()
{
	if (Version_1_0 && Version_1_0->id){
		return Version_1_0->id.value();
	}

	return QVariant();
}


void CSubstrateSpecificationInputObject::SetId(const QVariant& v)
{
	Version_1_0->id = v.value<QString>().toUtf8();
	idChanged();
}


bool CSubstrateSpecificationInputObject::hasId()
{
	 return Version_1_0->id.HasValue();
}


QVariant CSubstrateSpecificationInputObject::GetName()
{
	if (Version_1_0 && Version_1_0->name){
		return Version_1_0->name.value();
	}

	return QVariant();
}


void CSubstrateSpecificationInputObject::SetName(const QVariant& v)
{
	Version_1_0->name = v.value<QString>();
	nameChanged();
}


bool CSubstrateSpecificationInputObject::hasName()
{
	 return Version_1_0->name.HasValue();
}


QVariant CSubstrateSpecificationInputObject::GetDescription()
{
	if (Version_1_0 && Version_1_0->description){
		return Version_1_0->description.value();
	}

	return QVariant();
}


void CSubstrateSpecificationInputObject::SetDescription(const QVariant& v)
{
	Version_1_0->description = v.value<QString>();
	descriptionChanged();
}


bool CSubstrateSpecificationInputObject::hasDescription()
{
	 return Version_1_0->description.HasValue();
}


QVariant CSubstrateSpecificationInputObject::GetItem()
{
	if (Version_1_0 && Version_1_0->item){
		if (m_itemQObjectPtr.isValid()){
			if (const CCardboardSubstrateSpecification* val = std::get_if<CCardboardSubstrateSpecification>((Version_1_0->item).GetPtr())){
				CCardboardSubstrateSpecificationObject *newObjectPtr = new CCardboardSubstrateSpecificationObject(this);
				newObjectPtr->Version_1_0 = val->Version_1_0;
				m_itemQObjectPtr = QVariant::fromValue(newObjectPtr);
			}
			if (const CPaperSubstrateSpecification* val = std::get_if<CPaperSubstrateSpecification>((Version_1_0->item).GetPtr())){
				CPaperSubstrateSpecificationObject *newObjectPtr = new CPaperSubstrateSpecificationObject(this);
				newObjectPtr->Version_1_0 = val->Version_1_0;
				m_itemQObjectPtr = QVariant::fromValue(newObjectPtr);
			}
			if (const CFilmSubstrateSpecification* val = std::get_if<CFilmSubstrateSpecification>((Version_1_0->item).GetPtr())){
				CFilmSubstrateSpecificationObject *newObjectPtr = new CFilmSubstrateSpecificationObject(this);
				newObjectPtr->Version_1_0 = val->Version_1_0;
				m_itemQObjectPtr = QVariant::fromValue(newObjectPtr);
			}
		}
		return m_itemQObjectPtr;
	}

	return QVariant();
}


void CSubstrateSpecificationInputObject::SetItem(const QVariant& v)
{
	if (v.isValid()){
		if (const CCardboardSubstrateSpecificationObject* val = v.value<const CCardboardSubstrateSpecificationObject*>()){
			*Version_1_0->item = *val;
		}
		if (const CPaperSubstrateSpecificationObject* val = v.value<const CPaperSubstrateSpecificationObject*>()){
			*Version_1_0->item = *val;
		}
		if (const CFilmSubstrateSpecificationObject* val = v.value<const CFilmSubstrateSpecificationObject*>()){
			*Version_1_0->item = *val;
		}
	}
	else {
		Version_1_0->item = nullptr;
	}
	m_itemQObjectPtr = v;

	itemChanged();
}


bool CSubstrateSpecificationInputObject::hasItem()
{
	 return Version_1_0->item.HasValue();
}


void CSubstrateSpecificationInputObject::emplaceItem()
{
	Version_1_0->item.emplace();

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
	if (Version_1_0 && Version_1_0->typeId){
		return Version_1_0->typeId.value();
	}

	return QVariant();
}


void CSubstrateSpecificationInputObject::SetTypeId(const QVariant& v)
{
	Version_1_0->typeId = v.value<QString>().toUtf8();
	typeIdChanged();
}


bool CSubstrateSpecificationInputObject::hasTypeId()
{
	 return Version_1_0->typeId.HasValue();
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


void CSubstrateSpecificationInputObjectList::append(sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationInputObject* item)
{
	BaseClass::append(item);
}


sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationInputObjectList* sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationInputObjectList::copyMe()
{
	sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationInputObjectList* retVal = new sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationInputObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationInputObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationInputObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationInputObjectList::addElement(sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationInputObject* item)
{
	append(item);
}


void sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationInputObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationInputObjectList::isEqualWithModel(sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationInputObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationInputObjectList::insert(int index, sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationInputObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationInputObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationInputObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationInputObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationInputObject*>();
	if (itemPtr == nullptr) return QVariant();
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
		if (nameId == "m_id"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index)->id.value());
		}
		if (nameId == "m_name"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index)->name.value());
		}
		if (nameId == "m_description"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index)->description.value());
		}
		if (nameId == "m_item"){
			return itemPtr->GetItem();
		}
		if (nameId == "m_typeId"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index)->typeId.value());
		}
	return QVariant();
}
CSubstrateSpecificationDocumentMetaInfoObject::CSubstrateSpecificationDocumentMetaInfoObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &CSubstrateSpecificationDocumentMetaInfoObject::materialIdChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CSubstrateSpecificationDocumentMetaInfoObject::labChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CSubstrateSpecificationDocumentMetaInfoObject::GetMaterialId()
{
	if (Version_1_0 && Version_1_0->materialId){
		return Version_1_0->materialId.value();
	}

	return QVariant();
}


void CSubstrateSpecificationDocumentMetaInfoObject::SetMaterialId(const QVariant& v)
{
	Version_1_0->materialId = v.value<QString>().toUtf8();
	materialIdChanged();
}


bool CSubstrateSpecificationDocumentMetaInfoObject::hasMaterialId()
{
	 return Version_1_0->materialId.HasValue();
}


QVariant CSubstrateSpecificationDocumentMetaInfoObject::GetLab()
{
	if (Version_1_0 && Version_1_0->lab){
		return Version_1_0->lab.value();
	}

	return QVariant();
}


void CSubstrateSpecificationDocumentMetaInfoObject::SetLab(const QVariant& v)
{
	Version_1_0->lab = v.value<QString>();
	labChanged();
}


bool CSubstrateSpecificationDocumentMetaInfoObject::hasLab()
{
	 return Version_1_0->lab.HasValue();
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


void CSubstrateSpecificationDocumentMetaInfoObjectList::append(sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentMetaInfoObject* item)
{
	BaseClass::append(item);
}


sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentMetaInfoObjectList* sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentMetaInfoObjectList::copyMe()
{
	sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentMetaInfoObjectList* retVal = new sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentMetaInfoObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentMetaInfoObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentMetaInfoObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentMetaInfoObjectList::addElement(sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentMetaInfoObject* item)
{
	append(item);
}


void sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentMetaInfoObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentMetaInfoObjectList::isEqualWithModel(sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentMetaInfoObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentMetaInfoObjectList::insert(int index, sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentMetaInfoObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentMetaInfoObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentMetaInfoObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentMetaInfoObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentMetaInfoObject*>();
	if (itemPtr == nullptr) return QVariant();
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
		if (nameId == "m_materialId"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index)->materialId.value());
		}
		if (nameId == "m_lab"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index)->lab.value());
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
		if (const CCardboardSubstrateSpecification* val = std::get_if<CCardboardSubstrateSpecification>(this->Version_1_0->at(index).GetPtr())){
			CCardboardSubstrateSpecificationObject *newObjectPtr = new CCardboardSubstrateSpecificationObject();
			newObjectPtr->Version_1_0 = val->Version_1_0;
			retVal = QVariant::fromValue(newObjectPtr);
		}

		if (const CPaperSubstrateSpecification* val = std::get_if<CPaperSubstrateSpecification>(this->Version_1_0->at(index).GetPtr())){
			CPaperSubstrateSpecificationObject *newObjectPtr = new CPaperSubstrateSpecificationObject();
			newObjectPtr->Version_1_0 = val->Version_1_0;
			retVal = QVariant::fromValue(newObjectPtr);
		}

		if (const CFilmSubstrateSpecification* val = std::get_if<CFilmSubstrateSpecification>(this->Version_1_0->at(index).GetPtr())){
			CFilmSubstrateSpecificationObject *newObjectPtr = new CFilmSubstrateSpecificationObject();
			newObjectPtr->Version_1_0 = val->Version_1_0;
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
	beginInsertRows(QModelIndex(), Version_1_0->count(), 0);

	if (item.canConvert<CCardboardSubstrateSpecificationObject*>()){
		CCardboardSubstrateSpecificationObject* val = item.value<CCardboardSubstrateSpecificationObject*>();
		CCardboardSubstrateSpecification* newItemPtr = dynamic_cast<CCardboardSubstrateSpecification*>(val);
		Version_1_0->append(istd::TSharedNullable<SubstrateSpecificationDocument>(*newItemPtr));
	}

	if (item.canConvert<CPaperSubstrateSpecificationObject*>()){
		CPaperSubstrateSpecificationObject* val = item.value<CPaperSubstrateSpecificationObject*>();
		CPaperSubstrateSpecification* newItemPtr = dynamic_cast<CPaperSubstrateSpecification*>(val);
		Version_1_0->append(istd::TSharedNullable<SubstrateSpecificationDocument>(*newItemPtr));
	}

	if (item.canConvert<CFilmSubstrateSpecificationObject*>()){
		CFilmSubstrateSpecificationObject* val = item.value<CFilmSubstrateSpecificationObject*>();
		CFilmSubstrateSpecification* newItemPtr = dynamic_cast<CFilmSubstrateSpecification*>(val);
		Version_1_0->append(istd::TSharedNullable<SubstrateSpecificationDocument>(*newItemPtr));
	}

	ClearCache();
	endInsertRows();
}


sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentObjectList* CSubstrateSpecificationDocumentObjectList::copyMe()
{
	sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentObjectList* objectListPtr = new sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentObjectList();

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

bool CSubstrateSpecificationDocumentObjectList::isEqualWithModel(sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentObjectList* otherModelPtr)
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
	if (index < 0 || index > Version_1_0->size()) return;
	beginInsertRows(QModelIndex(), Version_1_0->count(), 0);

	if (item.canConvert<CCardboardSubstrateSpecificationObject*>()){
		CCardboardSubstrateSpecificationObject* val = item.value<CCardboardSubstrateSpecificationObject*>();
		CCardboardSubstrateSpecification* newItemPtr = dynamic_cast<CCardboardSubstrateSpecification*>(val);
		Version_1_0->append(istd::TSharedNullable<SubstrateSpecificationDocument>(*newItemPtr));
	}

	if (item.canConvert<CPaperSubstrateSpecificationObject*>()){
		CPaperSubstrateSpecificationObject* val = item.value<CPaperSubstrateSpecificationObject*>();
		CPaperSubstrateSpecification* newItemPtr = dynamic_cast<CPaperSubstrateSpecification*>(val);
		Version_1_0->append(istd::TSharedNullable<SubstrateSpecificationDocument>(*newItemPtr));
	}

	if (item.canConvert<CFilmSubstrateSpecificationObject*>()){
		CFilmSubstrateSpecificationObject* val = item.value<CFilmSubstrateSpecificationObject*>();
		CFilmSubstrateSpecification* newItemPtr = dynamic_cast<CFilmSubstrateSpecification*>(val);
		Version_1_0->append(istd::TSharedNullable<SubstrateSpecificationDocument>(*newItemPtr));
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
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
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
	if (!requestArguments.input.WriteToGraphQlObject(inputDataObject)){
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
					isInputRead = m_requestedArguments.input.OptReadFromGraphQlObject(*inputDataObjectPtr, CSubstrateSpecificationListInput::PV_1_0);
				}
				else {
					isInputRead = m_requestedArguments.input.ReadFromGraphQlObject(*inputDataObjectPtr, CSubstrateSpecificationListInput::PV_1_0);
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
				isInputRead = m_requestedArguments.input.OptReadFromGraphQlObject(*inputDataObjectPtr);
			}
			else {
				isInputRead = m_requestedArguments.input.ReadFromGraphQlObject(*inputDataObjectPtr);
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
	if (!requestArguments.input.WriteToGraphQlObject(inputDataObject)){
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
					isInputRead = m_requestedArguments.input.OptReadFromGraphQlObject(*inputDataObjectPtr, CInputId::PV_1_0);
				}
				else {
					isInputRead = m_requestedArguments.input.ReadFromGraphQlObject(*inputDataObjectPtr, CInputId::PV_1_0);
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
				isInputRead = m_requestedArguments.input.OptReadFromGraphQlObject(*inputDataObjectPtr);
			}
			else {
				isInputRead = m_requestedArguments.input.ReadFromGraphQlObject(*inputDataObjectPtr);
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
	if (!requestArguments.input.WriteToGraphQlObject(inputDataObject)){
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
					isInputRead = m_requestedArguments.input.OptReadFromGraphQlObject(*inputDataObjectPtr, CCollectionViewParamsTest::PV_1_0);
				}
				else {
					isInputRead = m_requestedArguments.input.ReadFromGraphQlObject(*inputDataObjectPtr, CCollectionViewParamsTest::PV_1_0);
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
				isInputRead = m_requestedArguments.input.OptReadFromGraphQlObject(*inputDataObjectPtr);
			}
			else {
				isInputRead = m_requestedArguments.input.ReadFromGraphQlObject(*inputDataObjectPtr);
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
	if (!requestArguments.input.WriteToGraphQlObject(inputDataObject)){
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
					isInputRead = m_requestedArguments.input.OptReadFromGraphQlObject(*inputDataObjectPtr, CSubstrateSpecificationInput::PV_1_0);
				}
				else {
					isInputRead = m_requestedArguments.input.ReadFromGraphQlObject(*inputDataObjectPtr, CSubstrateSpecificationInput::PV_1_0);
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
				isInputRead = m_requestedArguments.input.OptReadFromGraphQlObject(*inputDataObjectPtr);
			}
			else {
				isInputRead = m_requestedArguments.input.ReadFromGraphQlObject(*inputDataObjectPtr);
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
	if (!requestArguments.input.WriteToGraphQlObject(inputDataObject)){
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
					isInputRead = m_requestedArguments.input.OptReadFromGraphQlObject(*inputDataObjectPtr, CSubstrateSpecificationInput::PV_1_0);
				}
				else {
					isInputRead = m_requestedArguments.input.ReadFromGraphQlObject(*inputDataObjectPtr, CSubstrateSpecificationInput::PV_1_0);
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
				isInputRead = m_requestedArguments.input.OptReadFromGraphQlObject(*inputDataObjectPtr);
			}
			else {
				isInputRead = m_requestedArguments.input.ReadFromGraphQlObject(*inputDataObjectPtr);
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

		if (const CCardboardSubstrateSpecification* val = std::get_if<CCardboardSubstrateSpecification>(&representationObject)){
			if (!val->WriteToModel(dataModel)){
				return false;
			}
			dataModel.SetData("__typename", "CardboardSubstrateSpecification", 0);
		}
		else if (const CPaperSubstrateSpecification* val = std::get_if<CPaperSubstrateSpecification>(&representationObject)){
			if (!val->WriteToModel(dataModel)){
				return false;
			}
			dataModel.SetData("__typename", "PaperSubstrateSpecification", 0);
		}
		else if (const CFilmSubstrateSpecification* val = std::get_if<CFilmSubstrateSpecification>(&representationObject)){
			if (!val->WriteToModel(dataModel)){
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

		return CreateObjectFromRepresentation(*insertSubstrateSpecificationGqlRequest.GetRequestedArguments().input.Version_1_0->item, newObjectId, errorMessage);
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


bool CSubstrateSpecificationCollectionControllerCompBase::SetupGqlItem(const ::imtgql::CGqlRequest& gqlRequest, ::imtbase::CTreeItemModel& dataModel, int itemIndex,const ::imtbase::IObjectCollectionIterator* objectCollectionIterator, QString& errorMessage) const
{
	if (objectCollectionIterator == nullptr){
I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to create object iterator.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

		return false;
	}

	const QByteArray commandId = gqlRequest.GetCommandId();

	//GetSubstrateSpecificationList
	if (commandId == CGetSubstrateSpecificationListGqlRequest::GetCommandId()){
		CGetSubstrateSpecificationListGqlRequest getSubstrateSpecificationListGqlRequest(gqlRequest, false);
		CSubstrateSpecificationListItem::V1_0 representationObject;
		const bool isRepresentationCreated = CreateRepresentationFromObject(*objectCollectionIterator, getSubstrateSpecificationListGqlRequest, representationObject, errorMessage);
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

	errorMessage = QString("Bad request. Unexpected command-ID: '%1'").arg(qPrintable(commandId));

	SendErrorMessage(0, errorMessage);

	return false;
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

		imtbase::ImtBaseTypes::COptionsList replyPayload = OnGetOptionsList(getOptionsListGqlRequest, errorMessage);
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


bool CGraphQlHandlerCompBase::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
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


::imtbase::CTreeItemModel* CGraphQlHandlerCompBase::CreateInternalResponse(const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
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

		if (const CCardboardSubstrateSpecification* val = std::get_if<CCardboardSubstrateSpecification>(&replyPayload)){
			if (!val->WriteToModel(*dataModelPtr)){
				return nullptr;
			}
			if(dataModelPtr != nullptr){
				dataModelPtr->SetData("__typename", "CardboardSubstrateSpecification", 0);
			}
		}
		else if (const CPaperSubstrateSpecification* val = std::get_if<CPaperSubstrateSpecification>(&replyPayload)){
			if (!val->WriteToModel(*dataModelPtr)){
				return nullptr;
			}
			if(dataModelPtr != nullptr){
				dataModelPtr->SetData("__typename", "PaperSubstrateSpecification", 0);
			}
		}
		else if (const CFilmSubstrateSpecification* val = std::get_if<CFilmSubstrateSpecification>(&replyPayload)){
			if (!val->WriteToModel(*dataModelPtr)){
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

		imtbase::ImtBaseTypes::COptionsList replyPayload = OnGetOptionsList(getOptionsListGqlRequest, gqlRequest, errorMessage);
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


} // namespace sdl::substrate::SubstrateSpecifications
