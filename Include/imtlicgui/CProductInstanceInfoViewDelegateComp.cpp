#include <imtlicgui/CProductInstanceInfoViewDelegateComp.h>


// ImtCore includes
#include <imtrepo/IFileObjectCollection.h>


namespace imtlicgui
{


// public methods

CProductInstanceInfoViewDelegateComp::CProductInstanceInfoViewDelegateComp()
	:m_exportLicenseCommand(tr("Create License File"), 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, CG_LICENSE)
{
}


// reimplemented (imtgui::ICollectionViewDelegate)

void CProductInstanceInfoViewDelegateComp::UpdateItemSelection(
			const imtbase::ICollectionInfo::Ids& selectedItems,
			const QByteArray& selectedTypeId)
{
	BaseClass::UpdateItemSelection(selectedItems, selectedTypeId);

	m_exportLicenseCommand.setEnabled(selectedItems.count() == 1);
}


// reimplemented (ibase::TLocalizableWrap)

void CProductInstanceInfoViewDelegateComp::OnLanguageChanged()
{
	BaseClass::OnLanguageChanged();

	m_exportLicenseCommand.SetVisuals(tr("Create License File"), tr("Create License File"), tr("Export existing product installation license to file"), QIcon(":/ColorIcons/License"));
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
	BaseClass::SetupCommands();

	m_rootCommands.InsertChild(&m_licenseCommands);

	imtrepo::IFileObjectCollection* fileCollectionPtr = dynamic_cast<imtrepo::IFileObjectCollection*>(m_collectionPtr);
	if (fileCollectionPtr != nullptr) {
		connect(&m_exportLicenseCommand, &QAction::triggered, this, &CProductInstanceInfoViewDelegateComp::OnExportLicense);
		m_licenseCommands.InsertChild(&m_exportLicenseCommand);
	}
}


// protected slots

void CProductInstanceInfoViewDelegateComp::OnExportLicense()
{

}


} // namespace aculainspgui


