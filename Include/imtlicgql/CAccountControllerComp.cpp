#include <imtlicgql/CAccountControllerComp.h>


// ImtCore includes
#include <imtauth/CCompanyInfo.h>


namespace imtlicgql
{


// protected methods

// reimplemented (imtgql::CObjectCollectionControllerCompBase)

imtbase::CTreeItemModel* CAccountControllerComp::GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();
		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModel = new imtbase::CTreeItemModel();

	dataModel->SetData("Id", "");
	dataModel->SetData("Name", "");
	dataModel->SetData("Description", "");
	dataModel->SetData("Email", "");
	dataModel->SetData("CompanyName", "");

	QByteArray accountId = GetObjectIdFromInputParams(*gqlRequest.GetParams());

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(accountId, dataPtr)){
		const imtauth::CCompanyInfo* companyInfoPtr = dynamic_cast<const imtauth::CCompanyInfo*>(dataPtr.GetPtr());
		if (companyInfoPtr != nullptr){
			QString accountName = companyInfoPtr->GetAccountName();
			QString accountDescription = companyInfoPtr->GetAccountDescription();
			QString mail = companyInfoPtr->GetMail();
			QString companyName = companyInfoPtr->GetCompanyName();
			QByteArrayList groups = companyInfoPtr->GetGroups();

			const imtauth::CAddress address = companyInfoPtr->GetAddress();
			QString country = address.GetCountry();
			QString city = address.GetCity();
			int postalCode = address.GetPostalCode();
			QString street = address.GetStreet();

			dataModel->SetData("Id", accountId);
			dataModel->SetData("Name", accountName);
			dataModel->SetData("Description", accountDescription);
			dataModel->SetData("Email", mail);
			dataModel->SetData("CompanyName", companyName);
			dataModel->SetData("Country", country);
			dataModel->SetData("City", city);
			dataModel->SetData("PostalCode", postalCode);
			dataModel->SetData("Street", street);
			dataModel->SetData("Groups", groups.join(';'));
		}
	}

	rootModelPtr->SetExternTreeModel("data", dataModel);

	return rootModelPtr.PopPtr();
}


istd::IChangeable* CAccountControllerComp::CreateObject(
		const QList<imtgql::CGqlObject>& inputParams,
		QByteArray &objectId,
		QString &name,
		QString &description,
		QString& errorMessage) const
{
	if (!m_accountInfoFactCompPtr.IsValid() || !m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT(false);
		return nullptr;
	}

	QByteArray itemData = inputParams.at(0).GetFieldArgumentValue("Item").toByteArray();
	if (!itemData.isEmpty()){
		imtauth::ICompanyInfo* companyInstancePtr = m_accountInfoFactCompPtr.CreateInstance();
		if (companyInstancePtr == nullptr){
			return nullptr;
		}

		imtauth::CIdentifiableCompanyInfo* companyInfoPtr = dynamic_cast<imtauth::CIdentifiableCompanyInfo*>(companyInstancePtr);
		if (companyInfoPtr == nullptr){
			errorMessage = QT_TR_NOOP("Unable to get an account info!");
			return nullptr;
		}

		imtbase::CTreeItemModel itemModel;
		itemModel.CreateFromJson(itemData);

		if (itemModel.ContainsKey("Id")){
			QByteArray id = itemModel.GetData("Id").toByteArray();
			if (!id.isEmpty()){
				objectId = id;
			}
		}

		if (objectId.isEmpty()){
			objectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
		}

		companyInfoPtr->SetObjectUuid(objectId);

		if (itemModel.ContainsKey("Name")){
			name = itemModel.GetData("Name").toString();
		}

		if (name.isEmpty()){
			errorMessage = QT_TR_NOOP("Account name can't be empty");
			return nullptr;
		}

		companyInfoPtr->SetAccountName(name);

		if (itemModel.ContainsKey("Description")){
			description = itemModel.GetData("Description").toString();
			companyInfoPtr->SetAccountDescription(description);
		}

		if (itemModel.ContainsKey("Email")){
			QString email = itemModel.GetData("Email").toString();
			companyInfoPtr->SetMail(email);
		}

		if (itemModel.ContainsKey("CompanyName")){
			QString companyName = itemModel.GetData("CompanyName").toString();
			companyInfoPtr->SetCompanyName(companyName);
		}

		imtauth::CAddress address;
		if (itemModel.ContainsKey("Country")){
			QString country = itemModel.GetData("Country").toString();
			address.SetCountry(country);
		}

		if (itemModel.ContainsKey("City")){
			QString city = itemModel.GetData("City").toString();
			address.SetCity(city);
		}

		if (itemModel.ContainsKey("PostalCode")){
			int postalCode = itemModel.GetData("PostalCode").toInt();
			address.SetPostalCode(postalCode);
		}

		if (itemModel.ContainsKey("Street")){
			QString street = itemModel.GetData("Street").toString();
			address.SetStreet(street);
		}

		companyInfoPtr->SetAddress(address);

		if (itemModel.ContainsKey("Groups")){
			QByteArray groups = itemModel.GetData("Groups").toByteArray();
			if (!groups.isEmpty()){
				QByteArrayList groupIds = groups.split(';');
				for (const QByteArray& groupId : groupIds){
					companyInfoPtr->AddGroup(groupId);
				}
			}
		}

		return companyInfoPtr;
	}

	errorMessage = QObject::tr("Can not create account: %1").arg(QString(objectId));

	return nullptr;
}


} // namespace imtlicgql


