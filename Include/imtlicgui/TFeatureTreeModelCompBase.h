#pragma once


// ACF includes
#include <imod/TModelWrap.h>
#include <istd/CChangeGroup.h>

// ImtCore includes
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
class TFeatureTreeModelCompBase: public BaseComponent
{
public:
	typedef BaseComponent BaseClass;

	I_BEGIN_COMPONENT(TFeatureTreeModelCompBase);
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

	TFeatureTreeModelCompBase();

protected:
	void BuildDependencyMap(const imtbase::IObjectCollection& packageCollection);
	bool HasDependency(const QByteArray& fromFeatureId, const QByteArray& toFeatureId);

	virtual void UpdateFeatureTreeModels();

	void OnFeaturePackageCollectionUpdate(
				const istd::IChangeable::ChangeSet& /*changeSet*/,
				const imtbase::IObjectCollection* productCollectionPtr);

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	template <typename InterfaceType>
	static InterfaceType* ExtractFeatureTreeModel(TFeatureTreeModelCompBase& component)
	{
		return &component.m_featureTreeModel;
	}

	template <typename InterfaceType>
	static InterfaceType* ExtractSelectedFeaturesModel(TFeatureTreeModelCompBase& component)
	{
		return &component.m_selectedFeaturesModel;
	}

	template <typename InterfaceType>
	static InterfaceType* ExtractDisabledFeaturesModel(TFeatureTreeModelCompBase& component)
	{
		return &component.m_disabledFeaturesModel;
	}

protected:
	I_REF(imtbase::IObjectCollection, m_featurePackageCollectionCompPtr);

	imtbase::TModelUpdateBinder<imtbase::IObjectCollection, TFeatureTreeModelCompBase> m_featurePackageCollectionObserver;

	imtlic::CFeaturePackageCollection m_featurePackageCollectionMirror;
	FeatureDependencyMap m_featureDependencyMap;

	imod::TModelWrap<imtlic::CFeaturePackageCollection> m_featureTreeModel;
	imod::TModelWrap<imtbase::CMultiSelection> m_selectedFeaturesModel;
	imod::TModelWrap<imtbase::CMultiSelection> m_disabledFeaturesModel;
};


// public methods

template <class BaseComponent>
TFeatureTreeModelCompBase<BaseComponent>::TFeatureTreeModelCompBase()
	:m_featurePackageCollectionObserver(*this)
{
}


// protected methods

template <class BaseComponent>
void TFeatureTreeModelCompBase<BaseComponent>::BuildDependencyMap(const imtbase::IObjectCollection& packageCollection)
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


template <class BaseComponent>
bool TFeatureTreeModelCompBase<BaseComponent>::HasDependency(const QByteArray& fromFeatureId, const QByteArray& toFeatureId)
{
	if (m_featureDependencyMap.contains(fromFeatureId)){
		QByteArrayList featureIds = m_featureDependencyMap[fromFeatureId];

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


template <class BaseComponent>
void TFeatureTreeModelCompBase<BaseComponent>::UpdateFeatureTreeModels()
{
}


// private methods

template <class BaseComponent>
void TFeatureTreeModelCompBase<BaseComponent>::OnFeaturePackageCollectionUpdate(
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

	UpdateFeatureTreeModels();
}


// reimplemented (icomp::CComponentBase)

template <class BaseComponent>
void TFeatureTreeModelCompBase<BaseComponent>::OnComponentCreated()
{
	m_featurePackageCollectionObserver.RegisterObject(
				dynamic_cast<imtbase::IObjectCollection*>(m_featurePackageCollectionCompPtr.GetPtr()),
				&TFeatureTreeModelCompBase<BaseComponent>::OnFeaturePackageCollectionUpdate);
}


} // namespace imtlicgui


