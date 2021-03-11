#pragma once


// ACF includes
#include <ibase/ICommandsProvider.h>
#include <iqtgui/TDesignerGuiObserverCompBase.h>

// ImtCore includes
#include <imtbase/IMultiSelection.h>
#include <imtbase/IObjectCollection.h>
#include <imtlic/IProductLicensingInfo.h>
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

protected:
	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;
	virtual void OnGuiModelAttached() override;
	virtual void OnGuiModelDetached() override;
	virtual void UpdateModel() const;

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated();
	virtual void OnGuiDestroyed();
	virtual void OnGuiRetranslate() override;

private:
	typedef QMap<QByteArray, QByteArrayList> DependencyMap;

	void OnFeaturePackageCollectionUpdate();
	void OnLicenseSelectionChanged();
	void EnumerateMissingFeatures();
	void UpdateFeatureTree();
	void UpdateFeatureTreeCheckStates();
	QTreeWidgetItem* GetItem(const QByteArray& itemId);
	void BuildDependencyMap(const imtbase::IObjectCollection& packageCollection);
	bool HasDependency(const DependencyMap& dependencyMap, const QByteArray& fromFeatureId, const QByteArray& toFeatureId);
	void ActivateDependencies(const QByteArrayList& featureIds);

private Q_SLOTS:
	void on_Features_itemChanged(QTreeWidgetItem *item, int column);

private:
	enum DataRole
	{
		DR_ITEM_ID = Qt::UserRole,
		DR_ITEM_TYPE
	};

	enum ItemType
	{
		IT_PACKAGE = 0,
		IT_FEATURE
	};

	struct FeatureDescription
	{
		QByteArray id;
		QString name;
		QString description;
	};

	typedef QList<FeatureDescription> FeatureDescriptionList;

	struct PackageDescription
	{
		FeatureDescriptionList features;
		QString name;
	};

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
	QByteArrayList m_featureIds;
	QByteArrayList m_missingFeatureIds;

	// Feature package collection related members
	QMap<QByteArray, PackageDescription> m_packages;
	DependencyMap m_packageDependenciyMap;
};


} // namespace imtlicgui


