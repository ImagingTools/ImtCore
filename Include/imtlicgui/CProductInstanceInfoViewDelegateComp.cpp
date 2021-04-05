#include <imtlicgui/CProductInstanceInfoViewDelegateComp.h>


// ImtCore includes
#include <imtrepo/IFileObjectCollection.h>


namespace imtlicgui
{


// public methods

CProductInstanceInfoViewDelegateComp::CProductInstanceInfoViewDelegateComp()
	:m_exportLicenseCommand(tr("Export License"), 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, CG_EDIT)
{
}


// reimplemented (imtgui::ICollectionViewDelegate)

void CProductInstanceInfoViewDelegateComp::UpdateItemSelection(
			const imtbase::ICollectionInfo::Ids& selectedItems,
			const QByteArray& selectedTypeId)
{
	BaseClass2::UpdateItemSelection(selectedItems, selectedTypeId);

	m_exportLicenseCommand.setEnabled(selectedItems.count() == 1);
}


// reimplemented (ibase::TLocalizableWrap)

void CProductInstanceInfoViewDelegateComp::OnLanguageChanged()
{
	BaseClass2::OnLanguageChanged();

	m_exportLicenseCommand.SetVisuals(tr("Export License"), tr("Export License"), tr("Export existing license"), QIcon(":/Icons/Export"));
}


// protected methods

// reimplemented (imtgui::CObjectCollectionViewDelegate)

void CProductInstanceInfoViewDelegateComp::SetupSummaryInformation()
{
	BaseClass::SetupSummaryInformation();

	m_summaryInformationTypes.RemoveItem("TypeId");
}


void CProductInstanceInfoViewDelegateComp::SetupCommands()
{
	BaseClass2::SetupCommands();

	imtrepo::IFileObjectCollection* fileCollectionPtr = dynamic_cast<imtrepo::IFileObjectCollection*>(m_collectionPtr);
	if (fileCollectionPtr != nullptr) {
		connect(&m_exportLicenseCommand, &QAction::triggered, this, &CProductInstanceInfoViewDelegateComp::OnExportLicense);
		m_editCommands.InsertChild(&m_exportLicenseCommand);
	}
}


// protected slots

void CProductInstanceInfoViewDelegateComp::OnExportLicense()
{

}


} // namespace aculainspgui


