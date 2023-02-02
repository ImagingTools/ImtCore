#include <imtlicgql/CAccountControllerComp.h>


// ImtCore includes
#include <imtauth/CCompanyInfo.h>


namespace imtlicgql
{

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
			QByteArray accountId = accountName.toUtf8();
            QString mail = companyInfoPtr->GetMail();
            QString companyName = companyInfoPtr->GetCompanyName();

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

			imtbase::CTreeItemModel* addressesModel = new imtbase::CTreeItemModel();

            addressesModel->SetData("Country", country);
            addressesModel->SetData("City", city);
            addressesModel->SetData("PostalCode", postalCode);
            addressesModel->SetData("Street", street);

            dataModel->SetExternTreeModel("Addresses", addressesModel);
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
        istd::TDelPtr<imtauth::CCompanyInfo> companyInfoPtr = new imtauth::CCompanyInfo();

        if (companyInfoPtr == nullptr){
			errorMessage = QT_TR_NOOP("Unable to get an account info!");
			return nullptr;
		}

		imtbase::CTreeItemModel itemModel;
		itemModel.CreateFromJson(itemData);

		if (itemModel.ContainsKey("Name")){
			name = itemModel.GetData("Name").toString();
		}

		if (name.isEmpty()){
			errorMessage = QT_TR_NOOP("Account name can't be empty");
			return nullptr;
		}

		objectId = name.toUtf8();

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

        if (itemModel.ContainsKey("Addresses")){
            imtauth::CAddress address;
            imtbase::CTreeItemModel* addressModel = itemModel.GetTreeItemModel("Addresses");

            if (addressModel->ContainsKey("Country")){
                QString country = addressModel->GetData("Country").toString();
                address.SetCountry(country);
            }

            if (addressModel->ContainsKey("City")){
                QString city = addressModel->GetData("City").toString();
                address.SetCity(city);
            }

            if (addressModel->ContainsKey("PostalCode")){
                int postalCode = addressModel->GetData("PostalCode").toInt();
                address.SetPostalCode(postalCode);
            }

            if (addressModel->ContainsKey("Street")){
                QString street = addressModel->GetData("Street").toString();
                address.SetStreet(street);
            }

            companyInfoPtr->SetAddress(address);
        }

        return companyInfoPtr.PopPtr();
	}

	errorMessage = QObject::tr("Can not create account: %1").arg(QString(objectId));

	return nullptr;
}


} // namespace imtlicgql


