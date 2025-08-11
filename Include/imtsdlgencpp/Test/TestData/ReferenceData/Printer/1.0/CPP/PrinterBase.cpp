#include "PrinterBase.h"


#include "PrinterBase.h"


#include "PrinterBase.h"


#include "PrinterBase.h"


/// \file CPrinterSpecificationBase.cpp

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


CPrinterSpecificationBaseObject::CPrinterSpecificationBaseObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &CPrinterSpecificationBaseObject::nameChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QString CPrinterSpecificationBaseObject::GetName()
{
	if (Version_1_0->name.has_value()){
		return Version_1_0->name.value();
	}

	return QString();
}


void CPrinterSpecificationBaseObject::SetName(QString v)
{
	Version_1_0->name = v;
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


QObject* CPrinterSpecificationBaseObject::CreateObject(const QString& key)
{
	return nullptr;
}


QString CPrinterSpecificationBaseObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "name")){
		return "name";
	}

	return propertyName;
}


} // namespace sdl::modsdl::PrinterBase




/// \file CLink.cpp

namespace sdl::modsdl::PrinterBase
{


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


CLinkObject::CLinkObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &CLinkObject::linkChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QString CLinkObject::GetLink()
{
	if (Version_1_0->link.has_value()){
		return Version_1_0->link.value();
	}

	return QString();
}


void CLinkObject::SetLink(QString v)
{
	Version_1_0->link = v.toUtf8();
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


QObject* CLinkObject::CreateObject(const QString& key)
{
	return nullptr;
}


QString CLinkObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "link")){
		return "link";
	}

	return propertyName;
}


} // namespace sdl::modsdl::PrinterBase




/// \file CPrinterBase.cpp

namespace sdl::modsdl::PrinterBase
{


QByteArray CPrinterBase::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CPrinterBase::V1_0::operator==(const V1_0& other) const
{
	return 
				name == other.name &&
				specification == other.specification;
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
		QString itemTypename = specificationData.value<::imtbase::CTreeItemModel*>()->GetData("__typename").toString();

	if (itemTypename == "PrinterSpecificationBase") {
		CPrinterSpecificationBase specificationConvert;
		const bool isspecificationRead = specificationConvert.ReadFromModel(*model.GetTreeItemModel("specification", modelIndex)); 
		if (!isspecificationRead){
			return false;
		}
		specification = std::make_shared<PrinterSpecification>(specificationConvert);
	}
	else if (itemTypename == "Link") {
		CLink specificationConvert;
		const bool isspecificationRead = specificationConvert.ReadFromModel(*model.GetTreeItemModel("specification", modelIndex)); 
		if (!isspecificationRead){
			return false;
		}
		specification = std::make_shared<PrinterSpecification>(specificationConvert);
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
	QString itemTypename = specificationData.value<::imtbase::CTreeItemModel*>()->GetData("__typename").toString();

		if (itemTypename == "PrinterSpecificationBase") {
			CPrinterSpecificationBase specificationConvert;
			const bool isspecificationRead = specificationConvert.ReadFromModel(*model.GetTreeItemModel("specification", modelIndex)); 
			if (!isspecificationRead){
				return false;
			}
			specification = std::make_shared<PrinterSpecification>(specificationConvert);
		}
		else if (itemTypename == "Link") {
			CLink specificationConvert;
			const bool isspecificationRead = specificationConvert.ReadFromModel(*model.GetTreeItemModel("specification", modelIndex)); 
			if (!isspecificationRead){
				return false;
			}
			specification = std::make_shared<PrinterSpecification>(specificationConvert);
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

	return true;
}


bool CPrinterBase::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("name") && (gqlObject["name"].userType() == QMetaType::QString || gqlObject["name"].userType() == QMetaType::QByteArray)){
		name = gqlObject["name"].toString();
	}

	if (!gqlObject.ContainsParam("specification") || (((gqlObject.IsObject("specification") && gqlObject.GetParamArgumentObjectPtr("specification") == nullptr) || (!gqlObject.IsObject("specification") && gqlObject["specification"].isNull())) || gqlObject.GetParamArgumentObjectPtr("specification") == nullptr)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "specification");)

		return false;
	}
	const ::imtgql::CGqlParamObject* itemDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("specification");
	if (!itemDataObjectPtr->ContainsParam("__typename")) {
		qDebug() << "invalid typename for: specification";
		return false;
	}

	QString itemTypename = itemDataObjectPtr->GetParamArgumentValue("__typename").toString();;
	if (itemTypename == "PrinterSpecificationBase") {
		CPrinterSpecificationBase specificationConvert;
		const ::imtgql::CGqlParamObject* specificationDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("specification");
		const bool isSpecificationRead = specificationConvert.ReadFromGraphQlObject(*specificationDataObjectPtr);
		if (!isSpecificationRead){
			return false;
		}
		specification = std::make_shared<PrinterSpecification>(specificationConvert);
	}
	else if (itemTypename == "Link") {
		CLink specificationConvert;
		const ::imtgql::CGqlParamObject* specificationDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("specification");
		const bool isSpecificationRead = specificationConvert.ReadFromGraphQlObject(*specificationDataObjectPtr);
		if (!isSpecificationRead){
			return false;
		}
		specification = std::make_shared<PrinterSpecification>(specificationConvert);
	}

	return true;
}


bool CPrinterBase::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("name") && (gqlObject["name"].userType() == QMetaType::QString || gqlObject["name"].userType() == QMetaType::QByteArray)){
		name = gqlObject["name"].toString();
	}

	if (gqlObject.ContainsParam("specification") && (!((gqlObject.IsObject("specification") && gqlObject.GetParamArgumentObjectPtr("specification") == nullptr) || (!gqlObject.IsObject("specification") && gqlObject["specification"].isNull())) && gqlObject.GetParamArgumentObjectPtr("specification") != nullptr)){
		const ::imtgql::CGqlParamObject* itemDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("specification");
	if (!itemDataObjectPtr->ContainsParam("__typename")) {
		qDebug() << "invalid typename for: specification";
		return false;
	}

	QString itemTypename = itemDataObjectPtr->GetParamArgumentValue("__typename").toString();;
		if (itemTypename == "PrinterSpecificationBase") {
			CPrinterSpecificationBase specificationConvert;
			const ::imtgql::CGqlParamObject* specificationDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("specification");
			const bool isSpecificationRead = specificationConvert.ReadFromGraphQlObject(*specificationDataObjectPtr);
			if (!isSpecificationRead){
				return false;
			}
			specification = std::make_shared<PrinterSpecification>(specificationConvert);
		}
		else if (itemTypename == "Link") {
			CLink specificationConvert;
			const ::imtgql::CGqlParamObject* specificationDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("specification");
			const bool isSpecificationRead = specificationConvert.ReadFromGraphQlObject(*specificationDataObjectPtr);
			if (!isSpecificationRead){
				return false;
			}
			specification = std::make_shared<PrinterSpecification>(specificationConvert);
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


	return true;
}


bool CPrinterBase::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("name") && jsonObject["name"].isString()){
		name = jsonObject["name"].toString();
	}

	if (!jsonObject.contains("specification") || ! jsonObject["specification"].isObject()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "specification");)

		return false;
	}
	QJsonObject itemObject = jsonObject.value("specification").toObject();
	if (!itemObject.contains("__typename")){
		qDebug() << "invalid typename for: specification";
		return false;
	}

	QString itemTypename = itemObject.value("__typename").toString();;
	if (itemTypename == "PrinterSpecificationBase") {
		CPrinterSpecificationBase specificationConvert;
		const bool isspecificationRead = specificationConvert.ReadFromJsonObject(jsonObject["specification"].toObject());
		if (!isspecificationRead){
			return false;
		}
		specification = std::make_shared<PrinterSpecification>(specificationConvert);
	}
	else if (itemTypename == "Link") {
		CLink specificationConvert;
		const bool isspecificationRead = specificationConvert.ReadFromJsonObject(jsonObject["specification"].toObject());
		if (!isspecificationRead){
			return false;
		}
		specification = std::make_shared<PrinterSpecification>(specificationConvert);
	}

	return true;
}


bool CPrinterBase::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("name") && jsonObject["name"].isString()){
		name = jsonObject["name"].toString();
	}

	if (jsonObject.contains("specification") && jsonObject["specification"].isObject()){
		QJsonObject itemObject = jsonObject.value("specification").toObject();
	if (!itemObject.contains("__typename")){
		qDebug() << "invalid typename for: specification";
		return false;
	}

	QString itemTypename = itemObject.value("__typename").toString();;
		if (itemTypename == "PrinterSpecificationBase") {
			CPrinterSpecificationBase specificationConvert;
			const bool isspecificationRead = specificationConvert.ReadFromJsonObject(jsonObject["specification"].toObject());
			if (!isspecificationRead){
				return false;
			}
			specification = std::make_shared<PrinterSpecification>(specificationConvert);
		}
		else if (itemTypename == "Link") {
			CLink specificationConvert;
			const bool isspecificationRead = specificationConvert.ReadFromJsonObject(jsonObject["specification"].toObject());
			if (!isspecificationRead){
				return false;
			}
			specification = std::make_shared<PrinterSpecification>(specificationConvert);
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


CPrinterBaseObject::CPrinterBaseObject(QObject* parent): ::imtbase::CItemModelBase(parent)			, m_specificationQObjectPtr(nullptr)
{
	Version_1_0.emplace();

	QObject::connect(this, &CPrinterBaseObject::nameChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CPrinterBaseObject::specificationChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QString CPrinterBaseObject::GetName()
{
	if (Version_1_0->name.has_value()){
		return Version_1_0->name.value();
	}

	return QString();
}


void CPrinterBaseObject::SetName(QString v)
{
	Version_1_0->name = v;
	nameChanged();
}


bool CPrinterBaseObject::hasName()
{
	 return Version_1_0->name.HasValue();
}


sdl::modsdl::PrinterBase::CPrinterSpecificationObject* CPrinterBaseObject::GetSpecification()
{
	if (Version_1_0->specification.has_value()){
		if (!m_specificationQObjectPtr){
			m_specificationQObjectPtr = dynamic_cast<sdl::modsdl::PrinterBase::CPrinterSpecificationObject*>(CreateObject("specification"));
			m_specificationQObjectPtr->Version_1_0 = Version_1_0->specification;
		}
		return m_specificationQObjectPtr;
	}

	return nullptr;
}


void CPrinterBaseObject::SetSpecification(sdl::modsdl::PrinterBase::CPrinterSpecificationObject* v)
{
	if (v){
		Version_1_0->specification = v->Version_1_0;
		m_specificationQObjectPtr = v;
	}
	else {
		Version_1_0->specification = nullptr;
	}

	specificationChanged();
}


bool CPrinterBaseObject::hasSpecification()
{
	 return Version_1_0->specification.HasValue();
}


void CPrinterBaseObject::createSpecification()
{	Version_1_0->specification.emplace();

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


QObject* CPrinterBaseObject::CreateObject(const QString& key)
{
	if (key == "specification"){
		return new sdl::modsdl::PrinterBase::CPrinterSpecificationObject(this);
	}
	return nullptr;
}


QString CPrinterBaseObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "name")){
		return "name";
	}
	if (propertyName == (QString("m_") + "specification")){
		return "specification";
	}

	return propertyName;
}


} // namespace sdl::modsdl::PrinterBase




/// \file CPrinterList.cpp

namespace sdl::modsdl::PrinterBase
{


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


CPrinterListObject::CPrinterListObject(QObject* parent): ::imtbase::CItemModelBase(parent)			, m_dataQObjectPtr(nullptr)
{
	Version_1_0.emplace();

	QObject::connect(this, &CPrinterListObject::dataChanged, this, &CItemModelBase::OnInternalModelChanged);
}


sdl::modsdl::PrinterBase::CPrinterBaseObjectList* CPrinterListObject::GetData()
{
	if (Version_1_0->data.has_value()){
		if (!m_dataQObjectPtr){
			m_dataQObjectPtr = dynamic_cast<sdl::modsdl::PrinterBase::CPrinterBaseObjectList*>(CreateObject("data"));
			m_dataQObjectPtr->Version_1_0 = Version_1_0->data;
		}
		return m_dataQObjectPtr;
	}

	return nullptr;
}


void CPrinterListObject::SetData(sdl::modsdl::PrinterBase::CPrinterBaseObjectList* v)
{
	if (v){
		Version_1_0->data = v->Version_1_0;
		m_dataQObjectPtr = v;
	}
	else {
		Version_1_0->data = nullptr;
	}

	dataChanged();
}


bool CPrinterListObject::hasData()
{
	 return Version_1_0->data.HasValue();
}


void CPrinterListObject::createData()
{	Version_1_0->data.emplace();

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


QObject* CPrinterListObject::CreateObject(const QString& key)
{
	if (key == "data"){
		return new sdl::modsdl::PrinterBase::CPrinterBaseObjectList(this);
	}
	return nullptr;
}


QString CPrinterListObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "data")){
		return "data";
	}

	return propertyName;
}


} // namespace sdl::modsdl::PrinterBase




/// \file CGetPrintersGqlRequest.cpp


namespace sdl::modsdl::PrinterBase
{


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


} // namespace sdl::modsdl::PrinterBase



/// \file CGraphQlHandlerCompBase.cpp


namespace sdl::modsdl::PrinterBase
{


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



