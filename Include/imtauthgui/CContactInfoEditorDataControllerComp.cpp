#include <imtauthgui/CContactInfoEditorDataControllerComp.h>


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

CContactInfoEditorDataControllerComp::CContactInfoEditorDataControllerComp()
{
	m_contactInfoPtr = nullptr;
}


//QByteArray CContactInfoEditorDataControllerComp::CItemBasedRepresentationDataProvider::GetModelId() const
//{
//	return QByteArray();
//}


//imtbase::CTreeItemModel *CContactInfoEditorDataControllerComp::CItemBasedRepresentationDataProvider::GetTreeItemModel(const QList<imtgql::CGqlObject>& /*params*/,const QByteArrayList& /*fields*/)
//{
//	return &m_treeItemModel;
//}


// protected methods

// reimplemented (imod::IObserver)

//bool CContactInfoEditorDataControllerComp::OnModelAttached(imod::IModel *modelPtr, istd::IChangeable::ChangeSet &changeMask)
//{
//	m_contactInfoPtr = dynamic_cast<imtauth::IContactInfo*>(modelPtr);
//	m_itemBesedRepresentetionProvider.m_treeItemModel.AttachObserver(&m_treeItemModelObserver);
//	if (!BaseClass2::OnModelAttached(modelPtr, changeMask)){
//		return false;
//	}
//	return true;
//}


// reimplimented (imod::CSingleModelObserverBase)

//void CContactInfoEditorDataControllerComp::OnUpdate(const istd::IChangeable::ChangeSet &changeSet)
//{
////	if (m_representationDataControllerCompPtr.IsValid() && m_contactInfoPtr != nullptr){
////		m_representationDataControllerCompPtr->UpdateRepresentationFromBaseModel(
////					m_contactInfoPtr,
////					&m_itemBesedRepresentetionProvider.m_treeItemModel,
////					changeSet);
////	}
//}


//CContactInfoEditorDataControllerComp::CTreeItemModelObserver::CTreeItemModelObserver(CContactInfoEditorDataControllerComp *parent)
//{
//	m_parent = parent;
//}


//CContactInfoEditorDataControllerComp::CTreeItemModelObserver::~CTreeItemModelObserver()
//{

//}


//// reimplimented (imod::CSingleModelObserverBase)

//void CContactInfoEditorDataControllerComp::CTreeItemModelObserver::OnUpdate(const istd::IChangeable::ChangeSet &changeSet)
//{
//	if(m_parent->m_representationDataControllerCompPtr.IsValid() && m_parent->m_contactInfoPtr != nullptr){
//		m_parent->m_representationDataControllerCompPtr->UpdateBaseModelFromRepresentation(
//					m_parent->m_contactInfoPtr,
//					&m_parent->m_itemBesedRepresentetionProvider.m_treeItemModel,
//					changeSet);
//	}
//	if (m_parent->m_subscriberEngineCompPtr.IsValid()){
//		m_parent->m_subscriberEngineCompPtr->OnModelUpdate("__ContactInfo__", changeSet);
//	}

//	qDebug() << "OnUpdate";
//}


} // namespace imtauthgui


