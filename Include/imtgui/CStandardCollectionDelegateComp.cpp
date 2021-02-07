#include <imtgui/CStandardCollectionDelegateComp.h>


// Qt includes
#include <QtCore/QUuid>
#include <QtCore/QDir>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QInputDialog>

// ACF includes
#include <istd/CChangeGroup.h>
#include <istd/CSystem.h>
#include <idoc/CStandardDocumentMetaInfo.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>


namespace imtgui
{


CStandardCollectionDelegateComp::CStandardCollectionDelegateComp()
	:m_editContentsCommand("Edit", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, 1982)
{
}


// reimplemented (ICollectionViewDelegate)

QByteArray CStandardCollectionDelegateComp::GetSupportedTypeId() const
{
	return *m_objectTypeIdAttrPtr;
}


bool CStandardCollectionDelegateComp::InitializeDelegate(
			imtbase::IObjectCollection* collectionPtr,
			iqtgui::IGuiObject* parentGuiPtr)
{
	if (!BaseClass2::InitializeDelegate(collectionPtr, parentGuiPtr)){
		return false;
	}

	InitializeVisualStatus();

	return true;
}


bool CStandardCollectionDelegateComp::OpenDocumentEditor(
			const QByteArray& objectId,
			const QByteArray& /*viewTypeId*/) const
{
	if (m_collectionPtr != nullptr){
		imod::IModel* modelPtr = const_cast<imod::IModel*>(dynamic_cast<const imod::IModel*>(m_collectionPtr->GetObjectPtr(objectId)));
		if (modelPtr != nullptr){
			if (m_editDialogCompPtr.IsValid() && m_editDialogObserverCompPtr.IsValid()){
				if (modelPtr->AttachObserver(m_editDialogObserverCompPtr.GetPtr())){
					m_editDialogCompPtr->ExecuteDialog(nullptr);

					modelPtr->DetachObserver(m_editDialogObserverCompPtr.GetPtr());

					return true;
				}
			}
		}
	}

	return false;
}


// protected methods

// reimplemented (CObjectCollectionViewDelegate)

void CStandardCollectionDelegateComp::SetupCommands()
{
	BaseClass2::SetupCommands();

	disconnect(&m_editContentsCommand, SIGNAL(triggered()), this, SLOT(OnEdit()));

	if (m_collectionPtr != nullptr){
		connect(&m_editContentsCommand, SIGNAL(triggered()), this, SLOT(OnEdit()));

		m_editCommands.InsertChild(&m_editContentsCommand);
	}
}


// reimplemented (ibase::TLocalizableWrap)

void CStandardCollectionDelegateComp::OnLanguageChanged()
{
	BaseClass2::OnLanguageChanged();

	m_editContentsCommand.SetVisuals(tr("Edit"), tr("Edit"), tr("Edit existing object"), QIcon(":/Icons/Edit"));
}


// reimplemented (icomp::CComponentBase)

void CStandardCollectionDelegateComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();
}


void CStandardCollectionDelegateComp::OnComponentDestroyed()
{
	BaseClass::OnComponentDestroyed();
}


// private methods

void CStandardCollectionDelegateComp::InitializeVisualStatus()
{
	if (m_statusIconsProviderCompPtr.IsValid()){
		m_visualStatus.SetStatusIcon(m_statusIconsProviderCompPtr->GetIcon(0));

		const iprm::IOptionsList* typeListPtr = m_collectionPtr->GetObjectTypesInfo();
		if (typeListPtr != nullptr){
			for (int i = 0; i < typeListPtr->GetOptionsCount(); i++){
				if (typeListPtr->GetOptionId(i) == *m_objectTypeIdAttrPtr){
					m_visualStatus.SetStatusText(typeListPtr->GetOptionDescription(i));
					break;
				}
			}
		}
	}
}


// protected slots

void CStandardCollectionDelegateComp::OnEdit()
{
	for (const QByteArray& objectId : m_selectedItemIds){
		OpenDocumentEditor(objectId);
	}
}


} // namespace imtgui


