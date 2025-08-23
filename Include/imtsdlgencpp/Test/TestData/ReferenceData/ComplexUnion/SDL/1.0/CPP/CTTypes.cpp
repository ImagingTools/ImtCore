#include "CTTypes.h"


namespace sdl::complextest::CTTypes
{


QByteArray CPoint::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CPoint::V1_0::operator==(const V1_0& other) const
{
	return 
				X.has_value() == other.X.has_value() &&
				((X.has_value() && other.X.has_value()) ?
					qFuzzyCompare(*X, *other.X) : true) &&
				Y.has_value() == other.Y.has_value() &&
				((Y.has_value() && other.Y.has_value()) ?
					qFuzzyCompare(*Y, *other.Y) : true);
}


bool CPoint::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	if (!X){
		return false;
	}
	model.SetData("X", *X, modelIndex);

	if (!Y){
		return false;
	}
	model.SetData("Y", *Y, modelIndex);


	return true;
}


bool CPoint::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant xData = model.GetData("X", modelIndex);
	if (xData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "X");)

		return false;
	}
	X = xData.toFloat();

	QVariant yData = model.GetData("Y", modelIndex);
	if (yData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "Y");)

		return false;
	}
	Y = yData.toFloat();

	return true;
}


bool CPoint::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant xData = model.GetData("X", modelIndex);
	if (!xData.isNull()){
		X = xData.toFloat();
	}

	QVariant yData = model.GetData("Y", modelIndex);
	if (!yData.isNull()){
		Y = yData.toFloat();
	}

	return true;
}


bool CPoint::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!X){
		return false;
	}
	gqlObject.InsertParam("X", QVariant(*X));

	if (!Y){
		return false;
	}
	gqlObject.InsertParam("Y", QVariant(*Y));

	return true;
}


bool CPoint::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("X") || (gqlObject["X"].userType() != QMetaType::Float && gqlObject["X"].userType() != QMetaType::Double && gqlObject["X"].userType() != QMetaType::Int && gqlObject["X"].userType() != QMetaType::UInt && gqlObject["X"].userType() != QMetaType::LongLong && gqlObject["X"].userType() != QMetaType::ULongLong && gqlObject["X"].userType() != QMetaType::Long && gqlObject["X"].userType() != QMetaType::Short && gqlObject["X"].userType() != QMetaType::ULong && gqlObject["X"].userType() != QMetaType::UShort && gqlObject["X"].userType() != QMetaType::UChar)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "X");)

		return false;
	}
	X = gqlObject["X"].toDouble();

	if (!gqlObject.ContainsParam("Y") || (gqlObject["Y"].userType() != QMetaType::Float && gqlObject["Y"].userType() != QMetaType::Double && gqlObject["Y"].userType() != QMetaType::Int && gqlObject["Y"].userType() != QMetaType::UInt && gqlObject["Y"].userType() != QMetaType::LongLong && gqlObject["Y"].userType() != QMetaType::ULongLong && gqlObject["Y"].userType() != QMetaType::Long && gqlObject["Y"].userType() != QMetaType::Short && gqlObject["Y"].userType() != QMetaType::ULong && gqlObject["Y"].userType() != QMetaType::UShort && gqlObject["Y"].userType() != QMetaType::UChar)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Y");)

		return false;
	}
	Y = gqlObject["Y"].toDouble();

	return true;
}


bool CPoint::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("X") && (gqlObject["X"].userType() == QMetaType::Float || gqlObject["X"].userType() == QMetaType::Double || gqlObject["X"].userType() == QMetaType::Int || gqlObject["X"].userType() == QMetaType::UInt || gqlObject["X"].userType() == QMetaType::LongLong || gqlObject["X"].userType() == QMetaType::ULongLong || gqlObject["X"].userType() == QMetaType::Long || gqlObject["X"].userType() == QMetaType::Short || gqlObject["X"].userType() == QMetaType::ULong || gqlObject["X"].userType() == QMetaType::UShort || gqlObject["X"].userType() == QMetaType::UChar)){
		X = gqlObject["X"].toDouble();
	}

	if (gqlObject.ContainsParam("Y") && (gqlObject["Y"].userType() == QMetaType::Float || gqlObject["Y"].userType() == QMetaType::Double || gqlObject["Y"].userType() == QMetaType::Int || gqlObject["Y"].userType() == QMetaType::UInt || gqlObject["Y"].userType() == QMetaType::LongLong || gqlObject["Y"].userType() == QMetaType::ULongLong || gqlObject["Y"].userType() == QMetaType::Long || gqlObject["Y"].userType() == QMetaType::Short || gqlObject["Y"].userType() == QMetaType::ULong || gqlObject["Y"].userType() == QMetaType::UShort || gqlObject["Y"].userType() == QMetaType::UChar)){
		Y = gqlObject["Y"].toDouble();
	}

	return true;
}


bool CPoint::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!X){
		return false;
	}
	jsonObject["X"] = QJsonValue::fromVariant(*X);

	if (!Y){
		return false;
	}
	jsonObject["Y"] = QJsonValue::fromVariant(*Y);

	return true;
}


bool CPoint::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("X") || ! jsonObject["X"].isDouble()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "X");)

		return false;
	}
	X = jsonObject["X"].toDouble();

	if (!jsonObject.contains("Y") || ! jsonObject["Y"].isDouble()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Y");)

		return false;
	}
	Y = jsonObject["Y"].toDouble();

	return true;
}


bool CPoint::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
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

bool CPoint::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version object not initialized.").arg(__FILE__, QString::number(__LINE__));)

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


bool CPoint::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
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


bool CPoint::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
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


bool CPoint::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version object not initialized.").arg(__FILE__, QString::number(__LINE__));)

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


bool CPoint::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
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


bool CPoint::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
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


bool CPoint::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version object not initialized.").arg(__FILE__, QString::number(__LINE__));)

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


bool CPoint::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
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


bool CPoint::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
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


CPointObject::CPointObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &CPointObject::xChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CPointObject::yChanged, this, &CItemModelBase::OnInternalModelChanged);
}


double CPointObject::GetX()
{
	if (Version_1_0->X.has_value()){
		return Version_1_0->X.value();
	}

	return 0;
}


void CPointObject::SetX(double v)
{
	Version_1_0->X = v;
	xChanged();
}


bool CPointObject::hasX()
{
	 return Version_1_0->X.HasValue();
}


double CPointObject::GetY()
{
	if (Version_1_0->Y.has_value()){
		return Version_1_0->Y.value();
	}

	return 0;
}


void CPointObject::SetY(double v)
{
	Version_1_0->Y = v;
	yChanged();
}


bool CPointObject::hasY()
{
	 return Version_1_0->Y.HasValue();
}


QString CPointObject::toJson() const
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


bool CPointObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CPointObject::fromObject(const QJsonObject& jsonObject)
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


QString CPointObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QObject* CPointObject::CreateObject(const QString& key)
{
	return nullptr;
}


QString CPointObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "x")){
		return "X";
	}
	if (propertyName == (QString("m_") + "y")){
		return "Y";
	}

	return propertyName;
}


QByteArray CGeometry::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CGeometry::V1_0::operator==(const V1_0& other) const
{
	return 
				GeometryType == other.GeometryType &&
				Radius.has_value() == other.Radius.has_value() &&
				((Radius.has_value() && other.Radius.has_value()) ?
					qFuzzyCompare(*Radius, *other.Radius) : true) &&
				Points == other.Points &&
				RequiredPoints == other.RequiredPoints &&
				OptionalPoints == other.OptionalPoints;
}


bool CGeometry::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	if (!GeometryType){
		return false;
	}
	QString geometryTypeStringValue;
	switch (*GeometryType){
	case GeometryType::LINE:
		geometryTypeStringValue = "LINE";
		break;
	case GeometryType::POLYGON:
		geometryTypeStringValue = "POLYGON";
		break;
	case GeometryType::CIRCLE:
		geometryTypeStringValue = "CIRCLE";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	model.SetData("GeometryType", geometryTypeStringValue, modelIndex);

	if (Radius){
		model.SetData("Radius", *Radius, modelIndex);
	}

	if (!Points){
		return false;
	}
	::imtbase::CTreeItemModel* newPointsModelPtr = model.AddTreeModel("Points", modelIndex);
	newPointsModelPtr->setIsArray(true);
	for (qsizetype pointsIndex = 0; pointsIndex < Points->size(); ++pointsIndex){
		newPointsModelPtr->InsertNewItem();
		if (!(Points->at(pointsIndex).WriteToModel(*newPointsModelPtr, pointsIndex))){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "Points");)

			return false;
		}
	}
	if (!RequiredPoints || RequiredPoints->isEmpty()){
		return false;
	}
	::imtbase::CTreeItemModel* newRequiredPointsModelPtr = model.AddTreeModel("RequiredPoints", modelIndex);
	newRequiredPointsModelPtr->setIsArray(true);
	for (qsizetype requiredPointsIndex = 0; requiredPointsIndex < RequiredPoints->size(); ++requiredPointsIndex){
		newRequiredPointsModelPtr->InsertNewItem();
		if (!(RequiredPoints->at(requiredPointsIndex).WriteToModel(*newRequiredPointsModelPtr, requiredPointsIndex))){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "RequiredPoints");)

			return false;
		}
	}
	if (OptionalPoints){
		::imtbase::CTreeItemModel* newOptionalPointsModelPtr = model.AddTreeModel("OptionalPoints", modelIndex);
		newOptionalPointsModelPtr->setIsArray(true);
		for (qsizetype optionalPointsIndex = 0; optionalPointsIndex < OptionalPoints->size(); ++optionalPointsIndex){
			newOptionalPointsModelPtr->InsertNewItem();
			if (!(OptionalPoints->at(optionalPointsIndex).WriteToModel(*newOptionalPointsModelPtr, optionalPointsIndex))){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "OptionalPoints");)

				return false;
			}
		}
	}

	return true;
}


bool CGeometry::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant geometryTypeData = model.GetData("GeometryType", modelIndex);
	if (geometryTypeData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "GeometryType");)

		return false;
	}
	QString geometryTypeStringValue = geometryTypeData.toString();
	if(geometryTypeStringValue == "LINE"){
		GeometryType = GeometryType::LINE;
	}
	else if(geometryTypeStringValue == "POLYGON"){
		GeometryType = GeometryType::POLYGON;
	}
	else if(geometryTypeStringValue == "CIRCLE"){
		GeometryType = GeometryType::CIRCLE;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "GeometryType", geometryTypeStringValue);)

		return false;
	}

	QVariant radiusData = model.GetData("Radius", modelIndex);
	if (!radiusData.isNull()){
		Radius = radiusData.toFloat();
	}

	::imtbase::CTreeItemModel* pointsModel = model.GetTreeItemModel("Points", modelIndex);
	if (pointsModel == nullptr){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' is missing, but required.").arg(__FILE__, QString::number(__LINE__), "Points");)

		return false;
	}
	int pointsCount = pointsModel->GetItemsCount();
	QList<CPoint::V1_0> pointsList;
	for (int pointsIndex = 0; pointsIndex < pointsCount; ++pointsIndex){
		CPoint::V1_0 points;
		if (!points.ReadFromModel(*pointsModel, pointsIndex)){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Points");)

			return false;
		}
		pointsList << points;
	}
	Points = pointsList;


	::imtbase::CTreeItemModel* requiredPointsModel = model.GetTreeItemModel("RequiredPoints", modelIndex);
	if (requiredPointsModel == nullptr){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' is missing, but required.").arg(__FILE__, QString::number(__LINE__), "RequiredPoints");)

		return false;
	}
	int requiredPointsCount = requiredPointsModel->GetItemsCount();
	if (requiredPointsCount <= 0){
	I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' is empty").arg(__FILE__, QString::number(__LINE__), "RequiredPoints");)

		return false;
	}
	QList<CPoint::V1_0> requiredPointsList;
	for (int requiredPointsIndex = 0; requiredPointsIndex < requiredPointsCount; ++requiredPointsIndex){
		CPoint::V1_0 requiredPoints;
		if (!requiredPoints.ReadFromModel(*requiredPointsModel, requiredPointsIndex)){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "RequiredPoints");)

			return false;
		}
		requiredPointsList << requiredPoints;
	}
	RequiredPoints = requiredPointsList;


	::imtbase::CTreeItemModel* optionalPointsModel = model.GetTreeItemModel("OptionalPoints", modelIndex);
	if (optionalPointsModel != nullptr){
		int optionalPointsCount = optionalPointsModel->GetItemsCount();
		QList<CPoint::V1_0> optionalPointsList;
		for (int optionalPointsIndex = 0; optionalPointsIndex < optionalPointsCount; ++optionalPointsIndex){
			CPoint::V1_0 optionalPoints;
			if (!optionalPoints.ReadFromModel(*optionalPointsModel, optionalPointsIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "OptionalPoints");)

				return false;
			}
			optionalPointsList << optionalPoints;
		}
		OptionalPoints = optionalPointsList;

	}

	return true;
}


bool CGeometry::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant geometryTypeData = model.GetData("GeometryType", modelIndex);
	if (!geometryTypeData.isNull()){
		QString geometryTypeStringValue = geometryTypeData.toString();
		if(geometryTypeStringValue == "LINE"){
			GeometryType = GeometryType::LINE;
		}
		else if(geometryTypeStringValue == "POLYGON"){
			GeometryType = GeometryType::POLYGON;
		}
		else if(geometryTypeStringValue == "CIRCLE"){
			GeometryType = GeometryType::CIRCLE;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "GeometryType", geometryTypeStringValue);)

			return false;
		}
	}

	QVariant radiusData = model.GetData("Radius", modelIndex);
	if (!radiusData.isNull()){
		Radius = radiusData.toFloat();
	}

	::imtbase::CTreeItemModel* pointsModel = model.GetTreeItemModel("Points", modelIndex);
	if (pointsModel != nullptr){
		int pointsCount = pointsModel->GetItemsCount();
		QList<CPoint::V1_0> pointsList;
		for (int pointsIndex = 0; pointsIndex < pointsCount; ++pointsIndex){
			CPoint::V1_0 points;
			if (!points.OptReadFromModel(*pointsModel, pointsIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Points");)

				return false;
			}
			pointsList << points;
		}
		Points = pointsList;

	}

	::imtbase::CTreeItemModel* requiredPointsModel = model.GetTreeItemModel("RequiredPoints", modelIndex);
	if (requiredPointsModel != nullptr){
		int requiredPointsCount = requiredPointsModel->GetItemsCount();
		if (requiredPointsCount <= 0){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' is empty").arg(__FILE__, QString::number(__LINE__), "RequiredPoints");)

			return false;
		}
		QList<CPoint::V1_0> requiredPointsList;
		for (int requiredPointsIndex = 0; requiredPointsIndex < requiredPointsCount; ++requiredPointsIndex){
			CPoint::V1_0 requiredPoints;
			if (!requiredPoints.OptReadFromModel(*requiredPointsModel, requiredPointsIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "RequiredPoints");)

				return false;
			}
			requiredPointsList << requiredPoints;
		}
		RequiredPoints = requiredPointsList;

	}

	::imtbase::CTreeItemModel* optionalPointsModel = model.GetTreeItemModel("OptionalPoints", modelIndex);
	if (optionalPointsModel != nullptr){
		int optionalPointsCount = optionalPointsModel->GetItemsCount();
		QList<CPoint::V1_0> optionalPointsList;
		for (int optionalPointsIndex = 0; optionalPointsIndex < optionalPointsCount; ++optionalPointsIndex){
			CPoint::V1_0 optionalPoints;
			if (!optionalPoints.OptReadFromModel(*optionalPointsModel, optionalPointsIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "OptionalPoints");)

				return false;
			}
			optionalPointsList << optionalPoints;
		}
		OptionalPoints = optionalPointsList;

	}

	return true;
}


bool CGeometry::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!GeometryType){
		return false;
	}
	QString geometryTypeStringValue;
	switch (*GeometryType){
	case GeometryType::LINE:
		geometryTypeStringValue = "LINE";
		break;
	case GeometryType::POLYGON:
		geometryTypeStringValue = "POLYGON";
		break;
	case GeometryType::CIRCLE:
		geometryTypeStringValue = "CIRCLE";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	gqlObject.InsertParam("GeometryType", QVariant(geometryTypeStringValue));

	if (Radius){
		gqlObject.InsertParam("Radius", QVariant(*Radius));
	}

	if (!Points){
		return false;
	}
	QList<::imtgql::CGqlParamObject> pointsDataObjectList;
	for (qsizetype pointsIndex = 0; pointsIndex < Points->size(); ++pointsIndex){
		::imtgql::CGqlParamObject newPointsGqlObject;
		if (!Points->at(pointsIndex).WriteToGraphQlObject(newPointsGqlObject)){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "Points");)

			return false;
		}
		pointsDataObjectList << newPointsGqlObject;
	}
	gqlObject.InsertParam("Points", pointsDataObjectList);

	if (!RequiredPoints || RequiredPoints->isEmpty()){
		return false;
	}
	QList<::imtgql::CGqlParamObject> requiredPointsDataObjectList;
	for (qsizetype requiredPointsIndex = 0; requiredPointsIndex < RequiredPoints->size(); ++requiredPointsIndex){
		::imtgql::CGqlParamObject newRequiredPointsGqlObject;
		if (!RequiredPoints->at(requiredPointsIndex).WriteToGraphQlObject(newRequiredPointsGqlObject)){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "RequiredPoints");)

			return false;
		}
		requiredPointsDataObjectList << newRequiredPointsGqlObject;
	}
	gqlObject.InsertParam("RequiredPoints", requiredPointsDataObjectList);

	if (OptionalPoints){
		QList<::imtgql::CGqlParamObject> optionalPointsDataObjectList;
		for (qsizetype optionalPointsIndex = 0; optionalPointsIndex < OptionalPoints->size(); ++optionalPointsIndex){
			::imtgql::CGqlParamObject newOptionalPointsGqlObject;
			if (!OptionalPoints->at(optionalPointsIndex).WriteToGraphQlObject(newOptionalPointsGqlObject)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "OptionalPoints");)

				return false;
			}
			optionalPointsDataObjectList << newOptionalPointsGqlObject;
		}
		gqlObject.InsertParam("OptionalPoints", optionalPointsDataObjectList);
	}

	return true;
}


bool CGeometry::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("GeometryType") || (gqlObject["GeometryType"].userType() != QMetaType::QString && gqlObject["GeometryType"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "GeometryType");)

		return false;
	}
	const QString geometryTypeStringValue = gqlObject["GeometryType"].toString();
	if(geometryTypeStringValue == "LINE"){
		GeometryType = GeometryType::LINE;
	}
	else if(geometryTypeStringValue == "POLYGON"){
		GeometryType = GeometryType::POLYGON;
	}
	else if(geometryTypeStringValue == "CIRCLE"){
		GeometryType = GeometryType::CIRCLE;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "GeometryType", geometryTypeStringValue);)

		return false;
	}

	if (gqlObject.ContainsParam("Radius") && (gqlObject["Radius"].userType() == QMetaType::Float || gqlObject["Radius"].userType() == QMetaType::Double || gqlObject["Radius"].userType() == QMetaType::Int || gqlObject["Radius"].userType() == QMetaType::UInt || gqlObject["Radius"].userType() == QMetaType::LongLong || gqlObject["Radius"].userType() == QMetaType::ULongLong || gqlObject["Radius"].userType() == QMetaType::Long || gqlObject["Radius"].userType() == QMetaType::Short || gqlObject["Radius"].userType() == QMetaType::ULong || gqlObject["Radius"].userType() == QMetaType::UShort || gqlObject["Radius"].userType() == QMetaType::UChar)){
		Radius = gqlObject["Radius"].toDouble();
	}

	if (!gqlObject.ContainsParam("Points")){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Points");)

		return false;
	}
	const qsizetype pointsElementsCount = gqlObject.GetObjectsCount("points");
	Points = QList<CPoint::V1_0>();
	for (qsizetype pointsIndex = 0; pointsIndex < pointsElementsCount; ++pointsIndex){
		const ::imtgql::CGqlParamObject* pointsDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("Points", pointsIndex);
		if (pointsDataObjectPtr == nullptr){
			qDebug() << "invalid type" << pointsDataObjectPtr;
			return false;
		}
		CPoint::V1_0 tempPoints;
		if (!tempPoints.ReadFromGraphQlObject(*pointsDataObjectPtr)){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Points");)

			return false;
		}
		Points->append(tempPoints);
	}

	if (!gqlObject.ContainsParam("RequiredPoints")){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "RequiredPoints");)

		return false;
	}
	const qsizetype requiredPointsElementsCount = gqlObject.GetObjectsCount("requiredPoints");
	if (requiredPointsElementsCount <= 0){
	I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' is empty").arg(__FILE__, QString::number(__LINE__), "RequiredPoints");)

		return false;
	}
	RequiredPoints = QList<CPoint::V1_0>();
	for (qsizetype requiredPointsIndex = 0; requiredPointsIndex < requiredPointsElementsCount; ++requiredPointsIndex){
		const ::imtgql::CGqlParamObject* requiredPointsDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("RequiredPoints", requiredPointsIndex);
		if (requiredPointsDataObjectPtr == nullptr){
			qDebug() << "invalid type" << requiredPointsDataObjectPtr;
			return false;
		}
		CPoint::V1_0 tempRequiredPoints;
		if (!tempRequiredPoints.ReadFromGraphQlObject(*requiredPointsDataObjectPtr)){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "RequiredPoints");)

			return false;
		}
		RequiredPoints->append(tempRequiredPoints);
	}

	if (gqlObject.ContainsParam("OptionalPoints") && (gqlObject.GetObjectsCount("OptionalPoints") > 0)){
		const qsizetype optionalPointsElementsCount = gqlObject.GetObjectsCount("optionalPoints");
		OptionalPoints = QList<CPoint::V1_0>();
		for (qsizetype optionalPointsIndex = 0; optionalPointsIndex < optionalPointsElementsCount; ++optionalPointsIndex){
			const ::imtgql::CGqlParamObject* optionalPointsDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("OptionalPoints", optionalPointsIndex);
			if (optionalPointsDataObjectPtr == nullptr){
				qDebug() << "invalid type" << optionalPointsDataObjectPtr;
				return false;
			}
			CPoint::V1_0 tempOptionalPoints;
			if (!tempOptionalPoints.ReadFromGraphQlObject(*optionalPointsDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "OptionalPoints");)

				return false;
			}
			OptionalPoints->append(tempOptionalPoints);
		}
	}

	return true;
}


bool CGeometry::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("GeometryType") && (gqlObject["GeometryType"].userType() == QMetaType::QString || gqlObject["GeometryType"].userType() == QMetaType::QByteArray)){
		const QString geometryTypeStringValue = gqlObject["GeometryType"].toString();
		if(geometryTypeStringValue == "LINE"){
			GeometryType = GeometryType::LINE;
		}
		else if(geometryTypeStringValue == "POLYGON"){
			GeometryType = GeometryType::POLYGON;
		}
		else if(geometryTypeStringValue == "CIRCLE"){
			GeometryType = GeometryType::CIRCLE;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "GeometryType", geometryTypeStringValue);)

			return false;
		}
	}

	if (gqlObject.ContainsParam("Radius") && (gqlObject["Radius"].userType() == QMetaType::Float || gqlObject["Radius"].userType() == QMetaType::Double || gqlObject["Radius"].userType() == QMetaType::Int || gqlObject["Radius"].userType() == QMetaType::UInt || gqlObject["Radius"].userType() == QMetaType::LongLong || gqlObject["Radius"].userType() == QMetaType::ULongLong || gqlObject["Radius"].userType() == QMetaType::Long || gqlObject["Radius"].userType() == QMetaType::Short || gqlObject["Radius"].userType() == QMetaType::ULong || gqlObject["Radius"].userType() == QMetaType::UShort || gqlObject["Radius"].userType() == QMetaType::UChar)){
		Radius = gqlObject["Radius"].toDouble();
	}

	if (gqlObject.ContainsParam("Points") && (gqlObject.GetObjectsCount("Points") > 0)){
		const qsizetype pointsElementsCount = gqlObject.GetObjectsCount("points");
		Points = QList<CPoint::V1_0>();
		for (qsizetype pointsIndex = 0; pointsIndex < pointsElementsCount; ++pointsIndex){
			const ::imtgql::CGqlParamObject* pointsDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("Points", pointsIndex);
			if (pointsDataObjectPtr == nullptr){
				qDebug() << "invalid type" << pointsDataObjectPtr;
				return false;
			}
			CPoint::V1_0 tempPoints;
			if (!tempPoints.OptReadFromGraphQlObject(*pointsDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Points");)

				return false;
			}
			Points->append(tempPoints);
		}
	}

	if (gqlObject.ContainsParam("RequiredPoints") && (gqlObject.GetObjectsCount("RequiredPoints") > 0)){
		const qsizetype requiredPointsElementsCount = gqlObject.GetObjectsCount("requiredPoints");
		if (requiredPointsElementsCount <= 0){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' is empty").arg(__FILE__, QString::number(__LINE__), "RequiredPoints");)

			return false;
		}
		RequiredPoints = QList<CPoint::V1_0>();
		for (qsizetype requiredPointsIndex = 0; requiredPointsIndex < requiredPointsElementsCount; ++requiredPointsIndex){
			const ::imtgql::CGqlParamObject* requiredPointsDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("RequiredPoints", requiredPointsIndex);
			if (requiredPointsDataObjectPtr == nullptr){
				qDebug() << "invalid type" << requiredPointsDataObjectPtr;
				return false;
			}
			CPoint::V1_0 tempRequiredPoints;
			if (!tempRequiredPoints.OptReadFromGraphQlObject(*requiredPointsDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "RequiredPoints");)

				return false;
			}
			RequiredPoints->append(tempRequiredPoints);
		}
	}

	if (gqlObject.ContainsParam("OptionalPoints") && (gqlObject.GetObjectsCount("OptionalPoints") > 0)){
		const qsizetype optionalPointsElementsCount = gqlObject.GetObjectsCount("optionalPoints");
		OptionalPoints = QList<CPoint::V1_0>();
		for (qsizetype optionalPointsIndex = 0; optionalPointsIndex < optionalPointsElementsCount; ++optionalPointsIndex){
			const ::imtgql::CGqlParamObject* optionalPointsDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("OptionalPoints", optionalPointsIndex);
			if (optionalPointsDataObjectPtr == nullptr){
				qDebug() << "invalid type" << optionalPointsDataObjectPtr;
				return false;
			}
			CPoint::V1_0 tempOptionalPoints;
			if (!tempOptionalPoints.OptReadFromGraphQlObject(*optionalPointsDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "OptionalPoints");)

				return false;
			}
			OptionalPoints->append(tempOptionalPoints);
		}
	}

	return true;
}


bool CGeometry::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!GeometryType){
		return false;
	}
	QString geometryTypeStringValue;
	switch (*GeometryType){
	case GeometryType::LINE:
		geometryTypeStringValue = "LINE";
		break;
	case GeometryType::POLYGON:
		geometryTypeStringValue = "POLYGON";
		break;
	case GeometryType::CIRCLE:
		geometryTypeStringValue = "CIRCLE";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	jsonObject["GeometryType"] = QJsonValue::fromVariant(geometryTypeStringValue);

	if (Radius){
		jsonObject["Radius"] = QJsonValue::fromVariant(*Radius);
	}

	if (!Points){
		return false;
	}
	QJsonArray newPointsArray;
	for (qsizetype pointsIndex = 0; pointsIndex < Points->size(); ++pointsIndex){
		QJsonObject newPointsJsonObject;
		if (!Points->at(pointsIndex).WriteToJsonObject(newPointsJsonObject)){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "Points");)

			return false;
		}
		newPointsArray << newPointsJsonObject;
	}
	jsonObject["Points"] = newPointsArray;

	if (!RequiredPoints || RequiredPoints->isEmpty()){
		return false;
	}
	QJsonArray newRequiredPointsArray;
	for (qsizetype requiredPointsIndex = 0; requiredPointsIndex < RequiredPoints->size(); ++requiredPointsIndex){
		QJsonObject newRequiredPointsJsonObject;
		if (!RequiredPoints->at(requiredPointsIndex).WriteToJsonObject(newRequiredPointsJsonObject)){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "RequiredPoints");)

			return false;
		}
		newRequiredPointsArray << newRequiredPointsJsonObject;
	}
	jsonObject["RequiredPoints"] = newRequiredPointsArray;

	if (OptionalPoints){
		QJsonArray newOptionalPointsArray;
		for (qsizetype optionalPointsIndex = 0; optionalPointsIndex < OptionalPoints->size(); ++optionalPointsIndex){
			QJsonObject newOptionalPointsJsonObject;
			if (!OptionalPoints->at(optionalPointsIndex).WriteToJsonObject(newOptionalPointsJsonObject)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "OptionalPoints");)

				return false;
			}
			newOptionalPointsArray << newOptionalPointsJsonObject;
		}
		jsonObject["OptionalPoints"] = newOptionalPointsArray;
	}

	return true;
}


bool CGeometry::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("GeometryType") || ! jsonObject["GeometryType"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "GeometryType");)

		return false;
	}
	const QString geometryTypeStringValue = jsonObject["GeometryType"].toString();
	if(geometryTypeStringValue == "LINE"){
		GeometryType = GeometryType::LINE;
	}
	else if(geometryTypeStringValue == "POLYGON"){
		GeometryType = GeometryType::POLYGON;
	}
	else if(geometryTypeStringValue == "CIRCLE"){
		GeometryType = GeometryType::CIRCLE;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "GeometryType", geometryTypeStringValue);)

		return false;
	}

	if (jsonObject.contains("Radius") && jsonObject["Radius"].isDouble()){
		Radius = jsonObject["Radius"].toDouble();
	}

	if (!jsonObject.contains("Points") || ! jsonObject["Points"].isArray()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Points");)

		return false;
	}
	const QJsonArray pointsJsonArray = jsonObject["Points"].toArray();
	const qsizetype pointsArrayCount = pointsJsonArray.size();
	Points = QList<CPoint::V1_0>();
	for (qsizetype pointsIndex = 0; pointsIndex < pointsArrayCount; ++pointsIndex){
		CPoint::V1_0 tempPoints;
		if (!tempPoints.ReadFromJsonObject(pointsJsonArray[pointsIndex].toObject())){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Points");)

			return false;
		}
		Points->append(tempPoints);
	}

	if (!jsonObject.contains("RequiredPoints") || ! jsonObject["RequiredPoints"].isArray()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "RequiredPoints");)

		return false;
	}
	const QJsonArray requiredPointsJsonArray = jsonObject["RequiredPoints"].toArray();
	const qsizetype requiredPointsArrayCount = requiredPointsJsonArray.size();
	if (requiredPointsArrayCount <= 0){
	I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' is empty").arg(__FILE__, QString::number(__LINE__), "RequiredPoints");)

		return false;
	}
	RequiredPoints = QList<CPoint::V1_0>();
	for (qsizetype requiredPointsIndex = 0; requiredPointsIndex < requiredPointsArrayCount; ++requiredPointsIndex){
		CPoint::V1_0 tempRequiredPoints;
		if (!tempRequiredPoints.ReadFromJsonObject(requiredPointsJsonArray[requiredPointsIndex].toObject())){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "RequiredPoints");)

			return false;
		}
		RequiredPoints->append(tempRequiredPoints);
	}

	if (jsonObject.contains("OptionalPoints") && jsonObject["OptionalPoints"].isArray()){
		const QJsonArray optionalPointsJsonArray = jsonObject["OptionalPoints"].toArray();
		const qsizetype optionalPointsArrayCount = optionalPointsJsonArray.size();
		OptionalPoints = QList<CPoint::V1_0>();
		for (qsizetype optionalPointsIndex = 0; optionalPointsIndex < optionalPointsArrayCount; ++optionalPointsIndex){
			CPoint::V1_0 tempOptionalPoints;
			if (!tempOptionalPoints.ReadFromJsonObject(optionalPointsJsonArray[optionalPointsIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "OptionalPoints");)

				return false;
			}
			OptionalPoints->append(tempOptionalPoints);
		}
	}

	return true;
}


bool CGeometry::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("GeometryType") && jsonObject["GeometryType"].isString()){
		const QString geometryTypeStringValue = jsonObject["GeometryType"].toString();
		if(geometryTypeStringValue == "LINE"){
			GeometryType = GeometryType::LINE;
		}
		else if(geometryTypeStringValue == "POLYGON"){
			GeometryType = GeometryType::POLYGON;
		}
		else if(geometryTypeStringValue == "CIRCLE"){
			GeometryType = GeometryType::CIRCLE;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "GeometryType", geometryTypeStringValue);)

			return false;
		}
	}

	if (jsonObject.contains("Radius") && jsonObject["Radius"].isDouble()){
		Radius = jsonObject["Radius"].toDouble();
	}

	if (jsonObject.contains("Points") && jsonObject["Points"].isArray()){
		const QJsonArray pointsJsonArray = jsonObject["Points"].toArray();
		const qsizetype pointsArrayCount = pointsJsonArray.size();
		Points = QList<CPoint::V1_0>();
		for (qsizetype pointsIndex = 0; pointsIndex < pointsArrayCount; ++pointsIndex){
			CPoint::V1_0 tempPoints;
			if (!tempPoints.OptReadFromJsonObject(pointsJsonArray[pointsIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Points");)

				return false;
			}
			Points->append(tempPoints);
		}
	}

	if (jsonObject.contains("RequiredPoints") && jsonObject["RequiredPoints"].isArray()){
		const QJsonArray requiredPointsJsonArray = jsonObject["RequiredPoints"].toArray();
		const qsizetype requiredPointsArrayCount = requiredPointsJsonArray.size();
		if (requiredPointsArrayCount <= 0){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' is empty").arg(__FILE__, QString::number(__LINE__), "RequiredPoints");)

			return false;
		}
		RequiredPoints = QList<CPoint::V1_0>();
		for (qsizetype requiredPointsIndex = 0; requiredPointsIndex < requiredPointsArrayCount; ++requiredPointsIndex){
			CPoint::V1_0 tempRequiredPoints;
			if (!tempRequiredPoints.OptReadFromJsonObject(requiredPointsJsonArray[requiredPointsIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "RequiredPoints");)

				return false;
			}
			RequiredPoints->append(tempRequiredPoints);
		}
	}

	if (jsonObject.contains("OptionalPoints") && jsonObject["OptionalPoints"].isArray()){
		const QJsonArray optionalPointsJsonArray = jsonObject["OptionalPoints"].toArray();
		const qsizetype optionalPointsArrayCount = optionalPointsJsonArray.size();
		OptionalPoints = QList<CPoint::V1_0>();
		for (qsizetype optionalPointsIndex = 0; optionalPointsIndex < optionalPointsArrayCount; ++optionalPointsIndex){
			CPoint::V1_0 tempOptionalPoints;
			if (!tempOptionalPoints.OptReadFromJsonObject(optionalPointsJsonArray[optionalPointsIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "OptionalPoints");)

				return false;
			}
			OptionalPoints->append(tempOptionalPoints);
		}
	}

	return true;
}


// serialize methods

bool CGeometry::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version object not initialized.").arg(__FILE__, QString::number(__LINE__));)

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


bool CGeometry::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
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


bool CGeometry::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
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


bool CGeometry::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version object not initialized.").arg(__FILE__, QString::number(__LINE__));)

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


bool CGeometry::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
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


bool CGeometry::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
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


bool CGeometry::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version object not initialized.").arg(__FILE__, QString::number(__LINE__));)

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


bool CGeometry::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
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


bool CGeometry::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
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


CGeometryObject::CGeometryObject(QObject* parent): ::imtbase::CItemModelBase(parent)			, m_pointsQObjectPtr(nullptr)
			, m_requiredPointsQObjectPtr(nullptr)
			, m_optionalPointsQObjectPtr(nullptr)
{
	Version_1_0.emplace();

	QObject::connect(this, &CGeometryObject::geometryTypeChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CGeometryObject::radiusChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CGeometryObject::pointsChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CGeometryObject::requiredPointsChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CGeometryObject::optionalPointsChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QString CGeometryObject::GetGeometryType()
{
	if (Version_1_0->GeometryType.has_value()){
		sdl::complextest::CTTypes::GeometryType valueType = Version_1_0->GeometryType.value();
		QMetaEnum metaEnum = QMetaEnum::fromType<sdl::complextest::CTTypes::GeometryType>();
		QString retval = metaEnum.valueToKey((int)valueType);

		return retval;
	}

	return QString();
}


void CGeometryObject::SetGeometryType(QString v)
{
	Version_1_0->GeometryType.emplace();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::complextest::CTTypes::GeometryType>();
	int key = metaEnum.keyToValue(v.toUtf8());
	if (key > -1){
		Version_1_0->GeometryType = (sdl::complextest::CTTypes::GeometryType)key;
	}
	geometryTypeChanged();
}


bool CGeometryObject::hasGeometryType()
{
	 return Version_1_0->GeometryType.HasValue();
}


double CGeometryObject::GetRadius()
{
	if (Version_1_0->Radius.has_value()){
		return Version_1_0->Radius.value();
	}

	return 0;
}


void CGeometryObject::SetRadius(double v)
{
	Version_1_0->Radius = v;
	radiusChanged();
}


bool CGeometryObject::hasRadius()
{
	 return Version_1_0->Radius.HasValue();
}


sdl::complextest::CTTypes::CPointObjectList* CGeometryObject::GetPoints()
{
	if (Version_1_0->Points.has_value()){
		if (!m_pointsQObjectPtr){
			m_pointsQObjectPtr = dynamic_cast<sdl::complextest::CTTypes::CPointObjectList*>(CreateObject("Points"));
			m_pointsQObjectPtr->Version_1_0 = Version_1_0->Points;
		}
		return m_pointsQObjectPtr;
	}

	return nullptr;
}


void CGeometryObject::SetPoints(sdl::complextest::CTTypes::CPointObjectList* v)
{
	if (v){
		Version_1_0->Points = v->Version_1_0;
		m_pointsQObjectPtr = v;
	}
	else {
		Version_1_0->Points = nullptr;
	}

	pointsChanged();
}


bool CGeometryObject::hasPoints()
{
	 return Version_1_0->Points.HasValue();
}


void CGeometryObject::createPoints()
{	Version_1_0->Points.emplace();

}


sdl::complextest::CTTypes::CPointObjectList* CGeometryObject::GetRequiredPoints()
{
	if (Version_1_0->RequiredPoints.has_value()){
		if (!m_requiredPointsQObjectPtr){
			m_requiredPointsQObjectPtr = dynamic_cast<sdl::complextest::CTTypes::CPointObjectList*>(CreateObject("RequiredPoints"));
			m_requiredPointsQObjectPtr->Version_1_0 = Version_1_0->RequiredPoints;
		}
		return m_requiredPointsQObjectPtr;
	}

	return nullptr;
}


void CGeometryObject::SetRequiredPoints(sdl::complextest::CTTypes::CPointObjectList* v)
{
	if (v){
		Version_1_0->RequiredPoints = v->Version_1_0;
		m_requiredPointsQObjectPtr = v;
	}
	else {
		Version_1_0->RequiredPoints = nullptr;
	}

	requiredPointsChanged();
}


bool CGeometryObject::hasRequiredPoints()
{
	 return Version_1_0->RequiredPoints.HasValue();
}


void CGeometryObject::createRequiredPoints()
{	Version_1_0->RequiredPoints.emplace();

}


sdl::complextest::CTTypes::CPointObjectList* CGeometryObject::GetOptionalPoints()
{
	if (Version_1_0->OptionalPoints.has_value()){
		if (!m_optionalPointsQObjectPtr){
			m_optionalPointsQObjectPtr = dynamic_cast<sdl::complextest::CTTypes::CPointObjectList*>(CreateObject("OptionalPoints"));
			m_optionalPointsQObjectPtr->Version_1_0 = Version_1_0->OptionalPoints;
		}
		return m_optionalPointsQObjectPtr;
	}

	return nullptr;
}


void CGeometryObject::SetOptionalPoints(sdl::complextest::CTTypes::CPointObjectList* v)
{
	if (v){
		Version_1_0->OptionalPoints = v->Version_1_0;
		m_optionalPointsQObjectPtr = v;
	}
	else {
		Version_1_0->OptionalPoints = nullptr;
	}

	optionalPointsChanged();
}


bool CGeometryObject::hasOptionalPoints()
{
	 return Version_1_0->OptionalPoints.HasValue();
}


void CGeometryObject::createOptionalPoints()
{	Version_1_0->OptionalPoints.emplace();

}


QString CGeometryObject::toJson() const
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


bool CGeometryObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CGeometryObject::fromObject(const QJsonObject& jsonObject)
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


QString CGeometryObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QObject* CGeometryObject::CreateObject(const QString& key)
{
	if (key == "Points"){
		return new sdl::complextest::CTTypes::CPointObjectList(this);
	}
	if (key == "RequiredPoints"){
		return new sdl::complextest::CTTypes::CPointObjectList(this);
	}
	if (key == "OptionalPoints"){
		return new sdl::complextest::CTTypes::CPointObjectList(this);
	}
	return nullptr;
}


QString CGeometryObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "geometryType")){
		return "GeometryType";
	}
	if (propertyName == (QString("m_") + "radius")){
		return "Radius";
	}
	if (propertyName == (QString("m_") + "points")){
		return "Points";
	}
	if (propertyName == (QString("m_") + "requiredPoints")){
		return "RequiredPoints";
	}
	if (propertyName == (QString("m_") + "optionalPoints")){
		return "OptionalPoints";
	}

	return propertyName;
}


} // namespace sdl::complextest::CTTypes
