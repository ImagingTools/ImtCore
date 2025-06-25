#include "CTTypes.h"


#include "CTTypes.h"


#include "CTTypes.h"


/// \file CPoint.cpp

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
		return false;
	}
	X = xData.toFloat();

	QVariant yData = model.GetData("Y", modelIndex);
	if (yData.isNull()){
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
	if (!gqlObject.ContainsParam("X") || (gqlObject["X"].userType() != QMetaType::Float && gqlObject["X"].userType() != QMetaType::Double)){
		return false;
	}
	X = gqlObject["X"].toDouble();

	if (!gqlObject.ContainsParam("Y") || (gqlObject["Y"].userType() != QMetaType::Float && gqlObject["Y"].userType() != QMetaType::Double)){
		return false;
	}
	Y = gqlObject["Y"].toDouble();

	return true;
}


bool CPoint::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("X") && (gqlObject["X"].userType() == QMetaType::Float || gqlObject["X"].userType() == QMetaType::Double)){
		X = gqlObject["X"].toDouble();
	}

	if (gqlObject.ContainsParam("Y") && (gqlObject["Y"].userType() == QMetaType::Float || gqlObject["Y"].userType() == QMetaType::Double)){
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
		return false;
	}
	X = jsonObject["X"].toDouble();

	if (!jsonObject.contains("Y") || ! jsonObject["Y"].isDouble()){
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


} // namespace sdl::complextest::CTTypes




/// \file CGeometry.cpp

namespace sdl::complextest::CTTypes
{


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
				Points == other.Points;
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
			return false;
		}
	}

	return true;
}


bool CGeometry::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant geometryTypeData = model.GetData("GeometryType", modelIndex);
	if (geometryTypeData.isNull()){
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
		return false;
	}

	QVariant radiusData = model.GetData("Radius", modelIndex);
	if (!radiusData.isNull()){
		Radius = radiusData.toFloat();
	}

	::imtbase::CTreeItemModel* pointsModel = model.GetTreeItemModel("Points", modelIndex);
	if (pointsModel == nullptr){
		return false;
	}
	int pointsCount = pointsModel->GetItemsCount();
	QList<CPoint::V1_0> pointsList;
	for (int pointsIndex = 0; pointsIndex < pointsCount; ++pointsIndex){
		CPoint::V1_0 points;
		if (!points.ReadFromModel(*pointsModel, pointsIndex)){
			return false;
		}
		pointsList << points;
	}
	Points = pointsList;


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
				return false;
			}
			pointsList << points;
		}
		Points = pointsList;

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
			return false;
		}
		pointsDataObjectList << newPointsGqlObject;
	}
	gqlObject.InsertParam("Points", pointsDataObjectList);

	return true;
}


bool CGeometry::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("GeometryType") || (gqlObject["GeometryType"].userType() != QMetaType::QString && gqlObject["GeometryType"].userType() != QMetaType::QByteArray)){
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
		return false;
	}

	if (gqlObject.ContainsParam("Radius") && (gqlObject["Radius"].userType() == QMetaType::Float || gqlObject["Radius"].userType() == QMetaType::Double)){
		Radius = gqlObject["Radius"].toDouble();
	}

	if (!gqlObject.ContainsParam("Points") || (gqlObject.GetObjectsCount("Points") <= 0)){
		return false;
	}
	const qsizetype pointsElementsCount = gqlObject.GetObjectsCount("points");
	Points = QList<CPoint::V1_0>();
	for (qsizetype pointsIndex = 0; pointsIndex < pointsElementsCount; ++pointsIndex){
		const ::imtgql::CGqlParamObject* pointsDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("Points", pointsIndex);
		if (pointsDataObjectPtr == nullptr){
			return false;
		}
		CPoint::V1_0 tempPoints;
		if (!tempPoints.ReadFromGraphQlObject(*pointsDataObjectPtr)){
			return false;
		}
		Points->append(tempPoints);
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
			return false;
		}
	}

	if (gqlObject.ContainsParam("Radius") && (gqlObject["Radius"].userType() == QMetaType::Float || gqlObject["Radius"].userType() == QMetaType::Double)){
		Radius = gqlObject["Radius"].toDouble();
	}

	if (gqlObject.ContainsParam("Points") && (gqlObject.GetObjectsCount("Points") > 0)){
		const qsizetype pointsElementsCount = gqlObject.GetObjectsCount("points");
		Points = QList<CPoint::V1_0>();
		for (qsizetype pointsIndex = 0; pointsIndex < pointsElementsCount; ++pointsIndex){
			const ::imtgql::CGqlParamObject* pointsDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("Points", pointsIndex);
			if (pointsDataObjectPtr == nullptr){
				return false;
			}
			CPoint::V1_0 tempPoints;
			if (!tempPoints.OptReadFromGraphQlObject(*pointsDataObjectPtr)){
				return false;
			}
			Points->append(tempPoints);
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
			return false;
		}
		newPointsArray << newPointsJsonObject;
	}
	jsonObject["Points"] = newPointsArray;

	return true;
}


bool CGeometry::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("GeometryType") || ! jsonObject["GeometryType"].isString()){
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
		return false;
	}

	if (jsonObject.contains("Radius") && jsonObject["Radius"].isDouble()){
		Radius = jsonObject["Radius"].toDouble();
	}

	if (!jsonObject.contains("Points") || ! jsonObject["Points"].isArray()){
		return false;
	}
	const QJsonArray pointsJsonArray = jsonObject["Points"].toArray();
	const qsizetype pointsArrayCount = pointsJsonArray.size();
	Points = QList<CPoint::V1_0>();
	for (qsizetype pointsIndex = 0; pointsIndex < pointsArrayCount; ++pointsIndex){
		CPoint::V1_0 tempPoints;
		if (!tempPoints.ReadFromJsonObject(pointsJsonArray[pointsIndex].toObject())){
			return false;
		}
		Points->append(tempPoints);
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
				return false;
			}
			Points->append(tempPoints);
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


} // namespace sdl::complextest::CTTypes




