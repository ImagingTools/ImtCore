#include <imtlicgui/CFeaturePackageGuiComp.h>


// Qt includes
#include <QtWidgets/QMessageBox>

// ACF includes
#include <istd/CChangeGroup.h>

// ImtCore includes
#include <imtlic/IFeatureDependenciesManager.h>
#include <imtlic/IFeatureInfo.h>
#include <imtlic/CFeaturePackage.h>


namespace imtlicgui
{


// public methods

CFeaturePackageGuiComp::CFeaturePackageGuiComp()
	:m_blockItemChangedHandler(false),
	m_featurePackageProxy(*this),
	m_featureSelectionObserver(*this)
{
	m_unsavedItemsGroupId = QUuid::createUuid().toByteArray();
	m_missingItemsGroupId = QUuid::createUuid().toByteArray();
}


// reimplemented (imtlicgui::IFeatureItemStateHandler)

void CFeaturePackageGuiComp::OnItemChanged(const QByteArray& itemId, ChangeId changeId, QVariantList params)
{
	if (!itemId.isEmpty() && changeId == IItemChangeHandler::CI_ITEM_ACTIVATED && !m_blockItemChangedHandler){
		if (params.count() > 0 && params[0].type() == QVariant::Bool){
			bool isActivated = params[0].toBool();

			imtlic::CFeaturePackageCollectionUtility::FeatureDependencyMap collectionDependencyMap =
						imtlic::CFeaturePackageCollectionUtility::GetDependencies(*GetObjectCollection());

			for (const QByteArray& id : m_dependencyMap.keys()){
				collectionDependencyMap[id] = m_dependencyMap[id];
			}

			if (isActivated){
				if (!m_dependencyMap[m_selectedFeatureId].contains(itemId)){
					if (!imtlic::CFeaturePackageCollectionUtility::HasDependency(collectionDependencyMap, itemId, m_selectedFeatureId)){
						m_dependencyMap[m_selectedFeatureId].append(itemId);
					}
					else{
						QMessageBox::warning(
							GetQtWidget(),
							QObject::tr("Warning"),
							QObject::tr("Feature with ID '%1' depends on feature with ID '%2'")
							.arg(QString(itemId))
							.arg(QString(m_selectedFeatureId)));

						Q_EMIT EmitItemChangedHandler();

						return;
					}
				}
			}
			else{
				if (m_dependencyMap[m_selectedFeatureId].contains(itemId)){
					m_dependencyMap[m_selectedFeatureId].removeOne(itemId);
				}
			}

			DoUpdateModel();

			Q_EMIT EmitItemChangedHandler();
		}
	}
}


// protected methods

// reimplemented (imtlicgui::TFeatureTreeModelCompWrap)

void CFeaturePackageGuiComp::UpdateItemTree()
{
	m_blockItemChangedHandler = true;

	istd::CChangeGroup changeGroup(&m_itemTree);

	m_itemTree.ResetData();
	m_itemTree.SetItemChangeHandler(this);
		
	if (!m_selectedFeatureId.isEmpty()){
		imtlic::CFeaturePackageCollectionUtility::FeatureDependencyMap collectionDependencyMap =
					imtlic::CFeaturePackageCollectionUtility::GetDependencies(*GetObjectCollection());

		for (const QByteArray& id : m_dependencyMap.keys()){
			collectionDependencyMap[id] = m_dependencyMap[id];
		}

		QByteArrayList packageCollectionFeatureIds = imtlic::CFeaturePackageCollectionUtility::GetAllFeatureIds(*GetObjectCollection());

		// Unsaved features
		{
			CItem* unsavedItemsGroupObjectPtr = new CItem();
			unsavedItemsGroupObjectPtr->SetItemChangeHandler(this);

			ItemTreePtr unsavedItemsGroupPtr(unsavedItemsGroupObjectPtr);
			unsavedItemsGroupPtr->SetId(m_unsavedItemsGroupId);
			unsavedItemsGroupPtr->SetName(tr("Unsaved Features"));

			imtlic::IFeaturePackage* packagePtr = dynamic_cast<imtlic::IFeaturePackage*>(GetObservedObject());
			if (packagePtr != nullptr){
				imtbase::ICollectionInfo::Ids featureCollectionIds = packagePtr->GetFeatureList().GetElementIds();
				for (const QByteArray& featureCollectionId : featureCollectionIds){
					const imtlic::IFeatureInfo* featurePtr = dynamic_cast<const imtlic::IFeatureInfo*>(packagePtr->GetFeatureInfo(featureCollectionId));
					if (featurePtr != nullptr){
						if (!packageCollectionFeatureIds.contains(featurePtr->GetFeatureId())){
							CItem* unsavedItemObjectPtr = new CItem();
							unsavedItemObjectPtr->SetItemChangeHandler(this);

							ItemTreePtr unsavedItemPtr(unsavedItemObjectPtr);
							unsavedItemPtr->SetId(featurePtr->GetFeatureId());
							unsavedItemPtr->SetName(featurePtr->GetFeatureName());
							unsavedItemPtr->SetActivationEnabled(true);
							unsavedItemsGroupPtr->AddChild(unsavedItemPtr);
						}
					}
				}
			}

			if (unsavedItemsGroupPtr->GetChildsCount() > 0){
				m_itemTree.AddChild(unsavedItemsGroupPtr);
			}
		}

		// Package collection features
		{
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
					if (featureId == m_selectedFeatureId){
						continue;
					}

					const imtlic::IFeatureInfo * featurePtr = imtlic::CFeaturePackageCollectionUtility::GetFeaturePtr(*GetObjectCollection(), featureId);
					QString featureName = featurePtr->GetFeatureName();

					CItem* featureItemObjectPtr = new CItem();
					featureItemObjectPtr->SetItemChangeHandler(this);

					ItemTreePtr featureItemPtr(featureItemObjectPtr);
					featureItemPtr->SetId(featureId);
					featureItemPtr->SetName(featureName);
					featureItemPtr->SetActivationEnabled(true);
					featureItemPtr->SetActivated(m_dependencyMap[m_selectedFeatureId].contains(featureId));

					if (imtlic::CFeaturePackageCollectionUtility::HasDependency(collectionDependencyMap, featureId, m_selectedFeatureId)){
						featureItemPtr->SetEnabled(false);
					}

					packageItemPtr->AddChild(featureItemPtr);
				}

				m_itemTree.AddChild(packageItemPtr);
			}
		}

		// Missing features group
		{
			CItem* missingItemsGroupObjectPtr = new CItem();
			missingItemsGroupObjectPtr->SetItemChangeHandler(this);

			ItemTreePtr missingItemsGroupPtr(missingItemsGroupObjectPtr);
			missingItemsGroupPtr->SetId(m_missingItemsGroupId);
			missingItemsGroupPtr->SetName(tr("Missing Features"));

			QByteArrayList ownFeatureIds;

			imtlic::IFeaturePackage* packagePtr = dynamic_cast<imtlic::IFeaturePackage*>(GetObservedObject());
			if (packagePtr != nullptr){
				imtbase::ICollectionInfo::Ids featureCollectionIds = packagePtr->GetFeatureList().GetElementIds();
				for (const QByteArray& featureCollectionId : featureCollectionIds){
					const imtlic::IFeatureInfo* featurePtr = dynamic_cast<const imtlic::IFeatureInfo*>(packagePtr->GetFeatureInfo(featureCollectionId));
					if (featurePtr != nullptr){
						ownFeatureIds.append(featurePtr->GetFeatureId());
					}
				}

				const imtlic::IFeatureDependenciesProvider* dependencyProviderPtr = packagePtr->GetDependenciesInfoProvider();
				if (dependencyProviderPtr != nullptr){
					QByteArrayList dependencyIds = dependencyProviderPtr->GetFeatureDependencies(m_selectedFeatureId);
					for (const QByteArray& dependencyId : dependencyIds){
						if (!packageCollectionFeatureIds.contains(dependencyId) && !ownFeatureIds.contains(dependencyId)){
							CItem* missingItemObjectPtr = new CItem();
							missingItemObjectPtr->SetItemChangeHandler(this);

							ItemTreePtr missingItemPtr(missingItemObjectPtr);
							missingItemPtr->SetId(dependencyId);
							missingItemPtr->SetName(tr("ID: %1").arg(QString(dependencyId)));
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

void CFeaturePackageGuiComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	imtlic::IFeaturePackage* packagePtr = GetObservedObject();
	Q_ASSERT(packagePtr != nullptr);
	
	const imtlic::IFeatureDependenciesProvider* dependenciesProviderPtr = packagePtr->GetDependenciesInfoProvider();

	imtbase::ICollectionInfo::Ids featureCollectionIds = packagePtr->GetFeatureList().GetElementIds();
	for (const QByteArray& featureCollectionId : featureCollectionIds){
		const imtlic::IFeatureInfo* featurePtr = packagePtr->GetFeatureInfo(featureCollectionId);

		if (featurePtr != nullptr && dependenciesProviderPtr != nullptr){
			QByteArray featureId = featurePtr->GetFeatureId();
			m_dependencyMap[featureId] = dependenciesProviderPtr->GetFeatureDependencies(featureId);
		}
	}

	UpdateItemTree();
}


void CFeaturePackageGuiComp::OnGuiModelAttached()
{
	BaseClass::OnGuiModelAttached();

	if (m_objectCollectionViewCompPtr.IsValid() && m_objectCollectionObserverCompPtr.IsValid()){
		GetObservedModel()->AttachObserver(m_objectCollectionObserverCompPtr.GetPtr());

		m_featureSelectionObserver.RegisterObject(
					dynamic_cast<imtbase::ISelection*>(m_objectCollectionViewCompPtr.GetPtr()),
					&CFeaturePackageGuiComp::OnFeatureSelectionChanged);
	}
}


void CFeaturePackageGuiComp::OnGuiModelDetached()
{
	m_featureSelectionObserver.UnregisterAllObjects();

	if (m_objectCollectionViewCompPtr.IsValid() && m_objectCollectionObserverCompPtr.IsValid()){
		GetObservedModel()->DetachObserver(m_objectCollectionObserverCompPtr.GetPtr());
	}

	BaseClass::OnGuiModelDetached();
}


void CFeaturePackageGuiComp::UpdateModel() const
{
	if (!m_selectedFeatureId.isEmpty()){
		imtlic::IFeaturePackage* packagePtr = GetObservedObject();

		if (packagePtr != nullptr){
			imtlic::IFeatureDependenciesManager* dependenciesManagerPtr = dynamic_cast<imtlic::IFeatureDependenciesManager*>(
						const_cast<imtlic::IFeatureDependenciesProvider*>(packagePtr->GetDependenciesInfoProvider()));

			if (dependenciesManagerPtr != nullptr){
				dependenciesManagerPtr->SetFeatureDependencies(m_selectedFeatureId, m_dependencyMap[m_selectedFeatureId]);
			}
		}
	}
}


// reimplemented (iqtgui::CGuiComponentBase)

void CFeaturePackageGuiComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	connect(this, &CFeaturePackageGuiComp::EmitItemChangedHandler, this, &CFeaturePackageGuiComp::OnItemChangedHandler, Qt::QueuedConnection);

	if (m_objectCollectionViewCompPtr.IsValid() && m_objectCollectionObserverCompPtr.IsValid()){
		m_objectCollectionViewCompPtr->CreateGui(Features);
	}

	if (m_featureTreeCompPtr.IsValid() && m_featureTreeObserverCompPtr.IsValid()){
		m_featureTreeCompPtr->CreateGui(Dependencies);
	}
}


void CFeaturePackageGuiComp::OnGuiDestroyed()
{
	if (m_featureTreeCompPtr.IsValid() && m_featureTreeCompPtr->IsGuiCreated()){
		m_featureTreeCompPtr->DestroyGui();
	}

	if (m_objectCollectionViewCompPtr.IsValid() && m_objectCollectionViewCompPtr->IsGuiCreated()){
		m_objectCollectionViewCompPtr->DestroyGui();
	}

	disconnect(this, &CFeaturePackageGuiComp::EmitItemChangedHandler, this, &CFeaturePackageGuiComp::OnItemChangedHandler);

	BaseClass::OnGuiDestroyed();
}


// private methods

void CFeaturePackageGuiComp::OnFeatureSelectionChanged(
			const istd::IChangeable::ChangeSet& /*changeSet*/,
			const imtbase::ISelection* selectionPtr)
{
	m_selectedFeatureId.clear();

	imtbase::ISelection::Ids featureCollectionIds = selectionPtr->GetSelectedIds();

	if (featureCollectionIds.count() == 1){
		imtlic::IFeaturePackage* packagePtr = GetObservedObject();
		if (packagePtr != nullptr){

#if QT_VERSION < 0x060000
			const imtlic::IFeatureInfo* featurePtr = packagePtr->GetFeatureInfo(featureCollectionIds.toList().first());
#else
			const imtlic::IFeatureInfo* featurePtr = packagePtr->GetFeatureInfo(QByteArrayList(featureCollectionIds.cbegin(), featureCollectionIds.cend()).first());
#endif
			if (featurePtr != nullptr){
				m_selectedFeatureId = featurePtr->GetFeatureId();
			}
		}
	}

	UpdateItemTree();
}


// private slots

void CFeaturePackageGuiComp::OnItemChangedHandler()
{
	UpdateItemTree();
}


// public methods of the embedded class FeaturePackageProxy

CFeaturePackageGuiComp::FeaturePackageProxy::FeaturePackageProxy(CFeaturePackageGuiComp& parent)
	:m_parent(parent)
{
}


// reimplemented (IFeaturePackage)

QByteArray CFeaturePackageGuiComp::FeaturePackageProxy::GetPackageId() const
{
	imtlic::IFeaturePackage* featurePackagePtr = m_parent.GetObservedObject();
	if (featurePackagePtr != nullptr){
		return featurePackagePtr->GetPackageId();
	}

	return QByteArray();
}


void CFeaturePackageGuiComp::FeaturePackageProxy::SetPackageId(const QByteArray& packageId)
{
	imtlic::IFeaturePackage* featurePackagePtr = m_parent.GetObservedObject();
	if (featurePackagePtr != nullptr){
		featurePackagePtr->SetPackageId(packageId);
	}
}


QString CFeaturePackageGuiComp::FeaturePackageProxy::GetPackageName() const
{
	return QString();
}


void CFeaturePackageGuiComp::FeaturePackageProxy::SetPackageName(const QString& packageName)
{

}


// reimplemented (imtlic::IFeatureInfoProvider)

const imtbase::IObjectCollection* CFeaturePackageGuiComp::FeaturePackageProxy::GetFeaturePackages() const
{
	imtlic::IFeaturePackage* featurePackagePtr = m_parent.GetObservedObject();
	if (featurePackagePtr != nullptr){
		return featurePackagePtr->GetFeaturePackages();
	}

	return nullptr;
}


const imtbase::ICollectionInfo& CFeaturePackageGuiComp::FeaturePackageProxy::GetFeatureList() const
{
	imtlic::IFeaturePackage* featurePackagePtr = m_parent.GetObservedObject();
	if (featurePackagePtr != nullptr){
		return featurePackagePtr->GetFeatureList();
	}

	return m_collectionInfo;
}


const imtlic::IFeatureInfo* CFeaturePackageGuiComp::FeaturePackageProxy::GetFeatureInfo(const QByteArray& featureId) const
{
	imtlic::IFeaturePackage* featurePackagePtr = m_parent.GetObservedObject();
	if (featurePackagePtr != nullptr){
		return featurePackagePtr->GetFeatureInfo(featureId);
	}

	return nullptr;
}


const imtlic::IFeatureDependenciesProvider* CFeaturePackageGuiComp::FeaturePackageProxy::GetDependenciesInfoProvider() const
{
	imtlic::IFeatureInfoProvider* featurePackagePtr = m_parent.GetObservedObject();
	if (featurePackagePtr != nullptr){
		return featurePackagePtr->GetDependenciesInfoProvider();
	}

	return nullptr;
}


const imtbase::ICollectionInfo* CFeaturePackageGuiComp::FeaturePackageProxy::GetParentFeatureInfoProviderList() const
{
	imtlic::IFeaturePackage* featurePackagePtr = m_parent.GetObservedObject();
	if (featurePackagePtr != nullptr){
		return featurePackagePtr->GetParentFeatureInfoProviderList();
	}

	return nullptr;
}


const imtlic::IFeatureInfoProvider* CFeaturePackageGuiComp::FeaturePackageProxy::GetParentFeatureInfoProvider(const QByteArray& parentId) const
{
	imtlic::IFeaturePackage* featurePackagePtr = m_parent.GetObservedObject();
	if (featurePackagePtr != nullptr){
		return featurePackagePtr->GetParentFeatureInfoProvider(parentId);
	}

	return nullptr;
}

// reimplemented (iser::ISerializable)

bool CFeaturePackageGuiComp::FeaturePackageProxy::Serialize(iser::IArchive& archive)
{
	imtlic::IFeaturePackage* featurePackagePtr = m_parent.GetObservedObject();
	if (featurePackagePtr != nullptr){
		return featurePackagePtr->Serialize(archive);
	}

	return false;
}


quint32 CFeaturePackageGuiComp::FeaturePackageProxy::GetMinimalVersion(int versionId) const
{
	imtlic::IFeaturePackage* featurePackagePtr = m_parent.GetObservedObject();
	if (featurePackagePtr != nullptr){
		return featurePackagePtr->GetMinimalVersion(versionId);
	}

	return iser::ISerializable::GetMinimalVersion(versionId);
}


} // namespace imtlicgui


