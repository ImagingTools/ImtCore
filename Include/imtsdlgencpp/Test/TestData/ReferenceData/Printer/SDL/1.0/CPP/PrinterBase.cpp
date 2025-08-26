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
	if (!link){
		return false;
	}
	model.SetData("link", *link, modelIndex);


	return true;
}


bool CLink::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant linkData = model.GetData("link", modelIndex);
	if (linkData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "link");)

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
		return false;
	}
	gqlObject.InsertParam("link", QVariant(*link));

	return true;
}


bool CLink::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("link") || (gqlObject["link"].userType() != QMetaType::QString && gqlObject["link"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "link");)

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
		return false;
	}
	jsonObject["link"] = QJsonValue::fromVariant(*link);

	return true;
}


bool CLink::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("link") || ! jsonObject["link"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "link");)

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
	if (name){
		model.SetData("name", *name, modelIndex);
	}

	if (!specification){
		return false;
	}
	QVariant specificationVariantValue;
		if (const CPrinterSpecificationBase* val = std::get_if<CPrinterSpecificationBase>((*specification).get())){
			if (!val->WriteToModel(*(model.AddTreeModel("specification", modelIndex)), 0)){
				return false;
			}
		}
		else if (const CLink* val = std::get_if<CLink>((*specification).get())){
			if (!val->WriteToModel(*(model.AddTreeModel("specification", modelIndex)), 0)){
				return false;
			}
		}


	if (!simpleTest){
		return false;
	}
	QVariant simpleTestVariantValue;
		if (const QString* val = std::get_if<QString>((*simpleTest).get())){
			simpleTestVariantValue = *val;
			model.SetData("simpleTest", simpleTestVariantValue, modelIndex);
		}
		else if (const double* val = std::get_if<double>((*simpleTest).get())){
			simpleTestVariantValue = *val;
			model.SetData("simpleTest", simpleTestVariantValue, modelIndex);
		}


	if (!mixedTest){
		return false;
	}
	QVariant mixedTestVariantValue;
		if (const QString* val = std::get_if<QString>((*mixedTest).get())){
			mixedTestVariantValue = *val;
			model.SetData("mixedTest", mixedTestVariantValue, modelIndex);
		}
		else if (const CLink* val = std::get_if<CLink>((*mixedTest).get())){
			if (!val->WriteToModel(*(model.AddTreeModel("mixedTest", modelIndex)), 0)){
				return false;
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
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "specification");)

		return false;
	}
	QString specificationTypename = specificationData.value<::imtbase::CTreeItemModel*>()->GetData("__typename").toString();
	if (specificationTypename == "PrinterSpecificationBase") {
		CPrinterSpecificationBase specificationConvert;
		const bool isspecificationRead = specificationConvert.ReadFromModel(*model.GetTreeItemModel("specification", modelIndex)); 
		if (!isspecificationRead){
			return false;
		}
		specification = std::make_shared<PrinterSpecification>(specificationConvert);
	}
	else if (specificationTypename == "Link") {
		CLink specificationConvert;
		const bool isspecificationRead = specificationConvert.ReadFromModel(*model.GetTreeItemModel("specification", modelIndex)); 
		if (!isspecificationRead){
			return false;
		}
		specification = std::make_shared<PrinterSpecification>(specificationConvert);
	}

	QVariant simpleTestData = model.GetData("simpleTest", modelIndex);
	if (simpleTestData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "simpleTest");)

		return false;
	}
	QString simpleTestTypename = simpleTestData.value<::imtbase::CTreeItemModel*>()->GetData("__typename").toString();
	if (simpleTestData.canConvert<QString>()){
		simpleTest = std::make_shared<SimpleUnion>(simpleTestData.value<QString>());
	}
	else if (simpleTestData.canConvert<double>()){
		simpleTest = std::make_shared<SimpleUnion>(simpleTestData.value<double>());
	}

	QVariant mixedTestData = model.GetData("mixedTest", modelIndex);
	if (mixedTestData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "mixedTest");)

		return false;
	}
	QString mixedTestTypename = mixedTestData.value<::imtbase::CTreeItemModel*>()->GetData("__typename").toString();
	if (mixedTestData.canConvert<QString>()){
		mixedTest = std::make_shared<MixedUnion>(mixedTestData.value<QString>());
	}
	else if (mixedTestTypename == "Link") {
		CLink mixedTestConvert;
		const bool ismixedTestRead = mixedTestConvert.ReadFromModel(*model.GetTreeItemModel("mixedTest", modelIndex)); 
		if (!ismixedTestRead){
			return false;
		}
		mixedTest = std::make_shared<MixedUnion>(mixedTestConvert);
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
			specification = std::make_shared<PrinterSpecification>(specificationConvert);
		}
		else if (specificationTypename == "Link") {
			CLink specificationConvert;
			const bool isspecificationRead = specificationConvert.ReadFromModel(*model.GetTreeItemModel("specification", modelIndex)); 
			if (!isspecificationRead){
				return false;
			}
			specification = std::make_shared<PrinterSpecification>(specificationConvert);
		}
	}

	QVariant simpleTestData = model.GetData("simpleTest", modelIndex);
	if (!simpleTestData.isNull()){
		QString simpleTestTypename = simpleTestData.value<::imtbase::CTreeItemModel*>()->GetData("__typename").toString();
		if (simpleTestData.canConvert<QString>()){
			simpleTest = std::make_shared<SimpleUnion>(simpleTestData.value<QString>());
		}
		else if (simpleTestData.canConvert<double>()){
			simpleTest = std::make_shared<SimpleUnion>(simpleTestData.value<double>());
		}
	}

	QVariant mixedTestData = model.GetData("mixedTest", modelIndex);
	if (!mixedTestData.isNull()){
		QString mixedTestTypename = mixedTestData.value<::imtbase::CTreeItemModel*>()->GetData("__typename").toString();
		if (mixedTestData.canConvert<QString>()){
			mixedTest = std::make_shared<MixedUnion>(mixedTestData.value<QString>());
		}
		else if (mixedTestTypename == "Link") {
			CLink mixedTestConvert;
			const bool ismixedTestRead = mixedTestConvert.ReadFromModel(*model.GetTreeItemModel("mixedTest", modelIndex)); 
			if (!ismixedTestRead){
				return false;
			}
			mixedTest = std::make_shared<MixedUnion>(mixedTestConvert);
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
		return false;
	}
	::imtgql::CGqlParamObject specificationDataObject;
	if (const CPrinterSpecificationBase* val = std::get_if<CPrinterSpecificationBase>((*specification).get())){
		if (!val->WriteToGraphQlObject(specificationDataObject)){
			return false;
		}
	}
	else if (const CLink* val = std::get_if<CLink>((*specification).get())){
		if (!val->WriteToGraphQlObject(specificationDataObject)){
			return false;
		}
	}
	gqlObject.InsertParam("specification", specificationDataObject);

	if (!simpleTest){
		return false;
	}
	::imtgql::CGqlParamObject simpleTestDataObject;
	if (const QString* val = std::get_if<QString>((*simpleTest).get())){
		simpleTestDataObject.InsertParam("simpleTest", *val);
	}
	else if (const double* val = std::get_if<double>((*simpleTest).get())){
		simpleTestDataObject.InsertParam("simpleTest", *val);
	}
	gqlObject.InsertParam("simpleTest", simpleTestDataObject);

	if (!mixedTest){
		return false;
	}
	::imtgql::CGqlParamObject mixedTestDataObject;
	if (const QString* val = std::get_if<QString>((*mixedTest).get())){
		mixedTestDataObject.InsertParam("mixedTest", *val);
	}
	else if (const CLink* val = std::get_if<CLink>((*mixedTest).get())){
		if (!val->WriteToGraphQlObject(mixedTestDataObject)){
			return false;
		}
	}
	gqlObject.InsertParam("mixedTest", mixedTestDataObject);

	return true;
}


bool CPrinterBase::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("name") && (gqlObject["name"].userType() == QMetaType::QString || gqlObject["name"].userType() == QMetaType::QByteArray)){
		name = gqlObject["name"].toString();
	}

	if (!gqlObject.ContainsParam("specification")){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "specification");)

		return false;
	}
	const ::imtgql::CGqlParamObject* specificationDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("specification");
	if (!specificationDataObjectPtr->ContainsParam("__typename")){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "specification");)

		return false;
	}
	QString specificationTypename = specificationDataObjectPtr->GetParamArgumentValue("__typename").toString();
	if (specificationTypename == "PrinterSpecificationBase") {
		CPrinterSpecificationBase specificationConvert;
		const bool isSpecificationRead = specificationConvert.ReadFromGraphQlObject(*specificationDataObjectPtr);
		if (!isSpecificationRead){
			return false;
		}
		specification = std::make_shared<PrinterSpecification>(specificationConvert);
	}
	else if (specificationTypename == "Link") {
		CLink specificationConvert;
		const bool isSpecificationRead = specificationConvert.ReadFromGraphQlObject(*specificationDataObjectPtr);
		if (!isSpecificationRead){
			return false;
		}
		specification = std::make_shared<PrinterSpecification>(specificationConvert);
	}

	if (!gqlObject.ContainsParam("simpleTest")){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "simpleTest");)

		return false;
	}
	QVariant simpleTestVariantValue = gqlObject["simpleTest"];
	if (simpleTestVariantValue.canConvert<QString>()){
		simpleTest = std::make_shared<SimpleUnion>(simpleTestVariantValue.value<QString>());
	}
	else if (simpleTestVariantValue.canConvert<double>()){
		simpleTest = std::make_shared<SimpleUnion>(simpleTestVariantValue.value<double>());
	}

	if (!gqlObject.ContainsParam("mixedTest")){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "mixedTest");)

		return false;
	}
	const ::imtgql::CGqlParamObject* mixedTestDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("mixedTest");
	if (!mixedTestDataObjectPtr->ContainsParam("__typename")){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "mixedTest");)

		return false;
	}
	QString mixedTestTypename = mixedTestDataObjectPtr->GetParamArgumentValue("__typename").toString();
	QVariant mixedTestVariantValue = gqlObject["mixedTest"];
	if (mixedTestVariantValue.canConvert<QString>()){
		mixedTest = std::make_shared<MixedUnion>(mixedTestVariantValue.value<QString>());
	}
	else if (mixedTestTypename == "Link") {
		CLink mixedTestConvert;
		const bool isMixedTestRead = mixedTestConvert.ReadFromGraphQlObject(*mixedTestDataObjectPtr);
		if (!isMixedTestRead){
			return false;
		}
		mixedTest = std::make_shared<MixedUnion>(mixedTestConvert);
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
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "specification");)

			return false;
		}
		QString specificationTypename = specificationDataObjectPtr->GetParamArgumentValue("__typename").toString();
		if (specificationTypename == "PrinterSpecificationBase") {
			CPrinterSpecificationBase specificationConvert;
			const bool isSpecificationRead = specificationConvert.ReadFromGraphQlObject(*specificationDataObjectPtr);
			if (!isSpecificationRead){
				return false;
			}
			specification = std::make_shared<PrinterSpecification>(specificationConvert);
		}
		else if (specificationTypename == "Link") {
			CLink specificationConvert;
			const bool isSpecificationRead = specificationConvert.ReadFromGraphQlObject(*specificationDataObjectPtr);
			if (!isSpecificationRead){
				return false;
			}
			specification = std::make_shared<PrinterSpecification>(specificationConvert);
		}
	}

	if (gqlObject.ContainsParam("simpleTest")){
		QVariant simpleTestVariantValue = gqlObject["simpleTest"];
		if (simpleTestVariantValue.canConvert<QString>()){
			simpleTest = std::make_shared<SimpleUnion>(simpleTestVariantValue.value<QString>());
		}
		else if (simpleTestVariantValue.canConvert<double>()){
			simpleTest = std::make_shared<SimpleUnion>(simpleTestVariantValue.value<double>());
		}
	}

	if (gqlObject.ContainsParam("mixedTest")){
		const ::imtgql::CGqlParamObject* mixedTestDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("mixedTest");
		if (!mixedTestDataObjectPtr->ContainsParam("__typename")){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "mixedTest");)

			return false;
		}
		QString mixedTestTypename = mixedTestDataObjectPtr->GetParamArgumentValue("__typename").toString();
		QVariant mixedTestVariantValue = gqlObject["mixedTest"];
		if (mixedTestVariantValue.canConvert<QString>()){
			mixedTest = std::make_shared<MixedUnion>(mixedTestVariantValue.value<QString>());
		}
		else if (mixedTestTypename == "Link") {
			CLink mixedTestConvert;
			const bool isMixedTestRead = mixedTestConvert.ReadFromGraphQlObject(*mixedTestDataObjectPtr);
			if (!isMixedTestRead){
				return false;
			}
			mixedTest = std::make_shared<MixedUnion>(mixedTestConvert);
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
		return false;
	}
	if (const CPrinterSpecificationBase* val = std::get_if<CPrinterSpecificationBase>((*specification).get())){
		QJsonObject specificationJsonObject;
		const bool isspecificationAdded = val->WriteToJsonObject(specificationJsonObject);
		if (!isspecificationAdded){
			return false;
		}
		jsonObject["specification"] = specificationJsonObject;
	}
	else if (const CLink* val = std::get_if<CLink>((*specification).get())){
		QJsonObject specificationJsonObject;
		const bool isspecificationAdded = val->WriteToJsonObject(specificationJsonObject);
		if (!isspecificationAdded){
			return false;
		}
		jsonObject["specification"] = specificationJsonObject;
	}


	if (!simpleTest){
		return false;
	}
	if (const QString* val = std::get_if<QString>((*simpleTest).get())){
		if (!simpleTest){
			return false;
		}
		jsonObject["simpleTest"] = QJsonValue::fromVariant(*val);
	}
	else if (const double* val = std::get_if<double>((*simpleTest).get())){
		if (!simpleTest){
			return false;
		}
		jsonObject["simpleTest"] = QJsonValue::fromVariant(*val);
	}


	if (!mixedTest){
		return false;
	}
	if (const QString* val = std::get_if<QString>((*mixedTest).get())){
		if (!mixedTest){
			return false;
		}
		jsonObject["mixedTest"] = QJsonValue::fromVariant(*val);
	}
	else if (const CLink* val = std::get_if<CLink>((*mixedTest).get())){
		QJsonObject mixedTestJsonObject;
		const bool ismixedTestAdded = val->WriteToJsonObject(mixedTestJsonObject);
		if (!ismixedTestAdded){
			return false;
		}
		jsonObject["mixedTest"] = mixedTestJsonObject;
	}


	return true;
}


bool CPrinterBase::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("name") && jsonObject["name"].isString()){
		name = jsonObject["name"].toString();
	}

	if (!jsonObject.contains("specification")){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "specification");)

		return false;
	}
	const QJsonObject specificationObject = jsonObject.value("specification").toObject();
	if (!jsonObject.value("specification").isObject() || !specificationObject.contains("__typename")){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "specification");)

		return false;
	}
	QString specificationTypename = specificationObject.value("__typename").toString();
	if (specificationTypename == "PrinterSpecificationBase") {
		CPrinterSpecificationBase specificationConvert;
		const bool isspecificationRead = specificationConvert.ReadFromJsonObject(jsonObject["specification"].toObject());
		if (!isspecificationRead){
			return false;
		}
		specification = std::make_shared<PrinterSpecification>(specificationConvert);
	}
	else if (specificationTypename == "Link") {
		CLink specificationConvert;
		const bool isspecificationRead = specificationConvert.ReadFromJsonObject(jsonObject["specification"].toObject());
		if (!isspecificationRead){
			return false;
		}
		specification = std::make_shared<PrinterSpecification>(specificationConvert);
	}

	if (!jsonObject.contains("simpleTest")){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "simpleTest");)

		return false;
	}
	QVariant simpleTestVariantValue = jsonObject["simpleTest"].toVariant();
	if (simpleTestVariantValue.canConvert<QString>()){
		simpleTest = std::make_shared<SimpleUnion>(simpleTestVariantValue.value<QString>());
	}
	else if (simpleTestVariantValue.canConvert<double>()){
		simpleTest = std::make_shared<SimpleUnion>(simpleTestVariantValue.value<double>());
	}

	if (!jsonObject.contains("mixedTest")){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "mixedTest");)

		return false;
	}
	const QJsonObject mixedTestObject = jsonObject.value("mixedTest").toObject();
	if (!jsonObject.value("mixedTest").isObject() || !mixedTestObject.contains("__typename")){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "mixedTest");)

		return false;
	}
	QString mixedTestTypename = mixedTestObject.value("__typename").toString();
	QVariant mixedTestVariantValue = jsonObject["mixedTest"].toVariant();
	if (mixedTestVariantValue.canConvert<QString>()){
		mixedTest = std::make_shared<MixedUnion>(mixedTestVariantValue.value<QString>());
	}
	else if (mixedTestTypename == "Link") {
		CLink mixedTestConvert;
		const bool ismixedTestRead = mixedTestConvert.ReadFromJsonObject(jsonObject["mixedTest"].toObject());
		if (!ismixedTestRead){
			return false;
		}
		mixedTest = std::make_shared<MixedUnion>(mixedTestConvert);
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
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "specification");)

			return false;
		}
		QString specificationTypename = specificationObject.value("__typename").toString();
		if (specificationTypename == "PrinterSpecificationBase") {
			CPrinterSpecificationBase specificationConvert;
			const bool isspecificationRead = specificationConvert.ReadFromJsonObject(jsonObject["specification"].toObject());
			if (!isspecificationRead){
				return false;
			}
			specification = std::make_shared<PrinterSpecification>(specificationConvert);
		}
		else if (specificationTypename == "Link") {
			CLink specificationConvert;
			const bool isspecificationRead = specificationConvert.ReadFromJsonObject(jsonObject["specification"].toObject());
			if (!isspecificationRead){
				return false;
			}
			specification = std::make_shared<PrinterSpecification>(specificationConvert);
		}
	}

	if (jsonObject.contains("simpleTest")){
		QVariant simpleTestVariantValue = jsonObject["simpleTest"].toVariant();
		if (simpleTestVariantValue.canConvert<QString>()){
			simpleTest = std::make_shared<SimpleUnion>(simpleTestVariantValue.value<QString>());
		}
		else if (simpleTestVariantValue.canConvert<double>()){
			simpleTest = std::make_shared<SimpleUnion>(simpleTestVariantValue.value<double>());
		}
	}

	if (jsonObject.contains("mixedTest")){
		const QJsonObject mixedTestObject = jsonObject.value("mixedTest").toObject();
		if (!jsonObject.value("mixedTest").isObject() || !mixedTestObject.contains("__typename")){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "mixedTest");)

			return false;
		}
		QString mixedTestTypename = mixedTestObject.value("__typename").toString();
		QVariant mixedTestVariantValue = jsonObject["mixedTest"].toVariant();
		if (mixedTestVariantValue.canConvert<QString>()){
			mixedTest = std::make_shared<MixedUnion>(mixedTestVariantValue.value<QString>());
		}
		else if (mixedTestTypename == "Link") {
			CLink mixedTestConvert;
			const bool ismixedTestRead = mixedTestConvert.ReadFromJsonObject(jsonObject["mixedTest"].toObject());
			if (!ismixedTestRead){
				return false;
			}
			mixedTest = std::make_shared<MixedUnion>(mixedTestConvert);
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
	if (data){
		::imtbase::CTreeItemModel* newDataModelPtr = model.AddTreeModel("data", modelIndex);
		newDataModelPtr->setIsArray(true);
		for (qsizetype dataIndex = 0; dataIndex < data->size(); ++dataIndex){
			newDataModelPtr->InsertNewItem();
			if (!(data->at(dataIndex).WriteToModel(*newDataModelPtr, dataIndex))){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "data");)

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
		QList<CPrinterBase::V1_0> dataList;
		for (int dataIndex = 0; dataIndex < dataCount; ++dataIndex){
			CPrinterBase::V1_0 data;
			if (!data.ReadFromModel(*dataModel, dataIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "data");)

				return false;
			}
			dataList << data;
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
		QList<CPrinterBase::V1_0> dataList;
		for (int dataIndex = 0; dataIndex < dataCount; ++dataIndex){
			CPrinterBase::V1_0 data;
			if (!data.OptReadFromModel(*dataModel, dataIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "data");)

				return false;
			}
			dataList << data;
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
			if (!data->at(dataIndex).WriteToGraphQlObject(newDataGqlObject)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "data");)

				return false;
			}
			dataDataObjectList << newDataGqlObject;
		}
		gqlObject.InsertParam("data", dataDataObjectList);
	}

	return true;
}


bool CPrinterList::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("data") && (gqlObject.GetObjectsCount("data") > 0)){
		const qsizetype dataElementsCount = gqlObject.GetObjectsCount("data");
		data = QList<CPrinterBase::V1_0>();
		for (qsizetype dataIndex = 0; dataIndex < dataElementsCount; ++dataIndex){
			const ::imtgql::CGqlParamObject* dataDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("data", dataIndex);
			if (dataDataObjectPtr == nullptr){
				qDebug() << "invalid type" << dataDataObjectPtr;
				return false;
			}
			CPrinterBase::V1_0 tempData;
			if (!tempData.ReadFromGraphQlObject(*dataDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "data");)

				return false;
			}
			data->append(tempData);
		}
	}

	return true;
}


bool CPrinterList::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("data") && (gqlObject.GetObjectsCount("data") > 0)){
		const qsizetype dataElementsCount = gqlObject.GetObjectsCount("data");
		data = QList<CPrinterBase::V1_0>();
		for (qsizetype dataIndex = 0; dataIndex < dataElementsCount; ++dataIndex){
			const ::imtgql::CGqlParamObject* dataDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("data", dataIndex);
			if (dataDataObjectPtr == nullptr){
				qDebug() << "invalid type" << dataDataObjectPtr;
				return false;
			}
			CPrinterBase::V1_0 tempData;
			if (!tempData.OptReadFromGraphQlObject(*dataDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "data");)

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
			if (!data->at(dataIndex).WriteToJsonObject(newDataJsonObject)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "data");)

				return false;
			}
			newDataArray << newDataJsonObject;
		}
		jsonObject["data"] = newDataArray;
	}

	return true;
}


bool CPrinterList::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("data") && jsonObject["data"].isArray()){
		const QJsonArray dataJsonArray = jsonObject["data"].toArray();
		const qsizetype dataArrayCount = dataJsonArray.size();
		data = QList<CPrinterBase::V1_0>();
		for (qsizetype dataIndex = 0; dataIndex < dataArrayCount; ++dataIndex){
			CPrinterBase::V1_0 tempData;
			if (!tempData.ReadFromJsonObject(dataJsonArray[dataIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "data");)

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
		data = QList<CPrinterBase::V1_0>();
		for (qsizetype dataIndex = 0; dataIndex < dataArrayCount; ++dataIndex){
			CPrinterBase::V1_0 tempData;
			if (!tempData.OptReadFromJsonObject(dataJsonArray[dataIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "data");)

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
