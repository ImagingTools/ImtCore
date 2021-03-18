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
#include <imtlic/CFeaturePackageCollectionUtility.h>
#include <imtlicgui/IItemChangeHandler.h>
#include <imtlicgui/CItem.h>


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
		I_REGISTER_INTERFACE(imtbase::IObjectCollectionProvider)
		I_REGISTER_SUBELEMENT(ItemTree)
		I_REGISTER_SUBELEMENT_INTERFACE(ItemTree, istd::IChangeable, ExtractItemTree)
		I_REGISTER_SUBELEMENT_INTERFACE(ItemTree, imod::IModel, ExtractItemTree)
		I_REGISTER_SUBELEMENT_INTERFACE(ItemTree, imtlicgui::IItemTree, ExtractItemTree)
		I_ASSIGN(m_featurePackageCollectionCompPtr, "FeaturePackageCollection", "Feature package collection", true, "");
	I_END_COMPONENT;

	TFeatureTreeModelCompWrap();

	// reimplemented (imtbase::IObjectCollectionProvider)
	virtual const imtbase::IObjectCollection* GetObjectCollection() const override;

protected:
	/*
		Do update of the models to reflect the current contents of feature tree GUI.
		This method should be implmented by derived classes.
	*/
	virtual void UpdateItemTree() = 0;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	void OnFeaturePackageCollectionUpdate(
				const istd::IChangeable::ChangeSet& /*changeSet*/,
				const imtbase::IObjectCollection* productCollectionPtr);

private:
	template <typename InterfaceType>
	static InterfaceType* ExtractItemTree(TFeatureTreeModelCompWrap& component)
	{
		return &component.m_itemTree;
	}

protected:
	imod::TModelWrap<imtlicgui::CItem> m_itemTree;

private:
	I_REF(imtbase::IObjectCollection, m_featurePackageCollectionCompPtr);

	imtbase::TModelUpdateBinder<imtbase::IObjectCollection, TFeatureTreeModelCompWrap> m_featurePackageCollectionObserver;
	imtlic::CFeaturePackageCollection m_featurePackageCollectionMirror;
};


// public methods

template <class BaseComponent>
TFeatureTreeModelCompWrap<BaseComponent>::TFeatureTreeModelCompWrap()
	:m_featurePackageCollectionObserver(*this)
{
}


// reimplemented (imtbase::IObjectCollectionProvider)

template <class BaseComponent>
const imtbase::IObjectCollection* TFeatureTreeModelCompWrap<BaseComponent>::GetObjectCollection() const
{
	return &m_featurePackageCollectionMirror;
}


// protected methods

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

	UpdateItemTree();
}


} // namespace imtlicgui


