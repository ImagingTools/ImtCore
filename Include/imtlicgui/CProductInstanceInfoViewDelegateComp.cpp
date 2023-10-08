#include <imtlicgui/CProductInstanceInfoViewDelegateComp.h>


// Qt includes
#include <QtCore/QFile>

// ImtCore includes
#include <imtlic/IProductInstanceInfo.h>
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

bool CProductInstanceInfoViewDelegateComp::GetSummaryInformation(
			const QByteArray& objectId,
			const QVector<QByteArray> fieldIds,
			ObjectMetaInfo& objectMetaInfo) const
{
	if (m_collectionPtr == nullptr){
		return false;
	}
	idoc::MetaInfoPtr metaInfoPtr;
	metaInfoPtr = m_collectionPtr->GetDataMetaInfo(objectId);
	if (!metaInfoPtr.IsValid()){
		Q_ASSERT(metaInfoPtr.IsValid());

		return false;
	}

	for (const QByteArray& informationId: fieldIds){
		SummaryInformation summaryInformation;
		summaryInformation.infoId = informationId;
		
		if (informationId == QByteArray("InstanceId")){
			Q_ASSERT(metaInfoPtr.IsValid());

			imtgui::ICollectionViewDelegate::SummaryInformation retVal;

			retVal.text = qPrintable(metaInfoPtr->GetMetaInfo(imtlic::IProductInstanceInfo::MIT_PRODUCT_INSTANCE_ID).toByteArray());
			retVal.sortValue = retVal.text;
			objectMetaInfo.append(summaryInformation);
		}
		else if (informationId == QByteArray("Customer")){
			imtgui::ICollectionViewDelegate::SummaryInformation retVal;

			retVal.text = qPrintable(metaInfoPtr->GetMetaInfo(imtlic::IProductInstanceInfo::MIT_CUSTOMER_NAME).toString());
			retVal.sortValue = retVal.text;
			retVal.infoId = informationId;
			objectMetaInfo.append(summaryInformation);
		}
	}

	return BaseClass::GetSummaryInformation(objectId, fieldIds, objectMetaInfo);
}


void CProductInstanceInfoViewDelegateComp::UpdateItemSelection(
			const imtbase::ICollectionInfo::Ids& selectedItems,
			const QByteArray& selectedTypeId)
{
	BaseClass::UpdateItemSelection(selectedItems, selectedTypeId);

	m_exportLicenseCommand.setEnabled(selectedItems.count() == 1);
}


// protected methods

// reimplemented (imtgui::CObjectCollectionViewDelegate)

void CProductInstanceInfoViewDelegateComp::SetupSummaryInformation()
{
	BaseClass::SetupSummaryInformation();

	m_summaryInformationTypes.InsertItem("InstanceId", tr("Instance-ID"), "", 1);
	m_summaryInformationHeaders["InstanceId"] = HeaderInfo(false, HeaderInfo::IF_FILTERABLE | HeaderInfo::IF_SORTABLE);

	m_summaryInformationTypes.InsertItem("Customer", tr("Customer"), "", 2);
	m_summaryInformationHeaders["Customer"] = HeaderInfo(false, HeaderInfo::IF_FILTERABLE | HeaderInfo::IF_SORTABLE);

	m_summaryInformationTypes.RemoveItem("TypeId");
}


void CProductInstanceInfoViewDelegateComp::SetupCommands()
{
	BaseClass::SetupCommands();

	m_rootCommands.InsertChild(&m_licenseCommands);

	if (m_collectionPtr != nullptr) {
		connect(&m_exportLicenseCommand, &QAction::triggered, this, &CProductInstanceInfoViewDelegateComp::OnExportLicense);

		m_licenseCommands.InsertChild(&m_exportLicenseCommand);
	}
}


// reimplemented (ibase::TLocalizableWrap)

void CProductInstanceInfoViewDelegateComp::OnLanguageChanged()
{
	BaseClass::OnLanguageChanged();

	m_exportLicenseCommand.SetVisuals(tr("Create License File"), tr("Create License File"), tr("Export existing product installation license to file"), QIcon(":/ColorIcons/License"));
}


// reimplemented (imtcrypt::IEncryptionKeysProvider)

QByteArray CProductInstanceInfoViewDelegateComp::GetEncryptionKey(imtcrypt::IEncryptionKeysProvider::KeyType type) const
{
	QByteArray retVal;

	if (type == KT_PASSWORD){
		Q_ASSERT(m_collectionPtr != nullptr);
		iser::CMemoryWriteArchive archive;
		if (!m_selectedItemIds.isEmpty()){
			QByteArray id = m_selectedItemIds[0];
			imtbase::IObjectCollection::DataPtr dataPtr;
			m_collectionPtr->GetObjectData(id, dataPtr);
			if (dataPtr.IsValid()){
				imtlic::IProductInstanceInfo* productInstanceInfoPtr = dynamic_cast<imtlic::IProductInstanceInfo*>(dataPtr.GetPtr());
				Q_ASSERT(productInstanceInfoPtr != nullptr);

				if (productInstanceInfoPtr != nullptr){
					retVal = productInstanceInfoPtr->GetProductInstanceId();
				}
			}
		}
	}
	else if (type == KT_INIT_VECTOR){
		if (m_vectorKeyCompPtr.IsValid()){
			retVal = m_vectorKeyCompPtr->GetId();
		}
	}

	return retVal;
}


// protected slots

void CProductInstanceInfoViewDelegateComp::OnExportLicense()
{
	Q_ASSERT(m_collectionPtr != nullptr);

	if (m_collectionPtr != nullptr) {
		iser::CMemoryWriteArchive archive;
		if (!m_selectedItemIds.isEmpty()){
			QByteArray id = m_selectedItemIds[0];
			imtbase::IObjectCollection::DataPtr dataPtr;
			m_collectionPtr->GetObjectData(id, dataPtr);
			if (dataPtr.IsValid()){
				imtlic::IProductInstanceInfo* productInstanceInfoPtr;
				productInstanceInfoPtr = dynamic_cast<imtlic::IProductInstanceInfo*>(dataPtr.GetPtr());
				Q_ASSERT(productInstanceInfoPtr != nullptr);
				if (m_licensePersistenceCompPtr.IsValid()){
					m_licensePersistenceCompPtr->SaveToFile(*productInstanceInfoPtr);
				}
			}
		}
	}
}


} // namespace imtlicgui


