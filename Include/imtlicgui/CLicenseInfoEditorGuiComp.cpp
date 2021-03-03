#include <imtlicgui/CLicenseInfoEditorGuiComp.h>


// ACF includes
#include <istd/CChangeGroup.h>

// ImtCore includes
#include <imtlic/IFeatureInfo.h>


namespace imtlicgui
{


// public methods

CLicenseInfoEditorGuiComp::CLicenseInfoEditorGuiComp()
	:m_collectionObserver(*this),
	m_isGuiModelInitialized(false),
	m_isCollectionRepresentationInitialized(false)
{
}


// protected methods

void CLicenseInfoEditorGuiComp::OnFeaturePackageCollectionUpdate()
{
	imtbase::IObjectCollection* collectionPtr = m_collectionObserver.GetObservedObject();
	imtbase::ICollectionInfo::Ids packageIds = collectionPtr->GetElementIds();

	Features->clear();
	m_packageFeatures.clear();

	for (const QByteArray& packageId : packageIds){
		imtbase::IObjectCollection::DataPtr packageDataPtr;
		if (collectionPtr->GetObjectData(packageId, packageDataPtr)){
			imtlic::IFeatureInfoProvider* packagePtr = dynamic_cast<imtlic::IFeatureInfoProvider*>(packageDataPtr.GetPtr());
			if (packagePtr != nullptr){
				FeatureDescriptionList featureDescriptionList;

				imtbase::ICollectionInfo::Ids featureIds = packagePtr->GetFeatureList().GetElementIds();
				for (const QByteArray& featureId : featureIds){
					const imtlic::IFeatureInfo* featureInfoPtr = packagePtr->GetFeatureInfo(featureId);
					if (featureInfoPtr != nullptr){
						FeatureDescription featureDescription;

						featureDescription.id = featureInfoPtr->GetFeatureId();
						featureDescription.name = featureInfoPtr->GetFeatureName();
						featureDescription.description = packagePtr->GetFeatureList().GetElementInfo(featureId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();

						featureDescriptionList.append(featureDescription);
					}
				}

				m_packageFeatures[packageId] = featureDescriptionList;
				m_packageNames[packageId] = collectionPtr->GetElementInfo(packageId, imtbase::ICollectionInfo::EIT_NAME).toString();
			}
		}
	}

	m_isCollectionRepresentationInitialized = true;

	if (m_isGuiModelInitialized && m_isCollectionRepresentationInitialized){
		EnumerateMissingFeatures();
		UpdateFeatureTree();
		UpdateFeatureTreeCheckStates();
		DoUpdateModel();
	}
}


void CLicenseInfoEditorGuiComp::EnumerateMissingFeatures()
{
	QByteArrayList allFeatureIds;

	QByteArrayList packageIds = m_packageFeatures.keys();
	for (const QByteArray& packageId : packageIds){
		FeatureDescriptionList featureDescriptionList = m_packageFeatures[packageId];
		for (const FeatureDescription&  featureDescription : featureDescriptionList){
			allFeatureIds.append(featureDescription.id);
		}
	}

	m_missingFeatures.clear();

	for (const QByteArray& featureId : m_featureIds){
		if (!allFeatureIds.contains(featureId)){
			m_missingFeatures.push_back(featureId);
		}
	}
}


void CLicenseInfoEditorGuiComp::UpdateFeatureTree()
{
	QSignalBlocker blocker(Features);

	Features->clear();

	QByteArrayList packageIds = m_packageFeatures.keys();
	for (const QByteArray& packageId : packageIds){
		QTreeWidgetItem* packageItemPtr = new QTreeWidgetItem({m_packageNames[packageId]});
		Features->addTopLevelItem(packageItemPtr);
		packageItemPtr->setData(0, DR_ITEM_ID, packageId);
		packageItemPtr->setData(0, DR_ITEM_TYPE, IT_PACKAGE);

		FeatureDescriptionList featureDescriptionList = m_packageFeatures[packageId];
		for (const FeatureDescription& featureDescription : featureDescriptionList){
			QTreeWidgetItem* featureItemPtr = new QTreeWidgetItem({
						featureDescription.name,
						featureDescription.id,
						featureDescription.description});
			packageItemPtr->addChild(featureItemPtr);
			featureItemPtr->setData(0, DR_ITEM_ID, featureDescription.id);
			featureItemPtr->setData(0, DR_ITEM_TYPE, IT_FEATURE);
		}
	}

	if (!m_missingFeatures.isEmpty()){
		QTreeWidgetItem* packageItemPtr = new QTreeWidgetItem({tr("Missing features")});
		Features->addTopLevelItem(packageItemPtr);
		packageItemPtr->setData(0, DR_ITEM_ID, "MISSING_FEATURES");
		packageItemPtr->setData(0, DR_ITEM_TYPE, IT_PACKAGE);
		packageItemPtr->setForeground(0, QBrush(Qt::red));

		for (const QByteArray& featureId : m_missingFeatures){
			QTreeWidgetItem* featureItemPtr = new QTreeWidgetItem({tr("ID: %1").arg(QString(featureId))});
			packageItemPtr->addChild(featureItemPtr);
			featureItemPtr->setData(0, DR_ITEM_ID, featureId);
			featureItemPtr->setData(0, DR_ITEM_TYPE, IT_FEATURE);
			featureItemPtr->setForeground(0, QBrush(Qt::red));
		}
	}

	Features->expandAll();
}


void CLicenseInfoEditorGuiComp::UpdateFeatureTreeCheckStates()
{
	QSignalBlocker blocker(Features);

	QByteArrayList packageIds = m_packageFeatures.keys();
	for (const QByteArray& packageId : packageIds){
		FeatureDescriptionList featureDescriptionList = m_packageFeatures[packageId];
		for (const FeatureDescription& featureDescription : featureDescriptionList){
			QTreeWidgetItem* featureItemPtr = GetItem(featureDescription.id);
			bool isFeaturePresent = m_featureIds.contains(featureDescription.id);
			featureItemPtr->setCheckState(0, isFeaturePresent ? Qt::Checked : Qt::Unchecked);
		}
	}

	for (const QByteArray& packageId : packageIds){
		bool isAllFeaturesChecked = true;
		bool isAllFeaturesUnchecked = true;

		FeatureDescriptionList featureDescriptionList = m_packageFeatures[packageId];
		for (const FeatureDescription& featureDescription : featureDescriptionList){
			QTreeWidgetItem* featureItemPtr = GetItem(featureDescription.id);
			if (featureItemPtr->checkState(0) == Qt::Checked){
				isAllFeaturesUnchecked = false;
			}
			else if (featureItemPtr->checkState(0) == Qt::Unchecked){
				isAllFeaturesChecked = false;
			}
		}

		QTreeWidgetItem* packageItemPtr = GetItem(packageId);
		if (isAllFeaturesChecked){
			packageItemPtr->setCheckState(0, Qt::Checked);
		}
		else if (isAllFeaturesUnchecked){
			packageItemPtr->setCheckState(0, Qt::Unchecked);
		}
		else{
			packageItemPtr->setCheckState(0, Qt::PartiallyChecked);
		}
	}

	if (!m_missingFeatures.isEmpty()){
		GetItem("MISSING_FEATURES")->setCheckState(0, Qt::Checked);
		for (const QByteArray& featureId : m_missingFeatures){
			GetItem(featureId)->setCheckState(0, Qt::Checked);
		}
	}
}


QTreeWidgetItem* CLicenseInfoEditorGuiComp::GetItem(const QByteArray& itemId)
{
	int packageCount = Features->topLevelItemCount();
	for (int packageIndex = 0; packageIndex < packageCount; packageIndex++){
		QTreeWidgetItem* packageItemPtr = Features->topLevelItem(packageIndex);
		if (packageItemPtr->data(0, DR_ITEM_ID).toByteArray() == itemId){
			return packageItemPtr;
		}

		int featureCount = packageItemPtr->childCount();
		for (int featureIndex = 0; featureIndex < featureCount; featureIndex++){
			QTreeWidgetItem* featurteItemPtr = packageItemPtr->child(featureIndex);
			if (featurteItemPtr->data(0, DR_ITEM_ID).toByteArray() == itemId){
				return featurteItemPtr;
			}
		}
	}

	Q_ASSERT(false);
	return nullptr;
}


// reimplemented (iqtgui::TGuiObserverWrap)

void CLicenseInfoEditorGuiComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	imtlic::ILicenseInfo* licenseInfoPtr = GetObservedObject();
	Q_ASSERT(licenseInfoPtr != nullptr);

	NameEdit->setText(licenseInfoPtr->GetLicenseName());
	IdEdit->setText(licenseInfoPtr->GetLicenseId());
	m_featureIds = licenseInfoPtr->GetFeatures();

	m_isGuiModelInitialized = true;

	if (m_isGuiModelInitialized && m_isCollectionRepresentationInitialized){
		EnumerateMissingFeatures();
		UpdateFeatureTree();
		UpdateFeatureTreeCheckStates();
		DoUpdateModel();
	}
}


void CLicenseInfoEditorGuiComp::OnGuiModelAttached()
{
	BaseClass::OnGuiModelAttached();

	imtlic::ILicenseInfo* licenseInfoPtr = GetObservedObject();
	Q_ASSERT(licenseInfoPtr != nullptr);

	m_featureIds = licenseInfoPtr->GetFeatures();

	imod::IModel* modelPtr = const_cast<imod::IModel*>(
				dynamic_cast<const imod::IModel*>(
							dynamic_cast<const imtbase::IObjectCollection*>(licenseInfoPtr->GetFeaturePackages())));
	if (modelPtr != nullptr){
		modelPtr->AttachObserver(&m_collectionObserver);
	}
}


void CLicenseInfoEditorGuiComp::OnGuiModelDetached()
{
	imtlic::ILicenseInfo* licenseInfoPtr = GetObservedObject();
	Q_ASSERT(licenseInfoPtr != nullptr);

	imod::IModel* modelPtr = const_cast<imod::IModel*>(
				dynamic_cast<const imod::IModel*>(
					dynamic_cast<const imtbase::IObjectCollection*>(licenseInfoPtr->GetFeaturePackages())));
	if (modelPtr != nullptr){
		if (modelPtr->IsAttached(&m_collectionObserver)){
			modelPtr->DetachObserver(&m_collectionObserver);
		}
	}

	m_isGuiModelInitialized = false;
	m_isCollectionRepresentationInitialized = false;

	BaseClass::OnGuiModelDetached();
}


void CLicenseInfoEditorGuiComp::UpdateModel() const
{
	imtlic::ILicenseInfo* licenseInfoPtr = GetObservedObject();
	Q_ASSERT(licenseInfoPtr != nullptr);

	istd::CChangeGroup changeGroup(licenseInfoPtr);

	licenseInfoPtr->SetLicenseName(NameEdit->text());
	licenseInfoPtr->SetLicenseId(IdEdit->text().toUtf8());
	licenseInfoPtr->SetFeatures(m_featureIds);
}


// reimplemented (iqtgui::CGuiComponentBase)

void CLicenseInfoEditorGuiComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();
	connect(Features, &QTreeWidget::itemChanged, this, &CLicenseInfoEditorGuiComp::OnItemChanged, Qt::QueuedConnection);
}


void CLicenseInfoEditorGuiComp::OnGuiDestroyed()
{
	connect(Features, &QTreeWidget::itemChanged, this, &CLicenseInfoEditorGuiComp::OnItemChanged);
	BaseClass::OnGuiDestroyed();
}


// private slots

void CLicenseInfoEditorGuiComp::on_NameEdit_editingFinished()
{
	DoUpdateModel();
}


void CLicenseInfoEditorGuiComp::on_IdEdit_editingFinished()
{
	DoUpdateModel();
}


void CLicenseInfoEditorGuiComp::on_Features_itemChanged(QTreeWidgetItem *item, int column)
{
	if (item->data(0, DR_ITEM_TYPE) == IT_PACKAGE){
		Qt::CheckState state = item->checkState(0);

		for (int i = 0; i < item->childCount(); i++){
			QTreeWidgetItem* featureItemPtr = item->child(i);
			QByteArray id = featureItemPtr->data(0, DR_ITEM_ID).toByteArray();

			if (state == Qt::Checked){
				if (!m_featureIds.contains(id)){
					m_featureIds.append(id);
				}
			}
			else{
				if (m_featureIds.contains(id)){
					m_featureIds.removeOne(id);
				}
			}
		}
	}
	else{
		QByteArray id = item->data(0, DR_ITEM_ID).toByteArray();

		if (item->checkState(0) == Qt::Checked){
			if (!m_featureIds.contains(id)){
				m_featureIds.append(id);
			}
		}
		else{
			if (m_featureIds.contains(id)){
				m_featureIds.removeOne(id);
			}
		}
	}

	Q_EMIT EmitItemChanged();
}


void CLicenseInfoEditorGuiComp::OnItemChanged()
{
	if (m_isGuiModelInitialized && m_isCollectionRepresentationInitialized){
		EnumerateMissingFeatures();
		UpdateFeatureTree();
		UpdateFeatureTreeCheckStates();
		DoUpdateModel();
	}
}


// public methods of the embedded class FeaturePackageCollectionObserver

CLicenseInfoEditorGuiComp::FeaturePackageCollectionObserver::FeaturePackageCollectionObserver(CLicenseInfoEditorGuiComp& parent)
	:m_parent(parent)
{
}


// reimplemented (imod::CSingleModelObserverBase)

void CLicenseInfoEditorGuiComp::FeaturePackageCollectionObserver::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	m_parent.OnFeaturePackageCollectionUpdate();
}


} // namespace imtlicgui


