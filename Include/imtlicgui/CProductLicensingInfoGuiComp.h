#pragma once


// ACF includes
#include <ibase/ICommandsProvider.h>
#include <iqtgui/TDesignerGuiObserverCompBase.h>

// ImtCore includes
#include <imtbase/IMultiSelection.h>
#include <imtbase/IObjectCollection.h>
#include <imtlic/IProductLicensingInfo.h>
#include <imtlic/CFeaturePackageCollectionProxy.h>
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

	void OnFeaturePackageCollectionUpdate();
	void OnLicenseSelectionChanged();
	void EnumerateDependencies(const QByteArrayList& featureIds);
	void EnumerateMissingFeatures();
	void BuildDependencyMap(const imtbase::IObjectCollection& packageCollection);
	void ProcessChanges();

	void UpdateFeatureTreeItemEnableStates();
	bool HasDependency(const FeatureDependencyMap& dependencyMap, const QByteArray& fromFeatureId, const QByteArray& toFeatureId);

private:
	class FeaturePackageCollectionObserver: public imod::TSingleModelObserverBase<imtbase::IObjectCollection>
	{
	public:
		FeaturePackageCollectionObserver(CProductLicensingInfoGuiComp& parent);

		// reimplemented (imod::CSingleModelObserverBase)
		virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;
	private:
		CProductLicensingInfoGuiComp& m_parent;
	};

	class LicenseSelectionObserver: public imod::TSingleModelObserverBase<imtbase::IMultiSelection>
	{
	public:
		LicenseSelectionObserver(CProductLicensingInfoGuiComp& parent);

		// reimplemented (imod::CSingleModelObserverBase)
		virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

	private:
		CProductLicensingInfoGuiComp& m_parent;
	};

	template <class InterfaceType>
	static InterfaceType* ExtractLicenseSelection(CProductLicensingInfoGuiComp& component)
	{
		return &component.m_licenseSelectionObserver;
	}

private:
	I_REF(imtbase::IObjectCollection, m_featurePackageCollectionCompPtr);
	I_REF(iqtgui::IGuiObject, m_objectCollectionViewCompPtr);
	I_REF(imod::IObserver, m_objectCollectionObserverCompPtr);
	I_REF(iqtgui::IGuiObject, m_featureTreeCompPtr);
	I_REF(imod::IObserver, m_featureTreeObserverCompPtr);

	bool m_isGuiModelInitialized;
	bool m_isCollectionRepresentationInitialized;

	FeaturePackageCollectionObserver m_featurePackageCollectionObserver;
	LicenseSelectionObserver m_licenseSelectionObserver;

	imod::TModelWrap<imtlic::CFeaturePackageCollectionProxy> m_featurePackageCollectionProxy;

	// Selected license related members
	QByteArray m_selectedLicenseId;
	FeatureDependencyMap m_featureDependencyMap;
};


} // namespace imtlicgui


