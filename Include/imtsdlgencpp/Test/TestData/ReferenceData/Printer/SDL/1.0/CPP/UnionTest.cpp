#include "UnionTest.h"


namespace sdl::modsdl::UnionTest
{


QByteArray CCoords::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CCoords::V1_0::operator==(const V1_0& other) const
{
	return 
				X.has_value() == other.X.has_value() &&
				((X.has_value() && other.X.has_value()) ?
					qFuzzyCompare(*X, *other.X) : true) &&
				Y.has_value() == other.Y.has_value() &&
				((Y.has_value() && other.Y.has_value()) ?
					qFuzzyCompare(*Y, *other.Y) : true);
}


bool CCoords::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "Coords", modelIndex);

	if (!X){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "X").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("X", *X, modelIndex);

	if (!Y){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Y").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("Y", *Y, modelIndex);


	return true;
}


bool CCoords::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant xData = model.GetData("X", modelIndex);
	if (xData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "X").toLocal8Bit().constData();)

		return false;
	}
	X = xData.toDouble();

	QVariant yData = model.GetData("Y", modelIndex);
	if (yData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "Y").toLocal8Bit().constData();)

		return false;
	}
	Y = yData.toDouble();

	return true;
}


bool CCoords::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant xData = model.GetData("X", modelIndex);
	if (!xData.isNull()){
		X = xData.toDouble();
	}

	QVariant yData = model.GetData("Y", modelIndex);
	if (!yData.isNull()){
		Y = yData.toDouble();
	}

	return true;
}


bool CCoords::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!X){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "X").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("X", QVariant(*X));

	if (!Y){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Y").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("Y", QVariant(*Y));

	gqlObject.InsertParam("__typename", QVariant("Coords"));

	return true;
}


bool CCoords::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("X") || (gqlObject["X"].userType() != QMetaType::Float && gqlObject["X"].userType() != QMetaType::Double && gqlObject["X"].userType() != QMetaType::Int && gqlObject["X"].userType() != QMetaType::UInt && gqlObject["X"].userType() != QMetaType::LongLong && gqlObject["X"].userType() != QMetaType::ULongLong && gqlObject["X"].userType() != QMetaType::Long && gqlObject["X"].userType() != QMetaType::Short && gqlObject["X"].userType() != QMetaType::ULong && gqlObject["X"].userType() != QMetaType::UShort && gqlObject["X"].userType() != QMetaType::UChar)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "X").toLocal8Bit().constData();)

		return false;
	}
	X = gqlObject["X"].toDouble();

	if (!gqlObject.ContainsParam("Y") || (gqlObject["Y"].userType() != QMetaType::Float && gqlObject["Y"].userType() != QMetaType::Double && gqlObject["Y"].userType() != QMetaType::Int && gqlObject["Y"].userType() != QMetaType::UInt && gqlObject["Y"].userType() != QMetaType::LongLong && gqlObject["Y"].userType() != QMetaType::ULongLong && gqlObject["Y"].userType() != QMetaType::Long && gqlObject["Y"].userType() != QMetaType::Short && gqlObject["Y"].userType() != QMetaType::ULong && gqlObject["Y"].userType() != QMetaType::UShort && gqlObject["Y"].userType() != QMetaType::UChar)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Y").toLocal8Bit().constData();)

		return false;
	}
	Y = gqlObject["Y"].toDouble();

	return true;
}


bool CCoords::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("X") && (gqlObject["X"].userType() == QMetaType::Float || gqlObject["X"].userType() == QMetaType::Double || gqlObject["X"].userType() == QMetaType::Int || gqlObject["X"].userType() == QMetaType::UInt || gqlObject["X"].userType() == QMetaType::LongLong || gqlObject["X"].userType() == QMetaType::ULongLong || gqlObject["X"].userType() == QMetaType::Long || gqlObject["X"].userType() == QMetaType::Short || gqlObject["X"].userType() == QMetaType::ULong || gqlObject["X"].userType() == QMetaType::UShort || gqlObject["X"].userType() == QMetaType::UChar)){
		X = gqlObject["X"].toDouble();
	}

	if (gqlObject.ContainsParam("Y") && (gqlObject["Y"].userType() == QMetaType::Float || gqlObject["Y"].userType() == QMetaType::Double || gqlObject["Y"].userType() == QMetaType::Int || gqlObject["Y"].userType() == QMetaType::UInt || gqlObject["Y"].userType() == QMetaType::LongLong || gqlObject["Y"].userType() == QMetaType::ULongLong || gqlObject["Y"].userType() == QMetaType::Long || gqlObject["Y"].userType() == QMetaType::Short || gqlObject["Y"].userType() == QMetaType::ULong || gqlObject["Y"].userType() == QMetaType::UShort || gqlObject["Y"].userType() == QMetaType::UChar)){
		Y = gqlObject["Y"].toDouble();
	}

	return true;
}


bool CCoords::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!X){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "X").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["X"] = QJsonValue::fromVariant(*X);

	if (!Y){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Y").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["Y"] = QJsonValue::fromVariant(*Y);

	jsonObject["__typename"] = "Coords";

	return true;
}


bool CCoords::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("X") || ! jsonObject["X"].isDouble()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "X").toLocal8Bit().constData();)

		return false;
	}
	X = jsonObject["X"].toDouble();

	if (!jsonObject.contains("Y") || ! jsonObject["Y"].isDouble()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Y").toLocal8Bit().constData();)

		return false;
	}
	Y = jsonObject["Y"].toDouble();

	return true;
}


bool CCoords::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("X") && jsonObject["X"].isDouble()){
		X = jsonObject["X"].toDouble();
	}

	if (jsonObject.contains("Y") && jsonObject["Y"].isDouble()){
		Y = jsonObject["Y"].toDouble();
	}

	return true;
}


// serialize methods

bool CCoords::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
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


bool CCoords::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
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


bool CCoords::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
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


bool CCoords::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
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


bool CCoords::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
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


bool CCoords::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
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


bool CCoords::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
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


bool CCoords::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
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


bool CCoords::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
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




QByteArray CPrinterSpecificationBase::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CPrinterSpecificationBase::V1_0::operator==(const V1_0& other) const
{
	return 
				name == other.name;
}


bool CPrinterSpecificationBase::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "PrinterSpecificationBase", modelIndex);

	if (name){
		model.SetData("name", *name, modelIndex);
	}


	return true;
}


bool CPrinterSpecificationBase::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant nameData = model.GetData("name", modelIndex);
	if (!nameData.isNull()){
		name = nameData.toString();
	}

	return true;
}


bool CPrinterSpecificationBase::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant nameData = model.GetData("name", modelIndex);
	if (!nameData.isNull()){
		name = nameData.toString();
	}

	return true;
}


bool CPrinterSpecificationBase::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (name){
		gqlObject.InsertParam("name", QVariant(*name));
	}

	gqlObject.InsertParam("__typename", QVariant("PrinterSpecificationBase"));

	return true;
}


bool CPrinterSpecificationBase::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("name") && (gqlObject["name"].userType() == QMetaType::QString || gqlObject["name"].userType() == QMetaType::QByteArray)){
		name = gqlObject["name"].toString();
	}

	return true;
}


bool CPrinterSpecificationBase::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("name") && (gqlObject["name"].userType() == QMetaType::QString || gqlObject["name"].userType() == QMetaType::QByteArray)){
		name = gqlObject["name"].toString();
	}

	return true;
}


bool CPrinterSpecificationBase::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (name){
		jsonObject["name"] = QJsonValue::fromVariant(*name);
	}

	jsonObject["__typename"] = "PrinterSpecificationBase";

	return true;
}


bool CPrinterSpecificationBase::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("name") && jsonObject["name"].isString()){
		name = jsonObject["name"].toString();
	}

	return true;
}


bool CPrinterSpecificationBase::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("name") && jsonObject["name"].isString()){
		name = jsonObject["name"].toString();
	}

	return true;
}


// serialize methods

bool CPrinterSpecificationBase::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
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


bool CPrinterSpecificationBase::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
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


bool CPrinterSpecificationBase::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
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


bool CPrinterSpecificationBase::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
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


bool CPrinterSpecificationBase::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
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


bool CPrinterSpecificationBase::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
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


bool CPrinterSpecificationBase::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
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


bool CPrinterSpecificationBase::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
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


bool CPrinterSpecificationBase::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
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




QByteArray CLink::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CLink::V1_0::operator==(const V1_0& other) const
{
	return 
				link == other.link &&
				status == other.status &&
				statusList == other.statusList;
}


bool CLink::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "Link", modelIndex);

	if (!link){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "link").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("link", *link, modelIndex);

	if (status){
		QString statusStringValue;
		switch (*status){
		case LinkStatus::OPEN:
			statusStringValue = "OPEN";
			break;
		case LinkStatus::CLOSE:
			statusStringValue = "CLOSE";
			break;
		default:
			Q_ASSERT(false);
			break;
		}
		model.SetData("status", statusStringValue, modelIndex);
	}

	if (statusList){
		::imtbase::CTreeItemModel* newStatusListModelPtr = model.AddTreeModel("statusList", modelIndex);
		newStatusListModelPtr->setIsArray(true);
		for (qsizetype statusListIndex = 0; statusListIndex < statusList->size(); ++statusListIndex){
			QString statusListStringValue;
			switch (*statusList->at(statusListIndex)){
			case LinkStatus::OPEN:
				statusListStringValue = "OPEN";
				break;
			case LinkStatus::CLOSE:
				statusListStringValue = "CLOSE";
				break;
			default:
				Q_ASSERT(false);
				break;
			}
			newStatusListModelPtr->InsertNewItem();
			newStatusListModelPtr->SetData(QByteArray(), statusListStringValue, statusListIndex);
		}
	}


	return true;
}


bool CLink::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant linkData = model.GetData("link", modelIndex);
	if (linkData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "link").toLocal8Bit().constData();)

		return false;
	}
	link = linkData.toByteArray();

	QVariant statusData = model.GetData("status", modelIndex);
	if (!statusData.isNull()){
		QString statusStringValue = statusData.toString();
		if(statusStringValue == "OPEN"){
			status = LinkStatus::OPEN;
		}
		else if(statusStringValue == "CLOSE"){
			status = LinkStatus::CLOSE;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "LinkStatus", statusStringValue);)

			return false;
		}
	}

	::imtbase::CTreeItemModel* statusListModel = model.GetTreeItemModel("statusList", modelIndex);
	if (statusListModel != nullptr){
		int statusListCount = statusListModel->GetItemsCount();
		imtsdl::TElementList<LinkStatus> statusListList;
		for (int statusListIndex = 0; statusListIndex < statusListCount; ++statusListIndex){
			LinkStatus statusListData;
			QString statusListStringValue = statusListModel->GetData(QByteArray(), statusListIndex).toString();
			if(statusListStringValue == "OPEN"){
				statusListData = LinkStatus::OPEN;
			}
			else if(statusListStringValue == "CLOSE"){
				statusListData = LinkStatus::CLOSE;
			}
			else {
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "LinkStatus", statusListStringValue);)

				return false;
			}

			statusListList << statusListData;
		}
		statusList = statusListList;

	}

	return true;
}


bool CLink::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant linkData = model.GetData("link", modelIndex);
	if (!linkData.isNull()){
		link = linkData.toByteArray();
	}

	QVariant statusData = model.GetData("status", modelIndex);
	if (!statusData.isNull()){
		QString statusStringValue = statusData.toString();
		if(statusStringValue == "OPEN"){
			status = LinkStatus::OPEN;
		}
		else if(statusStringValue == "CLOSE"){
			status = LinkStatus::CLOSE;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "LinkStatus", statusStringValue);)

			return false;
		}
	}

	::imtbase::CTreeItemModel* statusListModel = model.GetTreeItemModel("statusList", modelIndex);
	if (statusListModel != nullptr){
		int statusListCount = statusListModel->GetItemsCount();
		imtsdl::TElementList<LinkStatus> statusListList;
		for (int statusListIndex = 0; statusListIndex < statusListCount; ++statusListIndex){
			LinkStatus statusListData;
			QString statusListStringValue = statusListModel->GetData(QByteArray(), statusListIndex).toString();
			if(statusListStringValue == "OPEN"){
				statusListData = LinkStatus::OPEN;
			}
			else if(statusListStringValue == "CLOSE"){
				statusListData = LinkStatus::CLOSE;
			}
			else {
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "LinkStatus", statusListStringValue);)

				return false;
			}

			statusListList << statusListData;
		}
		statusList = statusListList;

	}

	return true;
}


bool CLink::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!link){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "link").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("link", QVariant(*link));

	if (status){
		QString statusStringValue;
		switch (*status){
		case LinkStatus::OPEN:
			statusStringValue = "OPEN";
			break;
		case LinkStatus::CLOSE:
			statusStringValue = "CLOSE";
			break;
		default:
			Q_ASSERT(false);
			break;
		}
		gqlObject.InsertParam("status", QVariant(statusStringValue));
	}

	if (statusList){
		QVariantList statusListDataObjectList;
		for (qsizetype statusListIndex = 0; statusListIndex < statusList->size(); ++statusListIndex){
			QString statusListStringValue;
			switch (*statusList->at(statusListIndex)){
			case LinkStatus::OPEN:
				statusListStringValue = "OPEN";
				break;
			case LinkStatus::CLOSE:
				statusListStringValue = "CLOSE";
				break;
			default:
				Q_ASSERT(false);
				break;
			}
			statusListDataObjectList << statusListStringValue;
		}
		gqlObject.InsertParam("statusList", statusListDataObjectList);
	}

	gqlObject.InsertParam("__typename", QVariant("Link"));

	return true;
}


bool CLink::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("link") || (gqlObject["link"].userType() != QMetaType::QString && gqlObject["link"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "link").toLocal8Bit().constData();)

		return false;
	}
	link = gqlObject["link"].toByteArray();

	if (gqlObject.ContainsParam("status") && (gqlObject["status"].userType() == QMetaType::QString || gqlObject["status"].userType() == QMetaType::QByteArray)){
		const QString statusStringValue = gqlObject["status"].toString();
		if(statusStringValue == "OPEN"){
			status = LinkStatus::OPEN;
		}
		else if(statusStringValue == "CLOSE"){
			status = LinkStatus::CLOSE;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "LinkStatus", statusStringValue);)

			return false;
		}
	}

	if (gqlObject.ContainsParam("statusList")){
		statusList.emplace();
	}
	if (gqlObject.ContainsParam("statusList") && !(gqlObject["statusList"].isNull())){
		const QVariant statusListData = gqlObject["statusList"];
		const QVariantList statusListDataList = statusListData.toList();
		const qsizetype statusListElementsCount = statusListDataList.size();
		statusList = imtsdl::TElementList<modsdl::UnionTest::LinkStatus>();
		for (qsizetype statusListIndex = 0; statusListIndex < statusListElementsCount; ++statusListIndex){
			const QString tempStatusList = statusListDataList[statusListIndex].toString();
			modsdl::UnionTest::LinkStatus statusListDataValue;
			if(tempStatusList == "OPEN"){
				statusListDataValue = LinkStatus::OPEN;
			}
			else if(tempStatusList == "CLOSE"){
				statusListDataValue = LinkStatus::CLOSE;
			}
			else {
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "LinkStatus", tempStatusList);)

				return false;
			}
			statusList->append(statusListDataValue);
		}
	}

	return true;
}


bool CLink::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("link") && (gqlObject["link"].userType() == QMetaType::QString || gqlObject["link"].userType() == QMetaType::QByteArray)){
		link = gqlObject["link"].toByteArray();
	}

	if (gqlObject.ContainsParam("status") && (gqlObject["status"].userType() == QMetaType::QString || gqlObject["status"].userType() == QMetaType::QByteArray)){
		const QString statusStringValue = gqlObject["status"].toString();
		if(statusStringValue == "OPEN"){
			status = LinkStatus::OPEN;
		}
		else if(statusStringValue == "CLOSE"){
			status = LinkStatus::CLOSE;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "LinkStatus", statusStringValue);)

			return false;
		}
	}

	if (gqlObject.ContainsParam("statusList")){
		statusList.emplace();
	}
	if (gqlObject.ContainsParam("statusList") && !(gqlObject["statusList"].isNull())){
		const QVariant statusListData = gqlObject["statusList"];
		const QVariantList statusListDataList = statusListData.toList();
		const qsizetype statusListElementsCount = statusListDataList.size();
		statusList = imtsdl::TElementList<modsdl::UnionTest::LinkStatus>();
		for (qsizetype statusListIndex = 0; statusListIndex < statusListElementsCount; ++statusListIndex){
			const QString tempStatusList = statusListDataList[statusListIndex].toString();
			modsdl::UnionTest::LinkStatus statusListDataValue;
			if(tempStatusList == "OPEN"){
				statusListDataValue = LinkStatus::OPEN;
			}
			else if(tempStatusList == "CLOSE"){
				statusListDataValue = LinkStatus::CLOSE;
			}
			else {
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "LinkStatus", tempStatusList);)

				return false;
			}
			statusList->append(statusListDataValue);
		}
	}

	return true;
}


bool CLink::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!link){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "link").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["link"] = QJsonValue::fromVariant(*link);

	if (status){
		QString statusStringValue;
		switch (*status){
		case LinkStatus::OPEN:
			statusStringValue = "OPEN";
			break;
		case LinkStatus::CLOSE:
			statusStringValue = "CLOSE";
			break;
		default:
			Q_ASSERT(false);
			break;
		}
		jsonObject["status"] = QJsonValue::fromVariant(statusStringValue);
	}

	if (statusList){
		QJsonArray newStatusListArray;
		for (qsizetype statusListIndex = 0; statusListIndex < statusList->size(); ++statusListIndex){
			QString statusListStringValue;
			switch (*statusList->at(statusListIndex)){
			case LinkStatus::OPEN:
				statusListStringValue = "OPEN";
				break;
			case LinkStatus::CLOSE:
				statusListStringValue = "CLOSE";
				break;
			default:
				Q_ASSERT(false);
				break;
			}
			newStatusListArray << statusListStringValue;
		}
		jsonObject["statusList"] = newStatusListArray;
	}

	jsonObject["__typename"] = "Link";

	return true;
}


bool CLink::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("link") || ! jsonObject["link"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "link").toLocal8Bit().constData();)

		return false;
	}
	link = jsonObject["link"].toString().toUtf8();

	if (jsonObject.contains("status") && jsonObject["status"].isString()){
		const QString statusStringValue = jsonObject["status"].toString();
		if(statusStringValue == "OPEN"){
			status = LinkStatus::OPEN;
		}
		else if(statusStringValue == "CLOSE"){
			status = LinkStatus::CLOSE;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "LinkStatus", statusStringValue);)

			return false;
		}
	}

	if (jsonObject.contains("statusList") && jsonObject["statusList"].isArray()){
		const QJsonArray statusListJsonArray = jsonObject["statusList"].toArray();
		const qsizetype statusListArrayCount = statusListJsonArray.size();
		statusList = imtsdl::TElementList<modsdl::UnionTest::LinkStatus>();
		for (qsizetype statusListIndex = 0; statusListIndex < statusListArrayCount; ++statusListIndex){
			const QString tempStatusList = statusListJsonArray[statusListIndex].toString();
			modsdl::UnionTest::LinkStatus statusListDataValue;
			if(tempStatusList == "OPEN"){
				statusListDataValue = LinkStatus::OPEN;
			}
			else if(tempStatusList == "CLOSE"){
				statusListDataValue = LinkStatus::CLOSE;
			}
			else {
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "LinkStatus", tempStatusList);)

				return false;
			}
			statusList->append(statusListDataValue);
		}
	}

	return true;
}


bool CLink::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("link") && jsonObject["link"].isString()){
		link = jsonObject["link"].toString().toUtf8();
	}

	if (jsonObject.contains("status") && jsonObject["status"].isString()){
		const QString statusStringValue = jsonObject["status"].toString();
		if(statusStringValue == "OPEN"){
			status = LinkStatus::OPEN;
		}
		else if(statusStringValue == "CLOSE"){
			status = LinkStatus::CLOSE;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "LinkStatus", statusStringValue);)

			return false;
		}
	}

	if (jsonObject.contains("statusList") && jsonObject["statusList"].isArray()){
		const QJsonArray statusListJsonArray = jsonObject["statusList"].toArray();
		const qsizetype statusListArrayCount = statusListJsonArray.size();
		statusList = imtsdl::TElementList<modsdl::UnionTest::LinkStatus>();
		for (qsizetype statusListIndex = 0; statusListIndex < statusListArrayCount; ++statusListIndex){
			const QString tempStatusList = statusListJsonArray[statusListIndex].toString();
			modsdl::UnionTest::LinkStatus statusListDataValue;
			if(tempStatusList == "OPEN"){
				statusListDataValue = LinkStatus::OPEN;
			}
			else if(tempStatusList == "CLOSE"){
				statusListDataValue = LinkStatus::CLOSE;
			}
			else {
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "LinkStatus", tempStatusList);)

				return false;
			}
			statusList->append(statusListDataValue);
		}
	}

	return true;
}


// serialize methods

bool CLink::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
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


bool CLink::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
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


bool CLink::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
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


bool CLink::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
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


bool CLink::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
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


bool CLink::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
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


bool CLink::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
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


bool CLink::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
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


bool CLink::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
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




QByteArray CPrinterBase::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CPrinterBase::V1_0::operator==(const V1_0& other) const
{
	return 
				name == other.name &&
				linkList == other.linkList &&
				specification == other.specification &&
				specificationList == other.specificationList;
}


bool CPrinterBase::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "PrinterBase", modelIndex);

	if (name){
		model.SetData("name", *name, modelIndex);
	}

	if (!linkList){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "linkList").toLocal8Bit().constData();)

		return false;
	}
	::imtbase::CTreeItemModel* newLinkListModelPtr = model.AddTreeModel("linkList", modelIndex);
	newLinkListModelPtr->setIsArray(true);
	for (qsizetype linkListIndex = 0; linkListIndex < linkList->size(); ++linkListIndex){
		newLinkListModelPtr->InsertNewItem();
		if (!(linkList->at(linkListIndex)->WriteToModel(*newLinkListModelPtr, linkListIndex))){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "linkList").toLocal8Bit().constData();)

			return false;
		}
	}
	if (!specification){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "specification").toLocal8Bit().constData();)

		return false;
	}
	QVariant specificationVariantValue;
		if (const CPrinterSpecificationBase* val = std::get_if<CPrinterSpecificationBase>(specification.GetPtr())){
			if (!val->WriteToModel(*(model.AddTreeModel("specification", modelIndex)), 0)){
				return false;
			}
			if(model.GetTreeItemModel("specification", modelIndex) != nullptr){
				model.GetTreeItemModel("specification", modelIndex)->SetData("__typename", "PrinterSpecificationBase", 0);
			}
		}
		else if (const CLink* val = std::get_if<CLink>(specification.GetPtr())){
			if (!val->WriteToModel(*(model.AddTreeModel("specification", modelIndex)), 0)){
				return false;
			}
			if(model.GetTreeItemModel("specification", modelIndex) != nullptr){
				model.GetTreeItemModel("specification", modelIndex)->SetData("__typename", "Link", 0);
			}
		}
		else if (const QString* val = std::get_if<QString>(specification.GetPtr())){
			model.SetData("", *val);
		}
		else if (const double* val = std::get_if<double>(specification.GetPtr())){
			model.SetData("", *val);
		}


	::imtbase::CTreeItemModel* newSpecificationListModelPtr = model.AddTreeModel("specificationList", modelIndex);
	newSpecificationListModelPtr->setIsArray(true);
	for (qsizetype specificationListIndex = 0; specificationListIndex < specificationList->size(); ++specificationListIndex){
		QVariant specificationListVariantValue;
		if (const CPrinterSpecificationBase* val = std::get_if<CPrinterSpecificationBase>(specificationList->at(specificationListIndex).GetPtr())){
			newSpecificationListModelPtr->InsertNewItem();
			if(!val->WriteToModel(*newSpecificationListModelPtr, specificationListIndex)){
				return false;
			}
			newSpecificationListModelPtr->SetData("__typename", "PrinterSpecificationBase", specificationListIndex);
		}
		else if (const CLink* val = std::get_if<CLink>(specificationList->at(specificationListIndex).GetPtr())){
			newSpecificationListModelPtr->InsertNewItem();
			if(!val->WriteToModel(*newSpecificationListModelPtr, specificationListIndex)){
				return false;
			}
			newSpecificationListModelPtr->SetData("__typename", "Link", specificationListIndex);
		}
		else if (const QString* val = std::get_if<QString>(specificationList->at(specificationListIndex).GetPtr())){
			newSpecificationListModelPtr->InsertNewItem();
			newSpecificationListModelPtr->SetData("", *val);
		}
		else if (const double* val = std::get_if<double>(specificationList->at(specificationListIndex).GetPtr())){
			newSpecificationListModelPtr->InsertNewItem();
			newSpecificationListModelPtr->SetData("", *val);
		}
	}


	return true;
}


bool CPrinterBase::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant nameData = model.GetData("name", modelIndex);
	if (!nameData.isNull()){
		name = nameData.toString();
	}

	::imtbase::CTreeItemModel* linkListModel = model.GetTreeItemModel("linkList", modelIndex);
	if (linkListModel == nullptr){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' is missing, but required.").arg(__FILE__, QString::number(__LINE__), "linkList").toLocal8Bit().constData();)

		return false;
	}
	int linkListCount = linkListModel->GetItemsCount();
	imtsdl::TElementList<CLink::V1_0> linkListList;
	for (int linkListIndex = 0; linkListIndex < linkListCount; ++linkListIndex){
		CLink::V1_0 t_linkList;
		if (!t_linkList.ReadFromModel(*linkListModel, linkListIndex)){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "linkList").toLocal8Bit().constData();)

			return false;
		}
		linkListList << t_linkList;
	}
	linkList = linkListList;


	QVariant specificationData = model.GetData("specification", modelIndex);
	if (specificationData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "specification").toLocal8Bit().constData();)

		return false;
	}
	QString specificationTypename = specificationData.value<::imtbase::CTreeItemModel*>()->GetData("__typename").toString();
	if (specificationTypename == "PrinterSpecificationBase") {
		CPrinterSpecificationBase specificationConvert;
		const bool isspecificationRead = specificationConvert.ReadFromModel(*model.GetTreeItemModel("specification", modelIndex)); 
		if (!isspecificationRead){
			return false;
		}
		specification = PrinterSpecification(specificationConvert);
	}
	else if (specificationTypename == "Link") {
		CLink specificationConvert;
		const bool isspecificationRead = specificationConvert.ReadFromModel(*model.GetTreeItemModel("specification", modelIndex)); 
		if (!isspecificationRead){
			return false;
		}
		specification = PrinterSpecification(specificationConvert);
	}
	else if (specificationData.canConvert<QString>()){
		specification = PrinterSpecification(specificationData.value<QString>());
	}
	else if (specificationData.canConvert<double>()){
		specification = PrinterSpecification(specificationData.value<double>());
	}

	::imtbase::CTreeItemModel* specificationListModel = model.GetTreeItemModel("specificationList", modelIndex);
	if (specificationListModel == nullptr){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' is missing, but required").arg(__FILE__, QString::number(__LINE__), "specificationList").toLocal8Bit().constData();)

		return false;
	}
	int specificationListCount = specificationListModel->GetItemsCount();
	imtsdl::TElementList<PrinterSpecification> specificationListList;
	for (int specificationListIndex = 0; specificationListIndex < specificationListCount; ++specificationListIndex){
		PrinterSpecification specificationListData;
		QVariant specificationListVariantValue = specificationListModel->GetData(QByteArray(), specificationListIndex);
		QString specificationListDataTypename = specificationListModel->GetData("__typename", specificationListIndex).toString();
		if (specificationListDataTypename == "PrinterSpecificationBase") {
			CPrinterSpecificationBase specificationListDataConvert;
			const bool isspecificationListDataRead = specificationListDataConvert.ReadFromModel(*model.GetTreeItemModel("specificationList", specificationListIndex)); 
			if (!isspecificationListDataRead){
				return false;
			}
			specificationListData = PrinterSpecification(specificationListDataConvert);
		}
		else if (specificationListDataTypename == "Link") {
			CLink specificationListDataConvert;
			const bool isspecificationListDataRead = specificationListDataConvert.ReadFromModel(*model.GetTreeItemModel("specificationList", specificationListIndex)); 
			if (!isspecificationListDataRead){
				return false;
			}
			specificationListData = PrinterSpecification(specificationListDataConvert);
		}
		else if (specificationListVariantValue.canConvert<QString>()){
			specificationListData = PrinterSpecification(specificationListVariantValue.value<QString>());
		}
		else if (specificationListVariantValue.canConvert<double>()){
			specificationListData = PrinterSpecification(specificationListVariantValue.value<double>());
		}
		else{
			return false;
		}

		specificationListList << specificationListData;
	}
	specificationList = specificationListList;

	return true;
}


bool CPrinterBase::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant nameData = model.GetData("name", modelIndex);
	if (!nameData.isNull()){
		name = nameData.toString();
	}

	::imtbase::CTreeItemModel* linkListModel = model.GetTreeItemModel("linkList", modelIndex);
	if (linkListModel != nullptr){
		int linkListCount = linkListModel->GetItemsCount();
		imtsdl::TElementList<CLink::V1_0> linkListList;
		for (int linkListIndex = 0; linkListIndex < linkListCount; ++linkListIndex){
			CLink::V1_0 t_linkList;
			if (!t_linkList.OptReadFromModel(*linkListModel, linkListIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "linkList").toLocal8Bit().constData();)

				return false;
			}
			linkListList << t_linkList;
		}
		linkList = linkListList;

	}

	QVariant specificationData = model.GetData("specification", modelIndex);
	if (!specificationData.isNull()){
		QString specificationTypename = specificationData.value<::imtbase::CTreeItemModel*>()->GetData("__typename").toString();
		if (specificationTypename == "PrinterSpecificationBase") {
			CPrinterSpecificationBase specificationConvert;
			const bool isspecificationRead = specificationConvert.ReadFromModel(*model.GetTreeItemModel("specification", modelIndex)); 
			if (!isspecificationRead){
				return false;
			}
			specification = PrinterSpecification(specificationConvert);
		}
		else if (specificationTypename == "Link") {
			CLink specificationConvert;
			const bool isspecificationRead = specificationConvert.ReadFromModel(*model.GetTreeItemModel("specification", modelIndex)); 
			if (!isspecificationRead){
				return false;
			}
			specification = PrinterSpecification(specificationConvert);
		}
		else if (specificationData.canConvert<QString>()){
			specification = PrinterSpecification(specificationData.value<QString>());
		}
		else if (specificationData.canConvert<double>()){
			specification = PrinterSpecification(specificationData.value<double>());
		}
	}

	::imtbase::CTreeItemModel* specificationListModel = model.GetTreeItemModel("specificationList", modelIndex);
	if (specificationListModel != nullptr){
		int specificationListCount = specificationListModel->GetItemsCount();
		imtsdl::TElementList<PrinterSpecification> specificationListList;
		for (int specificationListIndex = 0; specificationListIndex < specificationListCount; ++specificationListIndex){
			PrinterSpecification specificationListData;
			QVariant specificationListVariantValue = specificationListModel->GetData(QByteArray(), specificationListIndex);
			QString specificationListDataTypename = specificationListModel->GetData("__typename", specificationListIndex).toString();
			if (specificationListDataTypename == "PrinterSpecificationBase") {
				CPrinterSpecificationBase specificationListDataConvert;
				const bool isspecificationListDataRead = specificationListDataConvert.ReadFromModel(*model.GetTreeItemModel("specificationList", specificationListIndex)); 
				if (!isspecificationListDataRead){
					return false;
				}
				specificationListData = PrinterSpecification(specificationListDataConvert);
			}
			else if (specificationListDataTypename == "Link") {
				CLink specificationListDataConvert;
				const bool isspecificationListDataRead = specificationListDataConvert.ReadFromModel(*model.GetTreeItemModel("specificationList", specificationListIndex)); 
				if (!isspecificationListDataRead){
					return false;
				}
				specificationListData = PrinterSpecification(specificationListDataConvert);
			}
			else if (specificationListVariantValue.canConvert<QString>()){
				specificationListData = PrinterSpecification(specificationListVariantValue.value<QString>());
			}
			else if (specificationListVariantValue.canConvert<double>()){
				specificationListData = PrinterSpecification(specificationListVariantValue.value<double>());
			}
			else{
				return false;
			}

			specificationListList << specificationListData;
		}
		specificationList = specificationListList;

	}

	return true;
}


bool CPrinterBase::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (name){
		gqlObject.InsertParam("name", QVariant(*name));
	}

	if (!linkList){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "linkList").toLocal8Bit().constData();)

		return false;
	}
	QList<::imtgql::CGqlParamObject> linkListDataObjectList;
	for (qsizetype linkListIndex = 0; linkListIndex < linkList->size(); ++linkListIndex){
		::imtgql::CGqlParamObject newLinkListGqlObject;
		if (!linkList->at(linkListIndex)->WriteToGraphQlObject(newLinkListGqlObject)){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "linkList").toLocal8Bit().constData();)

			return false;
		}
		linkListDataObjectList << newLinkListGqlObject;
	}
	gqlObject.InsertParam("linkList", linkListDataObjectList);

	if (!specification){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "specification").toLocal8Bit().constData();)

		return false;
	}
	::imtgql::CGqlParamObject specificationDataObject;
	if (const CPrinterSpecificationBase* val = std::get_if<CPrinterSpecificationBase>(specification.GetPtr())){
		if (!val->WriteToGraphQlObject(specificationDataObject)){
			return false;
		}
		specificationDataObject.InsertParam("__typename", QVariant("PrinterSpecificationBase"));
	}
	else if (const CLink* val = std::get_if<CLink>(specification.GetPtr())){
		if (!val->WriteToGraphQlObject(specificationDataObject)){
			return false;
		}
		specificationDataObject.InsertParam("__typename", QVariant("Link"));
	}
	else if (const QString* val = std::get_if<QString>(specification.GetPtr())){
		specificationDataObject.InsertParam("specification", *val);
	}
	else if (const double* val = std::get_if<double>(specification.GetPtr())){
		specificationDataObject.InsertParam("specification", *val);
	}
	gqlObject.InsertParam("specification", specificationDataObject);

	QList<::imtgql::CGqlParamObject> specificationListDataObjectList;
	for (qsizetype specificationListIndex = 0; specificationListIndex < specificationList->size(); ++specificationListIndex){
		::imtgql::CGqlParamObject specificationListDataObject;
		if (const CPrinterSpecificationBase* val = std::get_if<CPrinterSpecificationBase>(specificationList->at(specificationListIndex).GetPtr())){
			if (!val->WriteToGraphQlObject(specificationListDataObject)){
				return false;
			}
			specificationListDataObject.InsertParam("__typename", QVariant("PrinterSpecificationBase"));
		}
		else if (const CLink* val = std::get_if<CLink>(specificationList->at(specificationListIndex).GetPtr())){
			if (!val->WriteToGraphQlObject(specificationListDataObject)){
				return false;
			}
			specificationListDataObject.InsertParam("__typename", QVariant("Link"));
		}
		else if (const QString* val = std::get_if<QString>(specificationList->at(specificationListIndex).GetPtr())){
			specificationListDataObject.InsertParam("specificationList", *val);
		}
		else if (const double* val = std::get_if<double>(specificationList->at(specificationListIndex).GetPtr())){
			specificationListDataObject.InsertParam("specificationList", *val);
		}
		specificationListDataObjectList << specificationListDataObject;
	}
	gqlObject.InsertParam("specificationList", specificationListDataObjectList);

	gqlObject.InsertParam("__typename", QVariant("PrinterBase"));

	return true;
}


bool CPrinterBase::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("name") && (gqlObject["name"].userType() == QMetaType::QString || gqlObject["name"].userType() == QMetaType::QByteArray)){
		name = gqlObject["name"].toString();
	}

	if (!gqlObject.ContainsParam("linkList")){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "linkList").toLocal8Bit().constData();)

		return false;
	}
	const qsizetype linkListElementsCount = gqlObject.GetObjectsCount("linkList");
	linkList = imtsdl::TElementList<CLink::V1_0>();
	for (qsizetype linkListIndex = 0; linkListIndex < linkListElementsCount; ++linkListIndex){
		const ::imtgql::CGqlParamObject* linkListDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("linkList", linkListIndex);
		if (linkListDataObjectPtr == nullptr){
			qDebug() << "invalid type" << linkListDataObjectPtr;
			return false;
		}
		CLink::V1_0 tempLinkList;
		if (!tempLinkList.ReadFromGraphQlObject(*linkListDataObjectPtr)){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "linkList").toLocal8Bit().constData();)

			return false;
		}
		linkList->append(tempLinkList);
	}

	if (!gqlObject.ContainsParam("specification")){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "specification").toLocal8Bit().constData();)

		return false;
	}
	const ::imtgql::CGqlParamObject* specificationDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("specification");
	if (!specificationDataObjectPtr->ContainsParam("__typename")){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "specification").toLocal8Bit().constData();)

		return false;
	}
	QString specificationTypename = specificationDataObjectPtr->GetParamArgumentValue("__typename").toString();
	QVariant specificationVariantValue = gqlObject["specification"];
	if (specificationTypename == "PrinterSpecificationBase") {
		CPrinterSpecificationBase specificationConvert;
		const bool isSpecificationRead = specificationConvert.ReadFromGraphQlObject(*specificationDataObjectPtr);
		if (!isSpecificationRead){
			return false;
		}
		specification = PrinterSpecification(specificationConvert);
	}
	else if (specificationTypename == "Link") {
		CLink specificationConvert;
		const bool isSpecificationRead = specificationConvert.ReadFromGraphQlObject(*specificationDataObjectPtr);
		if (!isSpecificationRead){
			return false;
		}
		specification = PrinterSpecification(specificationConvert);
	}
	else if (specificationVariantValue.canConvert<QString>()){
		specification = PrinterSpecification(specificationVariantValue.value<QString>());
	}
	else if (specificationVariantValue.canConvert<double>()){
		specification = PrinterSpecification(specificationVariantValue.value<double>());
	}

	if (!gqlObject.ContainsParam("specificationList")){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "specificationList").toLocal8Bit().constData();)

		return false;
	}
	const QList<const ::imtgql::CGqlParamObject*> specificationListDataList = gqlObject.GetParamArgumentObjectPtrList("specificationList");
	const qsizetype specificationListElementsCount = specificationListDataList.size();
	specificationList = imtsdl::TElementList<modsdl::UnionTest::PrinterSpecification>();
	for (qsizetype specificationListIndex = 0; specificationListIndex < specificationListElementsCount; ++specificationListIndex){
		const ::imtgql::CGqlParamObject* tempSpecificationList = specificationListDataList[specificationListIndex];
		if (tempSpecificationList == nullptr){
			return false;
		}
		QString specificationListDataValueTypename = tempSpecificationList->GetParamArgumentValue("__typename").toString();
		modsdl::UnionTest::PrinterSpecification specificationListDataValue;
		if (specificationListDataValueTypename == "PrinterSpecificationBase") {
			CPrinterSpecificationBase specificationListDataValueConvert;
			const bool isSpecificationListRead = specificationListDataValueConvert.ReadFromGraphQlObject(*tempSpecificationList);
			if (!isSpecificationListRead){
				return false;
			}
			specificationListDataValue = PrinterSpecification(specificationListDataValueConvert);
		}
		else if (specificationListDataValueTypename == "Link") {
			CLink specificationListDataValueConvert;
			const bool isSpecificationListRead = specificationListDataValueConvert.ReadFromGraphQlObject(*tempSpecificationList);
			if (!isSpecificationListRead){
				return false;
			}
			specificationListDataValue = PrinterSpecification(specificationListDataValueConvert);
		}
		else if (tempSpecificationList->GetParamArgumentValue("").canConvert<QString>()){
			specificationListDataValue = PrinterSpecification(tempSpecificationList->GetParamArgumentValue("").value<QString>());
		}
		else if (tempSpecificationList->GetParamArgumentValue("").canConvert<double>()){
			specificationListDataValue = PrinterSpecification(tempSpecificationList->GetParamArgumentValue("").value<double>());
		}
		else{
			return false;
		}
		specificationList->append(specificationListDataValue);
	}

	return true;
}


bool CPrinterBase::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("name") && (gqlObject["name"].userType() == QMetaType::QString || gqlObject["name"].userType() == QMetaType::QByteArray)){
		name = gqlObject["name"].toString();
	}

	if (gqlObject.ContainsParam("linkList")){
		linkList.emplace();
	}
	if (gqlObject.ContainsParam("linkList") && (gqlObject.GetObjectsCount("linkList") > 0)){
		const qsizetype linkListElementsCount = gqlObject.GetObjectsCount("linkList");
		linkList = imtsdl::TElementList<CLink::V1_0>();
		for (qsizetype linkListIndex = 0; linkListIndex < linkListElementsCount; ++linkListIndex){
			const ::imtgql::CGqlParamObject* linkListDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("linkList", linkListIndex);
			if (linkListDataObjectPtr == nullptr){
				qDebug() << "invalid type" << linkListDataObjectPtr;
				return false;
			}
			CLink::V1_0 tempLinkList;
			if (!tempLinkList.OptReadFromGraphQlObject(*linkListDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "linkList").toLocal8Bit().constData();)

				return false;
			}
			linkList->append(tempLinkList);
		}
	}

	if (gqlObject.ContainsParam("specification")){
		const ::imtgql::CGqlParamObject* specificationDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("specification");
		if (!specificationDataObjectPtr->ContainsParam("__typename")){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "specification").toLocal8Bit().constData();)

			return false;
		}
		QString specificationTypename = specificationDataObjectPtr->GetParamArgumentValue("__typename").toString();
		QVariant specificationVariantValue = gqlObject["specification"];
		if (specificationTypename == "PrinterSpecificationBase") {
			CPrinterSpecificationBase specificationConvert;
			const bool isSpecificationRead = specificationConvert.ReadFromGraphQlObject(*specificationDataObjectPtr);
			if (!isSpecificationRead){
				return false;
			}
			specification = PrinterSpecification(specificationConvert);
		}
		else if (specificationTypename == "Link") {
			CLink specificationConvert;
			const bool isSpecificationRead = specificationConvert.ReadFromGraphQlObject(*specificationDataObjectPtr);
			if (!isSpecificationRead){
				return false;
			}
			specification = PrinterSpecification(specificationConvert);
		}
		else if (specificationVariantValue.canConvert<QString>()){
			specification = PrinterSpecification(specificationVariantValue.value<QString>());
		}
		else if (specificationVariantValue.canConvert<double>()){
			specification = PrinterSpecification(specificationVariantValue.value<double>());
		}
	}

	if (gqlObject.ContainsParam("specificationList")){
		specificationList.emplace();
	}
	if (gqlObject.ContainsParam("specificationList")){
		const QList<const ::imtgql::CGqlParamObject*> specificationListDataList = gqlObject.GetParamArgumentObjectPtrList("specificationList");
		const qsizetype specificationListElementsCount = specificationListDataList.size();
		specificationList = imtsdl::TElementList<modsdl::UnionTest::PrinterSpecification>();
		for (qsizetype specificationListIndex = 0; specificationListIndex < specificationListElementsCount; ++specificationListIndex){
			const ::imtgql::CGqlParamObject* tempSpecificationList = specificationListDataList[specificationListIndex];
			if (tempSpecificationList == nullptr){
				return false;
			}
			QString specificationListDataValueTypename = tempSpecificationList->GetParamArgumentValue("__typename").toString();
			modsdl::UnionTest::PrinterSpecification specificationListDataValue;
			if (specificationListDataValueTypename == "PrinterSpecificationBase") {
				CPrinterSpecificationBase specificationListDataValueConvert;
				const bool isSpecificationListRead = specificationListDataValueConvert.ReadFromGraphQlObject(*tempSpecificationList);
				if (!isSpecificationListRead){
					return false;
				}
				specificationListDataValue = PrinterSpecification(specificationListDataValueConvert);
			}
			else if (specificationListDataValueTypename == "Link") {
				CLink specificationListDataValueConvert;
				const bool isSpecificationListRead = specificationListDataValueConvert.ReadFromGraphQlObject(*tempSpecificationList);
				if (!isSpecificationListRead){
					return false;
				}
				specificationListDataValue = PrinterSpecification(specificationListDataValueConvert);
			}
			else if (tempSpecificationList->GetParamArgumentValue("").canConvert<QString>()){
				specificationListDataValue = PrinterSpecification(tempSpecificationList->GetParamArgumentValue("").value<QString>());
			}
			else if (tempSpecificationList->GetParamArgumentValue("").canConvert<double>()){
				specificationListDataValue = PrinterSpecification(tempSpecificationList->GetParamArgumentValue("").value<double>());
			}
			else{
				return false;
			}
			specificationList->append(specificationListDataValue);
		}
	}

	return true;
}


bool CPrinterBase::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (name){
		jsonObject["name"] = QJsonValue::fromVariant(*name);
	}

	if (!linkList){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "linkList").toLocal8Bit().constData();)

		return false;
	}
	QJsonArray newLinkListArray;
	for (qsizetype linkListIndex = 0; linkListIndex < linkList->size(); ++linkListIndex){
		QJsonObject newLinkListJsonObject;
		if (!linkList->at(linkListIndex)->WriteToJsonObject(newLinkListJsonObject)){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "linkList").toLocal8Bit().constData();)

			return false;
		}
		newLinkListArray << newLinkListJsonObject;
	}
	jsonObject["linkList"] = newLinkListArray;

	if (!specification){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "specification").toLocal8Bit().constData();)

		return false;
	}
	if (const CPrinterSpecificationBase* val = std::get_if<CPrinterSpecificationBase>(specification.GetPtr())){
		QJsonObject specificationJsonObject;
		const bool isspecificationAdded = val->WriteToJsonObject(specificationJsonObject);
		if (!isspecificationAdded){
			return false;
		}
		specificationJsonObject["__typename"] = "PrinterSpecificationBase";
		jsonObject["specification"] = specificationJsonObject;
	}
	else if (const CLink* val = std::get_if<CLink>(specification.GetPtr())){
		QJsonObject specificationJsonObject;
		const bool isspecificationAdded = val->WriteToJsonObject(specificationJsonObject);
		if (!isspecificationAdded){
			return false;
		}
		specificationJsonObject["__typename"] = "Link";
		jsonObject["specification"] = specificationJsonObject;
	}
	else if (const QString* val = std::get_if<QString>(specification.GetPtr())){
		if (!specification){
			return false;
		}
		jsonObject["specification"] = QJsonValue::fromVariant(*val);
	}
	else if (const double* val = std::get_if<double>(specification.GetPtr())){
		if (!specification){
			return false;
		}
		jsonObject["specification"] = QJsonValue::fromVariant(*val);
	}


	QJsonArray newSpecificationListArray;
	for (qsizetype specificationListIndex = 0; specificationListIndex < specificationList->size(); ++specificationListIndex){
		if (const CPrinterSpecificationBase* val = std::get_if<CPrinterSpecificationBase>(specificationList->at(specificationListIndex).GetPtr())){
			QJsonObject specificationListJsonObject;
			const bool isspecificationListAdded = val->WriteToJsonObject(specificationListJsonObject);
			if (!isspecificationListAdded){
				return false;
			}
			specificationListJsonObject["__typename"] = "PrinterSpecificationBase";
			newSpecificationListArray << specificationListJsonObject;
		}
		else if (const CLink* val = std::get_if<CLink>(specificationList->at(specificationListIndex).GetPtr())){
			QJsonObject specificationListJsonObject;
			const bool isspecificationListAdded = val->WriteToJsonObject(specificationListJsonObject);
			if (!isspecificationListAdded){
				return false;
			}
			specificationListJsonObject["__typename"] = "Link";
			newSpecificationListArray << specificationListJsonObject;
		}
		else if (const QString* val = std::get_if<QString>(specificationList->at(specificationListIndex).GetPtr())){
			if (!specificationList){
				return false;
			}
			newSpecificationListArray << QJsonValue::fromVariant(*val);
		}
		else if (const double* val = std::get_if<double>(specificationList->at(specificationListIndex).GetPtr())){
			if (!specificationList){
				return false;
			}
			newSpecificationListArray << QJsonValue::fromVariant(*val);
		}
	}
	jsonObject["specificationList"] = newSpecificationListArray;

	jsonObject["__typename"] = "PrinterBase";

	return true;
}


bool CPrinterBase::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("name") && jsonObject["name"].isString()){
		name = jsonObject["name"].toString();
	}

	if (!jsonObject.contains("linkList") || ! jsonObject["linkList"].isArray()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "linkList").toLocal8Bit().constData();)

		return false;
	}
	const QJsonArray linkListJsonArray = jsonObject["linkList"].toArray();
	const qsizetype linkListArrayCount = linkListJsonArray.size();
	linkList = imtsdl::TElementList<CLink::V1_0>();
	for (qsizetype linkListIndex = 0; linkListIndex < linkListArrayCount; ++linkListIndex){
		CLink::V1_0 tempLinkList;
		if (!tempLinkList.ReadFromJsonObject(linkListJsonArray[linkListIndex].toObject())){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "linkList").toLocal8Bit().constData();)

			return false;
		}
		linkList->append(tempLinkList);
	}

	if (!jsonObject.contains("specification")){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "specification").toLocal8Bit().constData();)

		return false;
	}
	const QJsonObject specificationObject = jsonObject.value("specification").toObject();
	if (!jsonObject.value("specification").isObject() || !specificationObject.contains("__typename")){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "specification").toLocal8Bit().constData();)

		return false;
	}
	QString specificationTypename = specificationObject.value("__typename").toString();
	QVariant specificationVariantValue = jsonObject["specification"].toVariant();
	if (specificationTypename == "PrinterSpecificationBase") {
		CPrinterSpecificationBase specificationConvert;
		const bool isspecificationRead = specificationConvert.ReadFromJsonObject(jsonObject["specification"].toObject());
		if (!isspecificationRead){
			return false;
		}
		specification = PrinterSpecification(specificationConvert);
	}
	else if (specificationTypename == "Link") {
		CLink specificationConvert;
		const bool isspecificationRead = specificationConvert.ReadFromJsonObject(jsonObject["specification"].toObject());
		if (!isspecificationRead){
			return false;
		}
		specification = PrinterSpecification(specificationConvert);
	}
	else if (specificationVariantValue.canConvert<QString>()){
		specification = PrinterSpecification(specificationVariantValue.value<QString>());
	}
	else if (specificationVariantValue.canConvert<double>()){
		specification = PrinterSpecification(specificationVariantValue.value<double>());
	}

	if (!jsonObject.contains("specificationList")){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "specificationList").toLocal8Bit().constData();)

		return false;
	}
	const QJsonArray specificationListJsonArray = jsonObject["specificationList"].toArray();
	const qsizetype specificationListArrayCount = specificationListJsonArray.size();
	specificationList = imtsdl::TElementList<modsdl::UnionTest::PrinterSpecification>();
	for (qsizetype specificationListIndex = 0; specificationListIndex < specificationListArrayCount; ++specificationListIndex){
		const QVariant tempSpecificationList = specificationListJsonArray[specificationListIndex].toVariant();
		QString specificationListDataValueTypename = specificationListJsonArray[specificationListIndex].toObject().value("__typename").toString();
		modsdl::UnionTest::PrinterSpecification specificationListDataValue;
		if (specificationListDataValueTypename == "PrinterSpecificationBase") {
			CPrinterSpecificationBase specificationListDataValueConvert;
			const bool isspecificationListDataValueRead = specificationListDataValueConvert.ReadFromJsonObject(specificationListJsonArray[specificationListIndex].toObject());
			if (!isspecificationListDataValueRead){
				return false;
			}
			specificationListDataValue = PrinterSpecification(specificationListDataValueConvert);
		}
		else if (specificationListDataValueTypename == "Link") {
			CLink specificationListDataValueConvert;
			const bool isspecificationListDataValueRead = specificationListDataValueConvert.ReadFromJsonObject(specificationListJsonArray[specificationListIndex].toObject());
			if (!isspecificationListDataValueRead){
				return false;
			}
			specificationListDataValue = PrinterSpecification(specificationListDataValueConvert);
		}
		else if (tempSpecificationList.canConvert<QString>()){
			specificationListDataValue = PrinterSpecification(tempSpecificationList.value<QString>());
		}
		else if (tempSpecificationList.canConvert<double>()){
			specificationListDataValue = PrinterSpecification(tempSpecificationList.value<double>());
		}
		else{
			return false;
		}
		specificationList->append(specificationListDataValue);
	}

	return true;
}


bool CPrinterBase::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("name") && jsonObject["name"].isString()){
		name = jsonObject["name"].toString();
	}

	if (jsonObject.contains("linkList") && jsonObject["linkList"].isArray()){
		const QJsonArray linkListJsonArray = jsonObject["linkList"].toArray();
		const qsizetype linkListArrayCount = linkListJsonArray.size();
		linkList = imtsdl::TElementList<CLink::V1_0>();
		for (qsizetype linkListIndex = 0; linkListIndex < linkListArrayCount; ++linkListIndex){
			CLink::V1_0 tempLinkList;
			if (!tempLinkList.OptReadFromJsonObject(linkListJsonArray[linkListIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "linkList").toLocal8Bit().constData();)

				return false;
			}
			linkList->append(tempLinkList);
		}
	}

	if (jsonObject.contains("specification")){
		const QJsonObject specificationObject = jsonObject.value("specification").toObject();
		if (!jsonObject.value("specification").isObject() || !specificationObject.contains("__typename")){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "specification").toLocal8Bit().constData();)

			return false;
		}
		QString specificationTypename = specificationObject.value("__typename").toString();
		QVariant specificationVariantValue = jsonObject["specification"].toVariant();
		if (specificationTypename == "PrinterSpecificationBase") {
			CPrinterSpecificationBase specificationConvert;
			const bool isspecificationRead = specificationConvert.ReadFromJsonObject(jsonObject["specification"].toObject());
			if (!isspecificationRead){
				return false;
			}
			specification = PrinterSpecification(specificationConvert);
		}
		else if (specificationTypename == "Link") {
			CLink specificationConvert;
			const bool isspecificationRead = specificationConvert.ReadFromJsonObject(jsonObject["specification"].toObject());
			if (!isspecificationRead){
				return false;
			}
			specification = PrinterSpecification(specificationConvert);
		}
		else if (specificationVariantValue.canConvert<QString>()){
			specification = PrinterSpecification(specificationVariantValue.value<QString>());
		}
		else if (specificationVariantValue.canConvert<double>()){
			specification = PrinterSpecification(specificationVariantValue.value<double>());
		}
	}

	if (jsonObject.contains("specificationList")){
		const QJsonArray specificationListJsonArray = jsonObject["specificationList"].toArray();
		const qsizetype specificationListArrayCount = specificationListJsonArray.size();
		specificationList = imtsdl::TElementList<modsdl::UnionTest::PrinterSpecification>();
		for (qsizetype specificationListIndex = 0; specificationListIndex < specificationListArrayCount; ++specificationListIndex){
			const QVariant tempSpecificationList = specificationListJsonArray[specificationListIndex].toVariant();
			QString specificationListDataValueTypename = specificationListJsonArray[specificationListIndex].toObject().value("__typename").toString();
			modsdl::UnionTest::PrinterSpecification specificationListDataValue;
			if (specificationListDataValueTypename == "PrinterSpecificationBase") {
				CPrinterSpecificationBase specificationListDataValueConvert;
				const bool isspecificationListDataValueRead = specificationListDataValueConvert.ReadFromJsonObject(specificationListJsonArray[specificationListIndex].toObject());
				if (!isspecificationListDataValueRead){
					return false;
				}
				specificationListDataValue = PrinterSpecification(specificationListDataValueConvert);
			}
			else if (specificationListDataValueTypename == "Link") {
				CLink specificationListDataValueConvert;
				const bool isspecificationListDataValueRead = specificationListDataValueConvert.ReadFromJsonObject(specificationListJsonArray[specificationListIndex].toObject());
				if (!isspecificationListDataValueRead){
					return false;
				}
				specificationListDataValue = PrinterSpecification(specificationListDataValueConvert);
			}
			else if (tempSpecificationList.canConvert<QString>()){
				specificationListDataValue = PrinterSpecification(tempSpecificationList.value<QString>());
			}
			else if (tempSpecificationList.canConvert<double>()){
				specificationListDataValue = PrinterSpecification(tempSpecificationList.value<double>());
			}
			else{
				return false;
			}
			specificationList->append(specificationListDataValue);
		}
	}

	return true;
}


// serialize methods

bool CPrinterBase::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
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


bool CPrinterBase::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
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


bool CPrinterBase::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
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


bool CPrinterBase::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
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


bool CPrinterBase::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
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


bool CPrinterBase::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
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


bool CPrinterBase::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
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


bool CPrinterBase::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
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


bool CPrinterBase::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
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




CCoordsObject::CCoordsObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &CCoordsObject::xChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCoordsObject::yChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CCoordsObject::GetX()
{
	if (Version_1_0 && Version_1_0->X){
		return Version_1_0->X.value();
	}

	return QVariant();
}


void CCoordsObject::SetX(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->X = v.value<double>();
	xChanged();
}


bool CCoordsObject::hasX()
{
	 return Version_1_0 && Version_1_0->X.HasValue();
}


QVariant CCoordsObject::GetY()
{
	if (Version_1_0 && Version_1_0->Y){
		return Version_1_0->Y.value();
	}

	return QVariant();
}


void CCoordsObject::SetY(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->Y = v.value<double>();
	yChanged();
}


bool CCoordsObject::hasY()
{
	 return Version_1_0 && Version_1_0->Y.HasValue();
}


QString CCoordsObject::toJson() const
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


bool CCoordsObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CCoordsObject::fromObject(const QJsonObject& jsonObject)
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


QString CCoordsObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CCoordsObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CCoordsObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "x")){
		return "X";
	}
	if (propertyName == (QString("m_") + "y")){
		return "Y";
	}

	return propertyName;
}





bool CCoordsObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CCoordsObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CCoordsObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CCoordsObjectList::append(sdl::modsdl::UnionTest::CCoordsObject* item)
{
	BaseClass::append(item);
}


sdl::modsdl::UnionTest::CCoordsObjectList* sdl::modsdl::UnionTest::CCoordsObjectList::copyMe()
{
	sdl::modsdl::UnionTest::CCoordsObjectList* retVal = new sdl::modsdl::UnionTest::CCoordsObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::modsdl::UnionTest::CCoordsObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::modsdl::UnionTest::CCoordsObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::modsdl::UnionTest::CCoordsObjectList::addElement(sdl::modsdl::UnionTest::CCoordsObject* item)
{
	append(item);
}


void sdl::modsdl::UnionTest::CCoordsObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::modsdl::UnionTest::CCoordsObjectList::isEqualWithModel(sdl::modsdl::UnionTest::CCoordsObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::modsdl::UnionTest::CCoordsObjectList::insert(int index, sdl::modsdl::UnionTest::CCoordsObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::modsdl::UnionTest::CCoordsObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::modsdl::UnionTest::CCoordsObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::modsdl::UnionTest::CCoordsObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::modsdl::UnionTest::CCoordsObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_x"){
		return QVariant::fromValue(Version_1_0->at(index)->X.value());
	}
	if (nameId == "m_y"){
		return QVariant::fromValue(Version_1_0->at(index)->Y.value());
	}

	return QVariant();
}
CPrinterSpecificationBaseObject::CPrinterSpecificationBaseObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &CPrinterSpecificationBaseObject::nameChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CPrinterSpecificationBaseObject::GetName()
{
	if (Version_1_0 && Version_1_0->name){
		return Version_1_0->name.value();
	}

	return QVariant();
}


void CPrinterSpecificationBaseObject::SetName(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->name = v.value<QString>();
	nameChanged();
}


bool CPrinterSpecificationBaseObject::hasName()
{
	 return Version_1_0 && Version_1_0->name.HasValue();
}


QString CPrinterSpecificationBaseObject::toJson() const
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


bool CPrinterSpecificationBaseObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CPrinterSpecificationBaseObject::fromObject(const QJsonObject& jsonObject)
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


QString CPrinterSpecificationBaseObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CPrinterSpecificationBaseObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CPrinterSpecificationBaseObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "name")){
		return "name";
	}

	return propertyName;
}





bool CPrinterSpecificationBaseObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CPrinterSpecificationBaseObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CPrinterSpecificationBaseObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CPrinterSpecificationBaseObjectList::append(sdl::modsdl::UnionTest::CPrinterSpecificationBaseObject* item)
{
	BaseClass::append(item);
}


sdl::modsdl::UnionTest::CPrinterSpecificationBaseObjectList* sdl::modsdl::UnionTest::CPrinterSpecificationBaseObjectList::copyMe()
{
	sdl::modsdl::UnionTest::CPrinterSpecificationBaseObjectList* retVal = new sdl::modsdl::UnionTest::CPrinterSpecificationBaseObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::modsdl::UnionTest::CPrinterSpecificationBaseObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::modsdl::UnionTest::CPrinterSpecificationBaseObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::modsdl::UnionTest::CPrinterSpecificationBaseObjectList::addElement(sdl::modsdl::UnionTest::CPrinterSpecificationBaseObject* item)
{
	append(item);
}


void sdl::modsdl::UnionTest::CPrinterSpecificationBaseObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::modsdl::UnionTest::CPrinterSpecificationBaseObjectList::isEqualWithModel(sdl::modsdl::UnionTest::CPrinterSpecificationBaseObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::modsdl::UnionTest::CPrinterSpecificationBaseObjectList::insert(int index, sdl::modsdl::UnionTest::CPrinterSpecificationBaseObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::modsdl::UnionTest::CPrinterSpecificationBaseObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::modsdl::UnionTest::CPrinterSpecificationBaseObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::modsdl::UnionTest::CPrinterSpecificationBaseObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::modsdl::UnionTest::CPrinterSpecificationBaseObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_name"){
		return QVariant::fromValue(Version_1_0->at(index)->name.value());
	}

	return QVariant();
}
CLinkObject::CLinkObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &CLinkObject::linkChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CLinkObject::statusChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CLinkObject::statusListChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CLinkObject::GetLink()
{
	if (Version_1_0 && Version_1_0->link){
		return Version_1_0->link.value();
	}

	return QVariant();
}


void CLinkObject::SetLink(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->link = v.value<QString>().toUtf8();
	linkChanged();
}


bool CLinkObject::hasLink()
{
	 return Version_1_0 && Version_1_0->link.HasValue();
}


QVariant CLinkObject::GetStatus()
{
	if (Version_1_0 && Version_1_0->status){
		sdl::modsdl::UnionTest::LinkStatus valueType = Version_1_0->status.value();
		QMetaEnum metaEnum = QMetaEnum::fromType<sdl::modsdl::UnionTest::LinkStatus>();
		QString retval = metaEnum.valueToKey((int)valueType);

		return retval;
	}

	return QVariant();
}


void CLinkObject::SetStatus(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->status.emplace();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::modsdl::UnionTest::LinkStatus>();
	int key = metaEnum.keyToValue(v.value<QString>().toUtf8());
	if (key > -1){
		Version_1_0->status = (sdl::modsdl::UnionTest::LinkStatus)key;
	}
	statusChanged();
}


bool CLinkObject::hasStatus()
{
	 return Version_1_0 && Version_1_0->status.HasValue();
}


QVariant CLinkObject::GetStatusList()
{
	if (Version_1_0 && Version_1_0->statusList){
		
	}

	return QVariant();
}


void CLinkObject::SetStatusList(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	
	statusListChanged();
}


bool CLinkObject::hasStatusList()
{
	 return Version_1_0 && Version_1_0->statusList.HasValue();
}


QString CLinkObject::toJson() const
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


bool CLinkObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CLinkObject::fromObject(const QJsonObject& jsonObject)
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


QString CLinkObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CLinkObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CLinkObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "link")){
		return "link";
	}
	if (propertyName == (QString("m_") + "status")){
		return "status";
	}
	if (propertyName == (QString("m_") + "statusList")){
		return "statusList";
	}

	return propertyName;
}





bool CLinkObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CLinkObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CLinkObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CLinkObjectList::append(sdl::modsdl::UnionTest::CLinkObject* item)
{
	BaseClass::append(item);
}


sdl::modsdl::UnionTest::CLinkObjectList* sdl::modsdl::UnionTest::CLinkObjectList::copyMe()
{
	sdl::modsdl::UnionTest::CLinkObjectList* retVal = new sdl::modsdl::UnionTest::CLinkObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::modsdl::UnionTest::CLinkObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::modsdl::UnionTest::CLinkObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::modsdl::UnionTest::CLinkObjectList::addElement(sdl::modsdl::UnionTest::CLinkObject* item)
{
	append(item);
}


void sdl::modsdl::UnionTest::CLinkObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::modsdl::UnionTest::CLinkObjectList::isEqualWithModel(sdl::modsdl::UnionTest::CLinkObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::modsdl::UnionTest::CLinkObjectList::insert(int index, sdl::modsdl::UnionTest::CLinkObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::modsdl::UnionTest::CLinkObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::modsdl::UnionTest::CLinkObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::modsdl::UnionTest::CLinkObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::modsdl::UnionTest::CLinkObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_link"){
		return QVariant::fromValue(Version_1_0->at(index)->link.value());
	}
	if (nameId == "m_status"){
		return QVariant::fromValue(Version_1_0->at(index)->status.value());
	}
	if (nameId == "m_statusList"){
		return QVariant::fromValue(Version_1_0->at(index)->statusList.value());
	}

	return QVariant();
}
CPrinterBaseObject::CPrinterBaseObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &CPrinterBaseObject::nameChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CPrinterBaseObject::linkListChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CPrinterBaseObject::specificationChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CPrinterBaseObject::specificationListChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CPrinterBaseObject::GetName()
{
	if (Version_1_0 && Version_1_0->name){
		return Version_1_0->name.value();
	}

	return QVariant();
}


void CPrinterBaseObject::SetName(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->name = v.value<QString>();
	nameChanged();
}


bool CPrinterBaseObject::hasName()
{
	 return Version_1_0 && Version_1_0->name.HasValue();
}


QVariant CPrinterBaseObject::GetLinkList()
{
	if (Version_1_0 && Version_1_0->linkList){
		if (!m_linkListQObjectPtr.isValid()){
			m_linkListQObjectPtr = CreateObject("linkList");
			auto itemPtr = m_linkListQObjectPtr.value<sdl::modsdl::UnionTest::CLinkObjectList*>();
			if (itemPtr != nullptr) itemPtr->Version_1_0 = Version_1_0->linkList;
		}
		return m_linkListQObjectPtr;
	}

	return QVariant();
}


void CPrinterBaseObject::SetLinkList(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	if (v.isValid()){
		sdl::modsdl::UnionTest::CLinkObjectList* itemPtr = v.value<sdl::modsdl::UnionTest::CLinkObjectList*>();
		if (itemPtr != nullptr)  Version_1_0->linkList = itemPtr->Version_1_0;
	}
	else {
		Version_1_0->linkList = nullptr;
	}
	m_linkListQObjectPtr = v;

	linkListChanged();
}


bool CPrinterBaseObject::hasLinkList()
{
	 return Version_1_0 && Version_1_0->linkList.HasValue();
}


void CPrinterBaseObject::emplaceLinkList()
{
	if(!Version_1_0){
		Version_1_0.emplace();
	}
	Version_1_0->linkList.emplace();
}


void CPrinterBaseObject::ResetLinkList()
{
	auto* itemPtr = m_linkListQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_linkListQObjectPtr = QVariant();
}


QVariant CPrinterBaseObject::createLinkListArrayElement(const QVariant& v)
{
	Q_UNUSED(v);
	return QVariant::fromValue(new sdl::modsdl::UnionTest::CLinkObject());
}


QVariant CPrinterBaseObject::GetSpecification()
{
	if (Version_1_0 && Version_1_0->specification){
		if (m_specificationQObjectPtr.isValid()){
			if (const CPrinterSpecificationBase* val = std::get_if<CPrinterSpecificationBase>((Version_1_0->specification).GetPtr())){
				CPrinterSpecificationBaseObject *newObjectPtr = new CPrinterSpecificationBaseObject(this);
				newObjectPtr->Version_1_0 = val->Version_1_0;
				m_specificationQObjectPtr = QVariant::fromValue(newObjectPtr);
			}
			if (const CLink* val = std::get_if<CLink>((Version_1_0->specification).GetPtr())){
				CLinkObject *newObjectPtr = new CLinkObject(this);
				newObjectPtr->Version_1_0 = val->Version_1_0;
				m_specificationQObjectPtr = QVariant::fromValue(newObjectPtr);
			}
			if (const QString* val = std::get_if<QString>((Version_1_0->specification).GetPtr())){
				m_specificationQObjectPtr = QVariant::fromValue(val);
			}
			if (const double* val = std::get_if<double>((Version_1_0->specification).GetPtr())){
				m_specificationQObjectPtr = QVariant::fromValue(val);
			}
		}
		return m_specificationQObjectPtr;
	}

	return QVariant();
}


void CPrinterBaseObject::SetSpecification(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	if (v.isValid()){
		if (const CPrinterSpecificationBaseObject* val = v.value<const CPrinterSpecificationBaseObject*>()){
			*Version_1_0->specification = *val;
		}
		if (const CLinkObject* val = v.value<const CLinkObject*>()){
			*Version_1_0->specification = *val;
		}
		if (const QString* val = v.value<const QString*>()){
			*Version_1_0->specification = *val;
		}
		if (const double* val = v.value<const double*>()){
			*Version_1_0->specification = *val;
		}
	}
	else {
		Version_1_0->specification = nullptr;
	}
	m_specificationQObjectPtr = v;

	specificationChanged();
}


bool CPrinterBaseObject::hasSpecification()
{
	 return Version_1_0 && Version_1_0->specification.HasValue();
}


void CPrinterBaseObject::emplaceSpecification()
{
	if(!Version_1_0){
		Version_1_0.emplace();
	}
	Version_1_0->specification.emplace();
}


void CPrinterBaseObject::ResetSpecification()
{
	auto* itemPtr = m_specificationQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_specificationQObjectPtr = QVariant();
}


QVariant CPrinterBaseObject::GetSpecificationList()
{
	if (Version_1_0 && Version_1_0->specificationList){
		if (!m_specificationListQObjectPtr.isValid()){
			m_specificationListQObjectPtr = CreateObject("specificationList");
			auto itemPtr = m_specificationListQObjectPtr.value<sdl::modsdl::UnionTest::CPrinterSpecificationObjectList*>();
			if (itemPtr != nullptr) itemPtr->Version_1_0 = Version_1_0->specificationList;
		}
		return m_specificationListQObjectPtr;
	}

	return QVariant();
}


void CPrinterBaseObject::SetSpecificationList(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	if (v.isValid()){
		sdl::modsdl::UnionTest::CPrinterSpecificationObjectList* itemPtr = v.value<sdl::modsdl::UnionTest::CPrinterSpecificationObjectList*>();
		if (itemPtr != nullptr)  Version_1_0->specificationList = itemPtr->Version_1_0;
	}
	else {
		Version_1_0->specificationList = nullptr;
	}
	m_specificationListQObjectPtr = v;

	specificationListChanged();
}


bool CPrinterBaseObject::hasSpecificationList()
{
	 return Version_1_0 && Version_1_0->specificationList.HasValue();
}


void CPrinterBaseObject::emplaceSpecificationList()
{
	if(!Version_1_0){
		Version_1_0.emplace();
	}
	Version_1_0->specificationList.emplace();
}


void CPrinterBaseObject::ResetSpecificationList()
{
	auto* itemPtr = m_specificationListQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_specificationListQObjectPtr = QVariant();
}


QString CPrinterBaseObject::toJson() const
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


bool CPrinterBaseObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CPrinterBaseObject::fromObject(const QJsonObject& jsonObject)
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


QString CPrinterBaseObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CPrinterBaseObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	if (key == "linkList"){
		return QVariant::fromValue(new sdl::modsdl::UnionTest::CLinkObjectList(this));
	}
	if (key == "specificationList"){
		return QVariant::fromValue(new sdl::modsdl::UnionTest::CPrinterSpecificationObjectList(this));
	}
	return QVariant();
}


QString CPrinterBaseObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "name")){
		return "name";
	}
	if (propertyName == (QString("m_") + "linkList")){
		return "linkList";
	}
	if (propertyName == (QString("m_") + "specification")){
		return "specification";
	}
	if (propertyName == (QString("m_") + "specificationList")){
		return "specificationList";
	}

	return propertyName;
}





bool CPrinterBaseObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CPrinterBaseObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CPrinterBaseObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CPrinterBaseObjectList::append(sdl::modsdl::UnionTest::CPrinterBaseObject* item)
{
	BaseClass::append(item);
}


sdl::modsdl::UnionTest::CPrinterBaseObjectList* sdl::modsdl::UnionTest::CPrinterBaseObjectList::copyMe()
{
	sdl::modsdl::UnionTest::CPrinterBaseObjectList* retVal = new sdl::modsdl::UnionTest::CPrinterBaseObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::modsdl::UnionTest::CPrinterBaseObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::modsdl::UnionTest::CPrinterBaseObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::modsdl::UnionTest::CPrinterBaseObjectList::addElement(sdl::modsdl::UnionTest::CPrinterBaseObject* item)
{
	append(item);
}


void sdl::modsdl::UnionTest::CPrinterBaseObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::modsdl::UnionTest::CPrinterBaseObjectList::isEqualWithModel(sdl::modsdl::UnionTest::CPrinterBaseObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::modsdl::UnionTest::CPrinterBaseObjectList::insert(int index, sdl::modsdl::UnionTest::CPrinterBaseObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::modsdl::UnionTest::CPrinterBaseObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::modsdl::UnionTest::CPrinterBaseObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::modsdl::UnionTest::CPrinterBaseObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::modsdl::UnionTest::CPrinterBaseObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_name"){
		return QVariant::fromValue(Version_1_0->at(index)->name.value());
	}
	if (nameId == "m_linkList"){
		return itemPtr->GetLinkList();
	}
	if (nameId == "m_specification"){
		return itemPtr->GetSpecification();
	}
	if (nameId == "m_specificationList"){
		return itemPtr->GetSpecificationList();
	}

	return QVariant();
}



QVariant CPrinterSpecificationObjectList::GetOrCreateCachedObject(int index) const
{
	QVariant retVal;
	if (this->m_objectDataTypeMap.contains(index)){
		retVal = this->m_objectDataTypeMap[index];
	}
	else{
		if (const CPrinterSpecificationBase* val = std::get_if<CPrinterSpecificationBase>(this->Version_1_0->at(index).GetPtr())){
			CPrinterSpecificationBaseObject *newObjectPtr = new CPrinterSpecificationBaseObject();
			newObjectPtr->Version_1_0 = val->Version_1_0;
			retVal = QVariant::fromValue(newObjectPtr);
		}

		if (const CLink* val = std::get_if<CLink>(this->Version_1_0->at(index).GetPtr())){
			CLinkObject *newObjectPtr = new CLinkObject();
			newObjectPtr->Version_1_0 = val->Version_1_0;
			retVal = QVariant::fromValue(newObjectPtr);
		}

		if (const QString* val = std::get_if<QString>(this->Version_1_0->at(index).GetPtr())){
			retVal =  QVariant::fromValue(val);
		}

		if (const double* val = std::get_if<double>(this->Version_1_0->at(index).GetPtr())){
			retVal =  QVariant::fromValue(val);
		}

		this->m_objectDataTypeMap.insert(index, retVal);
	}
	return retVal;
}


bool CPrinterSpecificationObjectList::containsKey(const QString& nameId, int /*index*/)
{
	return nameId == "item";
}


int CPrinterSpecificationObjectList::getItemsCount()
{
	return rowCount();
}


QVariantMap CPrinterSpecificationObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CPrinterSpecificationObjectList::append(QVariant item)
{
	beginInsertRows(QModelIndex(), Version_1_0->count(), 0);

	if (item.canConvert<CPrinterSpecificationBaseObject*>()){
		CPrinterSpecificationBaseObject* val = item.value<CPrinterSpecificationBaseObject*>();
		CPrinterSpecificationBase* newItemPtr = dynamic_cast<CPrinterSpecificationBase*>(val);
		Version_1_0->append(istd::TSharedNullable<PrinterSpecification>(*newItemPtr));
	}

	if (item.canConvert<CLinkObject*>()){
		CLinkObject* val = item.value<CLinkObject*>();
		CLink* newItemPtr = dynamic_cast<CLink*>(val);
		Version_1_0->append(istd::TSharedNullable<PrinterSpecification>(*newItemPtr));
	}

	if (item.canConvert<QString>()){
		QString val = item.value<QString>();
		Version_1_0->append(istd::TSharedNullable<PrinterSpecification>(val));
	}

	if (item.canConvert<double>()){
		double val = item.value<double>();
		Version_1_0->append(istd::TSharedNullable<PrinterSpecification>(val));
	}

	ClearCache();
	endInsertRows();
}


sdl::modsdl::UnionTest::CPrinterSpecificationObjectList* CPrinterSpecificationObjectList::copyMe()
{
	sdl::modsdl::UnionTest::CPrinterSpecificationObjectList* objectListPtr = new sdl::modsdl::UnionTest::CPrinterSpecificationObjectList();

	for (int i = 0; i < this->rowCount(); i++){
		QVariant item = this->getData("item", i);

		if (item.canConvert<CPrinterSpecificationBaseObject*>()){
			CPrinterSpecificationBaseObject* val = item.value<CPrinterSpecificationBaseObject*>();
			objectListPtr->addElement(QVariant::fromValue(val->copyMe()));
		}

		if (item.canConvert<CLinkObject*>()){
			CLinkObject* val = item.value<CLinkObject*>();
			objectListPtr->addElement(QVariant::fromValue(val->copyMe()));
		}

		if (item.canConvert<QString>()){
			QString val = item.value<QString>();
			objectListPtr->addElement(val);
		}

		if (item.canConvert<double>()){
			double val = item.value<double>();
			objectListPtr->addElement(val);
		}
	}
	return objectListPtr;
}


QString CPrinterSpecificationObjectList::toJson()
{
	QString retVal = QStringLiteral("[");

	for (int i = 0; i < this->rowCount(); i++){
		if (i > 0 && i < this->rowCount() - 1){
			retVal += QStringLiteral(", ");
		}

		QVariant item = this->getData("item", i);

		if (item.canConvert<CPrinterSpecificationBaseObject*>()){
			CPrinterSpecificationBaseObject* val = item.value<CPrinterSpecificationBaseObject*>();
			retVal += val->toJson();
		}

		if (item.canConvert<CLinkObject*>()){
			CLinkObject* val = item.value<CLinkObject*>();
			retVal += val->toJson();
		}

		if (item.canConvert<QString>()){
			QString val = item.value<QString>();
			retVal += QString(val);
		}

		if (item.canConvert<double>()){
			double val = item.value<double>();
			retVal += QString::number(val);
		}
	}

	retVal += QStringLiteral("]");

	return retVal;
}


QString CPrinterSpecificationObjectList::toGraphQL()
{
	QString retVal = QStringLiteral("[");

	for (int i = 0; i < this->rowCount(); i++){
		if (i > 0 && i < this->rowCount() - 1){
			retVal += QStringLiteral(", ");
		}

		QVariant item = this->getData("item", i);

		if (item.canConvert<CPrinterSpecificationBaseObject*>()){
			CPrinterSpecificationBaseObject* val = item.value<CPrinterSpecificationBaseObject*>();
			retVal += val->toGraphQL();
		}

		if (item.canConvert<CLinkObject*>()){
			CLinkObject* val = item.value<CLinkObject*>();
			retVal += val->toGraphQL();
		}

		if (item.canConvert<QString>()){
			QString val = item.value<QString>();
			retVal += QString(val);
		}

		if (item.canConvert<double>()){
			double val = item.value<double>();
			retVal += QString::number(val);
		}
	}

	retVal += QStringLiteral("]");

	return retVal;
}


void CPrinterSpecificationObjectList::addElement(QVariant item)
{
	append(item);
}

void CPrinterSpecificationObjectList::removeElement(int index)
{
	remove(index);
}

bool CPrinterSpecificationObjectList::isEqualWithModel(sdl::modsdl::UnionTest::CPrinterSpecificationObjectList* otherModelPtr)
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


void CPrinterSpecificationObjectList::insert(int index, QVariant item)
{
	if (index < 0 || index > Version_1_0->size()) return;
	beginInsertRows(QModelIndex(), Version_1_0->count(), 0);

	if (item.canConvert<CPrinterSpecificationBaseObject*>()){
		CPrinterSpecificationBaseObject* val = item.value<CPrinterSpecificationBaseObject*>();
		CPrinterSpecificationBase* newItemPtr = dynamic_cast<CPrinterSpecificationBase*>(val);
		Version_1_0->append(istd::TSharedNullable<PrinterSpecification>(*newItemPtr));
	}

	if (item.canConvert<CLinkObject*>()){
		CLinkObject* val = item.value<CLinkObject*>();
		CLink* newItemPtr = dynamic_cast<CLink*>(val);
		Version_1_0->append(istd::TSharedNullable<PrinterSpecification>(*newItemPtr));
	}

	if (item.canConvert<QString>()){
		QString val = item.value<QString>();
		Version_1_0->append(istd::TSharedNullable<PrinterSpecification>(val));
	}

	if (item.canConvert<double>()){
		double val = item.value<double>();
		Version_1_0->append(istd::TSharedNullable<PrinterSpecification>(val));
	}

	ClearCache();
	endInsertRows();
}


void CPrinterSpecificationObjectList::remove(int index)
{
	BaseClass::remove(index);
}


void CPrinterSpecificationObjectList::clear()
{
	BaseClass::clear();
}


QVariant CPrinterSpecificationObjectList::getData(const QString& nameId, int index)
{
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		QVariant retVal = GetOrCreateCachedObject(index);
		return retVal;
	}
	return QVariant();
}


QByteArray CGetSpecificationsGqlRequest::GetCommandId()
{
	return QByteArrayLiteral("GetSpecifications");
}


bool CGetSpecificationsGqlRequest::SetupGqlRequest(::imtgql::CGqlRequest& gqlRequest, const GetSpecificationsRequestArguments& requestArguments, const GetSpecificationsRequestInfo& /*requestInfo*/)
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


CGetSpecificationsGqlRequest::CGetSpecificationsGqlRequest(const ::imtgql::CGqlRequest& gqlRequest, bool optRead)
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
					isInputRead = m_requestedArguments.input.OptReadFromGraphQlObject(*inputDataObjectPtr, CLink::PV_1_0);
				}
				else {
					isInputRead = m_requestedArguments.input.ReadFromGraphQlObject(*inputDataObjectPtr, CLink::PV_1_0);
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


bool CGetSpecificationsGqlRequest::IsValid() const
{
	return m_isValid;
}


const ::imtgql::IGqlContext* CGetSpecificationsGqlRequest::GetRequestContext() const
{
	return m_gqlContextPtr;
}


const GetSpecificationsRequestArguments& CGetSpecificationsGqlRequest::GetRequestedArguments() const
{
	return m_requestedArguments;
}


GetSpecificationsRequestInfo CGetSpecificationsGqlRequest::GetRequestInfo() const
{
	return m_requestInfo;
}
QMap<int, QByteArray> CPrinterCollectionControllerCompBase::GetSupportedCommandIds() const
{
	static QMap<int, QByteArray> retVal = {
		std::make_pair(OT_GET, CGetSpecificationsGqlRequest::GetCommandId()),
	};

	return retVal;
}


bool CPrinterCollectionControllerCompBase::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	const QByteArray commandId = gqlRequest.GetCommandId();
	if (commandId == CGetSpecificationsGqlRequest::GetCommandId()){
		return true;
	}

	return BaseClass::IsRequestSupported(gqlRequest);
}


bool CPrinterCollectionControllerCompBase::GetOperationFromRequest(const ::imtgql::CGqlRequest& gqlRequest, ::imtgql::CGqlParamObject& gqlObject, QString& errorMessage, int& operationType) const
{
	const QByteArray commandId = gqlRequest.GetCommandId();

	if (commandId == CGetSpecificationsGqlRequest::GetCommandId()){
		operationType =  OT_GET;

		return true;
	}

	return BaseClass::GetOperationFromRequest(gqlRequest, gqlObject, errorMessage, operationType);
}


bool CPrinterCollectionControllerCompBase::CreateRepresentationFromObject(const istd::IChangeable& data, const QByteArray& objectTypeId, const ::imtgql::CGqlRequest& gqlRequest, ::imtbase::CTreeItemModel& dataModel, QString& errorMessage) const
{
	Q_UNUSED(objectTypeId);

	const QByteArray commandId = gqlRequest.GetCommandId();

	//GetSpecifications
	if (commandId == CGetSpecificationsGqlRequest::GetCommandId()){
		CGetSpecificationsGqlRequest getSpecificationsGqlRequest(gqlRequest, false);
		PrinterSpecification representationObject;
		const bool isRepresentationCreated = CreateRepresentationFromObject(data, getSpecificationsGqlRequest, representationObject, errorMessage);
		if (!isRepresentationCreated){
	I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to create representation").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

			return false;
		}

		if (const CPrinterSpecificationBase* val = std::get_if<CPrinterSpecificationBase>(&representationObject)){
			if (!val->WriteToModel(dataModel)){
				return false;
			}
			dataModel.SetData("__typename", "PrinterSpecificationBase", 0);
		}
		else if (const CLink* val = std::get_if<CLink>(&representationObject)){
			if (!val->WriteToModel(dataModel)){
				return false;
			}
			dataModel.SetData("__typename", "Link", 0);
		}
		else if (const QString* val = std::get_if<QString>(&representationObject)){
			dataModel.SetData("", *val);
		}
		else if (const double* val = std::get_if<double>(&representationObject)){
			dataModel.SetData("", *val);
		}

		return true;
	}

	errorMessage = QString("Bad request. Unexpected command-ID: '%1'").arg(qPrintable(commandId));

	SendErrorMessage(0, errorMessage);

	return false;
}


bool CGraphQlHandlerCompBase::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	const QByteArray commandId = gqlRequest.GetCommandId();
	if (commandId == CGetSpecificationsGqlRequest::GetCommandId()){
		return true;
	}

	return BaseClass::IsRequestSupported(gqlRequest);
}


::imtbase::CTreeItemModel* CGraphQlHandlerCompBase::CreateInternalResponse(const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	const QByteArray commandId = gqlRequest.GetCommandId();
	istd::TDelPtr<::imtbase::CTreeItemModel> modelPtr(new ::imtbase::CTreeItemModel);
	::imtbase::CTreeItemModel* dataModelPtr = modelPtr->AddTreeModel("data");

	// GetSpecifications
	if (commandId == CGetSpecificationsGqlRequest::GetCommandId()){
		CGetSpecificationsGqlRequest getSpecificationsGqlRequest(gqlRequest, false);
		if (!getSpecificationsGqlRequest.IsValid()){
			errorMessage = QString("Bad request. Unexpected request for command-ID: '%1'").arg(qPrintable(commandId));
			SendErrorMessage(0, errorMessage);

			return nullptr;
		}

		PrinterSpecification replyPayload = OnGetSpecifications(getSpecificationsGqlRequest, gqlRequest, errorMessage);
		if (!errorMessage.isEmpty()){
			SendErrorMessage(0, QString("The derived call [OnGetSpecifications] returned an error: %1").arg(errorMessage));

			return nullptr;
		}

		if (const CPrinterSpecificationBase* val = std::get_if<CPrinterSpecificationBase>(&replyPayload)){
			if (!val->WriteToModel(*dataModelPtr)){
				return nullptr;
			}
			if(dataModelPtr != nullptr){
				dataModelPtr->SetData("__typename", "PrinterSpecificationBase", 0);
			}
		}
		else if (const CLink* val = std::get_if<CLink>(&replyPayload)){
			if (!val->WriteToModel(*dataModelPtr)){
				return nullptr;
			}
			if(dataModelPtr != nullptr){
				dataModelPtr->SetData("__typename", "Link", 0);
			}
		}
		else if (const QString* val = std::get_if<QString>(&replyPayload)){
			dataModelPtr->SetData("", *val);
		}
		else if (const double* val = std::get_if<double>(&replyPayload)){
			dataModelPtr->SetData("", *val);
		}
		return modelPtr.PopPtr();
	}

	errorMessage = QString("Bad request. Unexpected command-ID: '%1'").arg(qPrintable(commandId));
	SendErrorMessage(0, errorMessage);

	return nullptr;
}


} // namespace sdl::modsdl::UnionTest
