#include <imtlicgui/CProductInstanceInfoViewDelegateComp.h>


// Qt includes
#include <QtWidgets/QFileDialog>
#include <QtCore/QFile>

// ImtCore includes
#include <imtrepo/IFileObjectCollection.h>
#include <imtlic/IProductInstanceInfo.h>
#include <imtlic/IProductLicensingInfo.h>
#include <imtlic/ILicenseInfo.h>
#include <imtlic/ILicenseInstance.h>
#include <imtlic/IFeaturePackage.h>
#include <imtlic/IFeatureInfo.h>
#include <imtlic/IFeaturePackage.h>
#include <imtlicgui/CProductInstanceInfoEditorComp.h>
#include <iser/CMemoryReadArchive.h>
#include <iser/CMemoryWriteArchive.h>
#include <imtcrypt/IEncryptionKeysProvider.h>


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


// reimplemented (imtcrypt::IEncryptionKeysProvider)

QByteArray CProductInstanceInfoViewDelegateComp::GetEncryptionKey(imtcrypt::IEncryptionKeysProvider::KeyType type) const
{
	QByteArray retVal;
	if (type == KT_PASSWORD){
		imtrepo::IFileObjectCollection* fileCollectionPtr = dynamic_cast<imtrepo::IFileObjectCollection*>(m_collectionPtr);
		Q_ASSERT(fileCollectionPtr != nullptr);
		iser::CMemoryWriteArchive archive;
		if (!m_selectedItemIds.isEmpty()){
			QByteArray id = m_selectedItemIds[0];
			imtbase::IObjectCollection::DataPtr dataPtr;
			fileCollectionPtr->GetObjectData(id, dataPtr);
			if (dataPtr.IsValid()){
				imtlic::IProductInstanceInfo* productInstanceInfoPtr;
				productInstanceInfoPtr = dynamic_cast<imtlic::IProductInstanceInfo*>(dataPtr.GetPtr());
				Q_ASSERT(productInstanceInfoPtr != nullptr);
				retVal = productInstanceInfoPtr->GetProductInstanceId();
			}
		}
	}
	if (type == KT_VECTOR){
		retVal = "vector";
	}
	return retVal;
}


// protected slots

void CProductInstanceInfoViewDelegateComp::OnExportLicense()
{
	imtrepo::IFileObjectCollection* fileCollectionPtr = dynamic_cast<imtrepo::IFileObjectCollection*>(m_collectionPtr);
	if (fileCollectionPtr != nullptr) {
		iser::CMemoryWriteArchive archive;
		if (!m_selectedItemIds.isEmpty()){
			QByteArray id = m_selectedItemIds[0];
			imtbase::IObjectCollection::DataPtr dataPtr;
			fileCollectionPtr->GetObjectData(id, dataPtr);
			if (dataPtr.IsValid()){
				imtlic::IProductInstanceInfo* productInstanceInfoPtr;
				productInstanceInfoPtr = dynamic_cast<imtlic::IProductInstanceInfo*>(dataPtr.GetPtr());
				Q_ASSERT(productInstanceInfoPtr != nullptr);
				if (m_filePersistence.IsValid()){
					m_filePersistence.GetPtr()->SaveToFile(*productInstanceInfoPtr);
				}
			}
		}
	}
}



} // namespace aculainspgui


