#include <imtlicgui/CProductLicensingInfoGuiComp.h>


// ACF includes
#include <istd/CChangeGroup.h>

// ImtCore includes
#include <imtlic/IFeatureDependenciesProvider.h>
#include <imtlic/IFeatureInfo.h>
#include <imtlic/IFeatureInfoProvider.h>
#include <imtlic/ILicenseInfo.h>
#include <imtlic/CFeaturePackage.h>
#include <imtlic/CFeaturePackageCollection.h>


namespace imtlicgui
{


// public methods

CProductLicensingInfoGuiComp::CProductLicensingInfoGuiComp()
	:m_blockItemChangedHandler(false),
	m_licenseSelectionObserver(*this)
{
	m_missingItemsGroupId = QUuid::createUuid().toByteArray();
}


// reimplemented (imtlicgui::IFeatureItemStateHandler)

void CProductLicensingInfoGuiComp::OnItemChanged(const QByteArray& itemId, IItemChangeHandler::ChangeId changeId, QVariantList params)
{
	if (!itemId.isEmpty() && changeId == IItemChangeHandler::CI_ITEM_ACTIVATED){
		if (params.count() > 0 && params[0].type() == QVariant::Bool){
			bool isActivated = params[0].toBool();

			int featureIndex = FindFeatureById(itemId, m_selectedFeatures);
			if (isActivated && (featureIndex < 0)){
				m_selectedFeatures.append(GetFeatureInfo(itemId));

				EnumerateDependencies(GetSelectedFeatureIds());

				DoUpdateModel();

				if (!m_blockItemChangedHandler){
					Q_EMIT EmitItemChangedHandler();
				}
			}

			if (!isActivated && (featureIndex >= 0)){
				m_selectedFeatures.removeAt(featureIndex);

				DoUpdateModel();

				if (!m_blockItemChangedHandler){
					Q_EMIT EmitItemChangedHandler();
				}
			}
		}
	}
}


// protected methods

// reimplemented (imtlicgui::TFeatureTreeModelCompWrap)

void CProductLicensingInfoGuiComp::UpdateItemTree()
{
	m_blockItemChangedHandler = true;

	istd::CChangeGroup changeGroup(&m_itemTree);

	m_itemTree.ResetData();
	m_itemTree.SetItemChangeHandler(this);

	if (!m_selectedLicenseId.isEmpty()){
		// Package collection features
		{
			imtlic::CFeaturePackageCollectionUtility::FeatureDependencyMap dependencyMap = imtlic::CFeaturePackageCollectionUtility::GetDependencies(*GetObjectCollection());
			QByteArrayList collectionFeatureIds = imtlic::CFeaturePackageCollectionUtility::GetAllFeatureIds(*GetObjectCollection());

			QByteArrayList packageIds = imtlic::CFeaturePackageCollectionUtility::GetPackageIds(*GetObjectCollection());
			for (const QByteArray& packageId : packageIds){
				QString packageName = imtlic::CFeaturePackageCollectionUtility::GetPackageName(*GetObjectCollection(), packageId);

				CItem* packageItemObjectPtr = new CItem();
				packageItemObjectPtr->SetItemChangeHandler(this);

				ItemTreePtr packageItemPtr(packageItemObjectPtr);
				packageItemPtr->SetId(packageId);
				packageItemPtr->SetName(packageName);

				QByteArrayList featureIds = imtlic::CFeaturePackageCollectionUtility::GetFeatureIds(*GetObjectCollection(), packageId);
				for (const QByteArray& featureId : featureIds){
					const imtlic::IFeatureInfo * featurePtr = imtlic::CFeaturePackageCollectionUtility::GetFeaturePtr(*GetObjectCollection(), featureId);
					QString featureName = featurePtr->GetFeatureName();

					CItem* featureItemObjectPtr = new CItem();
					featureItemObjectPtr->SetItemChangeHandler(this);

					ItemTreePtr featureItemPtr(featureItemObjectPtr);
					featureItemPtr->SetId(featureId);
					featureItemPtr->SetName(featureName);
					featureItemPtr->SetActivationEnabled(true);

					int featureIndex = FindFeatureById(featureId, m_selectedFeatures);
					if (featureIndex >= 0){
						featureItemPtr->SetActivated(true);

						for (const imtlic::ILicenseInfo::FeatureInfo& selectedFeature : m_selectedFeatures){
							if (imtlic::CFeaturePackageCollectionUtility::HasDependency(dependencyMap, selectedFeature.id, featureId)){
								featureItemPtr->SetEnabled(false);
							}
						}
					}

					packageItemPtr->AddChild(featureItemPtr);
				}

				m_itemTree.AddChild(packageItemPtr);
			}
		}

		// Missing features group
		{
			QByteArrayList packageCollectionFeatureIds = imtlic::CFeaturePackageCollectionUtility::GetAllFeatureIds(*GetObjectCollection());

			CItem* missingItemsGroupObjectPtr = new CItem();
			missingItemsGroupObjectPtr->SetItemChangeHandler(this);

			ItemTreePtr missingItemsGroupPtr(missingItemsGroupObjectPtr);
			missingItemsGroupPtr->SetId(m_missingItemsGroupId);
			missingItemsGroupPtr->SetName(tr("Missing Features"));

			imtlic::ILicenseInfoProvider* productPtr = dynamic_cast<imtlic::ILicenseInfoProvider*>(GetObservedObject());
			if (productPtr != nullptr){
				const imtlic::ILicenseInfo* licensePtr = nullptr;

				imtbase::ICollectionInfo::Ids licenseCollectionIds = productPtr->GetLicenseList().GetElementIds();
				for (const QByteArray& licenseCollectionId : licenseCollectionIds){
					const imtlic::ILicenseInfo* currentLicensePtr = productPtr->GetLicenseInfo(licenseCollectionId);
					if (currentLicensePtr->GetLicenseId() == m_selectedLicenseId){
						licensePtr = currentLicensePtr;
						break;
					}
				}

				if (licensePtr != nullptr){
					imtlic::ILicenseInfo::FeatureInfos features = licensePtr->GetFeatureInfos();
					for (const imtlic::ILicenseInfo::FeatureInfo& featureInfo : features){
						if (!packageCollectionFeatureIds.contains(featureInfo.id)){
							CItem* missingItemObjectPtr = new CItem();
							missingItemObjectPtr->SetItemChangeHandler(this);

							ItemTreePtr missingItemPtr(missingItemObjectPtr);
							missingItemPtr->SetId(featureInfo.id);
							missingItemPtr->SetName(tr("ID: %1").arg(QString(featureInfo.id)));
							missingItemPtr->SetActivationEnabled(true);
							missingItemPtr->SetActivated(true);
							missingItemsGroupPtr->AddChild(missingItemPtr);
						}
					}
				}
			}

			if (missingItemsGroupPtr->GetChildsCount() > 0){
				m_itemTree.AddChild(missingItemsGroupPtr);
			}
		}
	}

	m_blockItemChangedHandler = false;
}


// reimplemented (iqtgui::TGuiObserverWrap)

void CProductLicensingInfoGuiComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	imtlic::IProductLicensingInfo* productLicensingInfoPtr = GetObservedObject();
	Q_ASSERT(productLicensingInfoPtr != nullptr);

	const imtlic::ILicenseInfo* licenseInfoPtr = productLicensingInfoPtr->GetLicenseInfo(m_selectedLicenseId);
	if (licenseInfoPtr != nullptr){
		m_selectedFeatures = licenseInfoPtr->GetFeatureInfos();
	}

	UpdateItemTree();

	ProductIdEdit->setText(qPrintable(productLicensingInfoPtr->GetProductId()));
	ProductNameEdit->setText(productLicensingInfoPtr->GetName());
}


void CProductLicensingInfoGuiComp::OnGuiModelAttached()
{
	BaseClass::OnGuiModelAttached();

	if (m_objectCollectionViewCompPtr.IsValid() && m_objectCollectionObserverCompPtr.IsValid()){
		GetObservedModel()->AttachObserver(m_objectCollectionObserverCompPtr.GetPtr());

		m_licenseSelectionObserver.RegisterObject(
					dynamic_cast<imtbase::ISelection*>(m_objectCollectionViewCompPtr.GetPtr()),
					&CProductLicensingInfoGuiComp::OnLicenseSelectionChanged);
	}
}


void CProductLicensingInfoGuiComp::OnGuiModelDetached()
{
	m_licenseSelectionObserver.UnregisterAllObjects();

	if (m_objectCollectionViewCompPtr.IsValid() && m_objectCollectionObserverCompPtr.IsValid()){
		GetObservedModel()->DetachObserver(m_objectCollectionObserverCompPtr.GetPtr());
	}

	BaseClass::OnGuiModelDetached();
}


void CProductLicensingInfoGuiComp::UpdateModel() const
{
	imtlic::IProductLicensingInfo* productLicensingInfoPtr = GetObservedObject();
	Q_ASSERT(productLicensingInfoPtr != nullptr);

	istd::CChangeGroup changeGroup(productLicensingInfoPtr);

	imtbase::ICollectionInfo::Ids licenseCollectionIds = productLicensingInfoPtr->GetLicenseList().GetElementIds();
	for (const QByteArray& licenseCollectionId : licenseCollectionIds){
		imtlic::ILicenseInfo* licensePtr = const_cast<imtlic::ILicenseInfo*>(productLicensingInfoPtr->GetLicenseInfo(licenseCollectionId));
		if (licensePtr != nullptr && licensePtr->GetLicenseId() == m_selectedLicenseId){
			licensePtr->SetFeatureInfos(m_selectedFeatures);

			break;
		}
	}

	productLicensingInfoPtr->SetProductId(ProductIdEdit->text().toUtf8());
	productLicensingInfoPtr->SetName(ProductNameEdit->text());
}


// reimplemented (iqtgui::CGuiComponentBase)

void CProductLicensingInfoGuiComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	connect(this, &CProductLicensingInfoGuiComp::EmitItemChangedHandler, this, &CProductLicensingInfoGuiComp::OnItemChangedHandler, Qt::QueuedConnection);

	if (m_objectCollectionViewCompPtr.IsValid() && m_objectCollectionObserverCompPtr.IsValid()){
		m_objectCollectionViewCompPtr->CreateGui(Licenses);
	}

	if (m_featureTreeCompPtr.IsValid() && m_featureTreeObserverCompPtr.IsValid()){
		m_featureTreeCompPtr->CreateGui(Features);
	}
}


void CProductLicensingInfoGuiComp::OnGuiDestroyed()
{
	if (m_featureTreeCompPtr.IsValid() && m_featureTreeCompPtr->IsGuiCreated()){
		m_featureTreeCompPtr->DestroyGui();
	}

	if (m_objectCollectionViewCompPtr.IsValid() && m_objectCollectionViewCompPtr->IsGuiCreated()){
		m_objectCollectionViewCompPtr->DestroyGui();
	}

	disconnect(this, &CProductLicensingInfoGuiComp::EmitItemChangedHandler, this, &CProductLicensingInfoGuiComp::OnItemChangedHandler);

	BaseClass::OnGuiDestroyed();
}


// private methods

void CProductLicensingInfoGuiComp::OnLicenseSelectionChanged(
			const istd::IChangeable::ChangeSet& /*changeSet*/,
			const imtbase::ISelection* selectionPtr)
{
	m_selectedLicenseId.clear();
	m_selectedFeatures.clear();

	imtbase::ISelection::Ids licenseIds = selectionPtr->GetSelectedIds();

	if (licenseIds.count() == 1){
		imtlic::IProductLicensingInfo* productLicensingInfo = GetObservedObject();
		if (productLicensingInfo != nullptr){
			const imtlic::ILicenseInfo* licensePtr = productLicensingInfo->GetLicenseInfo(
						QByteArrayList(licenseIds.cbegin(), licenseIds.cend()).first());
			if (licensePtr != nullptr){
				m_selectedLicenseId = licensePtr->GetLicenseId();
				m_selectedFeatures = licensePtr->GetFeatureInfos();
			}
		}
	}

	UpdateItemTree();
}


void CProductLicensingInfoGuiComp::EnumerateDependencies(const QByteArrayList& featureIds)
{
	QByteArrayList nextIdsForEnumeration;

	const QMap<QByteArray, QByteArrayList> dependencyMap = imtlic::CFeaturePackageCollectionUtility::GetDependencies(*GetObjectCollection());

	for (const QByteArray& featureId : featureIds){
		if (dependencyMap.contains(featureId)){
			QByteArrayList dependencies = dependencyMap.value(featureId);

			for (const QByteArray& dependency : dependencies){
				int index = FindFeatureById(dependency, m_selectedFeatures);
				if (index < 0){
					m_selectedFeatures.append(GetFeatureInfo(dependency));

					nextIdsForEnumeration.append(dependency);
				}
			}
		}
	}

	if (!nextIdsForEnumeration.isEmpty()){
		EnumerateDependencies(nextIdsForEnumeration);
	}
}


int CProductLicensingInfoGuiComp::FindFeatureById(const QByteArray& featureId, const imtlic::ILicenseInfo::FeatureInfos& featureContainer) const
{
	for (int i = 0; i < featureContainer.count(); ++i){
		if (featureContainer[i].id == featureId){
			return i;
		}
	}

	return -1;
}


imtlic::ILicenseInfo::FeatureInfo CProductLicensingInfoGuiComp::GetFeatureInfo(const QByteArray& featureId) const
{
	QString featureName;
	const imtlic::IFeatureInfo* featurePtr = imtlic::CFeaturePackageCollectionUtility::GetFeaturePtr(*GetObjectCollection(), featureId);
	if (featurePtr != nullptr){
		featureName = featurePtr->GetFeatureName();
	}

	imtlic::ILicenseInfo::FeatureInfo featureInfo;
	featureInfo.id = featureId;
	featureInfo.name = featureName;

	return featureInfo;
}


QByteArrayList CProductLicensingInfoGuiComp::GetSelectedFeatureIds() const
{
	QByteArrayList retVal;

	for (const imtlic::ILicenseInfo::FeatureInfo& featureInfo : m_selectedFeatures){
		retVal.push_back(featureInfo.id);
	}

	return retVal;
}


void CProductLicensingInfoGuiComp::on_ProductIdEdit_textChanged()
{
	DoUpdateModel();
}


void CProductLicensingInfoGuiComp::on_ProductNameEdit_textChanged()
{
	DoUpdateModel();
}


// private slots

void CProductLicensingInfoGuiComp::OnItemChangedHandler()
{
	UpdateItemTree();
}


} // namespace imtlicgui


