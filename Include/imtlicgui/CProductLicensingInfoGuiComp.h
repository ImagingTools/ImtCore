#pragma once


// ACF includes
#include <ibase/ICommandsProvider.h>
#include <iqtgui/TDesignerGuiObserverCompBase.h>

// ImtCore includes
#include <imtbase/TModelUpdateBinder.h>
#include <imtbase/CMultiSelection.h>
#include <imtlic/IProductLicensingInfo.h>
#include <imtlic/CFeaturePackageCollection.h>
#include <imtlicgui/IFeatureItemStateHandler.h>
#include <GeneratedFiles/imtlicgui/ui_CProductLicensingInfoGuiComp.h>


namespace imtlicgui
{


/**
	Editor for the product-related licensing.
	\ingroup LicenseManagement
*/
class CProductLicensingInfoGuiComp:
			public iqtgui::TDesignerGuiObserverCompBase<
						Ui::CProductLicensingInfoGuiComp, imtlic::IProductLicensingInfo>,
			virtual public ibase::ICommandsProvider,
			virtual public imtlicgui::IFeatureItemStateHandler
{
	Q_OBJECT

public:
	typedef iqtgui::TDesignerGuiObserverCompBase<
				Ui::CProductLicensingInfoGuiComp, imtlic::IProductLicensingInfo> BaseClass;

	I_BEGIN_COMPONENT(CProductLicensingInfoGuiComp);
		I_REGISTER_SUBELEMENT(SelectedFeatures)
		I_REGISTER_SUBELEMENT_INTERFACE(SelectedFeatures, istd::IChangeable, ExtractSelectedFeatures)
		I_REGISTER_SUBELEMENT_INTERFACE(SelectedFeatures, imod::IModel, ExtractSelectedFeatures)
		I_REGISTER_SUBELEMENT_INTERFACE(SelectedFeatures, imtbase::IMultiSelection, ExtractSelectedFeatures)
		I_REGISTER_SUBELEMENT(DisabledFeatures)
		I_REGISTER_SUBELEMENT_INTERFACE(DisabledFeatures, istd::IChangeable, ExtractDisabledFeatures)
		I_REGISTER_SUBELEMENT_INTERFACE(DisabledFeatures, imod::IModel, ExtractDisabledFeatures)
		I_REGISTER_SUBELEMENT_INTERFACE(DisabledFeatures, imtbase::IMultiSelection, ExtractDisabledFeatures)
		I_REGISTER_INTERFACE(ibase::ICommandsProvider)
		I_REGISTER_INTERFACE(imtlicgui::IFeatureItemStateHandler)
		I_ASSIGN(m_featurePackageCollectionCompPtr, "FeaturePackageCollection", "Feature package collection", true, "");
		I_ASSIGN(m_objectCollectionViewCompPtr, "ObjectCollectionView", "Object collection view", true, "ObjectCollectionView");
		I_ASSIGN_TO(m_objectCollectionObserverCompPtr, m_objectCollectionViewCompPtr, true);
		I_ASSIGN(m_featureTreeCompPtr, "FeatureTreeView", "Feature tree view", true, "FeatureTreeGui");
		I_ASSIGN_TO(m_featureTreeObserverCompPtr, m_featureTreeCompPtr, true);
	I_END_COMPONENT;

	CProductLicensingInfoGuiComp();

	// reimplemented (imtlicgui::IFeatureItemStateHandler)
	virtual void OnItemStateChanged(const QByteArray& itemId, bool isChecked) override;
	
	// reimplemented (ibase::ICommandsProvider)
	virtual const ibase::IHierarchicalCommand* GetCommands() const override;

protected:
	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;
	virtual void OnGuiModelAttached() override;
	virtual void OnGuiModelDetached() override;
	virtual void UpdateModel() const;

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated();
	virtual void OnGuiDestroyed();

private:
	typedef QMap<QByteArray, QByteArrayList> FeatureDependencyMap;

	void OnFeaturePackageCollectionUpdate(
				const istd::IChangeable::ChangeSet& /*changeSet*/,
				const imtbase::IObjectCollection* productCollectionPtr);
	void OnLicenseSelectionChanged(
				const istd::IChangeable::ChangeSet& /*changeSet*/,
				const imtbase::IMultiSelection* selectionPtr);
	void EnumerateDependencies(const QByteArrayList& featureIds);
	void EnumerateMissingFeatures();
	void BuildDependencyMap(const imtbase::IObjectCollection& packageCollection);
	bool HasDependency(const FeatureDependencyMap& dependencyMap, const QByteArray& fromFeatureId, const QByteArray& toFeatureId);

	void UpdateFeaturePackageCollectionProxy();

	template <typename InterfaceType>
	static InterfaceType* ExtractSelectedFeatures(CProductLicensingInfoGuiComp& component)
	{
		return &component.m_featureSelectionModel;
	}

	template <typename InterfaceType>
	static InterfaceType* ExtractDisabledFeatures(CProductLicensingInfoGuiComp& component)
	{
		return &component.m_featureStateModel;
	}

private:
	I_REF(imtbase::IObjectCollection, m_featurePackageCollectionCompPtr);
	I_REF(iqtgui::IGuiObject, m_objectCollectionViewCompPtr);
	I_REF(imod::IObserver, m_objectCollectionObserverCompPtr);
	I_REF(iqtgui::IGuiObject, m_featureTreeCompPtr);
	I_REF(imod::IObserver, m_featureTreeObserverCompPtr);

	bool m_isGuiModelInitialized;
	bool m_isCollectionRepresentationInitialized;

	imtbase::TModelUpdateBinder<imtbase::IObjectCollection, CProductLicensingInfoGuiComp> m_featurePackageCollectionObserver;
	imtbase::TModelUpdateBinder<imtbase::IMultiSelection, CProductLicensingInfoGuiComp> m_licenseSelectionObserver;

	imod::TModelWrap<imtlic::CFeaturePackageCollection> m_featurePackageCollectionProxy;
	imtlic::CFeaturePackageCollection m_featurePackageCollection;

	imod::TModelWrap<imtbase::CMultiSelection> m_featureSelectionModel;
	imod::TModelWrap<imtbase::CMultiSelection> m_featureStateModel;

	// Selected license related members
	QByteArray m_selectedLicenseId;
	QByteArrayList m_selectedFeatures;
	QByteArrayList m_missingFeatures;
	FeatureDependencyMap m_featureDependencyMap;
};


} // namespace imtlicgui


