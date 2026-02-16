#include "PrinterBase.h"


namespace sdl::modsdl::PrinterBase
{


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
				link == other.link;
}


bool CLink::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "Link", modelIndex);

	if (!link){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "link").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("link", *link, modelIndex);


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

	return true;
}


bool CLink::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant linkData = model.GetData("link", modelIndex);
	if (!linkData.isNull()){
		link = linkData.toByteArray();
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

	return true;
}


bool CLink::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("link") && (gqlObject["link"].userType() == QMetaType::QString || gqlObject["link"].userType() == QMetaType::QByteArray)){
		link = gqlObject["link"].toByteArray();
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

	return true;
}


bool CLink::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("link") && jsonObject["link"].isString()){
		link = jsonObject["link"].toString().toUtf8();
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
				specification == other.specification &&
				simpleTest == other.simpleTest &&
				mixedTest == other.mixedTest;
}


bool CPrinterBase::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "PrinterBase", modelIndex);

	if (name){
		model.SetData("name", *name, modelIndex);
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


	if (!simpleTest){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "simpleTest").toLocal8Bit().constData();)

		return false;
	}
	QVariant simpleTestVariantValue;
		if (const QString* val = std::get_if<QString>(simpleTest.GetPtr())){
			model.SetData("", *val);
		}
		else if (const double* val = std::get_if<double>(simpleTest.GetPtr())){
			model.SetData("", *val);
		}


	if (!mixedTest){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "mixedTest").toLocal8Bit().constData();)

		return false;
	}
	QVariant mixedTestVariantValue;
		if (const QString* val = std::get_if<QString>(mixedTest.GetPtr())){
			model.SetData("", *val);
		}
		else if (const CLink* val = std::get_if<CLink>(mixedTest.GetPtr())){
			if (!val->WriteToModel(*(model.AddTreeModel("mixedTest", modelIndex)), 0)){
				return false;
			}
			if(model.GetTreeItemModel("mixedTest", modelIndex) != nullptr){
				model.GetTreeItemModel("mixedTest", modelIndex)->SetData("__typename", "Link", 0);
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

	QVariant simpleTestData = model.GetData("simpleTest", modelIndex);
	if (simpleTestData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "simpleTest").toLocal8Bit().constData();)

		return false;
	}
	QString simpleTestTypename = simpleTestData.value<::imtbase::CTreeItemModel*>()->GetData("__typename").toString();
	if (simpleTestData.canConvert<QString>()){
		simpleTest = SimpleUnion(simpleTestData.value<QString>());
	}
	else if (simpleTestData.canConvert<double>()){
		simpleTest = SimpleUnion(simpleTestData.value<double>());
	}

	QVariant mixedTestData = model.GetData("mixedTest", modelIndex);
	if (mixedTestData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "mixedTest").toLocal8Bit().constData();)

		return false;
	}
	QString mixedTestTypename = mixedTestData.value<::imtbase::CTreeItemModel*>()->GetData("__typename").toString();
	if (mixedTestData.canConvert<QString>()){
		mixedTest = MixedUnion(mixedTestData.value<QString>());
	}
	else if (mixedTestTypename == "Link") {
		CLink mixedTestConvert;
		const bool ismixedTestRead = mixedTestConvert.ReadFromModel(*model.GetTreeItemModel("mixedTest", modelIndex)); 
		if (!ismixedTestRead){
			return false;
		}
		mixedTest = MixedUnion(mixedTestConvert);
	}

	return true;
}


bool CPrinterBase::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant nameData = model.GetData("name", modelIndex);
	if (!nameData.isNull()){
		name = nameData.toString();
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
	}

	QVariant simpleTestData = model.GetData("simpleTest", modelIndex);
	if (!simpleTestData.isNull()){
		QString simpleTestTypename = simpleTestData.value<::imtbase::CTreeItemModel*>()->GetData("__typename").toString();
		if (simpleTestData.canConvert<QString>()){
			simpleTest = SimpleUnion(simpleTestData.value<QString>());
		}
		else if (simpleTestData.canConvert<double>()){
			simpleTest = SimpleUnion(simpleTestData.value<double>());
		}
	}

	QVariant mixedTestData = model.GetData("mixedTest", modelIndex);
	if (!mixedTestData.isNull()){
		QString mixedTestTypename = mixedTestData.value<::imtbase::CTreeItemModel*>()->GetData("__typename").toString();
		if (mixedTestData.canConvert<QString>()){
			mixedTest = MixedUnion(mixedTestData.value<QString>());
		}
		else if (mixedTestTypename == "Link") {
			CLink mixedTestConvert;
			const bool ismixedTestRead = mixedTestConvert.ReadFromModel(*model.GetTreeItemModel("mixedTest", modelIndex)); 
			if (!ismixedTestRead){
				return false;
			}
			mixedTest = MixedUnion(mixedTestConvert);
		}
	}

	return true;
}


bool CPrinterBase::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (name){
		gqlObject.InsertParam("name", QVariant(*name));
	}

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
	gqlObject.InsertParam("specification", specificationDataObject);

	if (!simpleTest){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "simpleTest").toLocal8Bit().constData();)

		return false;
	}
	::imtgql::CGqlParamObject simpleTestDataObject;
	if (const QString* val = std::get_if<QString>(simpleTest.GetPtr())){
		simpleTestDataObject.InsertParam("simpleTest", *val);
	}
	else if (const double* val = std::get_if<double>(simpleTest.GetPtr())){
		simpleTestDataObject.InsertParam("simpleTest", *val);
	}
	gqlObject.InsertParam("simpleTest", simpleTestDataObject);

	if (!mixedTest){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "mixedTest").toLocal8Bit().constData();)

		return false;
	}
	::imtgql::CGqlParamObject mixedTestDataObject;
	if (const QString* val = std::get_if<QString>(mixedTest.GetPtr())){
		mixedTestDataObject.InsertParam("mixedTest", *val);
	}
	else if (const CLink* val = std::get_if<CLink>(mixedTest.GetPtr())){
		if (!val->WriteToGraphQlObject(mixedTestDataObject)){
			return false;
		}
		mixedTestDataObject.InsertParam("__typename", QVariant("Link"));
	}
	gqlObject.InsertParam("mixedTest", mixedTestDataObject);

	gqlObject.InsertParam("__typename", QVariant("PrinterBase"));

	return true;
}


bool CPrinterBase::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("name") && (gqlObject["name"].userType() == QMetaType::QString || gqlObject["name"].userType() == QMetaType::QByteArray)){
		name = gqlObject["name"].toString();
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

	if (!gqlObject.ContainsParam("simpleTest")){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "simpleTest").toLocal8Bit().constData();)

		return false;
	}
	QVariant simpleTestVariantValue = gqlObject["simpleTest"];
	if (simpleTestVariantValue.canConvert<QString>()){
		simpleTest = SimpleUnion(simpleTestVariantValue.value<QString>());
	}
	else if (simpleTestVariantValue.canConvert<double>()){
		simpleTest = SimpleUnion(simpleTestVariantValue.value<double>());
	}

	if (!gqlObject.ContainsParam("mixedTest")){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "mixedTest").toLocal8Bit().constData();)

		return false;
	}
	const ::imtgql::CGqlParamObject* mixedTestDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("mixedTest");
	if (!mixedTestDataObjectPtr->ContainsParam("__typename")){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "mixedTest").toLocal8Bit().constData();)

		return false;
	}
	QString mixedTestTypename = mixedTestDataObjectPtr->GetParamArgumentValue("__typename").toString();
	QVariant mixedTestVariantValue = gqlObject["mixedTest"];
	if (mixedTestVariantValue.canConvert<QString>()){
		mixedTest = MixedUnion(mixedTestVariantValue.value<QString>());
	}
	else if (mixedTestTypename == "Link") {
		CLink mixedTestConvert;
		const bool isMixedTestRead = mixedTestConvert.ReadFromGraphQlObject(*mixedTestDataObjectPtr);
		if (!isMixedTestRead){
			return false;
		}
		mixedTest = MixedUnion(mixedTestConvert);
	}

	return true;
}


bool CPrinterBase::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("name") && (gqlObject["name"].userType() == QMetaType::QString || gqlObject["name"].userType() == QMetaType::QByteArray)){
		name = gqlObject["name"].toString();
	}

	if (gqlObject.ContainsParam("specification")){
		const ::imtgql::CGqlParamObject* specificationDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("specification");
		if (!specificationDataObjectPtr->ContainsParam("__typename")){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "specification").toLocal8Bit().constData();)

			return false;
		}
		QString specificationTypename = specificationDataObjectPtr->GetParamArgumentValue("__typename").toString();
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
	}

	if (gqlObject.ContainsParam("simpleTest")){
		QVariant simpleTestVariantValue = gqlObject["simpleTest"];
		if (simpleTestVariantValue.canConvert<QString>()){
			simpleTest = SimpleUnion(simpleTestVariantValue.value<QString>());
		}
		else if (simpleTestVariantValue.canConvert<double>()){
			simpleTest = SimpleUnion(simpleTestVariantValue.value<double>());
		}
	}

	if (gqlObject.ContainsParam("mixedTest")){
		const ::imtgql::CGqlParamObject* mixedTestDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("mixedTest");
		if (!mixedTestDataObjectPtr->ContainsParam("__typename")){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "mixedTest").toLocal8Bit().constData();)

			return false;
		}
		QString mixedTestTypename = mixedTestDataObjectPtr->GetParamArgumentValue("__typename").toString();
		QVariant mixedTestVariantValue = gqlObject["mixedTest"];
		if (mixedTestVariantValue.canConvert<QString>()){
			mixedTest = MixedUnion(mixedTestVariantValue.value<QString>());
		}
		else if (mixedTestTypename == "Link") {
			CLink mixedTestConvert;
			const bool isMixedTestRead = mixedTestConvert.ReadFromGraphQlObject(*mixedTestDataObjectPtr);
			if (!isMixedTestRead){
				return false;
			}
			mixedTest = MixedUnion(mixedTestConvert);
		}
	}

	return true;
}


bool CPrinterBase::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (name){
		jsonObject["name"] = QJsonValue::fromVariant(*name);
	}

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


	if (!simpleTest){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "simpleTest").toLocal8Bit().constData();)

		return false;
	}
	if (const QString* val = std::get_if<QString>(simpleTest.GetPtr())){
		if (!simpleTest){
			return false;
		}
		jsonObject["simpleTest"] = QJsonValue::fromVariant(*val);
	}
	else if (const double* val = std::get_if<double>(simpleTest.GetPtr())){
		if (!simpleTest){
			return false;
		}
		jsonObject["simpleTest"] = QJsonValue::fromVariant(*val);
	}


	if (!mixedTest){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "mixedTest").toLocal8Bit().constData();)

		return false;
	}
	if (const QString* val = std::get_if<QString>(mixedTest.GetPtr())){
		if (!mixedTest){
			return false;
		}
		jsonObject["mixedTest"] = QJsonValue::fromVariant(*val);
	}
	else if (const CLink* val = std::get_if<CLink>(mixedTest.GetPtr())){
		QJsonObject mixedTestJsonObject;
		const bool ismixedTestAdded = val->WriteToJsonObject(mixedTestJsonObject);
		if (!ismixedTestAdded){
			return false;
		}
		mixedTestJsonObject["__typename"] = "Link";
		jsonObject["mixedTest"] = mixedTestJsonObject;
	}


	jsonObject["__typename"] = "PrinterBase";

	return true;
}


bool CPrinterBase::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("name") && jsonObject["name"].isString()){
		name = jsonObject["name"].toString();
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

	if (!jsonObject.contains("simpleTest")){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "simpleTest").toLocal8Bit().constData();)

		return false;
	}
	QVariant simpleTestVariantValue = jsonObject["simpleTest"].toVariant();
	if (simpleTestVariantValue.canConvert<QString>()){
		simpleTest = SimpleUnion(simpleTestVariantValue.value<QString>());
	}
	else if (simpleTestVariantValue.canConvert<double>()){
		simpleTest = SimpleUnion(simpleTestVariantValue.value<double>());
	}

	if (!jsonObject.contains("mixedTest")){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "mixedTest").toLocal8Bit().constData();)

		return false;
	}
	const QJsonObject mixedTestObject = jsonObject.value("mixedTest").toObject();
	if (!jsonObject.value("mixedTest").isObject() || !mixedTestObject.contains("__typename")){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "mixedTest").toLocal8Bit().constData();)

		return false;
	}
	QString mixedTestTypename = mixedTestObject.value("__typename").toString();
	QVariant mixedTestVariantValue = jsonObject["mixedTest"].toVariant();
	if (mixedTestVariantValue.canConvert<QString>()){
		mixedTest = MixedUnion(mixedTestVariantValue.value<QString>());
	}
	else if (mixedTestTypename == "Link") {
		CLink mixedTestConvert;
		const bool ismixedTestRead = mixedTestConvert.ReadFromJsonObject(jsonObject["mixedTest"].toObject());
		if (!ismixedTestRead){
			return false;
		}
		mixedTest = MixedUnion(mixedTestConvert);
	}

	return true;
}


bool CPrinterBase::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("name") && jsonObject["name"].isString()){
		name = jsonObject["name"].toString();
	}

	if (jsonObject.contains("specification")){
		const QJsonObject specificationObject = jsonObject.value("specification").toObject();
		if (!jsonObject.value("specification").isObject() || !specificationObject.contains("__typename")){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "specification").toLocal8Bit().constData();)

			return false;
		}
		QString specificationTypename = specificationObject.value("__typename").toString();
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
	}

	if (jsonObject.contains("simpleTest")){
		QVariant simpleTestVariantValue = jsonObject["simpleTest"].toVariant();
		if (simpleTestVariantValue.canConvert<QString>()){
			simpleTest = SimpleUnion(simpleTestVariantValue.value<QString>());
		}
		else if (simpleTestVariantValue.canConvert<double>()){
			simpleTest = SimpleUnion(simpleTestVariantValue.value<double>());
		}
	}

	if (jsonObject.contains("mixedTest")){
		const QJsonObject mixedTestObject = jsonObject.value("mixedTest").toObject();
		if (!jsonObject.value("mixedTest").isObject() || !mixedTestObject.contains("__typename")){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "mixedTest").toLocal8Bit().constData();)

			return false;
		}
		QString mixedTestTypename = mixedTestObject.value("__typename").toString();
		QVariant mixedTestVariantValue = jsonObject["mixedTest"].toVariant();
		if (mixedTestVariantValue.canConvert<QString>()){
			mixedTest = MixedUnion(mixedTestVariantValue.value<QString>());
		}
		else if (mixedTestTypename == "Link") {
			CLink mixedTestConvert;
			const bool ismixedTestRead = mixedTestConvert.ReadFromJsonObject(jsonObject["mixedTest"].toObject());
			if (!ismixedTestRead){
				return false;
			}
			mixedTest = MixedUnion(mixedTestConvert);
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




QByteArray CPrinterList::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CPrinterList::V1_0::operator==(const V1_0& other) const
{
	return 
				data == other.data;
}


bool CPrinterList::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "PrinterList", modelIndex);

	if (data){
		::imtbase::CTreeItemModel* newDataModelPtr = model.AddTreeModel("data", modelIndex);
		newDataModelPtr->setIsArray(true);
		for (qsizetype dataIndex = 0; dataIndex < data->size(); ++dataIndex){
			newDataModelPtr->InsertNewItem();
			if (!(data->at(dataIndex)->WriteToModel(*newDataModelPtr, dataIndex))){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "data").toLocal8Bit().constData();)

				return false;
			}
		}
	}

	return true;
}


bool CPrinterList::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	::imtbase::CTreeItemModel* dataModel = model.GetTreeItemModel("data", modelIndex);
	if (dataModel != nullptr){
		int dataCount = dataModel->GetItemsCount();
		imtsdl::TElementList<CPrinterBase::V1_0> dataList;
		for (int dataIndex = 0; dataIndex < dataCount; ++dataIndex){
			CPrinterBase::V1_0 t_data;
			if (!t_data.ReadFromModel(*dataModel, dataIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "data").toLocal8Bit().constData();)

				return false;
			}
			dataList << t_data;
		}
		data = dataList;

	}

	return true;
}


bool CPrinterList::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	::imtbase::CTreeItemModel* dataModel = model.GetTreeItemModel("data", modelIndex);
	if (dataModel != nullptr){
		int dataCount = dataModel->GetItemsCount();
		imtsdl::TElementList<CPrinterBase::V1_0> dataList;
		for (int dataIndex = 0; dataIndex < dataCount; ++dataIndex){
			CPrinterBase::V1_0 t_data;
			if (!t_data.OptReadFromModel(*dataModel, dataIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "data").toLocal8Bit().constData();)

				return false;
			}
			dataList << t_data;
		}
		data = dataList;

	}

	return true;
}


bool CPrinterList::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (data){
		QList<::imtgql::CGqlParamObject> dataDataObjectList;
		for (qsizetype dataIndex = 0; dataIndex < data->size(); ++dataIndex){
			::imtgql::CGqlParamObject newDataGqlObject;
			if (!data->at(dataIndex)->WriteToGraphQlObject(newDataGqlObject)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "data").toLocal8Bit().constData();)

				return false;
			}
			dataDataObjectList << newDataGqlObject;
		}
		gqlObject.InsertParam("data", dataDataObjectList);
	}

	gqlObject.InsertParam("__typename", QVariant("PrinterList"));

	return true;
}


bool CPrinterList::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("data")){
		data.emplace();
	}
	if (gqlObject.ContainsParam("data") && (gqlObject.GetObjectsCount("data") > 0)){
		const qsizetype dataElementsCount = gqlObject.GetObjectsCount("data");
		data = imtsdl::TElementList<CPrinterBase::V1_0>();
		for (qsizetype dataIndex = 0; dataIndex < dataElementsCount; ++dataIndex){
			const ::imtgql::CGqlParamObject* dataDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("data", dataIndex);
			if (dataDataObjectPtr == nullptr){
				qDebug() << "invalid type" << dataDataObjectPtr;
				return false;
			}
			CPrinterBase::V1_0 tempData;
			if (!tempData.ReadFromGraphQlObject(*dataDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "data").toLocal8Bit().constData();)

				return false;
			}
			data->append(tempData);
		}
	}

	return true;
}


bool CPrinterList::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("data")){
		data.emplace();
	}
	if (gqlObject.ContainsParam("data") && (gqlObject.GetObjectsCount("data") > 0)){
		const qsizetype dataElementsCount = gqlObject.GetObjectsCount("data");
		data = imtsdl::TElementList<CPrinterBase::V1_0>();
		for (qsizetype dataIndex = 0; dataIndex < dataElementsCount; ++dataIndex){
			const ::imtgql::CGqlParamObject* dataDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("data", dataIndex);
			if (dataDataObjectPtr == nullptr){
				qDebug() << "invalid type" << dataDataObjectPtr;
				return false;
			}
			CPrinterBase::V1_0 tempData;
			if (!tempData.OptReadFromGraphQlObject(*dataDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "data").toLocal8Bit().constData();)

				return false;
			}
			data->append(tempData);
		}
	}

	return true;
}


bool CPrinterList::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (data){
		QJsonArray newDataArray;
		for (qsizetype dataIndex = 0; dataIndex < data->size(); ++dataIndex){
			QJsonObject newDataJsonObject;
			if (!data->at(dataIndex)->WriteToJsonObject(newDataJsonObject)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "data").toLocal8Bit().constData();)

				return false;
			}
			newDataArray << newDataJsonObject;
		}
		jsonObject["data"] = newDataArray;
	}

	jsonObject["__typename"] = "PrinterList";

	return true;
}


bool CPrinterList::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("data") && jsonObject["data"].isArray()){
		const QJsonArray dataJsonArray = jsonObject["data"].toArray();
		const qsizetype dataArrayCount = dataJsonArray.size();
		data = imtsdl::TElementList<CPrinterBase::V1_0>();
		for (qsizetype dataIndex = 0; dataIndex < dataArrayCount; ++dataIndex){
			CPrinterBase::V1_0 tempData;
			if (!tempData.ReadFromJsonObject(dataJsonArray[dataIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "data").toLocal8Bit().constData();)

				return false;
			}
			data->append(tempData);
		}
	}

	return true;
}


bool CPrinterList::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("data") && jsonObject["data"].isArray()){
		const QJsonArray dataJsonArray = jsonObject["data"].toArray();
		const qsizetype dataArrayCount = dataJsonArray.size();
		data = imtsdl::TElementList<CPrinterBase::V1_0>();
		for (qsizetype dataIndex = 0; dataIndex < dataArrayCount; ++dataIndex){
			CPrinterBase::V1_0 tempData;
			if (!tempData.OptReadFromJsonObject(dataJsonArray[dataIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "data").toLocal8Bit().constData();)

				return false;
			}
			data->append(tempData);
		}
	}

	return true;
}


// serialize methods

bool CPrinterList::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
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


bool CPrinterList::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
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


bool CPrinterList::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
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


bool CPrinterList::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
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


bool CPrinterList::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
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


bool CPrinterList::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
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


bool CPrinterList::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
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


bool CPrinterList::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
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


bool CPrinterList::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
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
	Version_1_0->name = v.value<QString>();
	nameChanged();
}


bool CPrinterSpecificationBaseObject::hasName()
{
	 return Version_1_0->name.HasValue();
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


void CPrinterSpecificationBaseObjectList::append(sdl::modsdl::PrinterBase::CPrinterSpecificationBaseObject* item)
{
	BaseClass::append(item);
}


sdl::modsdl::PrinterBase::CPrinterSpecificationBaseObjectList* sdl::modsdl::PrinterBase::CPrinterSpecificationBaseObjectList::copyMe()
{
	sdl::modsdl::PrinterBase::CPrinterSpecificationBaseObjectList* retVal = new sdl::modsdl::PrinterBase::CPrinterSpecificationBaseObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::modsdl::PrinterBase::CPrinterSpecificationBaseObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::modsdl::PrinterBase::CPrinterSpecificationBaseObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::modsdl::PrinterBase::CPrinterSpecificationBaseObjectList::addElement(sdl::modsdl::PrinterBase::CPrinterSpecificationBaseObject* item)
{
	append(item);
}


void sdl::modsdl::PrinterBase::CPrinterSpecificationBaseObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::modsdl::PrinterBase::CPrinterSpecificationBaseObjectList::isEqualWithModel(sdl::modsdl::PrinterBase::CPrinterSpecificationBaseObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::modsdl::PrinterBase::CPrinterSpecificationBaseObjectList::insert(int index, sdl::modsdl::PrinterBase::CPrinterSpecificationBaseObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::modsdl::PrinterBase::CPrinterSpecificationBaseObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::modsdl::PrinterBase::CPrinterSpecificationBaseObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::modsdl::PrinterBase::CPrinterSpecificationBaseObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::modsdl::PrinterBase::CPrinterSpecificationBaseObject*>();
	if (itemPtr == nullptr) return QVariant();
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
		if (nameId == "m_name"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index)->name.value());
		}
	return QVariant();
}
CLinkObject::CLinkObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &CLinkObject::linkChanged, this, &CItemModelBase::OnInternalModelChanged);
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
	Version_1_0->link = v.value<QString>().toUtf8();
	linkChanged();
}


bool CLinkObject::hasLink()
{
	 return Version_1_0->link.HasValue();
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


void CLinkObjectList::append(sdl::modsdl::PrinterBase::CLinkObject* item)
{
	BaseClass::append(item);
}


sdl::modsdl::PrinterBase::CLinkObjectList* sdl::modsdl::PrinterBase::CLinkObjectList::copyMe()
{
	sdl::modsdl::PrinterBase::CLinkObjectList* retVal = new sdl::modsdl::PrinterBase::CLinkObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::modsdl::PrinterBase::CLinkObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::modsdl::PrinterBase::CLinkObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::modsdl::PrinterBase::CLinkObjectList::addElement(sdl::modsdl::PrinterBase::CLinkObject* item)
{
	append(item);
}


void sdl::modsdl::PrinterBase::CLinkObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::modsdl::PrinterBase::CLinkObjectList::isEqualWithModel(sdl::modsdl::PrinterBase::CLinkObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::modsdl::PrinterBase::CLinkObjectList::insert(int index, sdl::modsdl::PrinterBase::CLinkObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::modsdl::PrinterBase::CLinkObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::modsdl::PrinterBase::CLinkObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::modsdl::PrinterBase::CLinkObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::modsdl::PrinterBase::CLinkObject*>();
	if (itemPtr == nullptr) return QVariant();
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
		if (nameId == "m_link"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index)->link.value());
		}
	return QVariant();
}
CPrinterBaseObject::CPrinterBaseObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &CPrinterBaseObject::nameChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CPrinterBaseObject::specificationChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CPrinterBaseObject::simpleTestChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CPrinterBaseObject::mixedTestChanged, this, &CItemModelBase::OnInternalModelChanged);
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
	Version_1_0->name = v.value<QString>();
	nameChanged();
}


bool CPrinterBaseObject::hasName()
{
	 return Version_1_0->name.HasValue();
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
		}
		return m_specificationQObjectPtr;
	}

	return QVariant();
}


void CPrinterBaseObject::SetSpecification(const QVariant& v)
{
	if (v.isValid()){
		if (const CPrinterSpecificationBaseObject* val = v.value<const CPrinterSpecificationBaseObject*>()){
			*Version_1_0->specification = *val;
		}
		if (const CLinkObject* val = v.value<const CLinkObject*>()){
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
	 return Version_1_0->specification.HasValue();
}


void CPrinterBaseObject::emplaceSpecification()
{
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


QVariant CPrinterBaseObject::GetSimpleTest()
{
	if (Version_1_0 && Version_1_0->simpleTest){
		if (m_simpleTestQObjectPtr.isValid()){
			if (const QString* val = std::get_if<QString>((Version_1_0->simpleTest).GetPtr())){
				m_simpleTestQObjectPtr = QVariant::fromValue(val);
			}
			if (const double* val = std::get_if<double>((Version_1_0->simpleTest).GetPtr())){
				m_simpleTestQObjectPtr = QVariant::fromValue(val);
			}
		}
		return m_simpleTestQObjectPtr;
	}

	return QVariant();
}


void CPrinterBaseObject::SetSimpleTest(const QVariant& v)
{
	if (v.isValid()){
		if (const QString* val = v.value<const QString*>()){
			*Version_1_0->simpleTest = *val;
		}
		if (const double* val = v.value<const double*>()){
			*Version_1_0->simpleTest = *val;
		}
	}
	else {
		Version_1_0->simpleTest = nullptr;
	}
	m_simpleTestQObjectPtr = v;

	simpleTestChanged();
}


bool CPrinterBaseObject::hasSimpleTest()
{
	 return Version_1_0->simpleTest.HasValue();
}


void CPrinterBaseObject::emplaceSimpleTest()
{
	Version_1_0->simpleTest.emplace();

}


void CPrinterBaseObject::ResetSimpleTest()
{
	auto* itemPtr = m_simpleTestQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_simpleTestQObjectPtr = QVariant();
}


QVariant CPrinterBaseObject::GetMixedTest()
{
	if (Version_1_0 && Version_1_0->mixedTest){
		if (m_mixedTestQObjectPtr.isValid()){
			if (const QString* val = std::get_if<QString>((Version_1_0->mixedTest).GetPtr())){
				m_mixedTestQObjectPtr = QVariant::fromValue(val);
			}
			if (const CLink* val = std::get_if<CLink>((Version_1_0->mixedTest).GetPtr())){
				CLinkObject *newObjectPtr = new CLinkObject(this);
				newObjectPtr->Version_1_0 = val->Version_1_0;
				m_mixedTestQObjectPtr = QVariant::fromValue(newObjectPtr);
			}
		}
		return m_mixedTestQObjectPtr;
	}

	return QVariant();
}


void CPrinterBaseObject::SetMixedTest(const QVariant& v)
{
	if (v.isValid()){
		if (const QString* val = v.value<const QString*>()){
			*Version_1_0->mixedTest = *val;
		}
		if (const CLinkObject* val = v.value<const CLinkObject*>()){
			*Version_1_0->mixedTest = *val;
		}
	}
	else {
		Version_1_0->mixedTest = nullptr;
	}
	m_mixedTestQObjectPtr = v;

	mixedTestChanged();
}


bool CPrinterBaseObject::hasMixedTest()
{
	 return Version_1_0->mixedTest.HasValue();
}


void CPrinterBaseObject::emplaceMixedTest()
{
	Version_1_0->mixedTest.emplace();

}


void CPrinterBaseObject::ResetMixedTest()
{
	auto* itemPtr = m_mixedTestQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_mixedTestQObjectPtr = QVariant();
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
	Q_UNUSED(key);	return QVariant();
}


QString CPrinterBaseObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "name")){
		return "name";
	}
	if (propertyName == (QString("m_") + "specification")){
		return "specification";
	}
	if (propertyName == (QString("m_") + "simpleTest")){
		return "simpleTest";
	}
	if (propertyName == (QString("m_") + "mixedTest")){
		return "mixedTest";
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


void CPrinterBaseObjectList::append(sdl::modsdl::PrinterBase::CPrinterBaseObject* item)
{
	BaseClass::append(item);
}


sdl::modsdl::PrinterBase::CPrinterBaseObjectList* sdl::modsdl::PrinterBase::CPrinterBaseObjectList::copyMe()
{
	sdl::modsdl::PrinterBase::CPrinterBaseObjectList* retVal = new sdl::modsdl::PrinterBase::CPrinterBaseObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::modsdl::PrinterBase::CPrinterBaseObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::modsdl::PrinterBase::CPrinterBaseObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::modsdl::PrinterBase::CPrinterBaseObjectList::addElement(sdl::modsdl::PrinterBase::CPrinterBaseObject* item)
{
	append(item);
}


void sdl::modsdl::PrinterBase::CPrinterBaseObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::modsdl::PrinterBase::CPrinterBaseObjectList::isEqualWithModel(sdl::modsdl::PrinterBase::CPrinterBaseObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::modsdl::PrinterBase::CPrinterBaseObjectList::insert(int index, sdl::modsdl::PrinterBase::CPrinterBaseObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::modsdl::PrinterBase::CPrinterBaseObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::modsdl::PrinterBase::CPrinterBaseObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::modsdl::PrinterBase::CPrinterBaseObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::modsdl::PrinterBase::CPrinterBaseObject*>();
	if (itemPtr == nullptr) return QVariant();
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
		if (nameId == "m_name"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index)->name.value());
		}
		if (nameId == "m_specification"){
			return itemPtr->GetSpecification();
		}
		if (nameId == "m_simpleTest"){
			return itemPtr->GetSimpleTest();
		}
		if (nameId == "m_mixedTest"){
			return itemPtr->GetMixedTest();
		}
	return QVariant();
}
CPrinterListObject::CPrinterListObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &CPrinterListObject::dataChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CPrinterListObject::GetData()
{
	if (Version_1_0 && Version_1_0->data){
		if (!m_dataQObjectPtr.isValid()){
			m_dataQObjectPtr = CreateObject("data");
			auto itemPtr = m_dataQObjectPtr.value<sdl::modsdl::PrinterBase::CPrinterBaseObjectList*>();
			if (itemPtr != nullptr) itemPtr->Version_1_0 = Version_1_0->data;
		}
		return m_dataQObjectPtr;
	}

	return QVariant();
}


void CPrinterListObject::SetData(const QVariant& v)
{
	if (v.isValid()){
		sdl::modsdl::PrinterBase::CPrinterBaseObjectList* itemPtr = v.value<sdl::modsdl::PrinterBase::CPrinterBaseObjectList*>();
		if (itemPtr != nullptr)  Version_1_0->data = itemPtr->Version_1_0;
	}
	else {
		Version_1_0->data = nullptr;
	}
	m_dataQObjectPtr = v;

	dataChanged();
}


bool CPrinterListObject::hasData()
{
	 return Version_1_0->data.HasValue();
}


void CPrinterListObject::emplaceData()
{
	Version_1_0->data.emplace();

}


void CPrinterListObject::ResetData()
{
	auto* itemPtr = m_dataQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_dataQObjectPtr = QVariant();
}


QVariant CPrinterListObject::createDataArrayElement(const QVariant& v)
{
	Q_UNUSED(v);
	return QVariant::fromValue(new sdl::modsdl::PrinterBase::CPrinterBaseObject());
}


QString CPrinterListObject::toJson() const
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


bool CPrinterListObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CPrinterListObject::fromObject(const QJsonObject& jsonObject)
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


QString CPrinterListObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CPrinterListObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	if (key == "data"){
		return QVariant::fromValue(new sdl::modsdl::PrinterBase::CPrinterBaseObjectList(this));
	}
	return QVariant();
}


QString CPrinterListObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "data")){
		return "data";
	}

	return propertyName;
}





bool CPrinterListObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CPrinterListObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CPrinterListObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CPrinterListObjectList::append(sdl::modsdl::PrinterBase::CPrinterListObject* item)
{
	BaseClass::append(item);
}


sdl::modsdl::PrinterBase::CPrinterListObjectList* sdl::modsdl::PrinterBase::CPrinterListObjectList::copyMe()
{
	sdl::modsdl::PrinterBase::CPrinterListObjectList* retVal = new sdl::modsdl::PrinterBase::CPrinterListObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::modsdl::PrinterBase::CPrinterListObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::modsdl::PrinterBase::CPrinterListObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::modsdl::PrinterBase::CPrinterListObjectList::addElement(sdl::modsdl::PrinterBase::CPrinterListObject* item)
{
	append(item);
}


void sdl::modsdl::PrinterBase::CPrinterListObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::modsdl::PrinterBase::CPrinterListObjectList::isEqualWithModel(sdl::modsdl::PrinterBase::CPrinterListObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::modsdl::PrinterBase::CPrinterListObjectList::insert(int index, sdl::modsdl::PrinterBase::CPrinterListObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::modsdl::PrinterBase::CPrinterListObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::modsdl::PrinterBase::CPrinterListObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::modsdl::PrinterBase::CPrinterListObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::modsdl::PrinterBase::CPrinterListObject*>();
	if (itemPtr == nullptr) return QVariant();
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
		if (nameId == "m_data"){
			return itemPtr->GetData();
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

	ClearCache();
	endInsertRows();
}


sdl::modsdl::PrinterBase::CPrinterSpecificationObjectList* CPrinterSpecificationObjectList::copyMe()
{
	sdl::modsdl::PrinterBase::CPrinterSpecificationObjectList* objectListPtr = new sdl::modsdl::PrinterBase::CPrinterSpecificationObjectList();

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

bool CPrinterSpecificationObjectList::isEqualWithModel(sdl::modsdl::PrinterBase::CPrinterSpecificationObjectList* otherModelPtr)
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



QVariant CSimpleUnionObjectList::GetOrCreateCachedObject(int index) const
{
	QVariant retVal;
	if (this->m_objectDataTypeMap.contains(index)){
		retVal = this->m_objectDataTypeMap[index];
	}
	else{
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


bool CSimpleUnionObjectList::containsKey(const QString& nameId, int /*index*/)
{
	return nameId == "item";
}


int CSimpleUnionObjectList::getItemsCount()
{
	return rowCount();
}


QVariantMap CSimpleUnionObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CSimpleUnionObjectList::append(QVariant item)
{
	beginInsertRows(QModelIndex(), Version_1_0->count(), 0);

	if (item.canConvert<QString>()){
		QString val = item.value<QString>();
		Version_1_0->append(istd::TSharedNullable<SimpleUnion>(val));
	}

	if (item.canConvert<double>()){
		double val = item.value<double>();
		Version_1_0->append(istd::TSharedNullable<SimpleUnion>(val));
	}

	ClearCache();
	endInsertRows();
}


sdl::modsdl::PrinterBase::CSimpleUnionObjectList* CSimpleUnionObjectList::copyMe()
{
	sdl::modsdl::PrinterBase::CSimpleUnionObjectList* objectListPtr = new sdl::modsdl::PrinterBase::CSimpleUnionObjectList();

	for (int i = 0; i < this->rowCount(); i++){
		QVariant item = this->getData("item", i);

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


QString CSimpleUnionObjectList::toJson()
{
	QString retVal = QStringLiteral("[");

	for (int i = 0; i < this->rowCount(); i++){
		if (i > 0 && i < this->rowCount() - 1){
			retVal += QStringLiteral(", ");
		}

		QVariant item = this->getData("item", i);

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


QString CSimpleUnionObjectList::toGraphQL()
{
	QString retVal = QStringLiteral("[");

	for (int i = 0; i < this->rowCount(); i++){
		if (i > 0 && i < this->rowCount() - 1){
			retVal += QStringLiteral(", ");
		}

		QVariant item = this->getData("item", i);

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


void CSimpleUnionObjectList::addElement(QVariant item)
{
	append(item);
}

void CSimpleUnionObjectList::removeElement(int index)
{
	remove(index);
}

bool CSimpleUnionObjectList::isEqualWithModel(sdl::modsdl::PrinterBase::CSimpleUnionObjectList* otherModelPtr)
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


void CSimpleUnionObjectList::insert(int index, QVariant item)
{
	if (index < 0 || index > Version_1_0->size()) return;
	beginInsertRows(QModelIndex(), Version_1_0->count(), 0);

	if (item.canConvert<QString>()){
		QString val = item.value<QString>();
		Version_1_0->append(istd::TSharedNullable<SimpleUnion>(val));
	}

	if (item.canConvert<double>()){
		double val = item.value<double>();
		Version_1_0->append(istd::TSharedNullable<SimpleUnion>(val));
	}

	ClearCache();
	endInsertRows();
}


void CSimpleUnionObjectList::remove(int index)
{
	BaseClass::remove(index);
}


void CSimpleUnionObjectList::clear()
{
	BaseClass::clear();
}


QVariant CSimpleUnionObjectList::getData(const QString& nameId, int index)
{
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		QVariant retVal = GetOrCreateCachedObject(index);
		return retVal;
	}
	return QVariant();
}



QVariant CMixedUnionObjectList::GetOrCreateCachedObject(int index) const
{
	QVariant retVal;
	if (this->m_objectDataTypeMap.contains(index)){
		retVal = this->m_objectDataTypeMap[index];
	}
	else{
		if (const QString* val = std::get_if<QString>(this->Version_1_0->at(index).GetPtr())){
			retVal =  QVariant::fromValue(val);
		}

		if (const CLink* val = std::get_if<CLink>(this->Version_1_0->at(index).GetPtr())){
			CLinkObject *newObjectPtr = new CLinkObject();
			newObjectPtr->Version_1_0 = val->Version_1_0;
			retVal = QVariant::fromValue(newObjectPtr);
		}

		this->m_objectDataTypeMap.insert(index, retVal);
	}
	return retVal;
}


bool CMixedUnionObjectList::containsKey(const QString& nameId, int /*index*/)
{
	return nameId == "item";
}


int CMixedUnionObjectList::getItemsCount()
{
	return rowCount();
}


QVariantMap CMixedUnionObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CMixedUnionObjectList::append(QVariant item)
{
	beginInsertRows(QModelIndex(), Version_1_0->count(), 0);

	if (item.canConvert<QString>()){
		QString val = item.value<QString>();
		Version_1_0->append(istd::TSharedNullable<MixedUnion>(val));
	}

	if (item.canConvert<CLinkObject*>()){
		CLinkObject* val = item.value<CLinkObject*>();
		CLink* newItemPtr = dynamic_cast<CLink*>(val);
		Version_1_0->append(istd::TSharedNullable<MixedUnion>(*newItemPtr));
	}

	ClearCache();
	endInsertRows();
}


sdl::modsdl::PrinterBase::CMixedUnionObjectList* CMixedUnionObjectList::copyMe()
{
	sdl::modsdl::PrinterBase::CMixedUnionObjectList* objectListPtr = new sdl::modsdl::PrinterBase::CMixedUnionObjectList();

	for (int i = 0; i < this->rowCount(); i++){
		QVariant item = this->getData("item", i);

		if (item.canConvert<QString>()){
			QString val = item.value<QString>();
			objectListPtr->addElement(val);
		}

		if (item.canConvert<CLinkObject*>()){
			CLinkObject* val = item.value<CLinkObject*>();
			objectListPtr->addElement(QVariant::fromValue(val->copyMe()));
		}
	}
	return objectListPtr;
}


QString CMixedUnionObjectList::toJson()
{
	QString retVal = QStringLiteral("[");

	for (int i = 0; i < this->rowCount(); i++){
		if (i > 0 && i < this->rowCount() - 1){
			retVal += QStringLiteral(", ");
		}

		QVariant item = this->getData("item", i);

		if (item.canConvert<QString>()){
			QString val = item.value<QString>();
			retVal += QString(val);
		}

		if (item.canConvert<CLinkObject*>()){
			CLinkObject* val = item.value<CLinkObject*>();
			retVal += val->toJson();
		}
	}

	retVal += QStringLiteral("]");

	return retVal;
}


QString CMixedUnionObjectList::toGraphQL()
{
	QString retVal = QStringLiteral("[");

	for (int i = 0; i < this->rowCount(); i++){
		if (i > 0 && i < this->rowCount() - 1){
			retVal += QStringLiteral(", ");
		}

		QVariant item = this->getData("item", i);

		if (item.canConvert<QString>()){
			QString val = item.value<QString>();
			retVal += QString(val);
		}

		if (item.canConvert<CLinkObject*>()){
			CLinkObject* val = item.value<CLinkObject*>();
			retVal += val->toGraphQL();
		}
	}

	retVal += QStringLiteral("]");

	return retVal;
}


void CMixedUnionObjectList::addElement(QVariant item)
{
	append(item);
}

void CMixedUnionObjectList::removeElement(int index)
{
	remove(index);
}

bool CMixedUnionObjectList::isEqualWithModel(sdl::modsdl::PrinterBase::CMixedUnionObjectList* otherModelPtr)
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


void CMixedUnionObjectList::insert(int index, QVariant item)
{
	if (index < 0 || index > Version_1_0->size()) return;
	beginInsertRows(QModelIndex(), Version_1_0->count(), 0);

	if (item.canConvert<QString>()){
		QString val = item.value<QString>();
		Version_1_0->append(istd::TSharedNullable<MixedUnion>(val));
	}

	if (item.canConvert<CLinkObject*>()){
		CLinkObject* val = item.value<CLinkObject*>();
		CLink* newItemPtr = dynamic_cast<CLink*>(val);
		Version_1_0->append(istd::TSharedNullable<MixedUnion>(*newItemPtr));
	}

	ClearCache();
	endInsertRows();
}


void CMixedUnionObjectList::remove(int index)
{
	BaseClass::remove(index);
}


void CMixedUnionObjectList::clear()
{
	BaseClass::clear();
}


QVariant CMixedUnionObjectList::getData(const QString& nameId, int index)
{
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		QVariant retVal = GetOrCreateCachedObject(index);
		return retVal;
	}
	return QVariant();
}


QByteArray CGetPrintersGqlRequest::GetCommandId()
{
	return QByteArrayLiteral("GetPrinters");
}


bool CGetPrintersGqlRequest::SetupGqlRequest(::imtgql::CGqlRequest& gqlRequest, const GetPrintersRequestArguments& requestArguments, const GetPrintersRequestInfo& /*requestInfo*/)
{
	gqlRequest.SetCommandId(GetCommandId());

	// writting input arguments

	return true;
}


CGetPrintersGqlRequest::CGetPrintersGqlRequest(const ::imtgql::CGqlRequest& gqlRequest, bool optRead)
	: m_isValid(true)
{
	Q_UNUSED(optRead)
	m_isValid = true;
	m_gqlContextPtr = gqlRequest.GetRequestContext();

	const QByteArray protocolVersion = gqlRequest.GetProtocolVersion();

	// reading input arguments

	// reading requested fields
	const imtgql::CGqlFieldObject* requestedFieldsObjectPtr = nullptr;
	if (!gqlRequest.GetFields().GetFieldIds().isEmpty()){
		requestedFieldsObjectPtr = gqlRequest.GetFields().GetFieldArgumentObjectPtr(gqlRequest.GetFields().GetFieldIds().constFirst());
		if (requestedFieldsObjectPtr != nullptr){
			const QByteArrayList requestedIds = requestedFieldsObjectPtr->GetFieldIds();
			if (!requestedIds.isEmpty()){
				m_requestInfo.isDataRequested = requestedIds.contains("data");
				const imtgql::CGqlFieldObject* dataRequestedFieldsPtr = requestedFieldsObjectPtr->GetFieldArgumentObjectPtr("data");
				if (dataRequestedFieldsPtr != nullptr){
					const QByteArrayList dataRequestedIds = dataRequestedFieldsPtr->GetFieldIds();
					if (!dataRequestedIds.isEmpty()){
						m_requestInfo.data.isNameRequested = dataRequestedIds.contains("name");
						m_requestInfo.data.isSpecificationRequested = true;
						m_requestInfo.data.isSimpleTestRequested = true;
						m_requestInfo.data.isMixedTestRequested = true;
					}
				}
			}
		}
	}
}


bool CGetPrintersGqlRequest::IsValid() const
{
	return m_isValid;
}


const ::imtgql::IGqlContext* CGetPrintersGqlRequest::GetRequestContext() const
{
	return m_gqlContextPtr;
}


const GetPrintersRequestArguments& CGetPrintersGqlRequest::GetRequestedArguments() const
{
	return m_requestedArguments;
}


GetPrintersRequestInfo CGetPrintersGqlRequest::GetRequestInfo() const
{
	return m_requestInfo;
}
bool CGraphQlHandlerCompBase::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	const QByteArray commandId = gqlRequest.GetCommandId();
	if (commandId == CGetPrintersGqlRequest::GetCommandId()){
		return true;
	}

	return BaseClass::IsRequestSupported(gqlRequest);
}


::imtbase::CTreeItemModel* CGraphQlHandlerCompBase::CreateInternalResponse(const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	const QByteArray commandId = gqlRequest.GetCommandId();
	istd::TDelPtr<::imtbase::CTreeItemModel> modelPtr(new ::imtbase::CTreeItemModel);
	::imtbase::CTreeItemModel* dataModelPtr = modelPtr->AddTreeModel("data");

	// GetPrinters
	if (commandId == CGetPrintersGqlRequest::GetCommandId()){
		CGetPrintersGqlRequest getPrintersGqlRequest(gqlRequest, false);
		if (!getPrintersGqlRequest.IsValid()){
			errorMessage = QString("Bad request. Unexpected request for command-ID: '%1'").arg(qPrintable(commandId));
			SendErrorMessage(0, errorMessage);

			return nullptr;
		}

		CPrinterList replyPayload = OnGetPrinters(getPrintersGqlRequest, gqlRequest, errorMessage);
		if (!errorMessage.isEmpty()){
			SendErrorMessage(0, QString("The derived call [OnGetPrinters] returned an error: %1").arg(errorMessage));

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


} // namespace sdl::modsdl::PrinterBase
