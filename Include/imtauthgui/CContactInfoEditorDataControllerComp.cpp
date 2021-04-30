#include <imtauthgui/CContactInfoEditorDataControllerComp.h>


// Qt includes
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>

// ACF includes
#include <istd/CChangeGroup.h>
#include <iser/CJsonStringWriteArchive.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>
#include <imtauth/IAddressManager.h>
#include <imtauth/CAddress.h>


namespace imtauthgui
{


// public methods

// reimplemented (imtauthgui::IDataController)

bool CContactInfoEditorDataController::GetJsonData(QByteArray& jsonData, const QString& /*typeData*/)
{
	iser::CJsonStringWriteArchive archive(jsonData);

	bool retVal = m_treeItemModel.Serialize(archive);
	return retVal;
}


CTreeItemModel *CContactInfoEditorDataController::GetTreeItemModel(const QString &typeData)
{
	return &m_treeItemModel;
}


// protected methods


bool CContactInfoEditorDataController::OnModelAttached(imod::IModel *modelPtr, istd::IChangeable::ChangeSet &changeMask)
{
	if (!BaseClass::OnModelAttached(modelPtr, changeMask)){
		return false;
	}

	imtauth::IContactInfo* contactPtr = GetObservedObject();
	Q_ASSERT(contactPtr != nullptr);

	m_treeItemModel.setData("firstName", contactPtr->GetNameField(imtauth::IContactInfo::NFT_FIRST_NAME));
	m_treeItemModel.setData("lastName", contactPtr->GetNameField(imtauth::IContactInfo::NFT_LAST_NAME));
	m_treeItemModel.setData("nicName", contactPtr->GetNameField(imtauth::IContactInfo::NFT_NICKNAME));
	qmlRegisterType<CTreeItemModel>("ACF", 1, 0, "TreeItemModel");

	const imtauth::IAddressProvider* addressesPtr = contactPtr->GetAddresses();
	if (addressesPtr != nullptr){
		imtbase::ICollectionInfo::Ids ids = addressesPtr->GetAddressList().GetElementIds();
		m_treeItemModel.addTreeModel("adresses");
		CTreeItemModel *addressesModel = m_treeItemModel.getTreeItemModel("adresses");
		for (const QByteArray& id : ids){
			const imtauth::IAddress* addressPtr = addressesPtr->GetAddress(id);
			if (addressPtr != nullptr){
				int index = addressesModel->addItem();
				addressesModel->setData("country",addressPtr->GetCountry(), index);
				addressesModel->setData("city",addressPtr->GetCity(), index);
				addressesModel->setData("postalCode",addressPtr->GetPostalCode(), index);
				addressesModel->setData("street",addressPtr->GetStreet(), index);
			}
		}
	}
	m_treeItemModel.setProperty("state","Ready");

}






} // namespace imtauthgui


