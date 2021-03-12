#pragma once


// ACF includes
#include <ibase/ICommandsProvider.h>
#include <iqtgui/TDesignerGuiObserverCompBase.h>

// ImtCore includes
#include <imtbase/IMultiSelection.h>
#include <imtbase/IObjectCollection.h>
#include <imtlic/IProductLicensingInfo.h>
#include <imtlicgui/CFeatureTreeManager.h>
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
			protected CFeatureTreeManager,
			virtual public ibase::ICommandsProvider
{
	Q_OBJECT

public:
	typedef iqtgui::TDesignerGuiObserverCompBase<
				Ui::CProductLicensingInfoGuiComp, imtlic::IProductLicensingInfo> BaseClass;

	I_BEGIN_COMPONENT(CProductLicensingInfoGuiComp);
		I_REGISTER_INTERFACE(ibase::ICommandsProvider)
		I_ASSIGN(m_featurePackageCollectionCompPtr, "FeaturePackageCollection", "Feature package collection", true, "");
		I_ASSIGN(m_objectCollectionViewCompPtr, "ObjectCollectionView", "Object collection view", true, "ObjectCollectionView");
		I_ASSIGN_TO(m_objectCollectionObserverCompPtr, m_objectCollectionViewCompPtr, true);
	I_END_COMPONENT;

	CProductLicensingInfoGuiComp();

	// reimplemented (ibase::ICommandsProvider)
	virtual const ibase::IHierarchicalCommand* GetCommands() const override;

Q_SIGNALS:
	void EmitFeatureTreeItemChanged();

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
	void OnFeaturePackageCollectionUpdate();
	void OnLicenseSelectionChanged();
	void EnumerateDependencies(const QByteArrayList& featureIds);
	void EnumerateMissingFeatures();
	void BuildDependencyMap(const imtbase::IObjectCollection& packageCollection);
	void ProcessChanges();

	void UpdateFeatureTreeItemEnableStates();
	bool HasDependency(const FeatureDependencyMap& dependencyMap, const QByteArray& fromFeatureId, const QByteArray& toFeatureId);

private Q_SLOTS:
	void OnFeatureTreeItemChanged();
	void on_Features_itemChanged(QTreeWidgetItem *item, int column);

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
	
	bool m_isGuiModelInitialized;
	bool m_isCollectionRepresentationInitialized;

	FeaturePackageCollectionObserver m_featurePackageCollectionObserver;
	LicenseSelectionObserver m_licenseSelectionObserver;

	// Selected license related members
	QByteArray m_selectedLicenseId;
	FeatureDependencyMap m_featureDependencyMap;
};


} // namespace imtlicgui


