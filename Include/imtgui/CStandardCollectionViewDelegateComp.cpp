#include <imtgui/CStandardCollectionViewDelegateComp.h>


// Qt includes
#include <QtCore/QUuid>
#include <QtCore/QDir>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QInputDialog>

// ACF includes
#include <iprm/IOptionsList.h>
#include <istd/CChangeGroup.h>
#include <istd/CSystem.h>
#include <idoc/CStandardDocumentMetaInfo.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>


namespace imtgui
{


CStandardCollectionViewDelegateComp::CStandardCollectionViewDelegateComp()
	:m_editContentsCommand(tr("Edit"), 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, 1982)
{
}


// reimplemented (ICollectionViewDelegate)

QByteArray CStandardCollectionViewDelegateComp::GetSupportedTypeId() const
{
	return *m_objectTypeIdAttrPtr;
}


bool CStandardCollectionViewDelegateComp::InitializeDelegate(
			imtbase::IObjectCollection* collectionPtr,
			iqtgui::IGuiObject* parentGuiPtr,
			const imtbase::ICollectionFilter* filterPtr)
{
	if (!BaseClass2::InitializeDelegate(collectionPtr, parentGuiPtr, filterPtr)){
		return false;
	}

	InitializeVisualStatus();

	return true;
}


bool CStandardCollectionViewDelegateComp::OpenDocumentEditor(
			const QByteArray& objectId,
			const QByteArray& /*viewTypeId*/) const
{
	if (m_collectionPtr != nullptr){
		imod::IModel* modelPtr = const_cast<imod::IModel*>(dynamic_cast<const imod::IModel*>(m_collectionPtr->GetObjectPtr(objectId)));
		if (modelPtr != nullptr){
			if (m_editDialogCompPtr.IsValid() && m_editDialogObserverCompPtr.IsValid()){
				if (modelPtr->AttachObserver(m_editDialogObserverCompPtr.GetPtr())){
					m_editDialogCompPtr->ExecuteDialog(m_parentGuiPtr);

					modelPtr->DetachObserver(m_editDialogObserverCompPtr.GetPtr());

					return true;
				}
			}
		}
	}

	return false;
}


// protected methods

void CStandardCollectionViewDelegateComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	SetupSummaryInformation();
}


// reimplemented (CObjectCollectionViewDelegate)

void CStandardCollectionViewDelegateComp::SetupCommands()
{
	BaseClass2::SetupCommands();

	disconnect(&m_editContentsCommand, SIGNAL(triggered()), this, SLOT(OnEdit()));

	if (m_collectionPtr != nullptr){
		connect(&m_editContentsCommand, SIGNAL(triggered()), this, SLOT(OnEdit()));

		m_editCommands.InsertChild(&m_editContentsCommand);
	}
}


bool CStandardCollectionViewDelegateComp::IsBundlePersistenceSupported(ifile::IFileTypeInfo::QueryFlags /*flags*/) const
{
	return *m_useCompressorAttrPtr;
}


// reimplemented (iqtgui::TDesignSchemaHandlerWrap)

void CStandardCollectionViewDelegateComp::OnDesignSchemaChanged(const QByteArray& themeId)
{
	BaseClass2::OnDesignSchemaChanged(themeId);

	m_editContentsCommand.SetVisuals(tr("Edit"), tr("Edit"), tr("Edit existing object"), GetIcon(":/Icons/Edit"));
}


// reimplemented (ibase::TLocalizableWrap)

void CStandardCollectionViewDelegateComp::OnLanguageChanged()
{
	BaseClass2::OnLanguageChanged();

	m_editContentsCommand.SetVisuals(tr("Edit"), tr("Edit"), tr("Edit existing object"), GetIcon(":/Icons/Edit"));
}


// private methods

void CStandardCollectionViewDelegateComp::InitializeVisualStatus()
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

void CStandardCollectionViewDelegateComp::OnEdit()
{
	for (const QByteArray& objectId : m_selectedItemIds){
		OpenDocumentEditor(objectId);
	}
}


} // namespace imtgui


