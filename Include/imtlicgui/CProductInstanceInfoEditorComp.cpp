#include <imtlicgui/CProductInstanceInfoEditorComp.h>


// ACF includes
#include <istd/CChangeGroup.h>


namespace imtlicgui
{


// public methods

CProductInstanceInfoEditorComp::CProductInstanceInfoEditorComp()
	:m_productCollectionObserver(*this)
{
}


// protected methods

void CProductInstanceInfoEditorComp::OnProductLicensesUpdated(
			const istd::IChangeable::ChangeSet& /*changeSet*/,
			const imtbase::IObjectCollection* /*productCollectionPtr*/)
{
	UpdateProductsCombo();
}


void CProductInstanceInfoEditorComp::UpdateProductsCombo()
{
	ProductCombo->clear();

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
			}
		}
	}

	if (selectedProductId.isEmpty()){
		ProductCombo->setCurrentIndex(-1);
	}
}


// reimplemented (iqtgui::TGuiObserverWrap)

void CProductInstanceInfoEditorComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	imtlic::IProductInstanceInfo* productInstanceInfoPtr = GetObservedObject();
	Q_ASSERT(productInstanceInfoPtr != nullptr);

	UpdateProductsCombo();
}


void CProductInstanceInfoEditorComp::OnGuiModelAttached()
{
	BaseClass::OnGuiModelAttached();

	imtlic::IProductInstanceInfo* productInstanceInfoPtr = GetObservedObject();
	Q_ASSERT(productInstanceInfoPtr != nullptr);

	m_productCollectionObserver.RegisterObject(productInstanceInfoPtr->GetProductDatabase(), &CProductInstanceInfoEditorComp::OnProductLicensesUpdated);
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

	productInstanceInfoPtr->SetupProductInstance(currentProductId, instanceId, customerId);
}


// reimplemented (iqtgui::CGuiComponentBase)

void CProductInstanceInfoEditorComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();
}


void CProductInstanceInfoEditorComp::OnGuiDestroyed()
{
	BaseClass::OnGuiDestroyed();
}


// private slots

void CProductInstanceInfoEditorComp::on_NameEdit_editingFinished()
{
	DoUpdateModel();
}


void CProductInstanceInfoEditorComp::on_IdEdit_editingFinished()
{
	DoUpdateModel();
}


void CProductInstanceInfoEditorComp::on_ProductCombo_currentIndexChanged(int /*index*/)
{
	DoUpdateModel();
}


} // namespace imtlicgui


