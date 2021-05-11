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

CContactInfoEditorDataControllerComp::CContactInfoEditorDataControllerComp() :
	m_treeItemModelObserver(this)
{
	m_contactInfoPtr = nullptr;
}

// implemented CItemBasedRepresentationProvider
// reimplemented (imtauthgui::IItemBasedRepresentationController)

const imtbase::ICollectionInfo &CContactInfoEditorDataControllerComp
::CItemBasedRepresentationDataProvider::GetModelsList() const
{
	//	imtbase::ICollectionInfo
}

imtbase::CTreeItemModel *CContactInfoEditorDataControllerComp
::CItemBasedRepresentationDataProvider
::GetTreeItemModel(const QByteArray &modelId)
{
	return &m_treeItemModel;
}


// protected methods

// reimplemented (imod::IObserver)

bool CContactInfoEditorDataControllerComp::OnModelAttached(imod::IModel *modelPtr, istd::IChangeable::ChangeSet &changeMask)
{
	m_contactInfoPtr = dynamic_cast<imtauth::IContactInfo*>(modelPtr);
	m_itemBesedRepresentetionProvider.m_treeItemModel.AttachObserver(&m_treeItemModelObserver);
	if (!BaseClass2::OnModelAttached(modelPtr, changeMask)){
		return false;
	}
//	m_treeItemModelObserver.AttachOrSetObject(&m_itemBesedRepresentetionProvider.m_treeItemModel);

	return true;
}


// reimplimented (imod::CSingleModelObserverBase)

void CContactInfoEditorDataControllerComp::OnUpdate(const istd::IChangeable::ChangeSet &changeSet)
{
	if (m_representationDataControllerCompPtr.IsValid() && m_contactInfoPtr != nullptr){
		m_representationDataControllerCompPtr->UpdateRepresentationFromBaseModel(
					m_contactInfoPtr,
					&m_itemBesedRepresentetionProvider.m_treeItemModel,
					changeSet);
	}
//	imtauth::IContactInfo* contactPtr = GetObservedObject();
//	Q_ASSERT(contactPtr != nullptr);

////	istd::CChangeGroup changeGroup(&m_treeItemModel);
//	m_itemBesedRepresentetionProvider.m_treeItemModel.SetData("firstName", contactPtr->GetNameField(imtauth::IContactInfo::NFT_FIRST_NAME));
//	m_itemBesedRepresentetionProvider.m_treeItemModel.SetData("lastName", contactPtr->GetNameField(imtauth::IContactInfo::NFT_LAST_NAME));
//	m_itemBesedRepresentetionProvider.m_treeItemModel.SetData("nickName", contactPtr->GetNameField(imtauth::IContactInfo::NFT_NICKNAME));
//	qmlRegisterType<imtbase::CTreeItemModel>("ACF", 1, 0, "TreeItemModel");

//	const imtauth::IAddressProvider* addressesPtr = contactPtr->GetAddresses();
//	if (addressesPtr != nullptr){
//		imtbase::ICollectionInfo::Ids ids = addressesPtr->GetAddressList().GetElementIds();
//		m_itemBesedRepresentetionProvider.m_treeItemModel.AddTreeModel("adresses");
//		imtbase::CTreeItemModel *addressesModel = m_itemBesedRepresentetionProvider.m_treeItemModel.GetTreeItemModel("adresses");
//		for (const QByteArray& id : ids){
//			const imtauth::IAddress* addressPtr = addressesPtr->GetAddress(id);
//			if (addressPtr != nullptr){
//				int index = addressesModel->InsertNewItem();
//				addressesModel->SetData("country", addressPtr->GetCountry(), index);
//				addressesModel->SetData("city", addressPtr->GetCity(), index);
//				addressesModel->SetData("postalCode", addressPtr->GetPostalCode(), index);
//				addressesModel->SetData("street", addressPtr->GetStreet(), index);
//			}
//		}
//	}
//	m_itemBesedRepresentetionProvider.m_treeItemModel.setState("Ready");

//	m_treeItemModel.setProperty("state", "Ready");
}

CContactInfoEditorDataControllerComp::CTreeItemModelObserver::CTreeItemModelObserver(CContactInfoEditorDataControllerComp *parent)
{
	m_parent = parent;
}


CContactInfoEditorDataControllerComp::CTreeItemModelObserver::~CTreeItemModelObserver()
{

}

// implimented
// reimplimented (imod::CSingleModelObserverBase)

void CContactInfoEditorDataControllerComp::CTreeItemModelObserver::OnUpdate(const istd::IChangeable::ChangeSet &changeSet)
{
	if (m_parent->m_representationDataControllerCompPtr.IsValid() &&
			m_parent->m_contactInfoPtr != nullptr){
		m_parent->m_representationDataControllerCompPtr->UpdateBaseModelFromRepresentation(
					m_parent->m_contactInfoPtr,
					&m_parent->m_itemBesedRepresentetionProvider.m_treeItemModel,
					changeSet);
	}
	qDebug() << "OnUpdate";

}



} // namespace imtauthgui


