#include <imtlicgui/CProductInstanceInfoEditorComp.h>


// Qt includes
#include <QtCore/QModelIndex>
#include <QtWidgets/QTreeWidgetItem>
#include <QtWidgets/QDateTimeEdit>

// ACF includes
#include <istd/CChangeGroup.h>

// ImtCore includes
#include <imtlic/IProductLicensingInfo.h>
#include <imtlic/ILicenseInfo.h>
#include <imtlic/ILicenseInstance.h>


namespace imtlicgui
{


// public methods

CProductInstanceInfoEditorComp::CProductInstanceInfoEditorComp()
	:m_productCollectionObserver(*this),
	m_licenseCollectionObserver(*this)
{
}


// protected methods

void CProductInstanceInfoEditorComp::OnProductsUpdated(
			const istd::IChangeable::ChangeSet& /*changeSet*/,
			const imtbase::IObjectCollection* /*productCollectionPtr*/)
{
	UpdateProductsCombo();
	UpdateLicenseInstancesEdit();
}


void CProductInstanceInfoEditorComp::OnLicensesUpdated(
			const istd::IChangeable::ChangeSet& /*changeSet*/,
			const imtbase::IObjectCollection* /*productCollectionPtr*/)
{
	UpdateLicenseInstancesEdit();
}


void CProductInstanceInfoEditorComp::UpdateProductsCombo()
{
	ProductCombo->clear();

	m_licenseCollectionObserver.UnregisterAllObjects();

	imtlic::IProductInstanceInfo* productInstanceInfoPtr = GetObservedObject();
	Q_ASSERT(productInstanceInfoPtr != nullptr);

	QByteArray selectedProductId = productInstanceInfoPtr->GetProductId();

	const imtbase::IObjectCollection* productsCollectionPtr = productInstanceInfoPtr->GetProductDatabase();
	if (productsCollectionPtr != nullptr){
		const imtbase::IObjectCollectionInfo::Ids productIds = productsCollectionPtr->GetElementIds();
		for ( const QByteArray& productId : productIds){
			QString productName = productsCollectionPtr->GetElementInfo(productId, imtbase::ICollectionInfo::EIT_NAME).toString();

			ProductCombo->addItem(productName, productId);

			if (selectedProductId == productId){
				ProductCombo->setCurrentText(productName);

				m_licenseCollectionObserver.RegisterObject(productsCollectionPtr->GetObjectPtr(productId), &CProductInstanceInfoEditorComp::OnLicensesUpdated);
			}
		}
	}

	if (selectedProductId.isEmpty()){
		ProductCombo->setCurrentIndex(-1);
	}
}


void CProductInstanceInfoEditorComp::UpdateLicenseInstancesEdit()
{
	LicenseInstancesEdit->clear();

	imtlic::IProductInstanceInfo* productInstanceInfoPtr = GetObservedObject();
	Q_ASSERT(productInstanceInfoPtr != nullptr);

	const imtbase::IObjectCollection* productsCollectionPtr = productInstanceInfoPtr->GetProductDatabase();
	if (productsCollectionPtr != nullptr){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (productsCollectionPtr->GetObjectData(productInstanceInfoPtr->GetProductId(), dataPtr)){
			const imtlic::IProductLicensingInfo* licensingInfoPtr = dynamic_cast<const imtlic::IProductLicensingInfo*>(dataPtr.GetPtr());
			if (licensingInfoPtr != nullptr){
				const imtbase::ICollectionInfo& licenseList = licensingInfoPtr->GetLicenseList();
				const imtbase::IObjectCollectionInfo::Ids licenseCollectionIds = licenseList.GetElementIds();
				for ( const QByteArray& collectionId : licenseCollectionIds){
					const imtlic::ILicenseInfo* licenseInfoPtr = licensingInfoPtr->GetLicenseInfo(collectionId);
					if (licenseInfoPtr != nullptr){
						const imtbase::ICollectionInfo& currentLicenses = productInstanceInfoPtr->GetLicenseInstances();
						imtbase::ICollectionInfo::Ids currentLicenseIds = currentLicenses.GetElementIds();

						QString licenseName = licenseInfoPtr->GetLicenseName();
						QByteArray licenseId = licenseInfoPtr->GetLicenseId();

						licenseName += " (" + licenseId + ")";
						QString licenseNameText = QString(licenseName);

						QTreeWidgetItem* itemPtr = new QTreeWidgetItem({licenseNameText, ""});
						itemPtr->setData(0, Qt::UserRole, licenseId);

						if (currentLicenseIds.contains(licenseId)){
							itemPtr->setCheckState(0, Qt::Checked);

							const imtlic::ILicenseInstance* licenseInstancePtr =  productInstanceInfoPtr->GetLicenseInstance(licenseId);
							Q_ASSERT(licenseInstancePtr != nullptr);

							itemPtr->setData(1, Qt::UserRole, licenseInstancePtr->GetExpiration());
							itemPtr->setText(1, licenseInstancePtr->GetExpiration().date().toString(Qt::DateFormat::SystemLocaleDate));
						}
						else{
							itemPtr->setCheckState(0, Qt::Unchecked);
							itemPtr->setData(1, Qt::UserRole, QDateTime(QDate(2000, 1, 1), QTime(0, 0)));
							itemPtr->setText(1, QDate(2000, 1, 1).toString(Qt::DateFormat::SystemLocaleDate));
						}

						LicenseInstancesEdit->addTopLevelItem(itemPtr);
					}
				}
			}
		}
	}

	LicenseInstancesEdit->setItemDelegateForColumn(1, &m_dateDelegate);
}


// reimplemented (iqtgui::TGuiObserverWrap)

void CProductInstanceInfoEditorComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	imtlic::IProductInstanceInfo* productInstanceInfoPtr = GetObservedObject();
	Q_ASSERT(productInstanceInfoPtr != nullptr);

	UpdateProductsCombo();
	UpdateLicenseInstancesEdit();

	ProductInstanceIdEdit->setText(productInstanceInfoPtr->GetProductInstanceId());
}


void CProductInstanceInfoEditorComp::OnGuiModelAttached()
{
	BaseClass::OnGuiModelAttached();

	imtlic::IProductInstanceInfo* productInstanceInfoPtr = GetObservedObject();
	Q_ASSERT(productInstanceInfoPtr != nullptr);

	m_productCollectionObserver.RegisterObject(productInstanceInfoPtr->GetProductDatabase(), &CProductInstanceInfoEditorComp::OnProductsUpdated);
}


void CProductInstanceInfoEditorComp::OnGuiModelDetached()
{
	m_productCollectionObserver.UnregisterAllObjects();

	BaseClass::OnGuiModelDetached();
}


void CProductInstanceInfoEditorComp::UpdateModel() const
{
	imtlic::IProductInstanceInfo* productInstanceInfoPtr = GetObservedObject();
	Q_ASSERT(productInstanceInfoPtr != nullptr);

	istd::CChangeGroup changeGroup(productInstanceInfoPtr);

	QByteArray currentProductId = ProductCombo->currentData().toByteArray();
	QByteArray customerId = CustomerCombo->currentData().toByteArray();
	QByteArray instanceId = ProductInstanceIdEdit->text().toUtf8();

	productInstanceInfoPtr->ResetData();
	productInstanceInfoPtr->SetupProductInstance(currentProductId, instanceId, customerId);

	productInstanceInfoPtr->ClearLicenses();

	int count = LicenseInstancesEdit->topLevelItemCount();
	for (int i = 0; i < count; i++){
		QTreeWidgetItem* itemPtr = LicenseInstancesEdit->topLevelItem(i);
		itemPtr->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsEditable);

		if (itemPtr->checkState(0) == Qt::Checked){
			QByteArray licenseId = itemPtr->data(0, Qt::UserRole).toByteArray();
			QDateTime expiration = itemPtr->data(1, Qt::UserRole).toDateTime();
			productInstanceInfoPtr->AddLicense(licenseId, expiration);
		}
	}
}


// reimplemented (iqtgui::CGuiComponentBase)

void CProductInstanceInfoEditorComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	LicenseInstancesEdit->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
}


void CProductInstanceInfoEditorComp::OnGuiDestroyed()
{
	BaseClass::OnGuiDestroyed();
}


// private slots

void CProductInstanceInfoEditorComp::on_ProductInstanceIdEdit_editingFinished()
{
	DoUpdateModel();
}


void CProductInstanceInfoEditorComp::on_ProductCombo_currentIndexChanged(int /*index*/)
{
	DoUpdateModel();

	if (!IsUpdateBlocked()){
		UpdateBlocker updateBlocker(this);

		UpdateLicenseInstancesEdit();
	}
}


void CProductInstanceInfoEditorComp::on_LicenseCombo_currentIndexChanged(int /*index*/)
{
	DoUpdateModel();
}


void CProductInstanceInfoEditorComp::on_ValidUntilDate_dateTimeChanged(const QDateTime& /*dateTime*/)
{
	DoUpdateModel();
}


void CProductInstanceInfoEditorComp::on_ExpireGroup_toggled(bool /*toggled*/)
{
	DoUpdateModel();
}


void CProductInstanceInfoEditorComp::on_LicenseInstancesEdit_itemChanged(QTreeWidgetItem *item, int column)
{
	if (column == 1){
		item->setText(1, item->data(1, Qt::UserRole).toDate().toString(Qt::DateFormat::SystemLocaleDate));
	}

	DoUpdateModel();
}


// public methods of the embedded class DateTimeDelegate

CProductInstanceInfoEditorComp::DateTimeDelegate::DateTimeDelegate(QObject *parent)
	:BaseClass(parent)
{
}


// reimplemented (QItemDelegate)

QWidget* CProductInstanceInfoEditorComp::DateTimeDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	if (index.column() == 1){
		return new QDateEdit(parent);
	}

	return nullptr;
}


void CProductInstanceInfoEditorComp::DateTimeDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
	QDateEdit* dateEditPtr = dynamic_cast<QDateEdit*>(editor);
	if (dateEditPtr != nullptr){
		if (index.column() == 1){
			dateEditPtr->setDateTime(index.data(Qt::UserRole).toDateTime());
		}
	}
}


void CProductInstanceInfoEditorComp::DateTimeDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
	QDateEdit* dateEditPtr = dynamic_cast<QDateEdit*>(editor);
	if (dateEditPtr != nullptr){
		if (index.column() == 1){
			model->setData(index, dateEditPtr->dateTime(), Qt::UserRole);
		}
	}
}

QSize CProductInstanceInfoEditorComp::DateTimeDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QSize retVal = BaseClass::sizeHint(option, index);

	retVal.setHeight(25);

	return retVal;
}


} // namespace imtlicgui


