#pragma once


// Qt includes
#include <QtCore/QMap>
#include <QtCore/QByteArray>

// ACF includes
#include <imod/TModelWrap.h>
#include <istd/CChangeGroup.h>

// ImtCore includes
#include <imtbase/IObjectCollectionProvider.h>
#include <imtbase/TModelUpdateBinder.h>
#include <imtbase/CMultiSelection.h>
#include <imtlic/IFeatureDependenciesProvider.h>
#include <imtlic/IFeatureInfo.h>
#include <imtlic/IFeatureInfoProvider.h>
#include <imtlic/IFeaturePackage.h>
#include <imtlic/ILicenseInfo.h>
#include <imtlic/IProductLicensingInfo.h>
#include <imtlic/CFeaturePackageCollection.h>
#include <imtlicgui/IFeatureItemStateHandler.h>


namespace imtlicgui
{


template <class BaseComponent>
class TFeatureTreeModelCompWrap:
			public BaseComponent,
			virtual public imtbase::IObjectCollectionProvider
{
public:
	typedef BaseComponent BaseClass;

	I_BEGIN_BASE_COMPONENT(TFeatureTreeModelCompWrap);
		I_REGISTER_SUBELEMENT(FeatureTreeModel)
		I_REGISTER_SUBELEMENT_INTERFACE(FeatureTreeModel, istd::IChangeable, ExtractFeatureTreeModel)
		I_REGISTER_SUBELEMENT_INTERFACE(FeatureTreeModel, imod::IModel, ExtractFeatureTreeModel)
		I_REGISTER_SUBELEMENT_INTERFACE(FeatureTreeModel, imtbase::IObjectCollection, ExtractFeatureTreeModel)
		I_REGISTER_SUBELEMENT(SelectedFeaturesModel)
		I_REGISTER_SUBELEMENT_INTERFACE(SelectedFeaturesModel, istd::IChangeable, ExtractSelectedFeaturesModel)
		I_REGISTER_SUBELEMENT_INTERFACE(SelectedFeaturesModel, imod::IModel, ExtractSelectedFeaturesModel)
		I_REGISTER_SUBELEMENT_INTERFACE(SelectedFeaturesModel, imtbase::IMultiSelection, ExtractSelectedFeaturesModel)
		I_REGISTER_SUBELEMENT(DisabledFeaturesModel)
		I_REGISTER_SUBELEMENT_INTERFACE(DisabledFeaturesModel, istd::IChangeable, ExtractDisabledFeaturesModel)
		I_REGISTER_SUBELEMENT_INTERFACE(DisabledFeaturesModel, imod::IModel, ExtractDisabledFeaturesModel)
		I_REGISTER_SUBELEMENT_INTERFACE(DisabledFeaturesModel, imtbase::IMultiSelection, ExtractDisabledFeaturesModel)
		I_ASSIGN(m_featurePackageCollectionCompPtr, "FeaturePackageCollection", "Feature package collection", true, "");
	I_END_COMPONENT;

	typedef QMap<QByteArray, QByteArrayList> FeatureDependencyMap;

	TFeatureTreeModelCompWrap();

	const FeatureDependencyMap* GetDependencyMap() const;
	bool HasDependency(const QByteArray& fromFeatureId, const QByteArray& toFeatureId, const FeatureDependencyMap* dependencyMapPtr = nullptr) const;

	// reimplemented (imtbase::IObjectCollectionProvider)
	virtual const imtbase::IObjectCollection* GetObjectCollection() const override;

protected:
	void DoUpdateFeatureTreeModels();

	virtual void UpdateFeatureTreeModels(
				imtbase::IObjectCollection* featureTreeModelPtr,
				imtbase::IMultiSelection* selectedFeaturesModelPtr,
				imtbase::IMultiSelection* disabledFeaturesModelPtr) = 0;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	void OnFeaturePackageCollectionUpdate(
				const istd::IChangeable::ChangeSet& /*changeSet*/,
				const imtbase::IObjectCollection* productCollectionPtr);

	void BuildDependencyMap(const imtbase::IObjectCollection& packageCollection);

private:
	template <typename InterfaceType>
	static InterfaceType* ExtractFeatureTreeModel(TFeatureTreeModelCompWrap& component)
	{
		return &component.m_featureTreeModel;
	}

	template <typename InterfaceType>
	static InterfaceType* ExtractSelectedFeaturesModel(TFeatureTreeModelCompWrap& component)
	{
		return &component.m_selectedFeaturesModel;
	}

	template <typename InterfaceType>
	static InterfaceType* ExtractDisabledFeaturesModel(TFeatureTreeModelCompWrap& component)
	{
		return &component.m_disabledFeaturesModel;
	}

private:
	I_REF(imtbase::IObjectCollection, m_featurePackageCollectionCompPtr);

	imtbase::TModelUpdateBinder<imtbase::IObjectCollection, TFeatureTreeModelCompWrap> m_featurePackageCollectionObserver;

	imtlic::CFeaturePackageCollection m_featurePackageCollectionMirror;
	FeatureDependencyMap m_featureDependencyMap;

	imod::TModelWrap<imtlic::CFeaturePackageCollection> m_featureTreeModel;
	imod::TModelWrap<imtbase::CMultiSelection> m_selectedFeaturesModel;
	imod::TModelWrap<imtbase::CMultiSelection> m_disabledFeaturesModel;
};


// public methods

template <class BaseComponent>
TFeatureTreeModelCompWrap<BaseComponent>::TFeatureTreeModelCompWrap()
	:m_featurePackageCollectionObserver(*this)
{
}


template <class BaseComponent>
const imtbase::IObjectCollection* TFeatureTreeModelCompWrap<BaseComponent>::GetObjectCollection() const
{
	return &m_featurePackageCollectionMirror;
}


template <class BaseComponent>
const TFeatureTreeModelCompWrap<BaseComponent>::FeatureDependencyMap* TFeatureTreeModelCompWrap<BaseComponent>::GetDependencyMap() const
{
	return &m_featureDependencyMap;
}


template <class BaseComponent>
bool TFeatureTreeModelCompWrap<BaseComponent>::HasDependency(
	const QByteArray& fromFeatureId,
	const QByteArray& toFeatureId,
	const TFeatureTreeModelCompWrap<BaseComponent>::FeatureDependencyMap* dependencyMapPtr) const
{
	if (dependencyMapPtr != nullptr){
		dependencyMapPtr = &m_featureDependencyMap;
	}

	if (dependencyMapPtr->contains(fromFeatureId)){
		QByteArrayList featureIds = dependencyMapPtr->value(fromFeatureId);

		if (featureIds.contains(toFeatureId)){
			return true;
		}

		for (const QByteArray& featureId : featureIds){
			if (HasDependency(featureId, toFeatureId)){
				return true;
			}
		}
	}

	return false;
}


// protected methods

template <class BaseComponent>
void TFeatureTreeModelCompWrap<BaseComponent>::DoUpdateFeatureTreeModels()
{
	UpdateFeatureTreeModels(
		&m_featureTreeModel,
		&m_selectedFeaturesModel,
		&m_disabledFeaturesModel);
}


// reimplemented (icomp::CComponentBase)

template <class BaseComponent>
void TFeatureTreeModelCompWrap<BaseComponent>::OnComponentCreated()
{
	m_featurePackageCollectionObserver.RegisterObject(
		dynamic_cast<imtbase::IObjectCollection*>(m_featurePackageCollectionCompPtr.GetPtr()),
		&TFeatureTreeModelCompWrap<BaseComponent>::OnFeaturePackageCollectionUpdate);
}


// private methods

template <class BaseComponent>
void TFeatureTreeModelCompWrap<BaseComponent>::OnFeaturePackageCollectionUpdate(
			const istd::IChangeable::ChangeSet& /*changeSet*/,
			const imtbase::IObjectCollection* collectionPtr)
{
	imtbase::ICollectionInfo::Ids packageCollectionIds = collectionPtr->GetElementIds();

	BuildDependencyMap(*collectionPtr);

	{
		istd::CChangeGroup changeGroup(&m_featurePackageCollectionMirror);

		m_featurePackageCollectionMirror.ResetData();

		for (const QByteArray& packageCollectionId : packageCollectionIds){
			imtbase::IObjectCollection::DataPtr packageDataPtr;
			if (collectionPtr->GetObjectData(packageCollectionId, packageDataPtr)){
				imtlic::IFeaturePackage* packagePtr = dynamic_cast<imtlic::IFeaturePackage*>(packageDataPtr.GetPtr());
				if (packagePtr != nullptr){
					// TODO: GetPackageId->GetPackageId();

					m_featurePackageCollectionMirror.InsertNewObject(
						"FeaturePackage",
						collectionPtr->GetElementInfo(packageCollectionId, imtbase::ICollectionInfo::EIT_NAME).toString(),
						collectionPtr->GetElementInfo(packageCollectionId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString(),
						packagePtr,
						packageCollectionId);
				}
			}
		}
	}

	DoUpdateFeatureTreeModels();
}


template <class BaseComponent>
void TFeatureTreeModelCompWrap<BaseComponent>::BuildDependencyMap(const imtbase::IObjectCollection& packageCollection)
{
	m_featureDependencyMap.clear();

	imtbase::ICollectionInfo::Ids packageIds = packageCollection.GetElementIds();
	for (const QByteArray& packageId : packageIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		packageCollection.GetObjectData(packageId, dataPtr);

		const imtlic::IFeatureInfoProvider* packagePtr = dynamic_cast<const imtlic::IFeatureInfoProvider*>(dataPtr.GetPtr());
		if (packagePtr != nullptr){
			const imtlic::IFeatureDependenciesProvider* dependenciesProvider = packagePtr->GetDependenciesInfoProvider();
			if (dependenciesProvider != nullptr){
				imtbase::ICollectionInfo::Ids featureCollectionIds = packagePtr->GetFeatureList().GetElementIds();
				for (const QByteArray& featureCollectionId : featureCollectionIds){
					const imtlic::IFeatureInfo* featureInfoPtr = packagePtr->GetFeatureInfo(featureCollectionId);
					if (featureInfoPtr != nullptr){
						QByteArray featureId = featureInfoPtr->GetFeatureId();

						m_featureDependencyMap[featureId] = dependenciesProvider->GetFeatureDependencies(featureId);
					}
				}
			}
		}
	}
}


} // namespace imtlicgui


