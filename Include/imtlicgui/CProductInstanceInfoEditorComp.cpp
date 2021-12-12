#include <imtlicgui/CProductInstanceInfoEditorComp.h>


// Qt includes
#include <QtCore/QModelIndex>
#include <QtWidgets/QTreeWidgetItem>
#include <QtWidgets/QDateTimeEdit>

// ACF includes
#include <istd/CChangeGroup.h>

// ImtCore includes
#include <imtauth/IAccountInfo.h>
#include <imtlic/IProductLicensingInfo.h>
#include <imtlic/ILicenseInfo.h>
#include <imtlic/ILicenseInstance.h>


namespace imtlicgui
{


// public methods

CProductInstanceInfoEditorComp::CProductInstanceInfoEditorComp()
	:m_itemUpdateBlocked(false),
	m_productCollectionObserver(*this),
	m_licenseCollectionObserver(*this),
	m_customerCollectionObserver(*this)
{
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
	m_customerCollectionObserver.RegisterObject(productInstanceInfoPtr->GetCustomerDatabase(), &CProductInstanceInfoEditorComp::OnCustomersUpdated);
}


void CProductInstanceInfoEditorComp::OnGuiModelDetached()
{
	m_customerCollectionObserver.UnregisterAllObjects();
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
	int count = LicenseInstancesEdit->topLevelItemCount();
	for (int i = 0; i < count; i++){
		LicenseInstancesEdit->topLevelItem(i)->setCheckState(0, Qt::Unchecked);
	}

	m_cachedLicenseExpiration.clear();

	DoUpdateModel();

	if (!IsUpdateBlocked()){
		UpdateBlocker updateBlocker(this);

		UpdateLicenseInstancesEdit();
	}
}


void CProductInstanceInfoEditorComp::on_LicenseInstancesEdit_itemChanged(QTreeWidgetItem *item, int column)
{
	if (m_itemUpdateBlocked){
		return;
	}

	m_itemUpdateBlocked = true;

	QByteArray licenseId = item->data(0, Qt::UserRole).toByteArray();
	QDateTime licenseDateTime = item->data(1, Qt::UserRole).toDateTime();

	if (column == 0){
		item->setData(1, Qt::UserRole, QDateTime());

		if (item->checkState(0) == Qt::Unchecked){
			if (licenseDateTime.isValid()){
				m_cachedLicenseExpiration[licenseId] = licenseDateTime;
			}

			item->setData(1, Qt::CheckStateRole, QVariant());
			item->setText(1, tr(""));
		}
		else{
			item->setData(1, Qt::CheckStateRole, Qt::Unchecked);
			item->setText(1, tr("Unlimited"));
		}
	}
	
	if (column == 1){
		if (licenseDateTime.isValid()){
			m_cachedLicenseExpiration[licenseId] = licenseDateTime;
		}

		QDateTime cachedDateTime = m_cachedLicenseExpiration[licenseId];
		if (!cachedDateTime.isValid()){
			cachedDateTime.setDate(QDate(2023, 1, 1));
			cachedDateTime.setTime(QTime(0, 0, 0));
		}

		if (item->checkState(1) == Qt::Unchecked){
			item->setData(1, Qt::UserRole, QDateTime());
			item->setText(1, tr("Unlimited"));
		}
		else{
			item->setData(1, Qt::UserRole, cachedDateTime);
			item->setText(1, cachedDateTime.date().toString(Qt::DateFormat::SystemLocaleDate));
		}
	}

	m_itemUpdateBlocked = false;

	DoUpdateModel();
}


void CProductInstanceInfoEditorComp::on_CustomerCombo_currentIndexChanged(int /*index*/)
{
	DoUpdateModel();
}


// private methods

void CProductInstanceInfoEditorComp::OnProductsUpdated(
	const istd::IChangeable::ChangeSet& /*changeSet*/,
	const imtbase::IObjectCollection* /*productCollectionPtr*/)
{
	if (!IsUpdateBlocked()){
		UpdateBlocker blocker(this);

		UpdateProductsCombo();
		UpdateLicenseInstancesEdit();
	}
}


void CProductInstanceInfoEditorComp::OnLicensesUpdated(
	const istd::IChangeable::ChangeSet& /*changeSet*/,
	const imtbase::IObjectCollection* /*productCollectionPtr*/)
{
	if (!IsUpdateBlocked()){
		UpdateBlocker blocker(this);

		UpdateLicenseInstancesEdit();
	}
}


void CProductInstanceInfoEditorComp::OnCustomersUpdated(
	const istd::IChangeable::ChangeSet& /*changeSet*/,
	const imtbase::IObjectCollection* /*productCollectionPtr*/)
{
	if (!IsUpdateBlocked()){
		UpdateBlocker blocker(this);

		imtlic::IProductInstanceInfo* productInstanceInfoPtr = GetObservedObject();
		Q_ASSERT(productInstanceInfoPtr != nullptr);

		QString selectedCustomerId = productInstanceInfoPtr->GetCustomerId();

		CustomerCombo->clear();

		QString selectedCustomerName;

		const imtbase::IObjectCollection* customerCollectionPtr = productInstanceInfoPtr->GetCustomerDatabase();
		if (customerCollectionPtr != nullptr){
			const imtbase::IObjectCollectionInfo::Ids customerIds = customerCollectionPtr->GetElementIds();
			for (const QByteArray& customerId : customerIds){
				imtbase::IObjectCollection::DataPtr dataPtr;
				if (customerCollectionPtr->GetObjectData(customerId, dataPtr)){
					imtauth::IAccountInfo* customerPtr = dynamic_cast<imtauth::IAccountInfo*>(dataPtr.GetPtr());
					if (customerPtr != nullptr){
						QString customerName =  customerPtr->GetAccountName();

						CustomerCombo->addItem(customerName, customerId);

						if (selectedCustomerId == customerId){
							selectedCustomerName = customerName;
						}
					}
				}
			}
		}

		if (selectedCustomerName.isEmpty()){
			CustomerCombo->setCurrentIndex(-1);
		}
		else{
			CustomerCombo->setCurrentText(selectedCustomerName);
		}
	}
}


void CProductInstanceInfoEditorComp::UpdateProductsCombo()
{
	ProductCombo->clear();

	m_licenseCollectionObserver.UnregisterAllObjects();

	imtlic::IProductInstanceInfo* productInstanceInfoPtr = GetObservedObject();
	Q_ASSERT(productInstanceInfoPtr != nullptr);

	QString selectedProductId = productInstanceInfoPtr->GetProductId();
	QString newSelectedProductId;

	const imtbase::IObjectCollection* productsCollectionPtr = productInstanceInfoPtr->GetProductDatabase();
	if (productsCollectionPtr != nullptr){
		const imtbase::IObjectCollectionInfo::Ids productIds = productsCollectionPtr->GetElementIds();
		for (const QByteArray& productId : productIds){
			QString productName = productsCollectionPtr->GetElementInfo(productId, imtbase::ICollectionInfo::EIT_NAME).toString();

			ProductCombo->addItem(productName, productId);

			if (selectedProductId == productName){
				ProductCombo->setCurrentText(productName);
				newSelectedProductId = selectedProductId;

				m_licenseCollectionObserver.RegisterObject(productsCollectionPtr->GetObjectPtr(productId), &CProductInstanceInfoEditorComp::OnLicensesUpdated);
			}
		}
	}

	if (newSelectedProductId.isEmpty()){
		ProductCombo->setCurrentIndex(-1);
	}
}


void CProductInstanceInfoEditorComp::UpdateLicenseInstancesEdit()
{
	LicenseInstancesEdit->clear();

	imtlic::IProductInstanceInfo* productInstanceInfoPtr = GetObservedObject();
	Q_ASSERT(productInstanceInfoPtr != nullptr);

	QByteArrayList activatedLicenseIds;

	const imtbase::IObjectCollectionInfo::Ids allInstanceLicenseIds = productInstanceInfoPtr->GetLicenseInstances().GetElementIds();
	for (const QByteArray& licenseCollectionId : allInstanceLicenseIds){
		const imtlic::ILicenseInstance* licenseInstance = productInstanceInfoPtr->GetLicenseInstance(licenseCollectionId);
		if (licenseInstance != nullptr){
			activatedLicenseIds.append(licenseInstance->GetLicenseId());
		}
	}

	const imtbase::IObjectCollection* productsCollectionPtr = productInstanceInfoPtr->GetProductDatabase();
	if (productsCollectionPtr != nullptr){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (productsCollectionPtr->GetObjectData(GetProductId(productInstanceInfoPtr->GetProductId()), dataPtr)){
			const imtlic::IProductLicensingInfo* licensingInfoPtr = dynamic_cast<const imtlic::IProductLicensingInfo*>(dataPtr.GetPtr());
			if (licensingInfoPtr != nullptr){
				const imtbase::ICollectionInfo& licenseList = licensingInfoPtr->GetLicenseList();
				const imtbase::IObjectCollectionInfo::Ids licenseCollectionIds = licenseList.GetElementIds();
				for (const QByteArray& collectionId : licenseCollectionIds){
					const imtlic::ILicenseInfo* licenseInfoPtr = licensingInfoPtr->GetLicenseInfo(collectionId);
					if (licenseInfoPtr != nullptr){
						QString licenseName = licenseInfoPtr->GetLicenseName();
						QByteArray licenseId = licenseInfoPtr->GetLicenseId();

						licenseName += " (" + licenseId + ")";
						QString licenseNameText = QString(licenseName);

						QTreeWidgetItem* itemPtr = new QTreeWidgetItem({licenseNameText, ""});
						itemPtr->setFlags(itemPtr->flags() | Qt::ItemIsEditable);
						itemPtr->setData(0, Qt::UserRole, licenseId);

						if (activatedLicenseIds.contains(licenseId)){
							itemPtr->setCheckState(0, Qt::Checked);

							const imtlic::ILicenseInstance* licenseInstancePtr = productInstanceInfoPtr->GetLicenseInstance(licenseId);
							Q_ASSERT(licenseInstancePtr != nullptr);

							itemPtr->setData(1, Qt::UserRole, licenseInstancePtr->GetExpiration());
							QDate date = licenseInstancePtr->GetExpiration().date();
							if (date.isValid()){
								itemPtr->setCheckState(1, Qt::Checked);
								itemPtr->setText(1, licenseInstancePtr->GetExpiration().date().toString(Qt::DateFormat::SystemLocaleDate));
							}
							else{
								itemPtr->setCheckState(1, Qt::Unchecked);
								itemPtr->setText(1, tr("Unlimited"));
							}
						}
						else{
							itemPtr->setCheckState(0, Qt::Unchecked);
							itemPtr->setData(1, Qt::UserRole, QDateTime());
							itemPtr->setText(1, "");
						}

						LicenseInstancesEdit->addTopLevelItem(itemPtr);
					}
				}
			}
		}
	}

	LicenseInstancesEdit->setItemDelegate(&m_dateDelegate);
}


QByteArray CProductInstanceInfoEditorComp::GetProductId(const QString& productName) const
{
	imtlic::IProductInstanceInfo* productInstanceInfoPtr = GetObservedObject();

	if (productInstanceInfoPtr != nullptr){
		const imtbase::IObjectCollection* productsCollectionPtr = productInstanceInfoPtr->GetProductDatabase();
		if (productsCollectionPtr != nullptr){
			imtbase::ICollectionInfo::Ids productCollectionIds = productsCollectionPtr->GetElementIds();
			for (const QByteArray productCollectionId : productCollectionIds){
				if (productsCollectionPtr->GetElementInfo(productCollectionId, imtbase::ICollectionInfo::EIT_NAME).toString() == productName){
					return productCollectionId;
				}
			}
		}
	}

	return QByteArray();
}


QByteArray CProductInstanceInfoEditorComp::GetCustomerId(const QString& customerName) const
{
	imtlic::IProductInstanceInfo* productInstanceInfoPtr = GetObservedObject();
	if (productInstanceInfoPtr != nullptr){
		const imtbase::IObjectCollection* customerCollectionPtr = productInstanceInfoPtr->GetCustomerDatabase();
		if (customerCollectionPtr != nullptr){
			imtbase::ICollectionInfo::Ids customerCollectionIds = customerCollectionPtr->GetElementIds();
			for (const QByteArray customerCollectionId : customerCollectionIds){
				imtbase::IObjectCollection::DataPtr dataPtr;
				if (customerCollectionPtr->GetObjectData(customerCollectionId, dataPtr)){
					imtauth::IAccountInfo* customerPtr = dynamic_cast<imtauth::IAccountInfo*>(dataPtr.GetPtr());
					if (customerPtr != nullptr){
						if (customerPtr->GetAccountName() == customerName){
							return customerCollectionId;
						}
					}
				}
			}
		}
	}

	return QByteArray();
}


// public methods of the embedded class DateTimeDelegate

CProductInstanceInfoEditorComp::DateTimeDelegate::DateTimeDelegate(QObject *parent)
	:BaseClass(parent)
{
}


// reimplemented (QItemDelegate)

QWidget* CProductInstanceInfoEditorComp::DateTimeDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& /*option*/, const QModelIndex& index) const
{
	if (index.column() == 1){
		if (index.data(Qt::CheckStateRole).toInt() == Qt::Checked){
			return new QDateEdit(parent);
		}
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
			model->setData(index, dateEditPtr->dateTime().date().toString(Qt::DateFormat::SystemLocaleDate), Qt::DisplayRole);
		}
	}
}

QSize CProductInstanceInfoEditorComp::DateTimeDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QSize retVal = BaseClass::sizeHint(option, index);

	return retVal;
}


} // namespace imtlicgui


