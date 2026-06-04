#include "PrinterBase.h"
#include "PrinterBase_fwd.h"


namespace sdl::V1_0::modsdl
{


QByteArray CPrinterSpecificationBase::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CPrinterSpecificationBase::operator==(const CPrinterSpecificationBase& other) const
{
	return 
				name == other.name;
}


bool CPrinterSpecificationBase::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "PrinterSpecificationBase", modelIndex);

	if (name){
		model.SetData("name", *name, modelIndex);
	}


	return true;
}


bool CPrinterSpecificationBase::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant nameData = model.GetData("name", modelIndex);
	if (!nameData.isNull()){
		name = nameData.toString();
	}

	return true;
}


bool CPrinterSpecificationBase::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant nameData = model.GetData("name", modelIndex);
	if (!nameData.isNull()){
		name = nameData.toString();
	}

	return true;
}


bool CPrinterSpecificationBase::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (name){
		gqlObject.InsertParam("name", QVariant(*name));
	}

	gqlObject.InsertParam("__typename", QVariant("PrinterSpecificationBase"));

	return true;
}


bool CPrinterSpecificationBase::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("name") && (gqlObject["name"].userType() == QMetaType::QString || gqlObject["name"].userType() == QMetaType::QByteArray)){
		name = gqlObject["name"].toString();
	}

	return true;
}


bool CPrinterSpecificationBase::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("name") && (gqlObject["name"].userType() == QMetaType::QString || gqlObject["name"].userType() == QMetaType::QByteArray)){
		name = gqlObject["name"].toString();
	}

	return true;
}


bool CPrinterSpecificationBase::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (name){
		jsonObject["name"] = QJsonValue::fromVariant(*name);
	}

	jsonObject["__typename"] = "PrinterSpecificationBase";

	return true;
}


bool CPrinterSpecificationBase::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("name") && jsonObject["name"].isString()){
		name = jsonObject["name"].toString();
	}

	return true;
}


bool CPrinterSpecificationBase::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("name") && jsonObject["name"].isString()){
		name = jsonObject["name"].toString();
	}

	return true;
}


// serialize methods

QByteArray CLink::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CLink::operator==(const CLink& other) const
{
	return 
				link == other.link;
}


bool CLink::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "Link", modelIndex);

	if (!link){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "link").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("link", *link, modelIndex);


	return true;
}


bool CLink::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant linkData = model.GetData("link", modelIndex);
	if (linkData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "link").toLocal8Bit().constData();)

		return false;
	}
	link = linkData.toByteArray();

	return true;
}


bool CLink::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant linkData = model.GetData("link", modelIndex);
	if (!linkData.isNull()){
		link = linkData.toByteArray();
	}

	return true;
}


bool CLink::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!link){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "link").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("link", QVariant(*link));

	gqlObject.InsertParam("__typename", QVariant("Link"));

	return true;
}


bool CLink::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("link") || (gqlObject["link"].userType() != QMetaType::QString && gqlObject["link"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "link").toLocal8Bit().constData();)

		return false;
	}
	link = gqlObject["link"].toByteArray();

	return true;
}


bool CLink::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("link") && (gqlObject["link"].userType() == QMetaType::QString || gqlObject["link"].userType() == QMetaType::QByteArray)){
		link = gqlObject["link"].toByteArray();
	}

	return true;
}


bool CLink::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!link){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "link").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["link"] = QString::fromUtf8(*link);

	jsonObject["__typename"] = "Link";

	return true;
}


bool CLink::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("link") || ! jsonObject["link"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "link").toLocal8Bit().constData();)

		return false;
	}
	link = jsonObject["link"].toString().toUtf8();

	return true;
}


bool CLink::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("link") && jsonObject["link"].isString()){
		link = jsonObject["link"].toString().toUtf8();
	}

	return true;
}


// serialize methods

QByteArray CPrinterBase::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CPrinterBase::operator==(const CPrinterBase& other) const
{
	return 
				name == other.name &&
				specification == other.specification &&
				simpleTest == other.simpleTest &&
				mixedTest == other.mixedTest;
}


bool CPrinterBase::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
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
		if (const CPrinterSpecificationBase* printerSpecificationBaseVal = std::get_if<CPrinterSpecificationBase>(specification.GetPtr())){
			if (!printerSpecificationBaseVal->WriteToModel(*(model.AddTreeModel("specification", modelIndex)), 0)){
				return false;
			}
			if(model.GetTreeItemModel("specification", modelIndex) != nullptr){
				model.GetTreeItemModel("specification", modelIndex)->SetData("__typename", "PrinterSpecificationBase", 0);
			}
		}
		else if (const CLink* linkVal = std::get_if<CLink>(specification.GetPtr())){
			if (!linkVal->WriteToModel(*(model.AddTreeModel("specification", modelIndex)), 0)){
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
		if (const QString* stringVal = std::get_if<QString>(simpleTest.GetPtr())){
			model.SetData("", *stringVal);
		}
		else if (const double* doubleVal = std::get_if<double>(simpleTest.GetPtr())){
			model.SetData("", *doubleVal);
		}


	if (!mixedTest){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "mixedTest").toLocal8Bit().constData();)

		return false;
	}
	QVariant mixedTestVariantValue;
		if (const QString* stringVal = std::get_if<QString>(mixedTest.GetPtr())){
			model.SetData("", *stringVal);
		}
		else if (const CLink* linkVal = std::get_if<CLink>(mixedTest.GetPtr())){
			if (!linkVal->WriteToModel(*(model.AddTreeModel("mixedTest", modelIndex)), 0)){
				return false;
			}
			if(model.GetTreeItemModel("mixedTest", modelIndex) != nullptr){
				model.GetTreeItemModel("mixedTest", modelIndex)->SetData("__typename", "Link", 0);
			}
		}



	return true;
}


bool CPrinterBase::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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
	QString specificationTypename = model.GetTreeItemModel("specification", modelIndex)->GetData("__typename").toString();
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
	QString mixedTestTypename = model.GetTreeItemModel("mixedTest", modelIndex)->GetData("__typename").toString();
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


bool CPrinterBase::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant nameData = model.GetData("name", modelIndex);
	if (!nameData.isNull()){
		name = nameData.toString();
	}

	QVariant specificationData = model.GetData("specification", modelIndex);
	if (!specificationData.isNull()){
		QString specificationTypename = model.GetTreeItemModel("specification", modelIndex)->GetData("__typename").toString();
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
		if (simpleTestData.canConvert<QString>()){
			simpleTest = SimpleUnion(simpleTestData.value<QString>());
		}
		else if (simpleTestData.canConvert<double>()){
			simpleTest = SimpleUnion(simpleTestData.value<double>());
		}
	}

	QVariant mixedTestData = model.GetData("mixedTest", modelIndex);
	if (!mixedTestData.isNull()){
		QString mixedTestTypename = model.GetTreeItemModel("mixedTest", modelIndex)->GetData("__typename").toString();
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


bool CPrinterBase::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (name){
		gqlObject.InsertParam("name", QVariant(*name));
	}

	if (!specification){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "specification").toLocal8Bit().constData();)

		return false;
	}
	::imtgql::CGqlParamObject specificationDataObject;
	if (const CPrinterSpecificationBase* printerSpecificationBaseVal = std::get_if<CPrinterSpecificationBase>(specification.GetPtr())){
		if (!printerSpecificationBaseVal->WriteToGraphQlObject(specificationDataObject)){
			return false;
		}
		specificationDataObject.InsertParam("__typename", QVariant("PrinterSpecificationBase"));
	}
	else if (const CLink* linkVal = std::get_if<CLink>(specification.GetPtr())){
		if (!linkVal->WriteToGraphQlObject(specificationDataObject)){
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
	if (const QString* stringVal = std::get_if<QString>(simpleTest.GetPtr())){
		simpleTestDataObject.InsertParam("simpleTest", *stringVal);
	}
	else if (const double* doubleVal = std::get_if<double>(simpleTest.GetPtr())){
		simpleTestDataObject.InsertParam("simpleTest", *doubleVal);
	}
	gqlObject.InsertParam("simpleTest", simpleTestDataObject);

	if (!mixedTest){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "mixedTest").toLocal8Bit().constData();)

		return false;
	}
	::imtgql::CGqlParamObject mixedTestDataObject;
	if (const QString* stringVal = std::get_if<QString>(mixedTest.GetPtr())){
		mixedTestDataObject.InsertParam("mixedTest", *stringVal);
	}
	else if (const CLink* linkVal = std::get_if<CLink>(mixedTest.GetPtr())){
		if (!linkVal->WriteToGraphQlObject(mixedTestDataObject)){
			return false;
		}
		mixedTestDataObject.InsertParam("__typename", QVariant("Link"));
	}
	gqlObject.InsertParam("mixedTest", mixedTestDataObject);

	gqlObject.InsertParam("__typename", QVariant("PrinterBase"));

	return true;
}


bool CPrinterBase::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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


bool CPrinterBase::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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


bool CPrinterBase::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (name){
		jsonObject["name"] = QJsonValue::fromVariant(*name);
	}

	if (!specification){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "specification").toLocal8Bit().constData();)

		return false;
	}
	if (const CPrinterSpecificationBase* printerSpecificationBaseVal = std::get_if<CPrinterSpecificationBase>(specification.GetPtr())){
		QJsonObject specificationJsonObject;
		const bool isspecificationAdded = printerSpecificationBaseVal->WriteToJsonObject(specificationJsonObject);
		if (!isspecificationAdded){
			return false;
		}
		specificationJsonObject["__typename"] = "PrinterSpecificationBase";
		jsonObject["specification"] = specificationJsonObject;
	}
	else if (const CLink* linkVal = std::get_if<CLink>(specification.GetPtr())){
		QJsonObject specificationJsonObject;
		const bool isspecificationAdded = linkVal->WriteToJsonObject(specificationJsonObject);
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
	if (const QString* stringVal = std::get_if<QString>(simpleTest.GetPtr())){
		if (!simpleTest){
			return false;
		}
		jsonObject["simpleTest"] = QJsonValue::fromVariant(*stringVal);
	}
	else if (const double* doubleVal = std::get_if<double>(simpleTest.GetPtr())){
		if (!simpleTest){
			return false;
		}
		jsonObject["simpleTest"] = QJsonValue::fromVariant(*doubleVal);
	}


	if (!mixedTest){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "mixedTest").toLocal8Bit().constData();)

		return false;
	}
	if (const QString* stringVal = std::get_if<QString>(mixedTest.GetPtr())){
		if (!mixedTest){
			return false;
		}
		jsonObject["mixedTest"] = QJsonValue::fromVariant(*stringVal);
	}
	else if (const CLink* linkVal = std::get_if<CLink>(mixedTest.GetPtr())){
		QJsonObject mixedTestJsonObject;
		const bool ismixedTestAdded = linkVal->WriteToJsonObject(mixedTestJsonObject);
		if (!ismixedTestAdded){
			return false;
		}
		mixedTestJsonObject["__typename"] = "Link";
		jsonObject["mixedTest"] = mixedTestJsonObject;
	}


	jsonObject["__typename"] = "PrinterBase";

	return true;
}


bool CPrinterBase::ReadFromJsonObject(const QJsonObject& jsonObject)
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


bool CPrinterBase::OptReadFromJsonObject(const QJsonObject& jsonObject)
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

QByteArray CPrinterList::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CPrinterList::operator==(const CPrinterList& other) const
{
	return 
				data == other.data;
}


bool CPrinterList::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
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


bool CPrinterList::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	::imtbase::CTreeItemModel* dataModel = model.GetTreeItemModel("data", modelIndex);
	if (dataModel != nullptr){
		int dataCount = dataModel->GetItemsCount();
		imtsdl::TElementList<CPrinterBase> dataList;
		for (int dataIndex = 0; dataIndex < dataCount; ++dataIndex){
			CPrinterBase t_data;
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


bool CPrinterList::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	::imtbase::CTreeItemModel* dataModel = model.GetTreeItemModel("data", modelIndex);
	if (dataModel != nullptr){
		int dataCount = dataModel->GetItemsCount();
		imtsdl::TElementList<CPrinterBase> dataList;
		for (int dataIndex = 0; dataIndex < dataCount; ++dataIndex){
			CPrinterBase t_data;
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


bool CPrinterList::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
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


bool CPrinterList::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("data")){
		data.emplace();
	}
	if (gqlObject.ContainsParam("data") && (gqlObject.GetObjectsCount("data") > 0)){
		const qsizetype dataElementsCount = gqlObject.GetObjectsCount("data");
		data = imtsdl::TElementList<CPrinterBase>();
		for (qsizetype dataIndex = 0; dataIndex < dataElementsCount; ++dataIndex){
			const ::imtgql::CGqlParamObject* dataDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("data", dataIndex);
			if (dataDataObjectPtr == nullptr){
				qDebug() << "invalid type" << dataDataObjectPtr;
				return false;
			}
			CPrinterBase tempData;
			if (!tempData.ReadFromGraphQlObject(*dataDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "data").toLocal8Bit().constData();)

				return false;
			}
			data->append(tempData);
		}
	}

	return true;
}


bool CPrinterList::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("data")){
		data.emplace();
	}
	if (gqlObject.ContainsParam("data") && (gqlObject.GetObjectsCount("data") > 0)){
		const qsizetype dataElementsCount = gqlObject.GetObjectsCount("data");
		data = imtsdl::TElementList<CPrinterBase>();
		for (qsizetype dataIndex = 0; dataIndex < dataElementsCount; ++dataIndex){
			const ::imtgql::CGqlParamObject* dataDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("data", dataIndex);
			if (dataDataObjectPtr == nullptr){
				qDebug() << "invalid type" << dataDataObjectPtr;
				return false;
			}
			CPrinterBase tempData;
			if (!tempData.OptReadFromGraphQlObject(*dataDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "data").toLocal8Bit().constData();)

				return false;
			}
			data->append(tempData);
		}
	}

	return true;
}


bool CPrinterList::WriteToJsonObject(QJsonObject& jsonObject) const
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


bool CPrinterList::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("data") && jsonObject["data"].isArray()){
		const QJsonArray dataJsonArray = jsonObject["data"].toArray();
		const qsizetype dataArrayCount = dataJsonArray.size();
		data = imtsdl::TElementList<CPrinterBase>();
		for (qsizetype dataIndex = 0; dataIndex < dataArrayCount; ++dataIndex){
			CPrinterBase tempData;
			if (!tempData.ReadFromJsonObject(dataJsonArray[dataIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "data").toLocal8Bit().constData();)

				return false;
			}
			data->append(tempData);
		}
	}

	return true;
}


bool CPrinterList::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("data") && jsonObject["data"].isArray()){
		const QJsonArray dataJsonArray = jsonObject["data"].toArray();
		const qsizetype dataArrayCount = dataJsonArray.size();
		data = imtsdl::TElementList<CPrinterBase>();
		for (qsizetype dataIndex = 0; dataIndex < dataArrayCount; ++dataIndex){
			CPrinterBase tempData;
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

CPrinterSpecificationBaseObject::CPrinterSpecificationBaseObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CPrinterSpecificationBaseObject::nameChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CPrinterSpecificationBaseObject::GetName()
{
	if (!CPrinterSpecificationBase::name.HasValue()) return QVariant();
	return CPrinterSpecificationBase::name.value();
}


void CPrinterSpecificationBaseObject::SetName(const QVariant& v)
{

	CPrinterSpecificationBase::name = v.value<QString>();
	nameChanged();
}


bool CPrinterSpecificationBaseObject::hasName()
{
	 return CPrinterSpecificationBase::name.HasValue();
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


void CPrinterSpecificationBaseObjectList::append(sdl::V1_0::modsdl::CPrinterSpecificationBaseObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::modsdl::CPrinterSpecificationBaseObjectList* sdl::V1_0::modsdl::CPrinterSpecificationBaseObjectList::copyMe()
{
	sdl::V1_0::modsdl::CPrinterSpecificationBaseObjectList* retVal = new sdl::V1_0::modsdl::CPrinterSpecificationBaseObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::modsdl::CPrinterSpecificationBaseObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::modsdl::CPrinterSpecificationBaseObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::modsdl::CPrinterSpecificationBaseObjectList::addElement(sdl::V1_0::modsdl::CPrinterSpecificationBaseObject* item)
{
	append(item);
}


void sdl::V1_0::modsdl::CPrinterSpecificationBaseObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::modsdl::CPrinterSpecificationBaseObjectList::isEqualWithModel(sdl::V1_0::modsdl::CPrinterSpecificationBaseObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::modsdl::CPrinterSpecificationBaseObjectList::insert(int index, sdl::V1_0::modsdl::CPrinterSpecificationBaseObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::modsdl::CPrinterSpecificationBaseObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::modsdl::CPrinterSpecificationBaseObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::modsdl::CPrinterSpecificationBaseObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::modsdl::CPrinterSpecificationBaseObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_name"){
		if (!at(index)->name.HasValue()) return QVariant();
		return QVariant::fromValue(at(index)->name.value());
	}

	return QVariant();
}
CLinkObject::CLinkObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CLinkObject::linkChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CLinkObject::GetLink()
{
	if (!CLink::link.HasValue()) return QVariant();
	return CLink::link.value();
}


void CLinkObject::SetLink(const QVariant& v)
{

	CLink::link = v.value<QString>().toUtf8();
	linkChanged();
}


bool CLinkObject::hasLink()
{
	 return CLink::link.HasValue();
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


void CLinkObjectList::append(sdl::V1_0::modsdl::CLinkObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::modsdl::CLinkObjectList* sdl::V1_0::modsdl::CLinkObjectList::copyMe()
{
	sdl::V1_0::modsdl::CLinkObjectList* retVal = new sdl::V1_0::modsdl::CLinkObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::modsdl::CLinkObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::modsdl::CLinkObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::modsdl::CLinkObjectList::addElement(sdl::V1_0::modsdl::CLinkObject* item)
{
	append(item);
}


void sdl::V1_0::modsdl::CLinkObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::modsdl::CLinkObjectList::isEqualWithModel(sdl::V1_0::modsdl::CLinkObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::modsdl::CLinkObjectList::insert(int index, sdl::V1_0::modsdl::CLinkObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::modsdl::CLinkObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::modsdl::CLinkObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::modsdl::CLinkObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::modsdl::CLinkObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_link"){
		if (!at(index)->link.HasValue()) return QVariant();
		return QVariant::fromValue(at(index)->link.value());
	}

	return QVariant();
}
CPrinterBaseObject::CPrinterBaseObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CPrinterBaseObject::nameChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CPrinterBaseObject::specificationChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CPrinterBaseObject::simpleTestChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CPrinterBaseObject::mixedTestChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CPrinterBaseObject::GetName()
{
	if (!CPrinterBase::name.HasValue()) return QVariant();
	return CPrinterBase::name.value();
}


void CPrinterBaseObject::SetName(const QVariant& v)
{

	CPrinterBase::name = v.value<QString>();
	nameChanged();
}


bool CPrinterBaseObject::hasName()
{
	 return CPrinterBase::name.HasValue();
}


QVariant CPrinterBaseObject::GetSpecification()
{
	if (!m_specificationQObjectPtr.isValid() && CPrinterBase::specification.HasValue()){
		if (const CPrinterSpecificationBase* val = std::get_if<CPrinterSpecificationBase>((CPrinterBase::specification).GetPtr())){
			CPrinterSpecificationBaseObject *newObjectPtr = new CPrinterSpecificationBaseObject(this);
			static_cast<decltype(*newObjectPtr)&>(*newObjectPtr) = *val;
			m_specificationQObjectPtr = QVariant::fromValue(newObjectPtr);
		}
		if (const CLink* val = std::get_if<CLink>((CPrinterBase::specification).GetPtr())){
			CLinkObject *newObjectPtr = new CLinkObject(this);
			static_cast<decltype(*newObjectPtr)&>(*newObjectPtr) = *val;
			m_specificationQObjectPtr = QVariant::fromValue(newObjectPtr);
		}
	}
	return m_specificationQObjectPtr;
}


void CPrinterBaseObject::SetSpecification(const QVariant& v)
{

	if (v.isValid()){
		if (const CPrinterSpecificationBaseObject* val = v.value<const CPrinterSpecificationBaseObject*>()){
			*specification = *val;
		}
		if (const CLinkObject* val = v.value<const CLinkObject*>()){
			*specification = *val;
		}
	}
	else {
		CPrinterBase::specification = nullptr;
	}
	m_specificationQObjectPtr = v;

	specificationChanged();
}


bool CPrinterBaseObject::hasSpecification()
{
	 return CPrinterBase::specification.HasValue();
}


void CPrinterBaseObject::emplaceSpecification()
{
	ResetSpecification();
	CPrinterBase::specification.emplace();
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
	if (!m_simpleTestQObjectPtr.isValid() && CPrinterBase::simpleTest.HasValue()){
		if (const QString* val = std::get_if<QString>((CPrinterBase::simpleTest).GetPtr())){
			m_simpleTestQObjectPtr = QVariant::fromValue(val);
		}
		if (const double* val = std::get_if<double>((CPrinterBase::simpleTest).GetPtr())){
			m_simpleTestQObjectPtr = QVariant::fromValue(val);
		}
	}
	return m_simpleTestQObjectPtr;
}


void CPrinterBaseObject::SetSimpleTest(const QVariant& v)
{

	if (v.isValid()){
		if (const QString* val = v.value<const QString*>()){
			*simpleTest = *val;
		}
		if (const double* val = v.value<const double*>()){
			*simpleTest = *val;
		}
	}
	else {
		CPrinterBase::simpleTest = nullptr;
	}
	m_simpleTestQObjectPtr = v;

	simpleTestChanged();
}


bool CPrinterBaseObject::hasSimpleTest()
{
	 return CPrinterBase::simpleTest.HasValue();
}


void CPrinterBaseObject::emplaceSimpleTest()
{
	ResetSimpleTest();
	CPrinterBase::simpleTest.emplace();
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
	if (!m_mixedTestQObjectPtr.isValid() && CPrinterBase::mixedTest.HasValue()){
		if (const QString* val = std::get_if<QString>((CPrinterBase::mixedTest).GetPtr())){
			m_mixedTestQObjectPtr = QVariant::fromValue(val);
		}
		if (const CLink* val = std::get_if<CLink>((CPrinterBase::mixedTest).GetPtr())){
			CLinkObject *newObjectPtr = new CLinkObject(this);
			static_cast<decltype(*newObjectPtr)&>(*newObjectPtr) = *val;
			m_mixedTestQObjectPtr = QVariant::fromValue(newObjectPtr);
		}
	}
	return m_mixedTestQObjectPtr;
}


void CPrinterBaseObject::SetMixedTest(const QVariant& v)
{

	if (v.isValid()){
		if (const QString* val = v.value<const QString*>()){
			*mixedTest = *val;
		}
		if (const CLinkObject* val = v.value<const CLinkObject*>()){
			*mixedTest = *val;
		}
	}
	else {
		CPrinterBase::mixedTest = nullptr;
	}
	m_mixedTestQObjectPtr = v;

	mixedTestChanged();
}


bool CPrinterBaseObject::hasMixedTest()
{
	 return CPrinterBase::mixedTest.HasValue();
}


void CPrinterBaseObject::emplaceMixedTest()
{
	ResetMixedTest();
	CPrinterBase::mixedTest.emplace();
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


void CPrinterBaseObjectList::append(sdl::V1_0::modsdl::CPrinterBaseObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::modsdl::CPrinterBaseObjectList* sdl::V1_0::modsdl::CPrinterBaseObjectList::copyMe()
{
	sdl::V1_0::modsdl::CPrinterBaseObjectList* retVal = new sdl::V1_0::modsdl::CPrinterBaseObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::modsdl::CPrinterBaseObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::modsdl::CPrinterBaseObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::modsdl::CPrinterBaseObjectList::addElement(sdl::V1_0::modsdl::CPrinterBaseObject* item)
{
	append(item);
}


void sdl::V1_0::modsdl::CPrinterBaseObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::modsdl::CPrinterBaseObjectList::isEqualWithModel(sdl::V1_0::modsdl::CPrinterBaseObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::modsdl::CPrinterBaseObjectList::insert(int index, sdl::V1_0::modsdl::CPrinterBaseObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::modsdl::CPrinterBaseObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::modsdl::CPrinterBaseObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::modsdl::CPrinterBaseObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::modsdl::CPrinterBaseObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_name"){
		if (!at(index)->name.HasValue()) return QVariant();
		return QVariant::fromValue(at(index)->name.value());
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

	QObject::connect(this, &CPrinterListObject::dataChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CPrinterListObject::GetData()
{
	if (!m_dataQObjectPtr.isValid() && CPrinterList::data.HasValue()){
		m_dataQObjectPtr = CreateObject("data");
		auto itemPtr = m_dataQObjectPtr.value<sdl::V1_0::modsdl::CPrinterBaseObjectList*>();
		if (itemPtr != nullptr) itemPtr->Version_1_0 = CPrinterList::data;
	}
	return m_dataQObjectPtr;
}


void CPrinterListObject::SetData(const QVariant& v)
{

	if (v.isValid()){
		sdl::V1_0::modsdl::CPrinterBaseObjectList* itemPtr = v.value<sdl::V1_0::modsdl::CPrinterBaseObjectList*>();
		if (itemPtr != nullptr)  data = static_cast<const decltype(data)::value_type&>(*itemPtr);
	}
	else {
		CPrinterList::data = nullptr;
	}
	m_dataQObjectPtr = v;

	dataChanged();
}


bool CPrinterListObject::hasData()
{
	 return CPrinterList::data.HasValue();
}


void CPrinterListObject::emplaceData()
{
	ResetData();
	CPrinterList::data.emplace();
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
	return QVariant::fromValue(new sdl::V1_0::modsdl::CPrinterBaseObject());
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
		return QVariant::fromValue(new sdl::V1_0::modsdl::CPrinterBaseObjectList(this));
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


void CPrinterListObjectList::append(sdl::V1_0::modsdl::CPrinterListObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::modsdl::CPrinterListObjectList* sdl::V1_0::modsdl::CPrinterListObjectList::copyMe()
{
	sdl::V1_0::modsdl::CPrinterListObjectList* retVal = new sdl::V1_0::modsdl::CPrinterListObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::modsdl::CPrinterListObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::modsdl::CPrinterListObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::modsdl::CPrinterListObjectList::addElement(sdl::V1_0::modsdl::CPrinterListObject* item)
{
	append(item);
}


void sdl::V1_0::modsdl::CPrinterListObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::modsdl::CPrinterListObjectList::isEqualWithModel(sdl::V1_0::modsdl::CPrinterListObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::modsdl::CPrinterListObjectList::insert(int index, sdl::V1_0::modsdl::CPrinterListObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::modsdl::CPrinterListObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::modsdl::CPrinterListObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::modsdl::CPrinterListObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::modsdl::CPrinterListObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
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
		if (const CPrinterSpecificationBase* val = std::get_if<CPrinterSpecificationBase>(this->at(index).GetPtr())){
			CPrinterSpecificationBaseObject *newObjectPtr = new CPrinterSpecificationBaseObject();
			static_cast<decltype(*newObjectPtr)&>(*newObjectPtr) = *val;
			retVal = QVariant::fromValue(newObjectPtr);
		}

		if (const CLink* val = std::get_if<CLink>(this->at(index).GetPtr())){
			CLinkObject *newObjectPtr = new CLinkObject();
			static_cast<decltype(*newObjectPtr)&>(*newObjectPtr) = *val;
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
	beginInsertRows(QModelIndex(), count(), 0);

	if (item.canConvert<CPrinterSpecificationBaseObject*>()){
		CPrinterSpecificationBaseObject* val = item.value<CPrinterSpecificationBaseObject*>();
		CPrinterSpecificationBase* newItemPtr = dynamic_cast<CPrinterSpecificationBase*>(val);
		append(istd::TNullableValue<PrinterSpecification>(*newItemPtr));
	}

	if (item.canConvert<CLinkObject*>()){
		CLinkObject* val = item.value<CLinkObject*>();
		CLink* newItemPtr = dynamic_cast<CLink*>(val);
		append(istd::TNullableValue<PrinterSpecification>(*newItemPtr));
	}

	ClearCache();
	endInsertRows();
}


sdl::V1_0::modsdl::CPrinterSpecificationObjectList* CPrinterSpecificationObjectList::copyMe()
{
	sdl::V1_0::modsdl::CPrinterSpecificationObjectList* objectListPtr = new sdl::V1_0::modsdl::CPrinterSpecificationObjectList();

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

bool CPrinterSpecificationObjectList::isEqualWithModel(sdl::V1_0::modsdl::CPrinterSpecificationObjectList* otherModelPtr)
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
	if (index < 0 || index > size()) return;
	beginInsertRows(QModelIndex(), count(), 0);

	if (item.canConvert<CPrinterSpecificationBaseObject*>()){
		CPrinterSpecificationBaseObject* val = item.value<CPrinterSpecificationBaseObject*>();
		CPrinterSpecificationBase* newItemPtr = dynamic_cast<CPrinterSpecificationBase*>(val);
		append(istd::TNullableValue<PrinterSpecification>(*newItemPtr));
	}

	if (item.canConvert<CLinkObject*>()){
		CLinkObject* val = item.value<CLinkObject*>();
		CLink* newItemPtr = dynamic_cast<CLink*>(val);
		append(istd::TNullableValue<PrinterSpecification>(*newItemPtr));
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
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
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
		if (const QString* val = std::get_if<QString>(this->at(index).GetPtr())){
			retVal =  QVariant::fromValue(val);
		}

		if (const double* val = std::get_if<double>(this->at(index).GetPtr())){
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
	beginInsertRows(QModelIndex(), count(), 0);

	if (item.canConvert<QString>()){
		QString val = item.value<QString>();
		append(istd::TNullableValue<SimpleUnion>(val));
	}

	if (item.canConvert<double>()){
		double val = item.value<double>();
		append(istd::TNullableValue<SimpleUnion>(val));
	}

	ClearCache();
	endInsertRows();
}


sdl::V1_0::modsdl::CSimpleUnionObjectList* CSimpleUnionObjectList::copyMe()
{
	sdl::V1_0::modsdl::CSimpleUnionObjectList* objectListPtr = new sdl::V1_0::modsdl::CSimpleUnionObjectList();

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

bool CSimpleUnionObjectList::isEqualWithModel(sdl::V1_0::modsdl::CSimpleUnionObjectList* otherModelPtr)
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
	if (index < 0 || index > size()) return;
	beginInsertRows(QModelIndex(), count(), 0);

	if (item.canConvert<QString>()){
		QString val = item.value<QString>();
		append(istd::TNullableValue<SimpleUnion>(val));
	}

	if (item.canConvert<double>()){
		double val = item.value<double>();
		append(istd::TNullableValue<SimpleUnion>(val));
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
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
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
		if (const QString* val = std::get_if<QString>(this->at(index).GetPtr())){
			retVal =  QVariant::fromValue(val);
		}

		if (const CLink* val = std::get_if<CLink>(this->at(index).GetPtr())){
			CLinkObject *newObjectPtr = new CLinkObject();
			static_cast<decltype(*newObjectPtr)&>(*newObjectPtr) = *val;
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
	beginInsertRows(QModelIndex(), count(), 0);

	if (item.canConvert<QString>()){
		QString val = item.value<QString>();
		append(istd::TNullableValue<MixedUnion>(val));
	}

	if (item.canConvert<CLinkObject*>()){
		CLinkObject* val = item.value<CLinkObject*>();
		CLink* newItemPtr = dynamic_cast<CLink*>(val);
		append(istd::TNullableValue<MixedUnion>(*newItemPtr));
	}

	ClearCache();
	endInsertRows();
}


sdl::V1_0::modsdl::CMixedUnionObjectList* CMixedUnionObjectList::copyMe()
{
	sdl::V1_0::modsdl::CMixedUnionObjectList* objectListPtr = new sdl::V1_0::modsdl::CMixedUnionObjectList();

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

bool CMixedUnionObjectList::isEqualWithModel(sdl::V1_0::modsdl::CMixedUnionObjectList* otherModelPtr)
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
	if (index < 0 || index > size()) return;
	beginInsertRows(QModelIndex(), count(), 0);

	if (item.canConvert<QString>()){
		QString val = item.value<QString>();
		append(istd::TNullableValue<MixedUnion>(val));
	}

	if (item.canConvert<CLinkObject*>()){
		CLinkObject* val = item.value<CLinkObject*>();
		CLink* newItemPtr = dynamic_cast<CLink*>(val);
		append(istd::TNullableValue<MixedUnion>(*newItemPtr));
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
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
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
bool CPrinterBaseGqlHandlerCompBase::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	const QByteArray commandId = gqlRequest.GetCommandId();
	if (commandId == CGetPrintersGqlRequest::GetCommandId()){
		return true;
	}

	return BaseClass::IsRequestSupported(gqlRequest);
}


QJsonObject CPrinterBaseGqlHandlerCompBase::CreateInternalResponse(const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	const QByteArray commandId = gqlRequest.GetCommandId();
	QJsonObject modelObj;
	QJsonObject dataModelObj;

	// GetPrinters
	if (commandId == CGetPrintersGqlRequest::GetCommandId()){
		CGetPrintersGqlRequest getPrintersGqlRequest(gqlRequest, false);
		if (!getPrintersGqlRequest.IsValid()){
			errorMessage = QString("Bad request. Unexpected request for command-ID: '%1'").arg(qPrintable(commandId));
			SendErrorMessage(0, errorMessage);

			return QJsonObject();
		}

		CPrinterList replyPayload = OnGetPrinters(getPrintersGqlRequest, gqlRequest, errorMessage);
		if (!errorMessage.isEmpty()){
			SendErrorMessage(0, QString("The derived call [OnGetPrinters] returned an error: %1").arg(errorMessage));

			return QJsonObject();
		}

		const bool isModelCreated = replyPayload.WriteToJsonObject(dataModelObj);
		if (!isModelCreated){
			errorMessage = QString("Internal error. Unable to create response for command-ID: '%1'").arg(qPrintable(commandId));
			SendCriticalMessage(0, errorMessage);

			return QJsonObject();
		}

		modelObj.insert(QStringLiteral("data"), dataModelObj); return modelObj;
	}

	errorMessage = QString("Bad request. Unexpected command-ID: '%1'").arg(qPrintable(commandId));
	SendErrorMessage(0, errorMessage);

	return QJsonObject();
}


} // namespace sdl::V1_0::modsdl
